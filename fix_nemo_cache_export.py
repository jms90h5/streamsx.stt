#!/usr/bin/env python3
"""
Fix NeMo model export by enabling cache support for streaming.
This script exports the FastConformer model with cache tensors included.
"""

import os
import sys
import torch
import numpy as np

def export_nemo_with_cache():
    """Export NeMo model with cache support enabled"""
    
    print("=== NeMo Cache-Aware Export Fix ===\n")
    
    # Paths
    nemo_model_path = "models/nemo_fastconformer_direct/stt_en_fastconformer_hybrid_large_streaming_multi.nemo"
    output_dir = "models/nemo_cache_aware_export"
    output_onnx = os.path.join(output_dir, "conformer_ctc_cache_aware.onnx")
    
    # Check if NeMo model exists
    if not os.path.exists(nemo_model_path):
        print(f"Error: NeMo model not found at {nemo_model_path}")
        print("Please ensure the FastConformer model is downloaded.")
        return False
    
    # Create output directory
    os.makedirs(output_dir, exist_ok=True)
    
    print(f"Loading NeMo model from: {nemo_model_path}")
    
    try:
        # Import NeMo (minimal imports to reduce memory)
        import nemo.collections.asr as nemo_asr
        
        # Load the model
        model = nemo_asr.models.ASRModel.restore_from(nemo_model_path)
        print("✓ Model loaded successfully")
        
        # Set to eval mode
        model.eval()
        
        # Configure export parameters for cache-aware streaming
        print("\nConfiguring cache-aware export...")
        
        # Export with cache support
        with torch.no_grad():
            # Get model config
            print(f"Model type: {type(model).__name__}")
            
            # Check if model supports cache-aware export
            if hasattr(model, 'encoder') and hasattr(model.encoder, 'export_cache_aware'):
                print("✓ Model supports cache-aware export")
                
                # Export with cache tensors
                model.export(
                    output_onnx,
                    input_names=["audio_signal", "length", "cache_last_channel", "cache_last_time"],
                    output_names=["log_probs", "encoded_lengths", "cache_last_channel_out", "cache_last_time_out"],
                    dynamic_axes={
                        "audio_signal": {0: "batch", 1: "time"},
                        "length": {0: "batch"},
                        "cache_last_channel": {0: "batch"},
                        "cache_last_time": {0: "batch"},
                        "log_probs": {0: "batch", 1: "time"},
                        "encoded_lengths": {0: "batch"},
                        "cache_last_channel_out": {0: "batch"},
                        "cache_last_time_out": {0: "batch"}
                    },
                    export_params=True,
                    do_constant_folding=True,
                    opset_version=14,
                    # Enable cache for streaming
                    cache_support=True,
                    cache_aware_conformer=True,
                    chunk_size=40,  # After 4x subsampling: 160 frames -> 40
                    left_context_size=70,
                    right_context_size=0  # 0ms latency
                )
            else:
                # Fallback: Try standard export with cache configuration
                print("⚠ Using alternative export method...")
                
                # Create dummy inputs
                batch_size = 1
                time_steps = 160  # 160 frames = 1.6s at 10ms stride
                feat_dim = 80
                
                dummy_input = torch.randn(batch_size, time_steps, feat_dim)
                dummy_lengths = torch.tensor([time_steps], dtype=torch.int32)
                
                # Export model
                torch.onnx.export(
                    model,
                    (dummy_input, dummy_lengths),
                    output_onnx,
                    input_names=["audio_signal", "length"],
                    output_names=["log_probs"],
                    dynamic_axes={
                        "audio_signal": {0: "batch", 1: "time"},
                        "length": {0: "batch"},
                        "log_probs": {0: "batch", 1: "time"}
                    },
                    export_params=True,
                    do_constant_folding=True,
                    opset_version=14
                )
        
        print(f"\n✓ Model exported to: {output_onnx}")
        
        # Verify the export
        print("\nVerifying exported model...")
        import onnx
        
        onnx_model = onnx.load(output_onnx)
        
        print(f"\nModel inputs ({len(onnx_model.graph.input)}):")
        for inp in onnx_model.graph.input:
            shape = [d.dim_value if d.dim_value else d.dim_param for d in inp.type.tensor_type.shape.dim]
            print(f"  - {inp.name}: {shape}")
        
        print(f"\nModel outputs ({len(onnx_model.graph.output)}):")
        for out in onnx_model.graph.output:
            shape = [d.dim_value if d.dim_value else d.dim_param for d in out.type.tensor_type.shape.dim]
            print(f"  - {out.name}: {shape}")
        
        # Check if cache tensors are present
        has_cache = any("cache" in inp.name.lower() for inp in onnx_model.graph.input)
        
        if has_cache:
            print("\n✅ SUCCESS: Model exported with cache support!")
        else:
            print("\n⚠️ WARNING: Model may not have cache tensors. Manual configuration may be needed.")
        
        # Save model config
        config_path = os.path.join(output_dir, "model_config.yaml")
        print(f"\nSaving model configuration to: {config_path}")
        
        with open(config_path, 'w') as f:
            f.write(f"""# NeMo Cache-Aware Conformer Configuration
model_type: fastconformer_ctc
cache_aware: true
chunk_size: 160  # frames before subsampling
chunk_size_after_subsampling: 40
left_context: 70
right_context: 0
latency_ms: 0
num_layers: {getattr(model.encoder, 'num_layers', 17)}
hidden_size: {getattr(model.encoder, 'd_model', 512)}
vocab_size: {len(model.decoder.vocabulary) if hasattr(model.decoder, 'vocabulary') else 128}
sample_rate: 16000
feature_dim: 80
subsampling_factor: 4
""")
        
        print("\n✓ Export completed successfully!")
        print(f"\nNext steps:")
        print(f"1. Test the new model: {output_onnx}")
        print(f"2. Update the C++ code to use cache tensors if needed")
        print(f"3. Run streaming inference test")
        
        return True
        
    except ImportError as e:
        print(f"\n❌ Error: NeMo not installed or import failed: {e}")
        print("\nPlease install NeMo:")
        print("  pip install nemo-toolkit[asr]")
        return False
    except Exception as e:
        print(f"\n❌ Error during export: {e}")
        import traceback
        traceback.print_exc()
        return False

