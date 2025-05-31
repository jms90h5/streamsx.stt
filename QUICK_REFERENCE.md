# Quick Reference - NeMo Cache-Aware Export

## TL;DR - How to Fix NeMo Streaming Models

### 🎉 SUCCESS! (May 31, 2025 - 6:15 PM)
- ✅ Fresh model downloaded: `models/nemo_fresh_export/fresh_fastconformer.nemo`
- ✅ Cache configuration working: `model.set_export_config({'cache_support': 'True'})`
- ✅ NeMo confirms: `[NeMo I] Caching support enabled: True`
- ✅ Fixed pytorch-lightning dependency issue
- ✅ **EXPORTED WITH CACHE SUPPORT!**

### Cache-Aware Models Created
- **Encoder**: `models/nemo_fresh_export/encoder-fastconformer_cache_final.onnx` (456MB)
  - Inputs: audio_signal, length, cache_last_channel, cache_last_time, cache_last_channel_len
  - Outputs: outputs, encoded_lengths, cache_last_channel_next, cache_last_time_next, cache_last_channel_next_len
- **Decoder**: `models/nemo_fresh_export/decoder_joint-fastconformer_cache_final.onnx` (21MB)
  - Inputs: encoder_outputs, targets, target_length, input_states_1, input_states_2
  - Outputs: outputs, prednet_lengths, output_states_1, output_states_2

### Understanding the Export
- **Separate files are CORRECT**: NeMo exports hybrid models as encoder+decoder components
- **LSTM warnings are normal**: Harmless warnings about batch size in RNN layers
- **"Export failed" was script error**: Script expected single file but got proper separate components

### Next Steps
1. Update C++ code to use the new cache-aware encoder
2. Test streaming with cache tensors
3. Verify "sh" repetition issue is resolved

### The Critical Line of Code
```python
model.set_export_config({'cache_support': 'True'})  # NEVER FORGET THIS
```

## Quick Verification
```bash
# Check if model has cache support
python3 -c "
import onnx
m = onnx.load('models/nemo_fresh_export/fastconformer_cache_aware.onnx')
has_cache = any('cache' in i.name for i in m.graph.input)
print(f'Cache support: {\"✅ YES\" if has_cache else \"❌ NO\"}')
print(f'Inputs: {len(m.graph.input)} (should be 4+)')
"
```

## Test the Fixed Model
```bash
export LD_LIBRARY_PATH=impl/lib:deps/onnxruntime/lib:$LD_LIBRARY_PATH
./samples/CppONNX_OnnxSTT/test_nemo_standalone \
  --nemo-model models/nemo_fresh_export/fastconformer_cache_aware.onnx \
  --audio-file test_data/audio/librispeech-1995-1837-0001.raw
```

## If You See "Only 'sh' Output"
The model was exported without cache support. Re-export with cache enabled.

## If Model is Corrupted
Download fresh: `wget https://huggingface.co/nvidia/stt_en_fastconformer_hybrid_large_streaming_multi/resolve/main/stt_en_fastconformer_hybrid_large_streaming_multi.nemo`

---
**See NEMO_CACHE_AWARE_GUIDE.md for complete documentation**