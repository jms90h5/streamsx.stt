#!/usr/bin/env python3
"""
Official NVIDIA method for exporting cache-aware streaming FastConformer
Based on: https://docs.nvidia.com/nemo-framework/user-guide/latest/nemotoolkit/asr/models.html#cache-aware-streaming-conformer
"""
import nemo.collections.asr as nemo_asr
import os

def export_cache_aware_model():
    """Export using NVIDIA's official method"""
    
    # Path to our fresh model
    model_path = "models/nemo_fresh_export/fresh_fastconformer.nemo"
    output_path = "models/nemo_fresh_export/fastconformer_cache_official.onnx"
    
    if not os.path.exists(model_path):
        print(f"❌ Model not found: {model_path}")
        print("Run download_and_export_fresh.py first")
        return False
    
    print("=== Official NVIDIA Cache-Aware Export ===")
    print(f"Loading model: {model_path}")
    
    # Load the model
    model = nemo_asr.models.EncDecHybridRNNTCTCBPEModel.restore_from(model_path)
    
    print("✓ Model loaded successfully")
    print(f"Model type: {type(model).__name__}")
    
    # CRITICAL: Set cache support before export (from NVIDIA docs)
    print("Setting cache support configuration...")
    model.set_export_config({'cache_support': 'True'})
    print("✓ Cache support enabled")
    
    # Export to ONNX
    print(f"Exporting to: {output_path}")
    os.makedirs(os.path.dirname(output_path), exist_ok=True)
    
    try:
        model.export(output_path)
        print("✅ Export completed successfully!")
        
        # Verify the export
        if os.path.exists(output_path):
            size_mb = os.path.getsize(output_path) / (1024 * 1024)
            print(f"✓ ONNX model created: {size_mb:.1f} MB")
            return True
        else:
            print("❌ Export failed - file not created")
            return False
            
    except Exception as e:
        print(f"❌ Export failed: {e}")
        return False

if __name__ == "__main__":
    success = export_cache_aware_model()
    if success:
        print("\n🎯 Next step: Verify the model has cache tensors:")
        print("python3 test_current_model.py")
    else:
        print("\n❌ Export failed. Check the error messages above.")