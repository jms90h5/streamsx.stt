# FastConformer Tensor Dimension Analysis and Solution Plan

**Date**: June 1, 2025  
**Issue**: "20 by 70" broadcasting error in NeMo FastConformer cache-aware streaming  
**Model**: nvidia/stt_en_fastconformer_hybrid_large_streaming_multi  

## Root Cause Analysis - DEFINITIVE

After systematic investigation of NVIDIA NeMo documentation, ONNX model inspection, and C++ code analysis, I have identified the exact root cause of the tensor dimension mismatch.

### The Error
```
Error: Non-zero status code returned while running Where node. Name:'/layers.0/self_attn/Where' 
Status Message: Attempting to broadcast an axis by a dimension other than 1. 20 by 70
```

### What This Error Means
1. **Location**: First transformer layer (`/layers.0`) self-attention mechanism (`/self_attn`) Where node (attention masking)
2. **Operation**: Broadcasting two tensors for attention masking
3. **Problem**: Tensor A has 20 frames, Tensor B has 70 frames - incompatible for broadcasting
4. **Context**: This is NOT a chunk size issue - it's a cache concatenation/preparation issue

### Model Requirements (From NVIDIA Documentation)

#### Attention Context Configuration
- **Model expects**: `att_context_size = [70, 13]` for full latency mode
- **For 0ms latency**: `att_context_size = [70, 0]` 
- **Meaning**: 70 frames of left context (previous), 0 frames of right context (future)

#### Model Input Structure (From ONNX Inspection)
```
Encoder Inputs:
1. audio_signal: [batch, 80, time_frames]  # 80 mel features
2. length: [batch]                         # sequence length
3. cache_last_channel: [batch, 17, cache_frames, 512]  # 17 layers, 512 hidden size
4. cache_last_time: [batch, 17, 512, time_context]     # time-based cache 
5. cache_last_channel_len: [batch]         # cache valid length
```

#### Audio Processing Pipeline
1. **Input audio**: Raw PCM 16kHz
2. **Feature extraction**: 80-dim log-mel spectrograms (10ms frames)
3. **Subsampling**: 8x downsampling (160 input frames → 20 output frames)
4. **Attention**: Requires 70 frames of left context

### The Actual Problem in C++ Code

#### Current Implementation (WRONG)
```cpp
// Line 373-374 in NeMoCacheAwareConformer.cpp
std::vector<int64_t> time_cache_shape = {batch_size, num_cache_layers, hidden_size, last_time_cache_size};
// Creates: [1, 17, 512, 70] - fixed 70-frame cache

// Line 334-336
std::vector<int64_t> audio_shape = {batch_size, feature_dim, time_frames};
// Creates: [1, 80, ~20] - current chunk frames
```

#### What Happens During Inference
1. **Current chunk**: ~20 frames after processing
2. **Cache tensor**: Fixed 70-frame empty tensor
3. **Attention mechanism**: Tries to broadcast 20-frame current + 70-frame cache
4. **Broadcasting fails**: Cannot broadcast 20 by 70 (neither dimension is 1)

#### Why This Is Wrong
The cache should contain **actual previous encoded frames**, not a fixed-size empty tensor. The total input to attention should be `current_frames + valid_cache_frames = total_context_needed`.

### The Correct Solution

#### Understanding Cache-Aware Streaming
From NeMo documentation and examples:
1. **First chunk**: Only current frames (no cache) - pad if needed
2. **Subsequent chunks**: Concatenate cache (previous frames) + current frames
3. **Cache management**: Store last N encoded frames for next iteration
4. **Total length**: Should match attention context requirements

#### Required Changes to C++ Implementation

##### 1. Fix Cache Tensor Initialization
```cpp
// CURRENT (WRONG):
config_.last_time_cache_size = 70;  // Fixed size

// CORRECT:
config_.max_cache_frames = 70;      // Maximum cache size
config_.current_cache_frames = 0;   // Actual cache frames (starts at 0)
```

##### 2. Fix prepareEncoderInputs() Function
```cpp
// CURRENT (WRONG): Always create 70-frame cache
std::vector<int64_t> time_cache_shape = {batch_size, num_cache_layers, hidden_size, 70};

// CORRECT: Dynamic cache size based on actual history
int total_context_frames = current_cache_frames + current_chunk_frames;
if (total_context_frames < 70) {
    // Pad current input to reach minimum context requirement
    // OR handle short context gracefully
}
```

