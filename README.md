# Teracloud Streams Speech-to-Text Toolkit

A production-ready speech-to-text toolkit for IBM Streams using NVIDIA NeMo FastConformer models with native C++ implementation.

## Status: WORKING ✅ (June 2025)

Successfully implemented real-time speech recognition with:
- ✅ **NVIDIA NeMo FastConformer** CTC model (114M parameters)
- ✅ **Native C++ implementation** using ONNX Runtime
- ✅ **Full SPL integration** with working operators
- ✅ **Production-quality transcription** accuracy

## Quick Start

### Prerequisites
- IBM Streams 7.2.0+
- ONNX Runtime 1.16.3 (included)
- Python 3.9+ (for model export only)

### 1. Set Up Environment
```bash
export STREAMS_INSTALL=/path/to/streams/7.2.0.0
source $STREAMS_INSTALL/bin/streamsprofile.sh
```

### 2. Export NeMo Model (One-time Setup)
```bash
# Download and export model to ONNX format
python export_nemo_ctc_simple.py
# Creates: models/fastconformer_ctc_export/model.onnx (459MB)
```

### 3. Build Toolkit
```bash
# Build interface library
cd impl
make -f Makefile.nemo_interface

# Generate toolkit
cd ..
spl-make-toolkit -i . --no-mixed-mode -m
```

### 4. Run Sample Application
```bash
cd samples
make BasicNeMoDemo

# Run the sample
cd output/BasicNeMoDemo
./bin/standalone
```

**Expected Output**:
```
BasicNeMoDemo Transcription: we are expanding together shoulder to shoulder all working for one common good...
```

## Key Features

### ✅ Working Implementation
- **NeMo FastConformer CTC**: State-of-the-art speech recognition
- **Interface Library Pattern**: Solves ONNX header compatibility issues
- **Kaldi Feature Extraction**: Robust audio preprocessing
- **Pure C++ Performance**: No Python dependencies at runtime

### 📁 Architecture
```
Audio Input → FileAudioSource → NeMoSTT Operator → Transcription Output
                                    ↓
                            Interface Library
                                    ↓
                            ONNX Runtime + Model
```

## Toolkit Components

### Operators

#### NeMoSTT
Primary speech recognition operator using NeMo models.

**Input**: `tuple<blob audioChunk, uint64 audioTimestamp>`  
**Output**: `tuple<rstring transcription>`  
**Parameters**:
- `modelPath` - Path to ONNX model file
- `tokensPath` - Path to vocabulary file
- `audioFormat` - Audio format (default: mono16k)

#### FileAudioSource
Reads audio files and streams chunks for processing.

**Output**: `tuple<blob audioChunk, uint64 audioTimestamp>`  
**Parameters**:
- `filename` - Path to WAV file
- `blockSize` - Chunk size in bytes
- `sampleRate` - Audio sample rate

### Sample Applications

#### BasicNeMoDemo
Complete working example demonstrating speech recognition on IBM culture audio file.

```spl
stream<rstring transcription> Transcription = NeMoSTT(AudioStream) {
    param
        modelPath: "/path/to/model.onnx";
        tokensPath: "/path/to/tokens.txt";
        audioFormat: mono16k;
}
```

## Technical Details

### Model Information
- **Name**: nvidia/stt_en_fastconformer_hybrid_large_streaming_multi
- **Type**: FastConformer with CTC decoder
- **Size**: 459MB (ONNX format)
- **Vocabulary**: 1024 SentencePiece tokens
- **Performance**: ~30x real-time on CPU

### Key Innovation: Interface Library Pattern
Solves ONNX Runtime C++17 header incompatibility with Streams compiler:
- Pure virtual interface exposed to SPL
- ONNX headers isolated in implementation
- Factory pattern for clean instantiation

See [NEMO_INTEGRATION_GUIDE.md](NEMO_INTEGRATION_GUIDE.md) for detailed technical documentation.

## Directory Structure
```
com.teracloud.streamsx.stt/
├── com.teracloud.streamsx.stt/    # SPL operators
│   ├── NeMoSTT/                   # Speech recognition operator
│   └── FileAudioSource.spl        # Audio input operator
├── impl/                          # C++ implementation
│   ├── include/                   # Interface headers
│   ├── src/                       # Implementation
│   └── lib/                       # Built libraries
├── models/                        # ONNX models (excluded from git)
├── samples/                       # Example applications
└── test_data/                     # Test audio files
```

## Building from Source

### Dependencies
- IBM Streams development environment
- C++14 compiler (g++ 7.3+)
- Python with NeMo ASR toolkit (for model export)
- ONNX Runtime 1.16.3 (included in deps/)

### Build Steps
```bash
# 1. Export model (if not done)
python export_nemo_ctc_simple.py

# 2. Build implementation library
cd impl && make -f Makefile.nemo_interface

# 3. Generate toolkit
cd .. && spl-make-toolkit -i . --no-mixed-mode -m

# 4. Build samples
cd samples && make all
```

## Troubleshooting

### Common Issues

**"noexcept" compilation error**
- Ensure using interface library, not direct ONNX headers
- Check `impl/lib/libnemo_ctc_interface.so` exists

**Model not found**
- Use absolute paths in SPL files
- Verify model export completed successfully

**Poor transcription quality**
- Ensure 16kHz, mono, 16-bit PCM audio format
- Check audio quality and noise levels

## Documentation

- [ARCHITECTURE.md](ARCHITECTURE.md) - System architecture and design
- [NEMO_INTEGRATION_GUIDE.md](NEMO_INTEGRATION_GUIDE.md) - Detailed implementation guide
- [samples/README.md](samples/README.md) - Sample applications guide

## Model Distribution

Large model files (>100MB) are excluded from the git repository. To obtain models:
1. Run the export script: `python export_nemo_ctc_simple.py`
2. Or download pre-exported models from your team's model repository

## License

Teracloud Proprietary

## Acknowledgments

- NVIDIA NeMo team for the FastConformer model
- Kaldi project for audio feature extraction
- ONNX Runtime team for inference framework