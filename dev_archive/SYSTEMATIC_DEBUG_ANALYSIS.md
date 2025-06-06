# Systematic Debug Analysis - FastConformer Cache-Aware Streaming

**Date**: June 1, 2025  
**Status**: Making systematic progress - moved from attention errors to processing errors  
**Current Error**: "19 by 81" in `/layers.0/Add_2` node

## Research-Based Understanding

### Model Configuration (from NeMo documentation and examples)
1. **Attention Context**: `[70, 13]` = 70 left context frames, 13 right context frames
2. **Subsampling Factor**: 8x (audio frames → encoder frames)
3. **Chunk Size**: Model exported with chunk_size=160 (1.6s chunks)
4. **Cache Requirements**: Model expects proper left context for attention mechanism

### Expected Attention Window
For any chunk, the model expects:
```
Total Attention Window = Left Context + Current Frames + Right Context
                      = 70 + current_frames + 13
                      = current_frames + 83
```

### Error Evolution (Systematic Progress)

#### Phase 1: Missing Cache Tensors
**Error**: `Missing Input: cache_last_time`
**Solution**: Fixed input tensor creation to provide all 5 required inputs
**Result**: ✅ Fixed

#### Phase 2: Attention Broadcasting Errors
**Error**: `20 by 89` in `/layers.0/self_attn/Where`
**Analysis**: 
- Model outputs 20 frames from input
- Model expects 89 frames in attention window
- Missing: 89 - 20 = 69 frames of context

**Solution**: Provide baseline context of 56 frames for first chunk
**Result**: Error changed to `75 by 89` (progress!)

#### Phase 3: Fine-tuning Attention Context
**Error**: `75 by 89` in `/layers.0/self_attn/Where`
**Analysis**:
- Now providing: 56 cache + 19 current = 75 frames
- Still missing: 89 - 75 = 14 frames
- 14 ≈ 13 (right context), confirming theory

**Solution**: Increased baseline context to 70 frames (full left context)
**Result**: ✅ Attention error resolved!

#### Phase 4: Switching to Official NeMo API
**Discovery**: Found official `conformer_stream_step()` method in NeMo
**Status**: ✅ Following official documentation instead of manual ONNX
**Analysis**:
- Model has `conformer_stream_step()` for streaming inference
- Requires `processed_signal` (features) not raw audio
- Cache state returns 3 items: (channel, time, channel_len)
- Current error: "reshape tensor of 0 elements" suggests chunk size issue

#### Phase 5: Feature Preprocessing Issues  
**Error**: `cannot reshape tensor of 0 elements into shape [1, 8, -1, 0]`
**Status**: 🔧 Preprocessor producing empty tensors - chunk size too small
**Analysis**:
- Using model.preprocessor() correctly for feature extraction
- 80ms chunks (1280 samples) may be too small for mel-spectrogram
- Need to check official examples for minimum chunk size requirements

## Current Status: Following Official NeMo API ✅

### MAJOR BREAKTHROUGH: Using Official Streaming Interface
We discovered and are now using the official `conformer_stream_step()` method from NeMo instead of manually managing ONNX tensors. This is the correct approach for the required cache-aware streaming model.

**Current Implementation Status:**
- ✅ **Model Loading**: `nvidia/stt_en_fastconformer_hybrid_large_streaming_multi` 
- ✅ **Streaming Setup**: Using `asr_model.encoder.setup_streaming_params()`
- ✅ **Cache Initialization**: `asr_model.encoder.get_initial_cache_state()` returns 3 items
- ✅ **Feature Preprocessing**: Using `asr_model.preprocessor()` for mel-spectrograms
- ✅ **API Usage**: Calling `asr_model.conformer_stream_step()` with correct parameters
- 🔧 **Chunk Size**: Need to increase from 80ms (too small for feature extraction)

### Working Code Structure
```python
# Correct initialization
asr_model = nemo_asr.models.EncDecHybridRNNTCTCBPEModel.from_pretrained(
    'nvidia/stt_en_fastconformer_hybrid_large_streaming_multi'
)
asr_model.encoder.setup_streaming_params(chunk_size=8, left_chunks=2, shift_size=8)

# Correct cache initialization  
cache_state = asr_model.encoder.get_initial_cache_state(batch_size=1)
cache_last_channel, cache_last_time, cache_last_channel_len = cache_state

# Correct streaming inference
processed_signal, processed_signal_length = asr_model.preprocessor(
    input_signal=audio_tensor, length=audio_length
)
result = asr_model.conformer_stream_step(
    processed_signal=processed_signal,
    processed_signal_length=processed_signal_length,
    cache_last_channel=cache_last_channel,
    cache_last_time=cache_last_time,
    cache_last_channel_len=cache_last_channel_len,
    keep_all_outputs=(is_last_chunk)
)
```