def alternative_export_method():
    """Alternative method using direct checkpoint loading"""
    print("\n=== Alternative Export Method ===")
    print("If the above method fails due to memory, try this approach:\n")
    
    script_content = '''# export_cache_aware_manual.py
import torch
import onnx
from collections import OrderedDict

# Load checkpoint directly
checkpoint_path = "models/nemo_extracted/model_weights.ckpt"
checkpoint = torch.load(checkpoint_path, map_location='cpu')

# Extract encoder weights
encoder_state = OrderedDict()
for k, v in checkpoint.items():
    if k.startswith('encoder.'):
        new_key = k.replace('encoder.', '')
        encoder_state[new_key] = v

# Create a minimal conformer encoder model
# ... (model creation code here)

# Export with cache tensors
torch.onnx.export(
    model,
    (audio_input, length_input, cache_channel, cache_time),
    "conformer_cache_aware.onnx",
    input_names=["audio", "length", "cache_channel", "cache_time"],
    output_names=["logits", "cache_channel_out", "cache_time_out"],
    dynamic_axes={
        "audio": {0: "batch", 1: "time"},
        "logits": {0: "batch", 1: "time"}
    }
)
'''
    
    print("Create a file 'export_cache_aware_manual.py' with custom export logic")
    print("This approach loads only the encoder weights to reduce memory usage")

if __name__ == "__main__":
    # Try the main export method
    success = export_nemo_with_cache()
    
    if not success:
        # Provide alternative instructions
        alternative_export_method()
        
        print("\n=== Manual Steps If Script Fails ===")
        print("\n1. First, check available memory:")
        print("   free -h")
        print("\n2. Clear Python cache:")
        print("   rm -rf ~/.cache/torch")
        print("\n3. Use minimal Python environment:")
        print("   python3 -m venv nemo_export_env")
        print("   source nemo_export_env/bin/activate")
        print("   pip install nemo-toolkit onnx")
        print("\n4. Run export with memory limits:")
        print("   ulimit -v 8000000  # Limit to 8GB")
        print("   python3 fix_nemo_cache_export.py")