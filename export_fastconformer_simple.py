#!/usr/bin/env python3
"""
Simple FastConformer ONNX export that avoids TTS module conflicts
"""
import os
import sys

def export_fastconformer():
    """Export the FastConformer model to ONNX with minimal imports"""
    try:
        print("=== Simple FastConformer ONNX Export ===")
        
        # Import only what we need for ASR
        import torch
        import nemo.collections.asr as nemo_asr
        
        # Path to the real FastConformer model
        model_path = "models/nemo_fastconformer_direct/stt_en_fastconformer_hybrid_large_streaming_multi.nemo"
        
        if not os.path.exists(model_path):
            print(f"✗ Model not found at {model_path}")
            return False
            
        print(f"Loading FastConformer model from {model_path}...")
        
        # Load the model - this is the 114M parameter hybrid model
        model = nemo_asr.models.EncDecHybridRNNTCTCBPEModel.restore_from(model_path)
        print("✓ FastConformer model loaded successfully")
        print(f"  Model type: {type(model).__name__}")
        print(f"  Encoder layers: {model.encoder.num_layers if hasattr(model.encoder, 'num_layers') else 'Unknown'}")
        
        # Set to eval mode
        model.eval()
        
        # Create output directory
        output_dir = "models/fastconformer_onnx"
        os.makedirs(output_dir, exist_ok=True)
        
        print("Attempting direct ONNX export...")
        
        # Try to export just the encoder part to avoid issues
        encoder_path = os.path.join(output_dir, "fastconformer_encoder.onnx")
        
        try:
            # Method 1: Direct export
            with torch.no_grad():
                model.export(encoder_path, check_trace=False)
            print(f"✓ Direct export successful: {encoder_path}")
            
        except Exception as e1:
            print(f"⚠ Direct export failed: {e1}")
            
            try:
                # Method 2: Export with specific configuration
                print("Trying alternative export method...")
                
                # Get the encoder specifically
                encoder = model.encoder
                decoder = model.decoder if hasattr(model, 'decoder') else None
                
                # Export encoder separately
                dummy_input = torch.randn(1, 80, 100)  # batch, features, time
                torch.onnx.export(
                    encoder,
                    dummy_input,
                    encoder_path,
                    export_params=True,
                    opset_version=11,
                    do_constant_folding=True,
                    input_names=['audio_features'],
                    output_names=['encoded_features'],
                    dynamic_axes={
                        'audio_features': {2: 'time'},
                        'encoded_features': {1: 'time'}
                    }
                )
                print(f"✓ Alternative export successful: {encoder_path}")
                
            except Exception as e2:
                print(f"✗ Alternative export also failed: {e2}")
                return False
        
        # Extract and save vocabulary/tokenizer
        print("Extracting vocabulary...")
        vocab_path = os.path.join(output_dir, "vocab.txt")
        
        if hasattr(model, 'tokenizer'):
            try:
                # Save vocabulary from tokenizer
                with open(vocab_path, 'w') as f:
                    if hasattr(model.tokenizer, 'vocab'):
                        for token in model.tokenizer.vocab:
                            f.write(f"{token}\n")
                    elif hasattr(model.tokenizer, 'vocab_size'):
                        # For SentencePiece tokenizers
                        for i in range(model.tokenizer.vocab_size):
                            token = model.tokenizer.ids_to_tokens([i])[0]
                            f.write(f"{token}\n")
                print(f"✓ Vocabulary saved: {vocab_path}")
            except Exception as e:
                print(f"⚠ Could not extract vocabulary: {e}")
        
        # Create a simple CMVN file if needed
        cmvn_path = os.path.join(output_dir, "global_cmvn.stats")
        if not os.path.exists(cmvn_path):
            with open(cmvn_path, 'w') as f:
                # Create dummy CMVN stats - in practice these should be computed from training data
                f.write("80\n")  # Number of features
                f.write(" ".join(["0.0"] * 80) + "\n")  # Mean
                f.write(" ".join(["1.0"] * 80) + "\n")  # Variance
            print(f"✓ CMVN stats created: {cmvn_path}")
        
        # Save model configuration
        config_path = os.path.join(output_dir, "model_config.yaml")
        try:
            import yaml
            with open(config_path, 'w') as f:
                yaml.dump(model._cfg, f, default_flow_style=False)
            print(f"✓ Model config saved: {config_path}")
        except:
            print("⚠ Could not save model config")
        
        print(f"\n✓ FastConformer export complete!")
        print(f"  Output directory: {output_dir}")
        print(f"  Encoder ONNX: {encoder_path}")
        print(f"  Vocabulary: {vocab_path}")
        print(f"  CMVN stats: {cmvn_path}")
        
        return True
        
    except Exception as e:
        print(f"✗ Export failed: {e}")
        import traceback
        traceback.print_exc()
        return False

if __name__ == "__main__":
    success = export_fastconformer()
    if not success:
        sys.exit(1)