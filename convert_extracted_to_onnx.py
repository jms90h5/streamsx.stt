#!/usr/bin/env python3
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
        
        print("\n✓ Model analysis complete")
        print("For ONNX conversion, you'll need to:")
        print("1. Define the model architecture in PyTorch")
        print("2. Load these weights into the model")
        print("3. Export to ONNX using torch.onnx.export()")
        
    except Exception as e:
        print(f"✗ Error loading model: {e}")

if __name__ == "__main__":
    convert_to_onnx()
