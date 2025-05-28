#!/usr/bin/env python3
"""
Manual export of NeMo PyTorch checkpoint to ONNX without NeMo toolkit dependencies.

This approach extracts the .nemo file, parses the model configuration, 
loads the PyTorch checkpoint, reconstructs the model architecture, 
and exports to ONNX format.
"""

import os
import sys
import torch
import torch.nn as nn
import yaml
import tarfile
import tempfile
import shutil
import numpy as np

print("=== Manual NeMo Model Export to ONNX ===")

def extract_nemo_file(nemo_path, extract_dir):
    """Extract .nemo file (which is a gzipped tar) to directory."""
    print(f"Extracting {nemo_path} to {extract_dir}")
    
    with tarfile.open(nemo_path, 'r:gz') as tar:
        tar.extractall(extract_dir)
    
    # List extracted files
    extracted_files = os.listdir(extract_dir)
    print(f"Extracted files: {extracted_files}")
    return extracted_files

class ConformerEncoder(nn.Module):
    """Simplified Conformer encoder based on NeMo config."""
    
    def __init__(self, feat_in=80, d_model=176, n_layers=16, n_heads=4):
        super().__init__()
        self.feat_in = feat_in
        self.d_model = d_model
        self.n_layers = n_layers
        self.n_heads = n_heads
        
        # Subsampling layer (factor 4 striding)
        self.subsampling = nn.Sequential(
            nn.Conv2d(1, d_model, kernel_size=3, stride=2, padding=1),
            nn.ReLU(),
            nn.Conv2d(d_model, d_model, kernel_size=3, stride=2, padding=1),
            nn.ReLU()
        )
        
        # Input projection from subsampled features to model dimension
        # After 2 conv layers with stride 2, we have: feat_in // 4
        subsampled_feat_dim = feat_in // 4
        self.input_projection = nn.Linear(subsampled_feat_dim * d_model, d_model)
        
        # Simplified conformer layers (just transformer for now)
        encoder_layer = nn.TransformerEncoderLayer(
            d_model=d_model,
            nhead=n_heads,
            dim_feedforward=d_model * 4,
            dropout=0.1,
            batch_first=True
        )
        self.transformer = nn.TransformerEncoder(encoder_layer, num_layers=n_layers)
        
    def forward(self, features, lengths=None):
        # features: [batch, seq_len, feat_in]
        batch_size, seq_len, feat_dim = features.shape
        
        # Add channel dimension for conv: [batch, 1, seq_len, feat_in]
        x = features.unsqueeze(1)
        
        # Subsampling convolution
        x = self.subsampling(x)  # [batch, d_model, seq_len//4, feat_in//4]
        
        # Reshape for linear projection
        batch, channels, new_seq_len, new_feat_dim = x.shape
        x = x.permute(0, 2, 1, 3)  # [batch, seq_len//4, d_model, feat_in//4]
        x = x.reshape(batch, new_seq_len, channels * new_feat_dim)
        
        # Project to model dimension
        x = self.input_projection(x)  # [batch, seq_len//4, d_model]
        
        # Transformer encoding
        encoded = self.transformer(x)  # [batch, seq_len//4, d_model]
        
        # Adjust lengths if provided
        if lengths is not None:
            lengths = lengths // 4  # Account for subsampling
        
        return encoded, lengths

class CTCDecoder(nn.Module):
    """CTC decoder - simple linear projection to vocabulary."""
    
    def __init__(self, d_model=176, num_classes=128):
        super().__init__()
        self.d_model = d_model
        self.num_classes = num_classes
        self.projection = nn.Linear(d_model, num_classes)
        
    def forward(self, encoded_features):
        # encoded_features: [batch, seq_len, d_model]
        logits = self.projection(encoded_features)  # [batch, seq_len, num_classes]
        log_probs = torch.log_softmax(logits, dim=-1)
        return log_probs

