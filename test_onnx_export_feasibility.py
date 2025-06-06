#!/usr/bin/env python3
"""
Step 1: Test if streaming ONNX export is feasible
No assumptions - just test if the model can export to ONNX at all
"""

import os
import sys

def test_onnx_export_basic():
    """Test basic ONNX export capability"""
    
    print("=== Step 1: Testing ONNX Export Feasibility ===")
    print("Goal: See if model exports to ONNX at all (success/failure)")
    
    try:
        import nemo.collections.asr as nemo_asr
        import torch
        
        # Load the exact model we know works in Python
        model_name = "nvidia/stt_en_fastconformer_hybrid_large_streaming_multi"
        print(f"Loading model: {model_name}")
        
        asr_model = nemo_asr.models.EncDecHybridRNNTCTCBPEModel.from_pretrained(model_name)
        asr_model.eval()
        print("✓ Model loaded")
        
        # Create output directory
        output_dir = "test_export"
        os.makedirs(output_dir, exist_ok=True)
        
        # Test 1: Basic export (no streaming config)
        print("\n--- Test 1: Basic Export (No Streaming) ---")
        try:
            basic_path = os.path.join(output_dir, "basic_export.onnx")
            asr_model.export(basic_path, check_trace=False)
            print(f"✅ Basic export SUCCESS: {basic_path}")
            
            # Check file exists and has reasonable size
            if os.path.exists(basic_path):
                size_mb = os.path.getsize(basic_path) / (1024*1024)
                print(f"   File size: {size_mb:.1f} MB")
            
        except Exception as e:
            print(f"❌ Basic export FAILED: {e}")
            return False
        
        # Test 2: Export with streaming configuration
        print("\n--- Test 2: Export with Streaming Config ---")
        try:
            # Configure streaming (same as our working Python version)
            asr_model.encoder.setup_streaming_params(
                chunk_size=40,      # 40 encoder frames = 1600ms
                left_chunks=2,      # Left context
                shift_size=40       # Non-overlapping
            )
            print("✓ Streaming configured")
            
            streaming_path = os.path.join(output_dir, "streaming_export.onnx")
            asr_model.export(streaming_path, check_trace=False)
            print(f"✅ Streaming export SUCCESS: {streaming_path}")
            
            # Check file
            if os.path.exists(streaming_path):
                size_mb = os.path.getsize(streaming_path) / (1024*1024)
                print(f"   File size: {size_mb:.1f} MB")
            
        except Exception as e:
            print(f"❌ Streaming export FAILED: {e}")
            print("   This means streaming ONNX export is not supported")
            return False
            
        # Test 3: Check if exported models can be loaded
        print("\n--- Test 3: Verify Exported Models Load ---")
        try:
            import onnx
            
            # Check basic model
            basic_model = onnx.load(basic_path)
            print(f"✓ Basic model loads: {len(basic_model.graph.input)} inputs, {len(basic_model.graph.output)} outputs")
            
            # Check streaming model  
            streaming_model = onnx.load(streaming_path)
            print(f"✓ Streaming model loads: {len(streaming_model.graph.input)} inputs, {len(streaming_model.graph.output)} outputs")
            
            # Compare input counts
            if len(streaming_model.graph.input) > len(basic_model.graph.input):
                print("✓ Streaming model has more inputs (likely includes cache)")
                print("   This suggests streaming export may actually work")
            else:
                print("⚠️  Streaming model has same inputs as basic (may not be truly streaming)")
                
        except ImportError:
            print("⚠️  Cannot verify models (onnx package not available)")
        except Exception as e:
            print(f"❌ Model verification FAILED: {e}")
            return False
            
        print("\n=== Step 1 Results ===")
        print("✅ ONNX export is FEASIBLE")
        print("✅ Both basic and streaming exports succeeded")
        print("✅ Models can be loaded and verified")
        print(f"📁 Files created in: {output_dir}/")
        
        return True
        
    except Exception as e:
        print(f"❌ Step 1 FAILED: {e}")
        import traceback
        traceback.print_exc()
        return False

if __name__ == "__main__":
    success = test_onnx_export_feasibility()
    
    if success:
        print("\n🎉 Step 1 PASSED: ONNX export is feasible")
        print("📋 Next: Test if exported ONNX actually works in C++")
    else:
        print("\n💥 Step 1 FAILED: ONNX export not feasible")
        print("📋 Next: Need different approach (or model doesn't support ONNX streaming)")
        sys.exit(1)