# How to Fix the NeMo Model

## The Problem
The current ONNX model (`conformer_ctc_dynamic.onnx`) is missing cache tensors. It only has:
- Input: `audio_signal` 
- Output: `log_probs`

For streaming, it needs:
- Inputs: `audio_signal`, `cache_last_channel`, `cache_last_time`
- Outputs: `log_probs`, `cache_last_channel_out`, `cache_last_time_out`

## Quick Test
First, verify the problem:
```bash
python3 test_current_model.py
```

## Solution Options

### Option 1: Use the Working Model
The `wav2vec2_base.onnx` model actually works:
```bash
# This model is proven to work
cp models/wav2vec2_base.onnx models/working_model.onnx
```

### Option 2: Export with Correct Script
Try the simple export script:
```bash
python3 export_cache_simple.py
```

### Option 3: Manual Python Export
If the scripts fail due to dependencies, try this in Python:

```python
# In Python interpreter
import torch
import nemo.collections.asr as nemo_asr

# Load model
model = nemo_asr.models.ASRModel.restore_from(
    'models/nemo_fastconformer_direct/stt_en_fastconformer_hybrid_large_streaming_multi.nemo'
)

# CRITICAL: Enable cache before export
model.encoder.setup_streaming_params(
    chunk_size=40,  # After 4x subsampling
    left_context_size=70,
    right_context_size=0,
    cache_aware=True
)

# Export with cache
model.export('models/fixed/model_with_cache.onnx', 
             onnx_opset_version=14,
             check_trace=False)
```

### Option 4: Use Checkpoint Version
The model worked in checkpoint3. Try using that:
```bash
cd /homes/jsharpe/teracloud/checkpoint3_extracted/com.teracloud.streamsx.stt
python3 test_nemo_cache_aware
```

## Verification
After export, verify the model has cache:
```bash
python3 -c "
import onnx
m = onnx.load('models/fixed/model_with_cache.onnx')
print(f'Inputs: {[i.name for i in m.graph.input]}')
print(f'Has cache: {any(\"cache\" in i.name for i in m.graph.input)}')
"
```

## If All Else Fails
1. The wav2vec2 model works - use that
2. The issue is the export was done without cache support
3. You need NeMo to re-export with cache enabled
4. Or modify the C++ code to handle stateless operation differently