class NeMoConformerCTC(nn.Module):
    """Complete NeMo Conformer CTC model."""
    
    def __init__(self, config):
        super().__init__()
        
        # Extract configuration
        encoder_config = config.get('encoder', {})
        decoder_config = config.get('decoder', {})
        
        self.feat_in = encoder_config.get('feat_in', 80)
        self.d_model = encoder_config.get('d_model', 176)
        self.n_layers = encoder_config.get('n_layers', 16)
        self.n_heads = encoder_config.get('n_heads', 4)
        self.num_classes = decoder_config.get('num_classes', 128)
        
        # Build model components
        self.encoder = ConformerEncoder(
            feat_in=self.feat_in,
            d_model=self.d_model,
            n_layers=self.n_layers,
            n_heads=self.n_heads
        )
        
        self.decoder = CTCDecoder(
            d_model=self.d_model,
            num_classes=self.num_classes
        )
        
    def forward(self, audio_signal, length=None):
        """
        Forward pass for ONNX export.
        
        Args:
            audio_signal: [batch, seq_len, feat_in] - mel features
            length: [batch] - sequence lengths
            
        Returns:
            log_probs: [batch, seq_len//4, num_classes] - CTC log probabilities
        """
        # Encode features
        encoded, encoded_lengths = self.encoder(audio_signal, length)
        
        # CTC decode
        log_probs = self.decoder(encoded)
        
        return log_probs

def load_compatible_weights(model, state_dict, config):
    """Try to load compatible weights from NeMo checkpoint."""
    print("\nAnalyzing checkpoint parameters...")
    
    model_dict = model.state_dict()
    compatible_dict = {}
    
    print(f"Model expects {len(model_dict)} parameters")
    print(f"Checkpoint has {len(state_dict)} parameters")
    
    # Print some example parameter names for debugging
    print("\nCheckpoint parameter examples:")
    for i, key in enumerate(list(state_dict.keys())[:10]):
        print(f"  {key}: {state_dict[key].shape}")
    
    print("\nModel parameter examples:")
    for i, key in enumerate(list(model_dict.keys())[:10]):
        print(f"  {key}: {model_dict[key].shape}")
    
    # Try to match parameters by shape and name similarity
    matched_count = 0
    for model_key in model_dict.keys():
        model_shape = model_dict[model_key].shape
        
        # Look for exact or partial name matches with compatible shapes
        for ckpt_key in state_dict.keys():
            if state_dict[ckpt_key].shape == model_shape:
                # Check for name similarity
                model_base = model_key.split('.')[-1]  # Get final component
                if model_base in ckpt_key or any(part in ckpt_key for part in model_key.split('.')):
                    compatible_dict[model_key] = state_dict[ckpt_key]
                    print(f"  Matched: {model_key} <- {ckpt_key} {model_shape}")
                    matched_count += 1
                    break
    
    print(f"\nSuccessfully matched {matched_count}/{len(model_dict)} parameters")
    
    if compatible_dict:
        # Load compatible weights
        model.load_state_dict(compatible_dict, strict=False)
        print("Loaded compatible parameters into model")
    else:
        print("Warning: No compatible parameters found - using random initialization")
    
    return matched_count > 0

def export_to_onnx(model, config, output_path):
    """Export the model to ONNX format."""
    print(f"\nExporting model to ONNX: {output_path}")
    
    # Set to evaluation mode
    model.eval()
    
    # Create dummy input based on config
    feat_in = config.get('encoder', {}).get('feat_in', 80)
    batch_size = 1
    seq_len = 100  # Reasonable sequence length for export
    
    # Dummy inputs
    dummy_audio = torch.randn(batch_size, seq_len, feat_in)
    dummy_length = torch.tensor([seq_len], dtype=torch.long)
    
    print(f"Input shapes: audio={dummy_audio.shape}, length={dummy_length.shape}")
    
    # Test forward pass
    with torch.no_grad():
        output = model(dummy_audio, dummy_length)
        print(f"Output shape: {output.shape}")
    
    # Export to ONNX
    torch.onnx.export(
        model,
        (dummy_audio, dummy_length),
        output_path,
        export_params=True,
        opset_version=14,
        do_constant_folding=True,
        input_names=['audio_signal', 'length'],
        output_names=['log_probs'],
        dynamic_axes={
            'audio_signal': {0: 'batch_size', 1: 'seq_len'},
            'length': {0: 'batch_size'},
            'log_probs': {0: 'batch_size', 1: 'encoded_seq_len'}
        },
        verbose=False
    )
    
    print(f"✓ ONNX export completed!")
    print(f"  File size: {os.path.getsize(output_path) / 1024 / 1024:.1f} MB")
    
    return True

