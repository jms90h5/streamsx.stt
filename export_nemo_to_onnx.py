#!/usr/bin/env python3
"""
Export NeMo FastConformer model to ONNX format with cache support
for use with TeraCloud Streams STT toolkit.

Requirements:
    pip install nemo_toolkit[all]
    pip install torch onnx onnxruntime
"""

import os
import sys
import argparse

def export_nemo_to_onnx(model_name, output_path, enable_cache=True):
    """Export NeMo model to ONNX format"""
    try:
        import nemo.collections.asr as nemo_asr
        print(f"Loading NeMo model: {model_name}")
        
        # Load the pre-trained model
        model = nemo_asr.models.EncDecRNNTBPEModel.from_pretrained(model_name)
        
        # Configure for cache-aware streaming if requested
        if enable_cache:
            print("Configuring model for cache-aware streaming...")
            model.set_export_config({'cache_support': True})
        
        # Export to ONNX
        print(f"Exporting to: {output_path}")
        model.export(output_path)
        
        print("Export completed successfully!")
        print(f"Model saved to: {output_path}")
        
        # Verify the exported model
        try:
            import onnx
            onnx_model = onnx.load(output_path)
            print(f"ONNX model verification: OK")
            print(f"Model inputs: {len(onnx_model.graph.input)}")
            print(f"Model outputs: {len(onnx_model.graph.output)}")
            
            for i, input_tensor in enumerate(onnx_model.graph.input):
                print(f"  Input {i}: {input_tensor.name}")
            for i, output_tensor in enumerate(onnx_model.graph.output):
                print(f"  Output {i}: {output_tensor.name}")
                
        except ImportError:
            print("Warning: onnx package not available for verification")
        
        return True
        
    except ImportError as e:
        print(f"Error: NeMo toolkit not available: {e}")
        print("Please install with: pip install nemo_toolkit[all]")
        return False
    except Exception as e:
        print(f"Error exporting model: {e}")
        return False

def main():
    parser = argparse.ArgumentParser(description="Export NeMo model to ONNX")
    parser.add_argument("--model", default="nvidia/stt_en_fastconformer_hybrid_large_streaming_multi",
                       help="NeMo model name")
    parser.add_argument("--output", default="models/nemo_fastconformer_streaming/fastconformer_streaming.onnx",
                       help="Output ONNX file path")
    parser.add_argument("--no-cache", action="store_true", 
                       help="Disable cache support (not recommended for streaming)")
    
    args = parser.parse_args()
    
    # Create output directory
    os.makedirs(os.path.dirname(args.output), exist_ok=True)
    
    # Export the model
    success = export_nemo_to_onnx(
        model_name=args.model,
        output_path=args.output,
        enable_cache=not args.no_cache
    )
    
    if success:
        print("\n=== Next Steps ===")
        print(f"1. Test the model with the STT toolkit:")
        print(f"   cd samples/CppONNX_OnnxSTT")
        print(f"   ./output/bin/standalone --nemo-model {args.output}")
        print(f"2. Integrate with SPL applications")
        sys.exit(0)
    else:
        sys.exit(1)

if __name__ == "__main__":
    main()
