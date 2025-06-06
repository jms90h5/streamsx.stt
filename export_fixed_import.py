#!/usr/bin/env python3
"""
Export NeMo model with cache support, fixing the import issue first
"""
import os
import sys

def fix_pytorch_lightning_import():
    """Fix the import path that NeMo is looking for"""
    
    try:
        # Import the correct module first
        from pytorch_lightning.utilities.data import CombinedLoader
        
        # Create the module path that NeMo expects
        import pytorch_lightning.utilities
        
        # Add the missing module
        class CombinedLoaderModule:
            CombinedLoader = CombinedLoader
        
        pytorch_lightning.utilities.combined_loader = CombinedLoaderModule()
        print("✓ Fixed pytorch_lightning.utilities.combined_loader import")
        
    except ImportError as e:
        print(f"❌ Failed to import CombinedLoader: {e}")
        # Try alternative approach - create a dummy that works
        import pytorch_lightning.utilities
        
        # Import the real class
        try:
            from pytorch_lightning.utilities.data import CombinedLoader as RealCombinedLoader
            
            # Create module with real functionality
            class WorkingCombinedLoaderModule:
                CombinedLoader = RealCombinedLoader
            
            pytorch_lightning.utilities.combined_loader = WorkingCombinedLoaderModule()
            print("✓ Fixed with alternative approach")
            
        except Exception as e2:
            print(f"❌ Alternative fix also failed: {e2}")
            raise

def export_with_cache():
    """Export the model with cache support"""
    
    import nemo.collections.asr as nemo_asr
    
    model_path = "models/nemo_fresh_export/fresh_fastconformer.nemo"
    output_path = "models/nemo_fresh_export/fastconformer_cache_success.onnx"
    
    if not os.path.exists(model_path):
        print(f"❌ Model not found: {model_path}")
        return False
    
    print("=== Exporting with Cache Support ===")
    print(f"Loading model: {model_path}")
    
    # Load the model
    model = nemo_asr.models.EncDecHybridRNNTCTCBPEModel.restore_from(model_path)
    print("✓ Model loaded successfully")
    
    # CRITICAL: Set cache support (the key step from NVIDIA docs)
    print("Setting cache support configuration...")
    model.set_export_config({'cache_support': 'True'})
    print("✓ Cache support enabled")
    
    # Export to ONNX
    print(f"Exporting to: {output_path}")
    os.makedirs(os.path.dirname(output_path), exist_ok=True)
    
    try:
        model.export(output_path)
        
        if os.path.exists(output_path):
            size_mb = os.path.getsize(output_path) / (1024 * 1024)
            print(f"✅ SUCCESS! ONNX model created: {size_mb:.1f} MB")
            print(f"✅ Model location: {output_path}")
            
            # Quick verification
            try:
                import onnx
                m = onnx.load(output_path)
                input_names = [i.name for i in m.graph.input]
                output_names = [o.name for o in m.graph.output]
                has_cache = any('cache' in name for name in input_names + output_names)
                
                print(f"✓ Model inputs: {len(input_names)} (Expected: 4+)")
                print(f"✓ Model outputs: {len(output_names)} (Expected: 4+)")
                print(f"✓ Cache tensors: {'YES' if has_cache else 'NO'}")
                
                if has_cache:
                    print("🎯 SUCCESS! Model exported with cache support!")
                else:
                    print("⚠️  WARNING: No cache tensors found")
                    
            except Exception as e:
                print(f"Note: Could not verify ONNX structure: {e}")
            
            return True
        else:
            print("❌ Export failed - file not created")
            return False
            
    except Exception as e:
        print(f"❌ Export failed: {e}")
        return False

if __name__ == "__main__":
    print("=== Export NeMo FastConformer with Cache Support ===")
    
    # Fix the import issue first
    try:
        fix_pytorch_lightning_import()
    except Exception as e:
        print(f"❌ Could not fix import: {e}")
        sys.exit(1)
    
    # Export with cache support
    success = export_with_cache()
    
    if success:
        print("\n🎯 EXPORT COMPLETE!")
        print("Next step: Test the model")
        print("python3 test_current_model.py")
    else:
        print("\n❌ Export failed")