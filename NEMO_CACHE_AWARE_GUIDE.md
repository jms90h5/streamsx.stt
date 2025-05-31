# NeMo Cache-Aware Streaming Models - Complete Guide

**Created**: May 31, 2025  
**Status**: CRITICAL REFERENCE - DO NOT DELETE

## The Problem We Solved

### What Was Wrong
- Current ONNX model (`conformer_ctc_dynamic.onnx`) only has 1 input/1 output
- Missing cache tensors (`cache_last_channel`, `cache_last_time`) 
- Model processes each chunk independently, losing context
- Results in repeated tokens (e.g., only "sh" output)
- **Root cause**: Model exported WITHOUT cache support

### What We Learned
- NeMo models export WITHOUT cache support by default
- **CRITICAL**: Must call `model.set_export_config({'cache_support': 'True'})` before export
- Cache tensors maintain state between chunks for streaming
- Original model was corrupted from multiple crash attempts

## How to Export NeMo Models with Cache Support

### Method 1: Complete Script (Recommended)
```bash
cd /homes/jsharpe/teracloud/com.teracloud.streamsx.stt
python3 download_and_export_fresh.py
```

This downloads fresh model and exports with cache support.

### Method 2: Manual Export (if you have working .nemo file)
```python
import nemo.collections.asr as nemo_asr

# Load model
model = nemo_asr.models.EncDecHybridRNNTCTCBPEModel.restore_from('model.nemo')
model.eval()

# *** CRITICAL STEP - NEVER FORGET THIS ***
model.set_export_config({'cache_support': 'True'})

# Export with cache tensors
model.export('cache_aware_model.onnx')
```

### Method 3: Download Fresh Model
```bash
# If model is corrupted, download fresh
wget https://huggingface.co/nvidia/stt_en_fastconformer_hybrid_large_streaming_multi/resolve/main/stt_en_fastconformer_hybrid_large_streaming_multi.nemo

# Then export with cache support (see Method 2)
```

## Model Architecture Requirements

### Working Model Must Have:
**Inputs (4+):**
- `audio_signal`: [batch, time, 80] - Mel-spectrogram features
- `audio_length`: [batch] - Sequence lengths
- `cache_last_channel`: [batch, layers, hidden] - Channel cache state
- `cache_last_time`: [batch, layers, hidden] - Time cache state

**Outputs (4+):**
- `encoded`: [batch, time_out, hidden] - Encoded features
- `encoded_length`: [batch] - Output sequence lengths  
- `cache_last_channel_out`: [batch, layers, hidden] - Updated channel cache
- `cache_last_time_out`: [batch, layers, hidden] - Updated time cache

### Broken Model Has:
**Inputs (1):**
- `audio_signal`: [batch, time, 80]

**Outputs (1):**
- `log_probs`: [batch, time, vocab_size]

## Verification Commands

### Check Model Structure
```python
import onnx
model = onnx.load('your_model.onnx')

print("Inputs:")
for inp in model.graph.input:
    print(f"  - {inp.name}")

print("\nOutputs:")  
for out in model.graph.output:
    print(f"  - {out.name}")

# Should see cache tensors if exported correctly
has_cache = any('cache' in inp.name for inp in model.graph.input)
print(f"\nCache support: {'✅ YES' if has_cache else '❌ NO'}")
```

### Test Model Loading
```python
import onnxruntime as ort
session = ort.InferenceSession('your_model.onnx')

print(f"Model loads: ✅")
print(f"Inputs: {len(session.get_inputs())}")
print(f"Outputs: {len(session.get_outputs())}")
```

## Working vs Broken Examples

### ✅ WORKING: Cache-Aware Model
```
Inputs (4):
  - audio_signal
  - audio_length  
  - cache_last_channel
  - cache_last_time

Outputs (4):
  - encoded
  - encoded_length
  - cache_last_channel_out
  - cache_last_time_out
```
**Result**: Proper streaming with context between chunks

### ❌ BROKEN: Stateless Model  
```
Inputs (1):
  - audio_signal

Outputs (1):
  - log_probs
```
**Result**: Repeated tokens, no streaming context

## C++ Integration Notes

### Current C++ Code Expects:
- Model with cache tensor inputs/outputs
- Path: `models/nemo_fastconformer_streaming/conformer_ctc_dynamic.onnx`
- Cache management in `NeMoCacheAwareConformer.cpp`

### To Use New Model:
1. Export model with cache support (see above)
2. Update model path in test:
   ```bash
   ./test_nemo_standalone --nemo-model models/nemo_fresh_export/fastconformer_cache_aware.onnx
   ```
3. Verify cache tensors are initialized in C++ code

## Troubleshooting

### "Only outputs 'sh' repeatedly"
- **Cause**: Model exported without cache support
- **Fix**: Re-export with `model.set_export_config({'cache_support': 'True'})`

### "unexpected end of data" during model load
- **Cause**: Corrupted .nemo file from crashes
- **Fix**: Download fresh model (see Method 3)

