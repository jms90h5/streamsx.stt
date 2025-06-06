#!/usr/bin/env python3
"""
Quick verification and recommendation to use the working model
"""

import os

def check_models():
    """Check what models we have and recommend the best approach"""
    
    print("=== Model Status Check ===\n")
    
    # Check fresh model
    fresh_nemo = "models/nemo_fresh_export/fresh_fastconformer.nemo"
    if os.path.exists(fresh_nemo):
        size_mb = os.path.getsize(fresh_nemo) / (1024*1024)
        print(f"✅ Fresh NeMo model: {fresh_nemo} ({size_mb:.1f}MB)")
    else:
        print(f"❌ Fresh NeMo model not found")
    
    # Check working model
    working_model = "models/wav2vec2_base.onnx"
    if os.path.exists(working_model):
        size_mb = os.path.getsize(working_model) / (1024*1024)
        print(f"✅ Working ONNX model: {working_model} ({size_mb:.1f}MB)")
    else:
        print(f"❌ Working ONNX model not found")
    
    # Check broken model
    broken_model = "models/nemo_fastconformer_streaming/conformer_ctc_dynamic.onnx"
    if os.path.exists(broken_model):
        size_mb = os.path.getsize(broken_model) / (1024*1024)
        print(f"⚠️  Broken ONNX model: {broken_model} ({size_mb:.1f}MB) - no cache support")
    else:
        print(f"❌ Broken model not found")
    
    print("\n=== Recommendation ===")
    print("🎯 Use the working wav2vec2_base.onnx model:")
    print("   - Proven to work with real transcriptions")
    print("   - 361MB, character-level output")
    print("   - No dependency issues")
    print("\n📝 Test command:")
    print("export LD_LIBRARY_PATH=impl/lib:deps/onnxruntime/lib:$LD_LIBRARY_PATH")
    print("./samples/CppONNX_OnnxSTT/test_nemo_standalone \\")
    print("  --nemo-model models/wav2vec2_base.onnx \\")
    print("  --audio-file test_data/audio/librispeech-1995-1837-0001.raw")
    
    print("\n🔧 For NeMo specifically:")
    print("   - Fresh model is downloaded but export has dependency conflicts")
    print("   - Would need clean Python environment to export properly")
    print("   - Current C++ code expects cache tensors that aren't in export")
    
    print("\n💡 Future work:")
    print("   - Set up isolated Python environment for NeMo export")
    print("   - Or modify C++ code to work with stateless ONNX models")
    print("   - Or use NeMo Python API directly instead of ONNX")

if __name__ == "__main__":
    check_models()