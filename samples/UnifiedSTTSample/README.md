# Unified STT Sample

This sample demonstrates both STT operators using the same audio processing pipeline, making it easy to compare different speech recognition approaches.

## Quick Start

```bash
# Test ONNX FastConformer model
make run-onnx

# Test NeMo native model  
make run-nemo

# Compare both approaches
make test
```

## Available Operators

### OnnxSTTDemo
Uses the `OnnxSTT` operator with NVIDIA FastConformer model exported to ONNX format.

**Features**:
- C++ ONNX Runtime backend
- Configurable execution providers (CPU/CUDA/TensorRT)
- Multi-threaded processing
- Works with any ONNX speech model

**Models**:
- FastConformer (114M parameters)
- Zipformer RNN-T (future)
- Custom ONNX models

### NeMoSTTDemo  
Uses the `NeMoSTT` operator with native NVIDIA NeMo .nemo model files.

**Features**:
- Native NeMo model support
- Optimized for NeMo architectures
- Cache-aware streaming
- Direct .nemo file loading

**Models**:
- FastConformer .nemo files
- Any compatible NeMo model

## Sample Architecture

Both samples follow the same pipeline:

```
Audio File → FileAudioSource → STT Operator → STTResultProcessor → Results
```

### Common Components

1. **FileAudioSource**: Reads audio in 100ms chunks with timestamps
2. **STTResultProcessor**: Unified result analysis and display
3. **Performance metrics**: Latency, throughput, confidence analysis

### Key Differences

| Aspect | OnnxSTTDemo | NeMoSTTDemo |
|--------|-------------|-------------|
| Model Format | ONNX (.onnx) | NeMo (.nemo) |
| Backend | ONNX Runtime | Native NeMo |
| GPU Support | Yes (CUDA/TensorRT) | Limited |
| Threading | Configurable | Single thread |
| Memory Usage | Higher | Lower |

## Build Options

### Basic Building
```bash
make onnx           # Build ONNX version
make nemo           # Build NeMo version
make clean          # Clean all outputs
```

### Running Samples
```bash
make run-onnx       # Build and run ONNX
make run-nemo       # Build and run NeMo
make test           # Run both and compare
```

### Configuration
```bash
# Custom audio file
make run-onnx AUDIO_FILE=/path/to/audio.wav

# GPU acceleration (ONNX only)
make run-onnx PROVIDER=CUDA

# Multi-threading (ONNX only)  
make run-onnx THREADS=8

# Different chunk sizes
make run-onnx BLOCK_SIZE=1600 CHUNK_SIZE=50  # Fast, lower accuracy
make run-onnx BLOCK_SIZE=6400 CHUNK_SIZE=200 # Slow, higher accuracy
```

## Expected Output

### Real-time Display
```
[1] PARTIAL: we are expanding (confidence: 0.95)
[2] FINAL: we are expanding together (confidence: 0.98) 
[3] PARTIAL: shoulder to (confidence: 0.92)
[4] FINAL: shoulder to shoulder (confidence: 0.97)
...
```

### Final Summary
```
======================================================================
    UNIFIED STT SAMPLE - TRANSCRIPTION COMPLETE
======================================================================
Model Type: FastConformer (ONNX)
Model File: ../../models/nemo_fastconformer_streaming/conformer_ctc_dynamic.onnx
Processing: ONNX Runtime
Provider: CPU (4 threads)
Audio File: ../../test_data/audio/11-ibm-culture-2min-16k.wav
----------------------------------------------------------------------
PERFORMANCE METRICS:
Total processing time: 12 seconds
Time to first result: 1 seconds
Total segments: 245
Final segments: 128
Partial segments: 117
Average confidence: 0.94
Confidence range: 0.75 - 0.99
----------------------------------------------------------------------
FULL TRANSCRIPT:
----------------------------------------------------------------------
we are expanding together shoulder to shoulder all working for one common good...
======================================================================
```

## Troubleshooting

### Build Issues
```bash
# Check toolkit status
make check-toolkit

# Verify models are available
make check-models

# Rebuild toolkit if needed
cd ../../.. && spl-make-toolkit -i . --no-mixed-mode -m
```

### Runtime Issues

**"Model file not found"**
- Check model paths in sample parameters
- Ensure models are downloaded: `cd ../.. && ./download_models.sh`

**"Operator not found"**  
- Regenerate toolkit: `make check-toolkit`
- Verify toolkit.xml includes both operators

**Poor transcription quality**
- Try different chunk sizes: `make run-onnx CHUNK_SIZE=200`
- Check audio format (must be 16kHz, 16-bit, mono)
- Verify model compatibility

**Performance issues**
- For ONNX: Try GPU acceleration `make run-onnx PROVIDER=CUDA`
- Adjust thread count: `make run-onnx THREADS=2`
- Use smaller chunks for lower latency: `CHUNK_SIZE=50`

## Comparison Testing

Run comprehensive comparison:
```bash
make test
```

This will:
1. Build both samples
2. Run both with the same audio
3. Generate comparison report
4. Save detailed logs to `test_results_*.txt`

### Performance Comparison

Typical results on modern CPU:

| Metric | ONNX FastConformer | NeMo FastConformer |
|--------|-------------------|-------------------|
| Latency | ~100-200ms | ~50-100ms |
| Throughput | 0.1-0.3x realtime | 0.05-0.15x realtime |
| Memory | ~500MB | ~300MB |
| CPU Usage | 50% (4 threads) | 25% (1 thread) |
| Accuracy | High | High |

## Advanced Usage

### Custom Models
```bash
# Use different ONNX model
make run-onnx encoderModel=/path/to/model.onnx vocabFile=/path/to/vocab.txt

# Use different NeMo model  
make run-nemo modelPath=/path/to/model.nemo
```

### Integration Testing
```bash
# Test with different audio files
for audio in *.wav; do
    echo "Testing $audio..."
    make run-onnx AUDIO_FILE="$audio" > "results_${audio%.wav}.txt"
done
```

### Performance Tuning
```bash
# Optimize for speed
make config-fast

# Optimize for accuracy
make config-accurate  

# Optimize for GPU
make config-gpu
```

This unified approach makes it easy to compare the two STT operators and choose the best one for your specific use case.