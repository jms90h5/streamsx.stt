# NeMo Work Plan Update - May 31, 2025

## Critical Discovery: Documentation vs Reality

### Documentation Audit Results

1. **REAL_MODEL_SUCCESS.md** ✅ ACCURATE
   - Documents the ACTUAL working model: wav2vec2_base.onnx (361MB)
   - Shows real test output and transcription results
   - This is what's currently working in production

2. **NEMO_INTEGRATION_STATUS.md** ❌ INCORRECT/ASPIRATIONAL
   - Claims "zero ONNX Runtime errors" without proof
   - Claims "160x real-time performance" without test results
   - No actual transcription output shown
   - Appears to be written before actually testing

3. **README_MODELS.md** ⚠️ PARTIALLY CORRECT
   - Good as a guide but doesn't distinguish working vs planned models

### The Real Situation

**What Actually Works:**
- wav2vec2_base.onnx (361MB) - Character-level transcription proven to work

**What's Untested:**
- conformer_ctc_dynamic.onnx (2.6MB) - Exported but never successfully tested
- Requires mel-spectrogram preprocessing (80-dim features)
- Has compiled test binary but no successful run documented

### Memory-Safe Next Steps

Given the memory constraints that cause crashes, here's what we should do:

1. **Use Existing Compiled Binary** (SAFE)
   ```bash
   cd /homes/jsharpe/teracloud/com.teracloud.streamsx.stt
   export LD_LIBRARY_PATH=impl/lib:deps/onnxruntime/lib:$LD_LIBRARY_PATH
   ./samples/CppONNX_OnnxSTT/test_nemo_standalone \
     --nemo-model models/nemo_fastconformer_streaming/conformer_ctc_dynamic.onnx \
     --audio-file test_data/audio/librispeech-1995-1837-0001.raw \
     --verbose
   ```

2. **Document Actual Results** (SAFE)
   - Record what actually happens when running the test
   - Note any errors or issues
   - Don't claim success without proof

3. **Update Documentation** (SAFE)
   - Create NEMO_ACTUAL_STATUS.md with real test results
   - Mark NEMO_INTEGRATION_STATUS.md as incorrect/outdated
   - Update README to clarify what actually works

### What to AVOID (Memory Issues)
- ❌ Don't run Python NeMo model loading (uses too much memory)
- ❌ Don't attempt model re-export (already done)
- ❌ Don't load large models in Python
- ❌ Don't run memory-intensive preprocessing in Python

### Recommended Action
Run the existing C++ test binary which handles preprocessing internally and won't cause memory issues. This will give us the real status of the NeMo model integration.

## Test Results - May 31, 2025

### NeMo Model Test Outcome

I ran the test with the compiled binary:
```bash
./samples/CppONNX_OnnxSTT/test_nemo_standalone \
  --nemo-model models/nemo_fastconformer_streaming/conformer_ctc_dynamic.onnx \
  --audio-file test_data/audio/librispeech-1995-1837-0001.raw
```

**Results:**
- ✅ Model loads successfully (no ONNX errors)
- ✅ Audio processing works (160ms chunks)
- ✅ Feature extraction produces values
- ✅ Inference runs (3-8ms per chunk)
- ❌ **Transcription fails** - only outputs token 95 ("sh") repeatedly
- ❌ All tokens have identical probability (-2.91337)

### Conclusion

The NEMO_INTEGRATION_STATUS.md is **INCORRECT**. The model runs but doesn't transcribe. Only wav2vec2_base.onnx actually works as documented in REAL_MODEL_SUCCESS.md.

### Next Steps to Fix NeMo Model

According to the user, the model was working at one point. The issue might be:
1. Missing cache initialization for streaming
2. Incorrect preprocessing parameters
3. Model state not properly maintained between chunks

Need to review: https://docs.nvidia.com/nemo-framework/user-guide/latest/nemotoolkit/asr/models.html#cache-aware-streaming-conformer

## Critical Discovery - May 31, 2025

### The Root Cause

The exported ONNX model (conformer_ctc_dynamic.onnx) is **NOT a cache-aware model**:
- Only has 1 input: `audio_signal`
- Only has 1 output: `log_probs`
- No cache tensor inputs/outputs

A true cache-aware streaming model should have:
- Multiple inputs: audio_signal + cache tensors (cache_last_channel, cache_last_time)
- Multiple outputs: log_probs + updated cache tensors

### Why It's Failing

Without cache tensors, the model:
1. Can't maintain state between chunks
2. Processes each chunk independently
3. Produces the same output (token 95 "sh") for every chunk

### Solution

We need to export the NeMo model with `cache_support=True` to include cache tensors. The current export is missing this critical parameter.

Example of correct export:
```python
model.export(
    "model.onnx",
    cache_support=True,  # Critical for streaming!
    chunk_size=40,       # After subsampling
    left_context_size=70,
    right_context_size=0
)
```