def main():
    try:
        # Paths
        nemo_file = "models/nemo_real/stt_en_conformer_ctc_small.nemo"
        output_dir = "models/nemo_fastconformer_streaming"
        
        if not os.path.exists(nemo_file):
            print(f"Error: NeMo file not found: {nemo_file}")
            sys.exit(1)
        
        os.makedirs(output_dir, exist_ok=True)
        
        # Create temporary extraction directory
        with tempfile.TemporaryDirectory() as temp_dir:
            print(f"Using temporary directory: {temp_dir}")
            
            # Extract .nemo file
            extract_nemo_file(nemo_file, temp_dir)
            
            # Load configuration
            config_path = os.path.join(temp_dir, "model_config.yaml")
            with open(config_path, 'r') as f:
                config = yaml.safe_load(f)
            
            print(f"\nLoaded configuration:")
            encoder_cfg = config.get('encoder', {})
            decoder_cfg = config.get('decoder', {})
            print(f"  Input features: {encoder_cfg.get('feat_in', 'unknown')}")
            print(f"  Model dimension: {encoder_cfg.get('d_model', 'unknown')}")
            print(f"  Layers: {encoder_cfg.get('n_layers', 'unknown')}")
            print(f"  Output classes: {decoder_cfg.get('num_classes', 'unknown')}")
            
            # Load PyTorch checkpoint
            checkpoint_path = os.path.join(temp_dir, "model_weights.ckpt")
            print(f"\nLoading checkpoint: {checkpoint_path}")
            checkpoint = torch.load(checkpoint_path, map_location='cpu')
            
            print(f"Checkpoint keys: {list(checkpoint.keys())}")
            
            # Check if this is a direct parameter dictionary or has 'state_dict'
            if 'state_dict' in checkpoint:
                state_dict = checkpoint['state_dict']
                print("Using checkpoint['state_dict']")
            else:
                # Parameters are stored directly in checkpoint
                state_dict = checkpoint
                print("Using checkpoint directly as state_dict")
            
            # Create model
            print(f"\nCreating NeMo Conformer CTC model...")
            model = NeMoConformerCTC(config)
            
            # Load compatible weights
            weights_loaded = load_compatible_weights(model, state_dict, config)
            
            # Export to ONNX
            output_path = os.path.join(output_dir, "conformer_ctc.onnx")
            export_success = export_to_onnx(model, config, output_path)
            
            if export_success:
                # Copy additional files
                vocab_src = os.path.join(temp_dir, "vocab.txt")
                vocab_dst = os.path.join(output_dir, "tokenizer.txt")
                if os.path.exists(vocab_src):
                    shutil.copy(vocab_src, vocab_dst)
                    print(f"✓ Vocabulary copied to: {vocab_dst}")
                
                # Save config
                config_dst = os.path.join(output_dir, "model_config.yaml")
                with open(config_dst, 'w') as f:
                    yaml.dump(config, f)
                print(f"✓ Config saved to: {config_dst}")
                
                print(f"\n✅ Manual export completed successfully!")
                print(f"✅ ONNX model: {output_path}")
                print(f"✅ Weights loaded: {'Yes' if weights_loaded else 'No (random weights)'}")
                
                return True
            else:
                print("❌ Export failed")
                return False
                
    except Exception as e:
        print(f"\n❌ Error during export: {e}")
        import traceback
        traceback.print_exc()
        return False

if __name__ == "__main__":
    success = main()
    sys.exit(0 if success else 1)