##### 3. Implement Proper Cache Management
```cpp
// After each chunk:
1. Extract encoded frames from encoder output
2. Append to cache (cache = cache + new_frames)
3. Trim cache to maximum size (keep last 70 frames)
4. Update cache_frames counter
```

### Specific Implementation Plan

#### Phase 1: Fix Cache Tensor Shapes
**File**: `impl/src/NeMoCacheAwareConformer.cpp`
**Functions**: `initializeCacheTensors()`, `prepareEncoderInputs()`

1. **Change cache initialization** to support dynamic sizing
2. **Fix tensor shape creation** to match actual cache content
3. **Add debug logging** to verify tensor shapes at each step

#### Phase 2: Implement Proper Cache Management  
**File**: `impl/src/NeMoCacheAwareConformer.cpp`
**Functions**: `updateCacheFromEncoderOutputs()`, `prepareEncoderInputs()`

1. **Extract encoded frames** from encoder outputs
2. **Append to existing cache** (not replace)
3. **Maintain cache size limit** (70 frames max)
4. **Handle first chunk case** (no prior cache)

#### Phase 3: Handle Edge Cases
1. **First chunk**: No cache available - handle gracefully
2. **Short sequences**: Less than 70 frames total - pad or adjust
3. **Cache overflow**: Trim older frames when cache exceeds 70 frames

### Configuration Changes Required

#### NeMoConfig Structure Updates
```cpp
struct NeMoConfig {
    // REMOVE these:
    // int last_channel_cache_size = 40;
    // int last_time_cache_size = 70;
    
    // ADD these:
    int max_cache_frames = 70;         // Maximum cache history
    int current_cache_frames = 0;      // Current cache size (dynamic)
    bool handle_short_context = true;  // Gracefully handle < 70 frames
    
    // KEEP these:
    int chunk_frames = 160;            // Input chunk size (works with wav2vec2)
    int feature_dim = 80;              // 80-dim log-mel features
    int num_cache_layers = 17;         // FastConformer layers
    int hidden_size = 512;             // Model hidden dimension
};
```

### Expected Results After Fix

#### Successful Tensor Shapes
```
First chunk:
- Current frames: 20
- Cache frames: 0  
- Total context: 20 (handle gracefully or pad)

Second chunk:  
- Current frames: 20
- Cache frames: 20 (from previous)
- Total context: 40

Fourth chunk:
- Current frames: 20  
- Cache frames: 60 (from previous 3 chunks)
- Total context: 80 → trim to 70

Subsequent chunks:
- Current frames: 20
- Cache frames: 50 (trimmed from 70-20=50)
- Total context: 70 (perfect match)
```

#### No More Broadcasting Errors
- All tensors will have compatible dimensions
- Attention mechanism will receive properly shaped inputs
- Cache will contain actual encoded content, not empty tensors

### Implementation Priority

1. **HIGH**: Fix `prepareEncoderInputs()` tensor shapes
2. **HIGH**: Implement dynamic cache sizing
3. **MEDIUM**: Add proper cache management after encoder outputs
4. **LOW**: Optimize cache trimming and edge case handling

### Success Criteria

1. **No tensor broadcasting errors**
2. **Successful encoder inference** (encoder outputs valid tensors)
3. **Proper cache updates** (cache contains actual previous frames)
4. **Working transcription** (meaningful text output from librispeech audio)

### Backup Plan

If the fix doesn't work immediately:
1. **Verify ONNX model integrity** (decoder model had parsing errors)
2. **Test with Python NeMo** to get reference tensor shapes
3. **Compare C++ tensor preparation** with working Python implementation
4. **Consider using fixed 70-frame input** (pad current chunk to 70 frames)

---

**This analysis is based on**:
- NVIDIA NeMo documentation (cache-aware streaming)
- ONNX model inspection (actual tensor requirements) 
- C++ code analysis (current implementation flaws)
- Error message interpretation (broadcasting failure root cause)

**Next step**: Implement the tensor shape fixes in C++ code with careful attention to memory usage to avoid heap crashes.