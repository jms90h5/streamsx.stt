#!/usr/bin/env python3
"""
Test the fixed NeMo model export
"""

import os
import numpy as np

def test_exported_models():
    """Test all exported ONNX models"""
    
    print("=== Testing Fixed NeMo Models ===\n")
    
    output_dir = "models/nemo_cache_fixed"
    
    if not os.path.exists(output_dir):
        print(f"❌ Output directory not found: {output_dir}")
        print("Run fix_nemo_export_final.py first")
        return
    
    # Find ONNX files
    onnx_files = [f for f in os.listdir(output_dir) if f.endswith('.onnx')]
    
    if not onnx_files:
        print(f"❌ No ONNX files found in {output_dir}")
        return
    
    try:
        import onnxruntime as ort
        
        for onnx_file in onnx_files:
            model_path = os.path.join(output_dir, onnx_file)
            print(f"Testing: {onnx_file}")
            
            try:
                # Load model
                session = ort.InferenceSession(model_path)
                
                # Check inputs/outputs
                inputs = session.get_inputs()
                outputs = session.get_outputs()
                
                print(f"  ✓ Model loaded successfully")
                print(f"  Inputs ({len(inputs)}):")
                for inp in inputs:
                    shape_str = str(inp.shape).replace("'", "")
                    print(f"    - {inp.name}: {shape_str}")
                
                print(f"  Outputs ({len(outputs)}):")
                for out in outputs:
                    shape_str = str(out.shape).replace("'", "")
                    print(f"    - {out.name}: {shape_str}")
                
                # Check for cache tensors
                has_cache_input = any('cache' in inp.name for inp in inputs)
                has_cache_output = any('cache' in out.name for out in outputs)
                
                print(f"  Cache inputs: {'✅ Yes' if has_cache_input else '❌ No'}")
                print(f"  Cache outputs: {'✅ Yes' if has_cache_output else '❌ No'}")
                
                # Try a simple inference test
                print("  Testing inference...")
                
                # Prepare inputs
                test_inputs = {}
                
                for inp in inputs:
                    if 'audio' in inp.name.lower():
                        # Audio signal: [batch, time, features]
                        test_inputs[inp.name] = np.random.randn(1, 160, 80).astype(np.float32)
                    elif 'length' in inp.name.lower():
                        # Length tensor
                        test_inputs[inp.name] = np.array([160], dtype=np.int32)
                    elif 'cache' in inp.name.lower():
                        # Cache tensor: [batch, layers, hidden]
                        test_inputs[inp.name] = np.zeros((1, 17, 512), dtype=np.float32)
                    else:
                        # Default shape based on input shape
                        shape = []
                        for dim in inp.shape:
                            if isinstance(dim, str) or dim == -1:
                                shape.append(1)  # Dynamic dims become 1
                            else:
                                shape.append(dim)
                        test_inputs[inp.name] = np.random.randn(*shape).astype(np.float32)
                
                # Run inference
                try:
                    outputs_result = session.run(None, test_inputs)
                    print(f"  ✅ Inference successful!")
                    
                    # Check output shapes
                    for i, (out, result) in enumerate(zip(outputs, outputs_result)):
                        print(f"    Output {out.name}: {result.shape}")
                        
                        # For log_probs/encoded, check if it's more than just one repeated value
                        if 'log' in out.name.lower() or 'encoded' in out.name.lower():
                            if len(result.shape) >= 2:
                                flat = result.flatten()
                                unique_vals = len(np.unique(np.round(flat, 2)))
                                print(f"      Unique values: {unique_vals} (good if > 10)")
                    
                except Exception as e:
                    print(f"  ❌ Inference failed: {e}")
                
            except Exception as e:
                print(f"  ❌ Failed to load {onnx_file}: {e}")
            
            print()
        
        # Test comparison with original
        print("=== Comparison with Original ===")
        
        original_path = "models/nemo_fastconformer_streaming/conformer_ctc_dynamic.onnx"
        if os.path.exists(original_path):
            try:
                orig_session = ort.InferenceSession(original_path)
                orig_inputs = orig_session.get_inputs()
                
                print(f"Original model inputs: {len(orig_inputs)}")
                for inp in orig_inputs:
                    print(f"  - {inp.name}")
                
                print(f"\nFixed models have cache: {any('cache' in f for f in [inp.name for model_file in onnx_files for inp in ort.InferenceSession(os.path.join(output_dir, model_file)).get_inputs()])}")
                print(f"Original has cache: {any('cache' in inp.name for inp in orig_inputs)}")
                
            except Exception as e:
                print(f"Could not load original for comparison: {e}")
        
    except ImportError:
        print("❌ onnxruntime not available. Install with: pip install onnxruntime")

if __name__ == "__main__":
    test_exported_models()