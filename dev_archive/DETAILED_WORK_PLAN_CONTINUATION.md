# Detailed Work Plan - FastConformer Implementation Continuation

**Date**: June 1, 2025  
**Current Phase**: BUILD AND TEST (Implementation Complete)  
**Objective**: Working C++ Streams application with nvidia/stt_en_fastconformer_hybrid_large_streaming_multi  

## Phase Status Overview

### ✅ PHASE 1: ANALYSIS (COMPLETE)
- Root cause identified: "20 by 70" tensor broadcasting error
- NVIDIA NeMo documentation studied and understood
- ONNX model requirements analyzed
- Attention context [70,0] requirements confirmed

### ✅ PHASE 2: IMPLEMENTATION (COMPLETE) 
- Dynamic cache management implemented
- prepareEncoderInputs() function completely rewritten
- Cache tensor creation fixed for proper dimensions
- Debug logging added throughout

### 🔄 PHASE 3: BUILD AND TEST (CURRENT)
- **Next Immediate Steps**: Compile and test the fix
- **Expected Result**: No broadcasting errors, successful transcription

### ⏳ PHASE 4: STREAMS INTEGRATION (PENDING)
- Create SPL operator wrapper
- Build complete sample application

## Immediate Action Plan (Next Steps)

### Step 1: Environment Setup
```bash
cd /homes/jsharpe/teracloud/com.teracloud.streamsx.stt
source /homes/jsharpe/teracloud/streams/7.2.0.0/bin/streamsprofile.sh
export LD_LIBRARY_PATH=/homes/jsharpe/teracloud/com.teracloud.streamsx.stt/deps/onnxruntime/lib:$LD_LIBRARY_PATH
```

### Step 2: Build the Implementation
```bash
# Clean previous builds
make -f impl/Makefile clean

# Build the updated library
make -f impl/Makefile

# Build the test program
g++ -o test_nemo_cache_aware_dual test_nemo_cache_aware_dual.cpp \
    -I./impl/include -L./impl/lib -lnemo_stt \
    -L./deps/onnxruntime/lib -lonnxruntime \
    -std=c++17 -pthread
```

### Step 3: Test the Fix
```bash
./test_nemo_cache_aware_dual test_data/audio/librispeech-1995-1837-0001.raw test_results/fixed_fastconformer.txt
```

### Step 4: Validate Results
**Expected Output Patterns**:
- `FIRST_CHUNK: Initializing empty cache tensors`
- `CACHE_STATE: current=0 max=70 actual=0`
- `PREPARE_INPUTS: Created 5 input tensors`
- NO ERROR: `Attempting to broadcast an axis by a dimension other than 1. 20 by 70`
- `✅ SUCCESS: No repeated 'sh' tokens detected!`

## Detailed Technical Implementation Summary

### What Was Fixed (Technical Details)

#### Problem: Fixed Cache Tensors
```cpp
// OLD CODE (CAUSED BROADCASTING ERROR):
std::vector<int64_t> time_cache_shape = {
    static_cast<int64_t>(config_.batch_size),           // 1
    static_cast<int64_t>(config_.num_cache_layers),     // 17
    static_cast<int64_t>(config_.hidden_size),          // 512
    static_cast<int64_t>(config_.last_time_cache_size)  // 70 (FIXED!)
};
```

#### Solution: Dynamic Cache Tensors
```cpp
// NEW CODE (FIXES BROADCASTING):
if (actual_cache_frames == 0) {
    // First chunk: minimal cache
    std::vector<int64_t> empty_time_shape = {1, 17, 512, 1};  // Only 1 frame
} else {
    // Subsequent chunks: actual cache content
    std::vector<int64_t> time_cache_shape = {1, 17, 512, actual_cache_frames};
}
```

### Key Configuration Changes
```cpp
// OLD CONFIG (PROBLEMATIC):
struct NeMoConfig {
    int last_channel_cache_size = 40;  // Fixed size
    int last_time_cache_size = 70;     // Fixed size - CAUSED ERROR
};

// NEW CONFIG (CORRECT):
struct NeMoConfig {
    int max_cache_frames = 70;         // Maximum limit
    int current_cache_frames = 0;      // Dynamic counter
    bool handle_short_context = true;  // Graceful handling
};
```

### Cache Growth Pattern (Expected)
```
Inference Step 1: current=0,  cache tensor = [1,17,512,1]    (minimal)
Inference Step 2: current=20, cache tensor = [1,17,512,20]   (growing)
Inference Step 3: current=40, cache tensor = [1,17,512,40]   (growing)
Inference Step 4: current=60, cache tensor = [1,17,512,60]   (growing)
Inference Step 5: current=50, cache tensor = [1,17,512,50]   (trimmed to 70 max)
```

## Contingency Plans

### If Build Fails
1. **ONNX Runtime Issues**: Check `deps/onnxruntime/` setup
2. **Missing Dependencies**: Run `setup_onnx_runtime.sh` if available
3. **Header Issues**: Verify include paths in Makefile

### If Test Fails with Memory Issues
1. **Reduce chunk size**: Change `chunk_frames` from 160 to 80
2. **Disable caching temporarily**: Set `enable_caching = false` for basic test
3. **Use smaller model**: Fall back to simpler ONNX model for verification

### If Broadcasting Error Still Occurs
1. **Check debug output**: Look for cache tensor shapes in logs
2. **Verify model files**: Ensure ONNX models are not corrupted
3. **Compare with Python**: Test same model in Python NeMo for reference

## Success Criteria

### ✅ Build Success
- No compilation errors
- Libraries linked correctly
- Test executable created

### ✅ Runtime Success  
- No "20 by 70" broadcasting error
- Cache tensors created with proper dimensions
- Debug output shows dynamic cache growth

### ✅ Transcription Success
- Meaningful text output (not empty or "sh sh sh")
- Reasonable transcription of librispeech audio
- No repeated token errors

## Next Phase Planning (After Success)

### Streams Application Development
1. **Create SPL Operator**: Wrap C++ implementation in SPL interface
2. **Build Sample App**: Complete Streams application demonstrating usage
3. **Integration Testing**: Test with various audio inputs and streaming scenarios
4. **Documentation**: User guide and deployment instructions

### File Locations for Reference
- **Implementation**: `impl/src/NeMoCacheAwareConformer.cpp`
- **Header**: `impl/include/NeMoCacheAwareConformer.hpp`  
- **Test Program**: `test_nemo_cache_aware_dual.cpp`
- **Models**: `models/nemo_fresh_export/encoder-fastconformer_cache_final.onnx`
- **Audio**: `test_data/audio/librispeech-1995-1837-0001.raw`
- **Results**: `test_results/` (output location)

## Recovery Information

### If Continuation Needed After Crash
1. **Read**: `CURRENT_WORK_STATUS_JUNE_1_2025.md` for immediate status
2. **Review**: `FASTCONFORMER_TENSOR_DIMENSION_ANALYSIS.md` for technical details
3. **Check**: Modified files for implemented changes
4. **Continue**: From build step - all implementation is complete

### Critical Files to Preserve
- All `.md` documentation files (contain full analysis and implementation details)
- Modified `.hpp` and `.cpp` files (contain the actual fix)
- ONNX model files in `models/nemo_fresh_export/`
- Test audio files in `test_data/audio/`

**READY TO PROCEED**: All analysis complete, implementation finished, documentation comprehensive. The fix should resolve the tensor broadcasting issue and enable working FastConformer transcription.