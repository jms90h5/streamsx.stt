#!/usr/bin/env python3
"""
Deep analysis of the ONNX model to find issues
"""

try:
    import onnx
    import numpy as np
    
    model_path = "models/nemo_fastconformer_streaming/conformer_ctc_dynamic.onnx"
    print(f"=== Analyzing {model_path} ===")
    
    # Load model
    model = onnx.load(model_path)
    
    # Check model validity
    try:
        onnx.checker.check_model(model)
        print("✅ Model passes ONNX validation")
    except Exception as e:
        print(f"❌ Model validation failed: {e}")
    
    # Analyze graph structure
    print(f"\nGraph info:")
    print(f"  Nodes: {len(model.graph.node)}")
    print(f"  Initializers: {len(model.graph.initializer)}")
    print(f"  Value infos: {len(model.graph.value_info)}")
    
    # Check if model has any parameters/weights
    total_params = 0
    for init in model.graph.initializer:
        shape = [d for d in init.dims]
        params = np.prod(shape) if shape else 0
        total_params += params
        if len(shape) > 1:  # Only print larger tensors
            print(f"  Weight: {init.name} shape={shape} ({params:,} params)")
    
    print(f"\nTotal parameters: {total_params:,}")
    
    if total_params == 0:
        print("❌ WARNING: Model has no parameters! This could be the issue.")
    
    # Check for obvious problems
    print(f"\n=== Potential Issues ===")
    
    # Look for constant nodes that might indicate a broken export
    constant_nodes = [n for n in model.graph.node if n.op_type == "Constant"]
    print(f"Constant nodes: {len(constant_nodes)}")
    
    # Look for identity nodes that might indicate pass-through
    identity_nodes = [n for n in model.graph.node if n.op_type == "Identity"]
    print(f"Identity nodes: {len(identity_nodes)}")
    
    # Check if output is directly connected to a constant
    output_name = model.graph.output[0].name
    output_producers = [n for n in model.graph.node if output_name in n.output]
    if output_producers:
        producer = output_producers[0]
        print(f"Output producer: {producer.op_type}")
        if producer.op_type in ["Constant", "Identity"]:
            print("❌ WARNING: Output is produced by a constant/identity node!")
    
except ImportError:
    print("ONNX not available")
    print("To install: pip install onnx")
except Exception as e:
    print(f"Error analyzing model: {e}")

# Also check file size as a sanity check
import os
if os.path.exists(model_path):
    size_mb = os.path.getsize(model_path) / (1024 * 1024)
    print(f"\nModel file size: {size_mb:.1f} MB")
    if size_mb < 1:
        print("❌ WARNING: Model file is very small, might be incomplete")
    elif size_mb < 10:
        print("⚠️  Model is smaller than expected for a full Conformer model")
    else:
        print("✅ Model size looks reasonable")