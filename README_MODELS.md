# Teracloud Streams STT Toolkit - Model Support

This toolkit supports multiple speech-to-text models through different operators.

## Supported Models

### 1. NVIDIA NeMo Models ✅ WORKING
- **Operator**: `NeMoSTT`
- **Model Format**: `.nemo` files converted to `.onnx` with cache support
- **Example Model**: `nvidia/stt_en_fastconformer_hybrid_large_streaming_multi`
- **Features**: 
  - Cache-aware streaming (114M parameters)
  - Trained on 10,000+ hours of speech data across 13 datasets
  - Multiple latency modes (0ms, 80ms, 480ms, 1040ms)
  - Hybrid CTC + RNN-Transducer decoders
- **✅ SUCCESS**: Cache-aware export completed May 31, 2025
- **Current Models**:
  - Encoder: `models/nemo_fresh_export/encoder-fastconformer_cache_final.onnx` (456MB)
  - Decoder: `models/nemo_fresh_export/decoder_joint-fastconformer_cache_final.onnx` (21MB)

### 2. ONNX Models
- **Operator**: `OnnxSTT`
- **Model Format**: `.onnx` files
- **Example Models**:
  - Sherpa ONNX Zipformer models
  - Exported Whisper models
  - Any ONNX-compatible ASR model

### 3. WeNet Models
- **Operator**: `WenetSTT`
- **Model Format**: WeNet checkpoint files
- **Features**: Streaming-capable models

## Using Different Models

### Command Line Usage

```bash
# Using NeMo model
./output/bin/standalone \
    modelType=nemo \
    modelPath=/path/to/model.nemo \
    audioFile=/path/to/audio.wav

# Using ONNX model  
./output/bin/standalone \
    modelType=onnx \
    modelPath=/path/to/model.onnx \
    audioFile=/path/to/audio.wav
```

### SPL Application Example

```spl
// For NeMo models
stream<rstring text> Transcription = NeMoSTT(AudioStream) {
    param
        modelPath: "/path/to/model.nemo";
        audioFormat: "mono16k";
        chunkDurationMs: 5000;
}

// For ONNX models
stream<rstring text, boolean isFinal, float64 confidence> 
    Transcription = OnnxSTT(AudioStream) {
    param
        encoderModel: "/path/to/encoder.onnx";
        vocabFile: "/path/to/tokens.txt";
        sampleRate: 16000;
}
```

## Model Configuration

### NeMo Models
- Download from HuggingFace or NVIDIA NGC
- Ensure Python environment has NeMo dependencies installed
- Models include vocabulary and configuration

### ONNX Models
- Export from PyTorch/TensorFlow models
- Requires separate vocabulary file
- Can use different execution providers (CPU, CUDA)

## Performance Considerations

- **NeMo**: Best accuracy, requires Python runtime
- **ONNX**: Good performance, pure C++ implementation
- **WeNet**: Lightweight, good for embedded systems

## Adding New Models

To add support for a new model type:

1. Create a new operator in the toolkit
2. Implement the model loading and inference logic
3. Update the generic sample application
4. Document the model requirements

## ✅ SUCCESSFUL NeMo Cache-Aware Export (May 31, 2025)

### Exact Steps That Worked
1. **Download fresh model**: `python3 download_and_export_fresh.py`
2. **Fix pytorch-lightning dependency**: Create `/pytorch_lightning/utilities/combined_loader.py`
3. **Export with cache support**: `python3 final_working_export.py`

### Critical Configuration
```python
# THE KEY LINE - NEVER FORGET THIS
model.set_export_config({'cache_support': 'True'})
```

### Verification Commands
```bash
# Check encoder model has cache tensors
python3 -c "
import onnx
m = onnx.load('models/nemo_fresh_export/encoder-fastconformer_cache_final.onnx')
print('Inputs:', [i.name for i in m.graph.input])
print('Outputs:', [o.name for o in m.graph.output])
cache_inputs = [i.name for i in m.graph.input if 'cache' in i.name]
print('Cache tensors:', cache_inputs)
"
```

**Expected output**: Should show cache_last_channel, cache_last_time, cache_last_channel_len inputs

### What Was Fixed
- **Problem**: Model exported without cache support by default
- **Root cause**: Missing `model.set_export_config({'cache_support': 'True'})`
- **Result**: Model only output repeated "sh" tokens
- **Solution**: Proper cache-aware export with state preservation

### Understanding NeMo Export Format
- **Separate files are CORRECT**: FastConformer-Hybrid exports as encoder+decoder components
- **Encoder (456MB)**: Handles audio input with cache tensors for streaming
- **Decoder (21MB)**: Handles text generation with RNN states
- **LSTM warnings**: Normal for RNN components, doesn't affect functionality

## Troubleshooting

### NeMo Models
- **"Only outputs 'sh' repeatedly"**: Model exported without cache support
- **"pytorch_lightning.utilities.combined_loader not found"**: Create missing module file
- **"Model corrupted"**: Download fresh .nemo file
- Ensure Python 3.9+ is installed
- Install required packages: `pip install nemo-toolkit`

### ONNX Models
- Verify ONNX Runtime is installed
- Check model input/output shapes (should have 4+ for cache models)
- Ensure vocabulary file matches model