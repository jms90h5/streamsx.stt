# TeraCloud Streams Speech-to-Text Toolkit

A high-performance speech-to-text toolkit for Teracloud Streams using ONNX Runtime and the Zipformer RNN-T architecture.

## Quick Start

### Prerequisites
- Teracloud Streams 7.2+ installed and configured
- GCC 4.8+ (C++11 support)
- GNU Make

### Build and Run in 5 Minutes

```bash
# 1. Set up Streams environment
export STREAMS_INSTALL=/path/to/streams  # e.g., /homes/jsharpe/teracloud/streams/7.2.0.0
source $STREAMS_INSTALL/bin/streamsprofile.sh

# 2. Download required models (one-time setup)
./download_sherpa_onnx_model.sh

# 3. Build the implementation library
cd impl
make clean && make

# 4. Build the toolkit
cd ..
make

# 5. Build and run the sample
cd samples/CppONNX_OnnxSTT
make
./output/bin/standalone --data-directory $(pwd)
```

Expected output:
```
Loading encoder from: ../../models/sherpa_onnx_paraformer/...
Loading decoder from: ../../models/sherpa_onnx_paraformer/...
Loading joiner from: ../../models/sherpa_onnx_paraformer/...
Loaded 6257 tokens, blank_id=0
ZipformerRNNT initialized successfully
[Transcription output...]
```

## Overview

This toolkit provides real-time speech-to-text capabilities for Teracloud Streams applications using:

- **Zipformer RNN-T** architecture for streaming ASR
- **ONNX Runtime** for efficient inference
- **Chunk-based processing** for low latency (390ms chunks)
- **Chinese/English bilingual** support (current model)

### Key Features
- ✅ Streaming speech recognition with ~0.4x real-time factor on CPU
- ✅ No external dependencies (ONNX Runtime included)
- ✅ SPL operator integration for easy Streams development
- ✅ Verified working implementation as of 2025-05-27

### Current Limitations
- ❌ No voice activity detection (VAD)
- ❌ No feature extraction pipeline (uses simple filterbank)
- ❌ Transcription quality issues (many "S" tokens)
- ❌ Fixed chunk size (39 frames / 390ms)

## Architecture

The toolkit implements a three-model Zipformer RNN-T pipeline:

```
Audio Input (16kHz)
    ↓
SPL FileSource
    ↓
OnnxSTT Operator
    ↓
C++ Implementation (OnnxSTTImpl)
    ↓
ZipformerRNNT Engine
    ├── Encoder (ONNX) - Processes features with 35 cache tensors
    ├── Decoder (ONNX) - Beam search with configurable beam size
    └── Joiner (ONNX)  - Combines encoder/decoder outputs
    ↓
Transcription Text
```

### Cache Management
The Zipformer maintains 35 cache tensors (7 types × 5 layers) for streaming:
- `cached_len`: Frame counts
- `cached_avg`: Running averages
- `cached_key/val/val2`: Attention states
- `cached_conv1/conv2`: Convolution states

## Real-time Design Principles

This toolkit is optimized for **real-time** speech-to-text with minimal latency:

### Key Principles
- **Process chunks immediately** - No batching or accumulation delays
- **Stream partial results** - Users see transcription while speaking  
- **Minimal buffering** - Only buffer to align with model chunk size (390ms)
- **Consistent latency** - Target <150ms for real-time feel

### Why No Batching?
```
With batching (BAD):     Without batching (GOOD):
Audio chunks: [..][..][..] → Process    Audio: [..] → Process → Result
Latency: 500ms+ 😱                      Latency: ~100ms 🚀
```

### Implementation Details
- **39-frame chunks**: Fixed by model training (390ms at 100 fps)
- **Immediate processing**: Each chunk processed as it arrives
- **Cache management**: 35 tensors maintain streaming state between chunks
- **Beam search**: Configurable beam size for accuracy/speed tradeoff

### Performance Targets
- **Excellent**: < 150ms (feels instantaneous)
- **Good**: 150-300ms (barely noticeable)  
- **Poor**: > 500ms (noticeable lag)

