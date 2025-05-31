#!/usr/bin/env python3
"""
Final working export - fix import then export with cache support
"""
import os

# Fix the import issue BEFORE importing NeMo
print("=== Fixing pytorch_lightning import ===")
from pytorch_lightning.utilities.data import CombinedLoader
import pytorch_lightning.utilities

# Create the module that NeMo expects
class CombinedLoaderModule:
    CombinedLoader = CombinedLoader

pytorch_lightning.utilities.combined_loader = CombinedLoaderModule()
print("✓ Fixed pytorch_lightning.utilities.combined_loader")

# Now import NeMo (after the fix is in place)
import nemo.collections.asr as nemo_asr

def export_with_cache():
    """Export the model with cache support"""
    
    model_path = "models/nemo_fresh_export/fresh_fastconformer.nemo"
    output_path = "models/nemo_fresh_export/fastconformer_cache_final.onnx"
    
    if not os.path.exists(model_path):
        print(f"❌ Model not found: {model_path}")
        return False
    
    print("=== Exporting NeMo FastConformer with Cache Support ===")
    print(f"Loading model: {model_path}")
    
    # Load the model
    model = nemo_asr.models.EncDecHybridRNNTCTCBPEModel.restore_from(model_path)
    print("✓ Model loaded successfully")
    print(f"Model type: {type(model).__name__}")
    
    # CRITICAL: Set cache support (from NVIDIA documentation)
    print("Setting cache support configuration...")
    model.set_export_config({'cache_support': 'True'})
    print("✓ Cache support enabled")
    
    # Export to ONNX
    print(f"Exporting to: {output_path}")
    os.makedirs(os.path.dirname(output_path), exist_ok=True)
    
    try:
        model.export(output_path)
        
        # Check for either combined model or separate components
        output_dir = os.path.dirname(output_path)
        base_name = os.path.splitext(os.path.basename(output_path))[0]
        
        # Look for the standard NeMo export patterns
        encoder_file = os.path.join(output_dir, f"encoder-{base_name}.onnx")
        decoder_file = os.path.join(output_dir, f"decoder_joint-{base_name}.onnx")
        combined_file = output_path
        
        if os.path.exists(combined_file):
            # Single combined model (rare for hybrid models)
            size_mb = os.path.getsize(combined_file) / (1024 * 1024)
            print(f"✅ SUCCESS! Combined ONNX model created: {size_mb:.1f} MB")
            print(f"✅ Model saved to: {combined_file}")
            return True
            
        elif os.path.exists(encoder_file) and os.path.exists(decoder_file):
            # Separate encoder/decoder components (standard for hybrid models)
            encoder_size = os.path.getsize(encoder_file) / (1024 * 1024)
            decoder_size = os.path.getsize(decoder_file) / (1024 * 1024)
            
            print(f"✅ SUCCESS! NeMo exported separate components:")
            print(f"✅ Encoder: {encoder_file} ({encoder_size:.1f} MB)")
            print(f"✅ Decoder: {decoder_file} ({decoder_size:.1f} MB)")
            
            # Verify the encoder has cache tensors (most important for streaming)
            try:
                import onnx
                encoder_model = onnx.load(encoder_file)
                input_names = [i.name for i in encoder_model.graph.input]
                output_names = [o.name for o in encoder_model.graph.output]
                
                print(f"\nEncoder verification:")
                print(f"✓ Inputs ({len(input_names)}): {input_names}")
                print(f"✓ Outputs ({len(output_names)}): {output_names}")
                
                # Check for cache tensors
                cache_inputs = [name for name in input_names if 'cache' in name.lower()]
                cache_outputs = [name for name in output_names if 'cache' in name.lower()]
                
                if cache_inputs and cache_outputs:
                    print(f"🎯 CACHE TENSORS CONFIRMED IN ENCODER!")
                    print(f"  Cache inputs: {cache_inputs}")
                    print(f"  Cache outputs: {cache_outputs}")
                    print(f"✅ Streaming model export successful!")
                else:
                    print(f"⚠️  No cache tensors found in encoder")
                    
            except Exception as e:
                print(f"Note: Could not verify encoder structure: {e}")
            
            return True
        else:
            print("❌ Export failed - no model files created")
            print(f"Expected either: {combined_file}")
            print(f"Or both: {encoder_file} AND {decoder_file}")
            return False
            
    except Exception as e:
        print(f"❌ Export failed: {e}")
        import traceback
        traceback.print_exc()
        return False

if __name__ == "__main__":
    success = export_with_cache()
    
    if success:
        print("\n🎉 EXPORT COMPLETED SUCCESSFULLY!")
        print("\nNext steps:")
        print("1. Test the exported model: python3 test_current_model.py") 
        print("2. Update C++ code to use the new model")
        print("3. Test streaming with cache support")
    else:
        print("\n❌ Export failed")