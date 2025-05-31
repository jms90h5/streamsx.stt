#!/usr/bin/env python3
"""
Check and fix pytorch-lightning dependency, then export with cache support
"""
import subprocess
import sys
import os

def check_and_fix_dependencies():
    """Check what we have and fix if needed"""
    
    print("=== Checking Current Versions ===")
    try:
        import pytorch_lightning
        print(f"PyTorch Lightning: {pytorch_lightning.__version__}")
    except ImportError:
        print("PyTorch Lightning: NOT INSTALLED")
        return False
    
    try:
        import nemo
        print(f"NeMo: {nemo.__version__}")
    except ImportError:
        print("NeMo: NOT INSTALLED")
        return False
    
    # Check if combined_loader exists
    try:
        from pytorch_lightning.utilities.combined_loader import CombinedLoader
        print("✓ CombinedLoader: Available")
        return True
    except ImportError:
        print("❌ CombinedLoader: Missing")
        
        # Try to install compatible version
        print("\n=== Installing compatible PyTorch Lightning ===")
        try:
            subprocess.check_call([sys.executable, "-m", "pip", "install", "pytorch-lightning==1.9.5", "--user"])
            print("✓ Installed pytorch-lightning 1.9.5")
            
            # Test again
            from pytorch_lightning.utilities.combined_loader import CombinedLoader
            print("✓ CombinedLoader now available")
            return True
        except Exception as e:
            print(f"❌ Failed to install: {e}")
            return False

def export_with_cache():
    """Export the model with cache support"""
    
    import nemo.collections.asr as nemo_asr
    
    model_path = "models/nemo_fresh_export/fresh_fastconformer.nemo"
    output_path = "models/nemo_fresh_export/fastconformer_cache_working.onnx"
    
    if not os.path.exists(model_path):
        print(f"❌ Model not found: {model_path}")
        return False
    
    print("\n=== Exporting with Cache Support ===")
    print(f"Loading model: {model_path}")
    
    # Load the model
    model = nemo_asr.models.EncDecHybridRNNTCTCBPEModel.restore_from(model_path)
    print("✓ Model loaded")
    
    # Set cache support (CRITICAL)
    print("Setting cache support...")
    model.set_export_config({'cache_support': 'True'})
    print("✓ Cache support enabled")
    
    # Export
    print(f"Exporting to: {output_path}")
    os.makedirs(os.path.dirname(output_path), exist_ok=True)
    
    try:
        model.export(output_path)
        
        if os.path.exists(output_path):
            size_mb = os.path.getsize(output_path) / (1024 * 1024)
            print(f"✅ SUCCESS! ONNX model created: {size_mb:.1f} MB")
            print(f"✅ Model location: {output_path}")
            return True
        else:
            print("❌ Export failed - no file created")
            return False
            
    except Exception as e:
        print(f"❌ Export failed: {e}")
        return False

if __name__ == "__main__":
    print("=== Fix Dependencies and Export NeMo Cache Model ===\n")
    
    # Fix dependencies first
    if not check_and_fix_dependencies():
        print("\n❌ Could not fix dependencies")
        sys.exit(1)
    
    # Export with cache support
    if export_with_cache():
        print("\n🎯 SUCCESS! Next step:")
        print("python3 test_current_model.py")
    else:
        print("\n❌ Export failed despite dependency fix")