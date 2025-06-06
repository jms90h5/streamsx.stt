#!/usr/bin/env python3
"""
Extract NeMo model files manually and prepare for ONNX conversion
"""
import os
import tarfile
import yaml
import json
import shutil

def extract_nemo_model():
    """Extract .nemo model archive manually"""
    # Path to downloaded model
    model_path = "models/nemo_cache_aware_conformer/models--nvidia--stt_en_fastconformer_hybrid_large_streaming_multi/snapshots/ae98143333690bd7ced4bc8ec16769bcb8918374/stt_en_fastconformer_hybrid_large_streaming_multi.nemo"
    
    if not os.path.exists(model_path):
        print(f"✗ Model not found at {model_path}")
        return False
        
    # Create extraction directory
    extract_dir = "models/nemo_extracted"
    os.makedirs(extract_dir, exist_ok=True)
    
    print(f"Extracting {model_path}...")
    
    try:
        # Extract .nemo file (it's a tar archive)
        with tarfile.open(model_path, 'r') as tar:
            tar.extractall(extract_dir)
        
        print(f"✓ Model extracted to {extract_dir}")
        
        # List extracted files
        print("\nExtracted files:")
        for root, dirs, files in os.walk(extract_dir):
            for file in files:
                file_path = os.path.join(root, file)
                rel_path = os.path.relpath(file_path, extract_dir)
                print(f"  {rel_path}")
        
        return True
        
    except Exception as e:
        print(f"✗ Extraction failed: {e}")
        return False

def analyze_model_structure():
    """Analyze the extracted model structure"""
    extract_dir = "models/nemo_extracted"
    
    # Look for key files
    config_file = None
    weights_file = None
    vocab_file = None
    
    for root, dirs, files in os.walk(extract_dir):
        for file in files:
            file_path = os.path.join(root, file)
            if file.endswith('.yaml') or file.endswith('.yml'):
                config_file = file_path
            elif file.endswith('.ckpt') or file.endswith('.pt'):
                weights_file = file_path
            elif 'vocab' in file.lower() or 'token' in file.lower():
                vocab_file = file_path
    
    print(f"\n=== Model Structure Analysis ===")
    if config_file:
        print(f"Config file: {config_file}")
        try:
            with open(config_file, 'r') as f:
                config = yaml.safe_load(f)
            print("Key configuration parameters:")
            if 'model' in config:
                model_config = config['model']
                print(f"  Architecture: {model_config.get('_target_', 'Unknown')}")
                if 'encoder' in model_config:
                    encoder = model_config['encoder']
                    print(f"  Encoder layers: {encoder.get('n_layers', 'Unknown')}")
                    print(f"  Model dimension: {encoder.get('d_model', 'Unknown')}")
                if 'decoder' in model_config:
                    decoder = model_config['decoder']
                    print(f"  Decoder type: {decoder.get('_target_', 'Unknown')}")
        except Exception as e:
            print(f"  Error reading config: {e}")
    
    if weights_file:
        print(f"Weights file: {weights_file}")
        # Get file size
        size_mb = os.path.getsize(weights_file) / (1024 * 1024)
        print(f"  Size: {size_mb:.1f} MB")
    
    if vocab_file:
        print(f"Vocabulary file: {vocab_file}")
        try:
            with open(vocab_file, 'r') as f:
                lines = f.readlines()
            print(f"  Vocabulary size: {len(lines)}")
        except Exception as e:
            print(f"  Error reading vocab: {e}")
    
    return config_file, weights_file, vocab_file

def create_onnx_conversion_script():
    """Create a simplified ONNX conversion script"""
    script_content = '''#!/usr/bin/env python3
"""
Convert extracted NeMo model to ONNX using PyTorch
This script works with extracted model files
"""
import torch
import onnx
import os

def convert_to_onnx():
    """Convert model to ONNX format"""
    weights_file = "models/nemo_extracted/model_weights.ckpt"
    
    if not os.path.exists(weights_file):
        print("Model weights not found. Trying alternative paths...")
        # Look for .pt or .ckpt files
        for root, dirs, files in os.walk("models/nemo_extracted"):
            for file in files:
                if file.endswith('.ckpt') or file.endswith('.pt'):
                    weights_file = os.path.join(root, file)
                    print(f"Found weights: {weights_file}")
                    break
    
    try:
        # Load model state dict
        checkpoint = torch.load(weights_file, map_location='cpu')
        print("✓ Model weights loaded")
        
        # Extract model state if it's wrapped
        if 'state_dict' in checkpoint:
            state_dict = checkpoint['state_dict']
        else:
            state_dict = checkpoint
        
        print(f"Model has {len(state_dict)} parameters")
        
        # List some key parameter names to understand structure
        param_names = list(state_dict.keys())[:10]
        print("Sample parameter names:")
        for name in param_names:
            print(f"  {name}")
        
        print("\\n✓ Model analysis complete")
        print("For ONNX conversion, you'll need to:")
        print("1. Define the model architecture in PyTorch")
        print("2. Load these weights into the model")
        print("3. Export to ONNX using torch.onnx.export()")
        
    except Exception as e:
        print(f"✗ Error loading model: {e}")

if __name__ == "__main__":
    convert_to_onnx()
'''
    
    script_path = "convert_extracted_to_onnx.py"
    with open(script_path, 'w') as f:
        f.write(script_content)
    
    print(f"✓ ONNX conversion script created: {script_path}")

def main():
    print("=== NeMo Model Extraction and Analysis ===")
    
    if extract_nemo_model():
        config_file, weights_file, vocab_file = analyze_model_structure()
        create_onnx_conversion_script()
        
        print(f"\n=== Next Steps ===")
        print("1. Analyze the model architecture from config file")
        print("2. Create PyTorch model definition matching the architecture")
        print("3. Load the extracted weights")
        print("4. Export to ONNX for streaming inference")
        print("\nAlternatively, consider using pre-converted ONNX models from:")
        print("- Hugging Face Model Hub")
        print("- NVIDIA NGC catalog")
    else:
        print("✗ Model extraction failed")

if __name__ == "__main__":
    main()