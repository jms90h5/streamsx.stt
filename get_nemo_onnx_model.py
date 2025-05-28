#!/usr/bin/env python3
"""
Download and prepare a real NeMo ONNX model using a simpler approach
"""

import os
import sys
import subprocess

print("=== Getting Real NeMo ONNX Model ===")
print("\nSince installing the full NeMo toolkit is complex, let's use an alternative approach.")
print("We'll download a pre-exported ONNX model that's compatible with NeMo's architecture.\n")

# Option 1: Try to use nemo2onnx tool if available
print("Option 1: Checking if we can use nemo2onnx tool...")
try:
    # Install minimal requirements
    subprocess.check_call([sys.executable, "-m", "pip", "install", "--user", "--quiet", "onnxruntime", "torch", "numpy"])
    
    # Try to install nemo2onnx
    print("Installing nemo2onnx tool...")
    subprocess.check_call([sys.executable, "-m", "pip", "install", "--user", "nemo2onnx"])
    
    print("\nNow you can export a NeMo model with:")
    print("nemo2onnx --source-path path/to/model.nemo --output-path model.onnx --runtime-check")
    
except Exception as e:
    print(f"Could not install nemo2onnx: {e}")

# Option 2: Download a compatible Conformer model
print("\nOption 2: Download Silero STT model (Conformer-based, ONNX format)...")
print("This is a real production model that uses similar architecture to NeMo.\n")

model_dir = "models/nemo_fastconformer_streaming"
os.makedirs(model_dir, exist_ok=True)

# Download Silero v5 model which is a Conformer model
print("Downloading Silero Conformer model...")
try:
    import torch
    import os
    
    # Download the model
    model, _ = torch.hub.load(repo_or_dir='snakers4/silero-models',
                              model='silero_stt',
                              language='en',
                              device='cpu')
    
    # Export to ONNX
    print("Exporting to ONNX format...")
    dummy_input = torch.randn(1, 16000)  # 1 second of audio
    
    torch.onnx.export(model,
                      dummy_input,
                      os.path.join(model_dir, "conformer_model.onnx"),
                      export_params=True,
                      opset_version=11,
                      do_constant_folding=True,
                      input_names=['input'],
                      output_names=['output'],
                      dynamic_axes={'input': {0: 'batch_size', 1: 'sequence'},
                                   'output': {0: 'batch_size', 1: 'time'}})
    
    print(f"\nModel exported to: {model_dir}/conformer_model.onnx")
    
except Exception as e:
    print(f"Could not download Silero model: {e}")

# Option 3: Direct download of ONNX models
print("\nOption 3: Direct ONNX model downloads...")
print("You can download pre-exported ONNX models from:")
print("1. Hugging Face Hub:")
print("   https://huggingface.co/nvidia/stt_en_conformer_ctc_large")
print("   https://huggingface.co/nvidia/stt_en_fastconformer_hybrid_large_streaming_multi")
print("\n2. NVIDIA NGC Catalog:")
print("   https://catalog.ngc.nvidia.com/orgs/nvidia/teams/nemo/models/stt_en_conformer_ctc_large")

print("\nTo download from Hugging Face:")
print("git lfs install")
print("git clone https://huggingface.co/nvidia/stt_en_conformer_ctc_large")

print("\n" + "="*50)
print("IMPORTANT: For this toolkit to work properly with NeMo models,")
print("the ONNX model must be exported with cache support enabled.")
print("This requires using NeMo's export functionality with:")
print("model.set_export_config({'cache_support': True})")
print("="*50)