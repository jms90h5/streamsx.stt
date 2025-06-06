#!/usr/bin/env python3
"""
Inspect ONNX model metadata without loading it into memory to understand
the exact tensor requirements for FastConformer cache-aware streaming.
"""

import onnx
import sys

def inspect_onnx_model(model_path):
    """Inspect ONNX model without loading weights"""
    print(f"=== Inspecting ONNX Model: {model_path} ===")
    
    # Load model metadata only
    model = onnx.load(model_path)
    graph = model.graph
    
    print(f"\nModel name: {graph.name}")
    print(f"IR version: {model.ir_version}")
    print(f"Producer: {model.producer_name}")
    
    # Input specifications
    print(f"\n=== INPUTS ({len(graph.input)}) ===")
    for i, inp in enumerate(graph.input):
        print(f"Input {i}: {inp.name}")
        shape = [d.dim_value if d.dim_value > 0 else d.dim_param for d in inp.type.tensor_type.shape.dim]
        print(f"  Shape: {shape}")
        print(f"  Type: {inp.type.tensor_type.elem_type}")
        print()
    
    # Output specifications  
    print(f"=== OUTPUTS ({len(graph.output)}) ===")
    for i, out in enumerate(graph.output):
        print(f"Output {i}: {out.name}")
        shape = [d.dim_value if d.dim_value > 0 else d.dim_param for d in out.type.tensor_type.shape.dim]
        print(f"  Shape: {shape}")
        print(f"  Type: {out.type.tensor_type.elem_type}")
        print()
    
    # Look for important nodes that might give us clues about dimensions
    print(f"=== KEY NODES ===")
    attention_nodes = []
    reshape_nodes = []
    
    for node in graph.node:
        if 'attn' in node.name.lower() or 'attention' in node.name.lower():
            attention_nodes.append(node.name)
        elif 'reshape' in node.name.lower():
            reshape_nodes.append(node.name)
    
    print(f"Attention nodes: {len(attention_nodes)}")
    for node in attention_nodes[:5]:  # Show first 5
        print(f"  {node}")
    
    print(f"Reshape nodes: {len(reshape_nodes)}")
    for node in reshape_nodes[:5]:  # Show first 5
        print(f"  {node}")
    
    # Look for constants that might indicate expected dimensions
    print(f"\n=== CONSTANTS (showing first 10) ===")
    constants = []
    for init in graph.initializer:
        if len(init.dims) > 0:
            constants.append((init.name, list(init.dims)))
    
    for name, dims in constants[:10]:
        print(f"  {name}: {dims}")
    
    print(f"\nTotal constants: {len(constants)}")

if __name__ == "__main__":
    encoder_model = "models/nemo_fresh_export/encoder-fastconformer_cache_final.onnx"
    decoder_model = "models/nemo_fresh_export/decoder_joint-fastconformer_cache_final.onnx"
    
    try:
        inspect_onnx_model(encoder_model)
        print("\n" + "="*80 + "\n")
        inspect_onnx_model(decoder_model)
        
    except Exception as e:
        print(f"Error: {e}")
        sys.exit(1)