#!/usr/bin/env python3
"""
Export the proven working NeMo model to ONNX for C++ implementation
Using the exact same model and configuration that just produced perfect results
"""

import os
import sys

def export_proven_model():
    """Export the working model using standard approach"""
    
    print("=== Exporting Proven Working Model to ONNX ===")
    print("Using exact model that just produced perfect transcription")
    
    try:
        import nemo.collections.asr as nemo_asr
        import torch
        
        # Use the exact same model path that just worked
        model_path = "models/nemo_cache_aware_conformer/models--nvidia--stt_en_fastconformer_hybrid_large_streaming_multi/snapshots/ae98143333690bd7ced4bc8ec16769bcb8918374/stt_en_fastconformer_hybrid_large_streaming_multi.nemo"
        
        print(f"Loading proven working model: {model_path}")
        
        # Load using the exact same method that just worked
        model = nemo_asr.models.EncDecHybridRNNTCTCBPEModel.restore_from(model_path)
        model.eval()
        print("✓ Model loaded successfully (same as proven test)")
        
        # Create output directory
        output_dir = "models/proven_onnx_export"
        os.makedirs(output_dir, exist_ok=True)
        
        # Export using standard approach (not streaming complexity)
        onnx_path = os.path.join(output_dir, "proven_fastconformer.onnx")
        print(f"Exporting to: {onnx_path}")
        print("Using standard export (file-level processing, not streaming)")
        
        try:
            # Standard export without streaming complications
            model.export(onnx_path, check_trace=False)
            print("✅ ONNX export successful!")
            
            # Verify the exported file
            if os.path.exists(onnx_path):
                size_mb = os.path.getsize(onnx_path) / (1024*1024)
                print(f"✓ Exported file size: {size_mb:.1f} MB")
            
            # Try to load and inspect the ONNX model
            try:
                import onnx
                onnx_model = onnx.load(onnx_path)
                
                print(f"\n=== ONNX Model Details ===")
                print(f"Inputs: {len(onnx_model.graph.input)}")
                for i, inp in enumerate(onnx_model.graph.input):
                    print(f"  Input {i}: {inp.name}")
                    if hasattr(inp.type, 'tensor_type') and hasattr(inp.type.tensor_type, 'shape'):
                        dims = [d.dim_value if d.dim_value > 0 else '?' for d in inp.type.tensor_type.shape.dim]
                        print(f"    Shape: {dims}")
                
                print(f"Outputs: {len(onnx_model.graph.output)}")
                for i, out in enumerate(onnx_model.graph.output):
                    print(f"  Output {i}: {out.name}")
                    if hasattr(out.type, 'tensor_type') and hasattr(out.type.tensor_type, 'shape'):
                        dims = [d.dim_value if d.dim_value > 0 else '?' for d in out.type.tensor_type.shape.dim]
                        print(f"    Shape: {dims}")
                
                # Save model info
                info_path = os.path.join(output_dir, "model_info.txt")
                with open(info_path, 'w') as f:
                    f.write(f"Proven Working Model Export Info\n")
                    f.write(f"Source: {model_path}\n")
                    f.write(f"Export: {onnx_path}\n")
                    f.write(f"Size: {size_mb:.1f} MB\n")
                    f.write(f"Inputs: {len(onnx_model.graph.input)}\n")
                    f.write(f"Outputs: {len(onnx_model.graph.output)}\n")
                    f.write(f"Proven result: 'it was the first great sorrow of his life...'\n")
                
                print(f"✓ Model info saved to: {info_path}")
                
            except ImportError:
                print("⚠️  Cannot inspect ONNX model (onnx package not available)")
            except Exception as e:
                print(f"⚠️  Could not inspect ONNX model: {e}")
            
            # Extract vocabulary/tokenizer info
            try:
                vocab_path = os.path.join(output_dir, "vocabulary.txt")
                print(f"Extracting vocabulary to: {vocab_path}")
                
                # Get tokenizer info
                if hasattr(model, 'tokenizer'):
                    tokenizer = model.tokenizer
                    print(f"✓ Tokenizer type: {type(tokenizer)}")
                    
                    # Save vocabulary
                    with open(vocab_path, 'w', encoding='utf-8') as f:
                        if hasattr(tokenizer, 'vocab'):
                            vocab = tokenizer.vocab
                            f.write(f"# Vocabulary from proven working model\n")
                            f.write(f"# Size: {len(vocab)} tokens\n")
                            for token, id in vocab.items():
                                f.write(f"{token}\t{id}\n")
                            print(f"✓ Saved {len(vocab)} vocabulary tokens")
                        else:
                            f.write(f"# Tokenizer info\n")
                            f.write(f"# Type: {type(tokenizer)}\n")
                            f.write(f"# Unable to extract vocabulary directly\n")
                    
            except Exception as e:
                print(f"⚠️  Could not extract vocabulary: {e}")
            
            print(f"\n=== Export Summary ===")
            print(f"✅ ONNX Model: {onnx_path}")
            print(f"✅ Model Info: {info_path}")
            print(f"✅ Vocabulary: {vocab_path}")
            print(f"✅ Source model: PROVEN WORKING (perfect transcription)")
            print(f"📁 All files in: {output_dir}/")
            
            return True
            
        except Exception as e:
            print(f"❌ ONNX export failed: {e}")
            import traceback
            traceback.print_exc()
            return False
        
    except Exception as e:
        print(f"❌ Model loading failed: {e}")
        import traceback
        traceback.print_exc()
        return False

if __name__ == "__main__":
    success = export_proven_model()
    
    if success:
        print("\n🎉 PROVEN MODEL EXPORT SUCCESSFUL!")
        print("📋 Next: Test ONNX model in C++ ONNX Runtime")
        print("📋 Goal: Verify exported model produces same results as Python")
    else:
        print("\n💥 EXPORT FAILED!")
        print("📋 Need to debug export process or try different approach")
        sys.exit(1)