## Directory Structure

```
com.teracloud.streamsx.stt/
├── impl/                      # C++ implementation
│   ├── include/              # Headers only
│   ├── src/                  # Source files
│   ├── build/                # Build artifacts (generated)
│   └── lib/                  # Output library
├── streamsx.stt/             # SPL operators (shorter namespace)
│   ├── OnnxSTT/             # Main STT operator
│   └── FileAudioSource.spl  # Audio input operator
├── models/                    # Required models
│   └── sherpa_onnx_paraformer/
├── samples/                   # Example applications
│   └── CppONNX_OnnxSTT/     # Working sample
├── deps/                      # Dependencies
│   └── onnxruntime/         # ONNX Runtime 1.16.3
└── test_data/                # Test audio files
```

## Model Requirements

The toolkit requires Sherpa-ONNX Zipformer models with specific schema:

**Location**: `models/sherpa_onnx_paraformer/sherpa-onnx-streaming-zipformer-bilingual-zh-en-2023-02-20/`

**Required files**:
- `encoder-epoch-99-avg-1.onnx` - Encoder with 36 inputs (features + 35 cache tensors)
- `decoder-epoch-99-avg-1.onnx` - RNN-T decoder
- `joiner-epoch-99-avg-1.onnx` - Output projection
- `tokens.txt` - Vocabulary (6257 tokens)

**Critical**: WeNet ONNX models have incompatible schemas and will NOT work!

## Building from Source

### Implementation Library
```bash
cd impl
make                    # Builds to lib/libs2t_impl.so
make clean             # Removes build artifacts
```

The Makefile:
- Compiles from `src/` with headers in `include/`
- Generates objects in `build/` (not in source directories)
- Links against ONNX Runtime in `deps/`

### SPL Toolkit
```bash
make                    # Indexes SPL operators
make clean             # Removes toolkit.xml
```

## Troubleshooting

### Common Issues

**"Model input/output schema mismatch"**
- Cause: Using wrong model format
- Solution: Use only Sherpa-ONNX Zipformer models

**"not enough space: expected 12480, got 11840"**
- Cause: Chunk size mismatch
- Solution: Already fixed - OnnxSTTImpl pads to 39 frames

**"undefined symbol: _ZN13ZipformerRNNT..."**
- Cause: Missing source file in build
- Solution: Check impl/Makefile includes all .cpp files

**Poor transcription (many "S" tokens)**
- Cause: Model/preprocessing mismatch
- Status: Known issue, needs proper feature extraction

### Recovery
If the toolkit stops working:
```bash
# Restore from backup
tar -xzf /path/to/com.teracloud.streamsx.stt_working_backup_20250527_222149.tar.gz

# Rebuild everything
cd impl && make clean && make
cd .. && make
cd samples/CppONNX_OnnxSTT && make
```

## Development Notes

### Why Things Are The Way They Are

1. **Fixed 39-frame chunks**: The Zipformer model was trained with this specific chunk size
2. **35 cache tensors**: Required by the streaming Zipformer architecture
3. **No WeNet dependency**: Originally included but discovered to be unnecessary
4. **Simple feature extraction**: Proper kaldifeat integration pending

### Known Issues
- ONNX Runtime version mismatch (using 1.16.3, docs reference 1.11.0)
- Missing CMVN normalization
- No voice activity detection
- Feature extraction needs improvement

### Future Improvements
1. Implement proper feature extraction with kaldifeat
2. Add voice activity detection (Silero VAD)
3. Support variable chunk sizes
4. Improve model quality

## Additional Documentation

- **[ARCHITECTURE.md](ARCHITECTURE.md)** - Technical details, design decisions, and historical context
- **[samples/README.md](samples/README.md)** - Guide to sample applications and choosing the right implementation
- **[test_data/README.md](test_data/README.md)** - Information about test audio files and verification

## License

Apache License 2.0

## Support

This toolkit is under active development. For issues:
1. Check the Troubleshooting section
2. Verify model compatibility
3. Ensure correct build order (impl → toolkit → sample)
4. Test with provided audio files in test_data/