### "No module named 'pytorch_lightning.utilities.combined_loader'"
- **Cause**: Version conflicts in NeMo installation
- **Fix**: Use fresh Python environment or avoid TTS imports

### Model loads but inference fails
- **Cause**: Missing cache tensor inputs
- **Fix**: Check model has 4+ inputs (see Verification)

## Reference Documentation

### NVIDIA Official Docs:
- Cache-aware streaming: https://docs.nvidia.com/nemo-framework/user-guide/latest/nemotoolkit/asr/models.html#cache-aware-streaming-conformer
- Model export: https://docs.nvidia.com/deeplearning/nemo/user-guide/docs/en/main/core/export.html

### Key NVIDIA Quote:
> "Cache-aware models are being exported without caching support by default. To include caching support, model.set_export_config({'cache_support' : 'True'}) should be called before export."

### GitHub Examples:
- Streaming inference: https://github.com/NVIDIA/NeMo/blob/main/examples/asr/asr_cache_aware_streaming/speech_to_text_cache_aware_streaming_infer.py

## Files in This Project

### Working Models:
- `models/wav2vec2_base.onnx` - Actually works, produces transcriptions
- `models/nemo_fresh_export/fastconformer_cache_aware.onnx` - Should work after export

### Broken Models:
- `models/nemo_fastconformer_streaming/conformer_ctc_dynamic.onnx` - No cache support
- `models/nemo_fastconformer_direct/stt_en_fastconformer_hybrid_large_streaming_multi.nemo` - Corrupted

### Scripts:
- `download_and_export_fresh.py` - Complete solution
- `test_fixed_model.py` - Verify exported models
- `fix_nemo_export_final.py` - Alternative export method

### Documentation:
- `NEMO_INTEGRATION_STATUS.md` - ❌ INCORRECT (claims success without proof)
- `REAL_MODEL_SUCCESS.md` - ✅ CORRECT (documents working wav2vec2)
- `NEMO_WORK_PLAN_UPDATE.md` - Investigation notes
- `NEMO_CACHE_AWARE_GUIDE.md` - ✅ THIS FILE (complete reference)

## SUCCESSFUL EXPORT (May 31, 2025)

### ✅ WE SOLVED IT!

**Exact commands that worked:**
1. `python3 download_and_export_fresh.py` - Downloaded fresh model
2. Fixed pytorch-lightning dependency: Created `/pytorch_lightning/utilities/combined_loader.py`
3. `python3 final_working_export.py` - Exported with cache support

### ✅ Successfully Created Cache-Aware Models

**Encoder Model**: `models/nemo_fresh_export/encoder-fastconformer_cache_final.onnx` (456MB)
- **Inputs (5)**: audio_signal, length, cache_last_channel, cache_last_time, cache_last_channel_len
- **Outputs (5)**: outputs, encoded_lengths, cache_last_channel_next, cache_last_time_next, cache_last_channel_next_len

**Decoder Model**: `models/nemo_fresh_export/decoder_joint-fastconformer_cache_final.onnx` (21MB)  
- **Inputs (5)**: encoder_outputs, targets, target_length, input_states_1, input_states_2
- **Outputs (4)**: outputs, prednet_lengths, output_states_1, output_states_2

### ✅ Cache Export Log Verification
```
[NeMo I 2025-05-31 18:13:09 asr_model:223] Caching support enabled: True
[NeMo W] input cache_last_channel
[NeMo W] input cache_last_time
[NeMo W] input cache_last_channel_len
[NeMo W] output cache_last_channel_next
[NeMo W] output cache_last_time_next
[NeMo W] output cache_last_channel_next_len
[NeMo I] Successfully exported ConformerEncoder to models/nemo_fresh_export/encoder-fastconformer_cache_final.onnx
[NeMo I] Successfully exported RNNTDecoderJoint to models/nemo_fresh_export/decoder_joint-fastconformer_cache_final.onnx
```

### ✅ Understanding NeMo Export Behavior

**NeMo exports hybrid models as separate components (this is CORRECT):**
- **Encoder**: Handles audio processing with cache tensors for streaming
- **Decoder**: Handles text generation with RNN states
- **Not a failure**: Separate files are the standard for FastConformer-Hybrid models

**Warning about LSTM export is normal and harmless.**

### ✅ Key Fix: pytorch-lightning Dependency
The missing `pytorch_lightning.utilities.combined_loader` was fixed by creating:
```python
# /pytorch_lightning/utilities/combined_loader.py
from pytorch_lightning.utilities.data import CombinedLoader
__all__ = ['CombinedLoader']
```

## Summary

**The core issue was simple but critical**: NeMo models export without cache support by default. The solution is one line of code before export:

```python
model.set_export_config({'cache_support': 'True'})
```

**✅ SUCCESS CONFIRMED**: nvidia/stt_en_fastconformer_hybrid_large_streaming_multi model successfully exported with cache support on May 31, 2025.

**Next step**: Test these models to verify they eliminate the "sh" repetition issue.