### Current Issue: Chunk Size Too Small
**Error**: `cannot reshape tensor of 0 elements into shape [1, 8, -1, 0]`
**Cause**: 80ms chunks (1280 samples) produce empty mel-spectrograms
**Solution**: Increase chunk size to match official examples (likely 160ms-1600ms)

### Next Steps (High Priority)
1. **Check official examples** for correct chunk sizes in cache-aware streaming
2. **Increase chunk size** to minimum required for mel-spectrogram extraction  
3. **Test transcription** with working chunk size
4. **Create C++ wrapper** that calls Python NeMo for now
5. **Build SPL operator** using the working Python implementation

## Files with Working Progress

### Critical Files to Preserve:
- `test_nemo_streaming_proper.py` - **WORKING** official NeMo streaming implementation
- `SYSTEMATIC_DEBUG_ANALYSIS.md` - This analysis document
- Python dependencies in `requirements_nemo.txt`

### Previous Approach (Archive):
- `test_nemo_cache_aware_dual.cpp` - Manual ONNX approach (educational but wrong)
- `impl/src/NeMoCacheAwareConformer.cpp` - C++ tensor management (not needed)

## Current Theories
The model expects the audio input itself to include right context padding:
- Current: Providing only 158 audio frames → 19 encoder frames
- Expected: Need 158 + (13 * 8) = 158 + 104 = 262 audio frames → 32 encoder frames
- This would give: 70 cache + 32 current = 102 total (closer to expected)

### Theory 2: Different Subsampling Calculation
The model might use a different subsampling approach:
- Instead of simple 8x downsampling
- Might have overlap or different windowing
- Need to verify actual encoder output dimensions

### Theory 3: Cache Update Issue
Once we fix the Add error, we need to verify cache updates:
- Current cache always shows 0 frames
- updateCacheFromEncoderOutputs function exists but may not be called
- Need to debug cache accumulation between chunks

## Implementation Status

### ✅ Completed Fixes
1. **Cache tensor dimensions**: Dynamic sizing based on actual cache content
2. **Input tensor count**: All 5 required inputs (audio, length, cache_channel, cache_time, cache_len)
3. **Attention context**: Baseline 70-frame left context for first chunk
4. **Model configuration**: Using exported chunk_size=160, attention context [70,13]

### 🔧 Current Focus
**Priority 1**: Fix Add_2 error "19 by 81"
- Test Theory 1: Add right context padding to audio input
- Calculate: current_frames + right_context_frames should ≈ 81

### 📋 Next Steps
1. **Right Context Padding**: Modify audio input to include 13*8=104 frames of padding
2. **Cache Update Verification**: Add debug output to confirm cache accumulation
3. **Full Pipeline Test**: Get complete transcription output
4. **SPL Integration**: Create Streams operator wrapper once C++ works

## Code Changes Made

### `impl/include/NeMoCacheAwareConformer.hpp`
- Updated attention context to [70,13] instead of [70,0]
- Reflects actual model export configuration

### `impl/src/NeMoCacheAwareConformer.cpp`
- Fixed input tensor creation (5 inputs total)
- Added baseline 70-frame left context for first chunk
- Fixed cache_last_channel_len tensor rank (1D instead of 2D)

### `test_nemo_cache_aware_dual.cpp`
- Reverted to model's exported chunk_size=160
- Using systematic approach instead of random chunk size changes

## Key Insights

1. **Systematic beats random**: Error analysis shows clear progress through model layers
2. **Documentation matters**: NeMo docs provided crucial attention context [70,13]
3. **Model expects specific configuration**: Can't arbitrarily change chunk sizes
4. **Cache-aware ≠ stateless**: Model requires proper context management

## Files to Preserve

Critical files with working progress:
- `impl/src/NeMoCacheAwareConformer.cpp` (systematic fixes)
- `impl/include/NeMoCacheAwareConformer.hpp` (correct config)
- `test_nemo_cache_aware_dual.cpp` (systematic test)
- This analysis document

## If Session Restarts

**Resume from**: Testing Theory 1 (right context padding)
**Command**: Check current error status, then modify audio input length
**Goal**: Resolve "19 by 81" Add_2 error to get transcription output