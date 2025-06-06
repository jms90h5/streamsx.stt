#!/usr/bin/env python3
"""Inspect cache-aware NeMo model dimensions to debug broadcasting error"""

import onnx
import numpy as np

def inspect_model(model_path, model_name):
    print(f"\n=== {model_name} Model Inspection ===")
    model = onnx.load(model_path)
    
    print(f"\nInputs:")
    for i, input_tensor in enumerate(model.graph.input):
        shape = []
        for dim in input_tensor.type.tensor_type.shape.dim:
            if dim.HasField('dim_value'):
                shape.append(dim.dim_value)
            elif dim.HasField('dim_param'):
                shape.append(dim.dim_param)
            else:
                shape.append('?')
        print(f"  {i}: {input_tensor.name} - shape: {shape}")
    
    print(f"\nOutputs:")
    for i, output_tensor in enumerate(model.graph.output):
        shape = []
        for dim in output_tensor.type.tensor_type.shape.dim:
            if dim.HasField('dim_value'):
                shape.append(dim.dim_value)
            elif dim.HasField('dim_param'):
                shape.append(dim.dim_param)
            else:
                shape.append('?')
        print(f"  {i}: {output_tensor.name} - shape: {shape}")
    
    # Look for attention-related nodes
    print(f"\nAttention-related nodes:")
    for node in model.graph.node:
        if 'self_attn' in node.name or 'Where' in node.name:
            print(f"  Node: {node.name}")
            print(f"    Op type: {node.op_type}")
            print(f"    Inputs: {list(node.input)}")
            print(f"    Outputs: {list(node.output)}")
            
            # Get attributes
            if node.attribute:
                print("    Attributes:")
                for attr in node.attribute:
                    print(f"      {attr.name}: {attr}")

def check_chunk_calculations():
    """Calculate expected dimensions based on chunk configuration"""
    print("\n=== Chunk Size Calculations ===")
    
    # Based on the configuration
    chunk_frames = 160  # 1.6 seconds at 10ms frame shift
    feature_dim = 80
    downsample_factor = 4  # Conformer downsampling
    
    # After downsampling
    downsampled_frames = chunk_frames // downsample_factor
    print(f"Input chunk: {chunk_frames} frames ({chunk_frames * 10}ms)")
    print(f"After 4x downsampling: {downsampled_frames} frames")
    
    # Attention context calculations
    att_context_left = 70
    att_context_right = 0  # Zero look-ahead mode
    
    total_context = att_context_left + downsampled_frames + att_context_right
    print(f"\nAttention context:")
    print(f"  Left context: {att_context_left}")
    print(f"  Chunk size (downsampled): {downsampled_frames}")
    print(f"  Right context: {att_context_right}")
    print(f"  Total attention size: {total_context}")
    
    # The error shows 83 vs 89
    print(f"\nError analysis:")
    print(f"  Expected: 83 (possibly {att_context_left} + some frames)")
    print(f"  Actual: 89 (possibly {att_context_left} + different frames)")
    print(f"  Difference: 6 frames")
    
    # Possible explanations
    print(f"\nPossible issues:")
    print(f"  1. Padding mismatch: Input might need specific padding")
    print(f"  2. Cache initialization: Cache might have wrong initial size")
    print(f"  3. Attention mask calculation: Mask dimensions might be off")

if __name__ == "__main__":
    # Inspect encoder
    inspect_model("models/nemo_fresh_export/encoder-fastconformer_cache_final.onnx", "Encoder")
    
    # Inspect decoder
    inspect_model("models/nemo_fresh_export/decoder_joint-fastconformer_cache_final.onnx", "Decoder")
    
    # Calculate expected dimensions
    check_chunk_calculations()