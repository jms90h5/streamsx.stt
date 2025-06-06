#!/usr/bin/env python3
"""
Analyze the exported NeMo ONNX models to understand their structure
"""
import onnx

def analyze_model(model_path, model_name):
    print(f"\n=== {model_name} ===")
    print(f"Path: {model_path}")
    
    try:
        model = onnx.load(model_path)
        
        print(f"Inputs ({len(model.graph.input)}):")
        for i, input_tensor in enumerate(model.graph.input):
            print(f"  {i}: {input_tensor.name}")
            print(f"     Type: {input_tensor.type.tensor_type.elem_type}")
            if input_tensor.type.tensor_type.shape.dim:
                shape = [dim.dim_value if dim.dim_value > 0 else f"dim_{dim.dim_param}" 
                        for dim in input_tensor.type.tensor_type.shape.dim]
                print(f"     Shape: {shape}")
            
        print(f"Outputs ({len(model.graph.output)}):")
        for i, output_tensor in enumerate(model.graph.output):
            print(f"  {i}: {output_tensor.name}")
            print(f"     Type: {output_tensor.type.tensor_type.elem_type}")
            if output_tensor.type.tensor_type.shape.dim:
                shape = [dim.dim_value if dim.dim_value > 0 else f"dim_{dim.dim_param}" 
                        for dim in output_tensor.type.tensor_type.shape.dim]
                print(f"     Shape: {shape}")
                
    except Exception as e:
        print(f"Error analyzing {model_name}: {e}")

if __name__ == "__main__":
    # Analyze our exported NeMo models
    analyze_model("models/proven_onnx_export/encoder-proven_fastconformer.onnx", "NeMo Encoder")
    analyze_model("models/proven_onnx_export/decoder_joint-proven_fastconformer.onnx", "NeMo Decoder")
    
    print("\n=== Analysis Complete ===")
    print("This will help understand the correct tensor shapes and data types.")