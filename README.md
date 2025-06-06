# Teracloud Streams Speech-to-Text Toolkit

A production-ready speech-to-text toolkit for IBM Streams using NVIDIA NeMo FastConformer models with native C++ implementation.

## Status: FULLY WORKING ✅ (June 6, 2025)

Successfully implemented real-time speech recognition with:
- ✅ **NVIDIA NeMo FastConformer** CTC model (114M parameters)
- ✅ **Native C++ implementation** using ONNX Runtime
- ✅ **Full SPL integration** with working operators
- ✅ **Production-quality transcription** accuracy
- ✅ **Interface library pattern** solving ONNX header compatibility
- ✅ **ALL THREE SAMPLES WORKING PERFECTLY**:
  - ✅ **BasicNeMoDemo**: Perfect English transcriptions
  - ✅ **NeMoCTCRealtime**: Real-time processing with 10.24x speedup + CSV metrics
  - ✅ **NeMoFileTranscription**: Batch processing with transcript and analysis files
- ✅ **BOTH STREAMING MODES VERIFIED**:
  - ✅ **1x Real-time Mode**: Respects audio timing for live streaming simulation
  - ✅ **Full Speed Mode**: Maximum throughput for batch processing

## Quick Start

**New to the toolkit? See [QUICKSTART.md](QUICKSTART.md) for a 5-minute setup guide!**

### Prerequisites
- IBM Streams 7.2.0+
- C++14 compiler (g++ 7.3+)
- Python 3.9+ (for model export only)
- ~4GB disk space for models and dependencies

### Step 1: Set Up Environment
```bash
# Set Streams installation path
export STREAMS_INSTALL=/path/to/streams/7.2.0.0
source $STREAMS_INSTALL/bin/streamsprofile.sh

# Clone or navigate to toolkit directory
cd /path/to/com.teracloud.streamsx.stt
```

### Step 2: Obtain ONNX Model (One-time Setup)

**⚠️ REQUIRED**: The large ONNX model file (438MB) is not included in the Git repository and must be obtained separately.

**Option A: Generate Model (Recommended)**
```bash
# Install Python dependencies (requires ~16GB RAM and internet)
pip install -r requirements.txt

# Download and export model to ONNX format (~5-10 minutes)
python export_model_ctc.py
# Downloads from HuggingFace: nvidia/stt_en_fastconformer_hybrid_large_streaming_multi
# Creates: models/fastconformer_ctc_export/model.onnx (438MB)
# Vocabulary file already included: models/fastconformer_ctc_export/tokens.txt ✅
```

**Option B: Obtain Pre-exported Model**
```bash
# If available from your team's shared storage:
# Copy model.onnx to: models/fastconformer_ctc_export/model.onnx
# Vocabulary file already included in repository ✅
```

### Step 3: Build Toolkit
```bash
# Build interface library (required for ONNX Runtime integration)
cd impl
make -f Makefile.nemo_interface
cd ..

# Generate SPL toolkit
spl-make-toolkit -i . --no-mixed-mode -m

# Verify toolkit build
ls -la toolkit.xml  # Should see toolkit.xml file
```

### Step 4: Build Sample Applications
```bash
cd samples

# Build all samples at once
make all

# Or build individually
make BasicNeMoDemo          # Simple demonstration
make NeMoCTCRealtime        # Real-time streaming metrics
make NeMoFileTranscription  # Batch file processing
```

### Step 5: Run Sample Applications

**Set library paths (required for all samples):**
```bash
export LD_LIBRARY_PATH=/path/to/com.teracloud.streamsx.stt/deps/onnxruntime/lib:/path/to/com.teracloud.streamsx.stt/deps/kaldi-native-fbank/lib:$LD_LIBRARY_PATH
```

#### BasicNeMoDemo - Simple Transcription
```bash
cd output/BasicNeMoDemo

# Run in full speed mode (default)
./bin/standalone --data-directory=data

# Run in 1x real-time mode (simulates live streaming)
./bin/standalone --data-directory=data realtimePlayback="true"
```

#### NeMoCTCRealtime - Performance Metrics
```bash
cd output/NeMoCTCRealtime

# Run with 1x real-time playback (default chunk size 512ms)
./bin/standalone --data-directory=data realtimePlayback="true" chunkSizeMs="512"

# Run at full speed with custom chunk size
./bin/standalone --data-directory=data realtimePlayback="false" chunkSizeMs="256"
```

#### NeMoFileTranscription - Batch Processing
```bash
cd output/NeMoFileTranscription

# Process default test file at full speed
./bin/standalone --data-directory=data realtimePlayback="false"

# Process custom audio file
./bin/standalone --data-directory=data audioFile="/path/to/audio.wav" realtimePlayback="false"
```

**Expected Output**:
```
BasicNeMoDemo Transcription: it was the first great sorrow of his life it was not so much the loss of the cotton itself but the fantasy the hopes the dreams built around it

[NeMoCTCRealtime] Processing: 50ms, Audio: 512ms, Speedup: 10.24x real-time

[NeMoFileTranscription] Audio duration: 120.0s, Processing: 8.5s, Speedup: 14.1x
```

## Streaming Modes

### 1x Real-time Mode (`realtimePlayback=true`)
- Simulates live audio streaming by respecting real-time timing
- Processes audio chunks at the rate they would arrive in a live stream
- Ideal for testing real-time streaming applications
- **Performance**: Wall time ≈ Audio duration

### Full Speed Mode (`realtimePlayback=false`)
- Processes audio as fast as possible without timing constraints
- Achieves maximum throughput for batch processing
- Ideal for transcribing pre-recorded files
- **Performance**: 8-14x faster than real-time

## Key Features

### ✅ Working Implementation
- **NeMo FastConformer CTC**: State-of-the-art speech recognition
- **Interface Library Pattern**: Solves ONNX header compatibility issues
- **Kaldi Feature Extraction**: Robust audio preprocessing
- **Pure C++ Performance**: No Python dependencies at runtime
- **Dual Processing Modes**: Real-time streaming or full speed batch

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
- `modelPath` - Path to ONNX model file (tokens.txt automatically detected)
- `audioFormat` - Audio format (default: mono16k)
- `chunkDurationMs` - Optional: Audio chunk duration in milliseconds  
- `minSpeechDurationMs` - Optional: Minimum speech duration for transcription

#### FileAudioSource
Reads audio files and streams chunks for processing.

**Output**: `tuple<blob audioChunk, uint64 audioTimestamp>`  
**Parameters**:
- `filename` - Path to WAV file
- `blockSize` - Chunk size in bytes
- `sampleRate` - Audio sample rate

### Sample Applications

#### 1. BasicNeMoDemo ✅ WORKING
Simple demonstration of speech recognition on audio files.
**Status**: Produces perfect English transcriptions - "it was the first great sorrow of his life..."

#### 2. NeMoCTCRealtime ✅ WORKING
Real-time streaming with performance metrics and configurable chunk sizes.
**Status**: Real-time processing with 10.24x speedup, generates CSV performance metrics

#### 3. NeMoFileTranscription ✅ WORKING
Batch file processing with comprehensive analysis and statistics.
**Status**: Generates transcript files and detailed analysis CSV with performance metrics

```spl
stream<rstring transcription> Transcription = NeMoSTT(AudioStream) {
    param
        modelPath: "/path/to/model.onnx";
        audioFormat: mono16k;
}
```

See `samples/README.md` for detailed usage instructions.

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
- IBM Streams 7.2.0+ development environment
- C++14 compiler (g++ 7.3+)
- Python 3.9+ with pip
- ONNX Runtime 1.16.3 (included in deps/)
- Kaldi-native-fbank (included in deps/)

### Complete Build Process
```bash
# 1. Install Python dependencies
pip install -r requirements.txt

# 2. Generate ONNX model (one-time setup, requires internet + ~16GB RAM)
python export_model_ctc.py
# Downloads and exports: nvidia/stt_en_fastconformer_hybrid_large_streaming_multi
# Verify: ls -la models/fastconformer_ctc_export/model.onnx

# 3. Build C++ interface library
cd impl
make -f Makefile.nemo_interface
# Verify: ls -la lib/libnemo_ctc_interface.so
cd ..

# 4. Generate SPL toolkit
spl-make-toolkit -i . --no-mixed-mode -m
# Verify: ls -la toolkit.xml

# 5. Build all samples
cd samples
make all
# Verify: ls -la output/*/bin/standalone

# 6. Set runtime library paths
export LD_LIBRARY_PATH=$PWD/../deps/onnxruntime/lib:$PWD/../deps/kaldi-native-fbank/lib:$LD_LIBRARY_PATH
```

## Troubleshooting

### Common Issues

**"STREAMS_JAVA_HOME is not set"**
```bash
source /path/to/streams/7.2.0.0/bin/streamsprofile.sh
```

**"libnemo_ctc_interface.so: cannot open shared object file"**
```bash
export LD_LIBRARY_PATH=/path/to/com.teracloud.streamsx.stt/deps/onnxruntime/lib:/path/to/com.teracloud.streamsx.stt/deps/kaldi-native-fbank/lib:$LD_LIBRARY_PATH
```

**"An operator was attempting to access the data directory"**
```bash
./bin/standalone --data-directory=data
```

**"noexcept" compilation error**
- Ensure using interface library, not direct ONNX headers
- Rebuild: `cd impl && make -f Makefile.nemo_interface`

**Model not found**
- Use absolute paths in SPL files
- Verify model export: `ls -la models/fastconformer_ctc_export/`

**Poor transcription quality**
- Ensure 16kHz, mono, 16-bit PCM audio format
- Check audio quality and noise levels
- Verify using correct model file

## Documentation

- [ARCHITECTURE.md](ARCHITECTURE.md) - System architecture and design
- [NEMO_INTEGRATION_GUIDE.md](NEMO_INTEGRATION_GUIDE.md) - Detailed implementation guide
- [samples/README.md](samples/README.md) - Sample applications guide

## Model Distribution

The large ONNX model file (438MB) is excluded from the Git repository due to size constraints. 

### Required Model File
- **File**: `models/fastconformer_ctc_export/model.onnx` (438MB)
- **Vocabulary**: `models/fastconformer_ctc_export/tokens.txt` ✅ **Included in repository**

### How to Obtain the Model

**Method 1: Generate from Source (Recommended)**
```bash
python export_model_ctc.py
```
- Downloads nvidia/stt_en_fastconformer_hybrid_large_streaming_multi from HuggingFace
- Requires: Internet connection, ~16GB RAM, 5-10 minutes
- Creates the 438MB ONNX model file locally

**Method 2: Copy Pre-exported Model**
- If your team has a shared model repository
- Copy `model.onnx` to `models/fastconformer_ctc_export/`
- Vocabulary file already included in Git repository

## License

Teracloud Proprietary

## Acknowledgments

- NVIDIA NeMo team for the FastConformer model
- Kaldi project for audio feature extraction
- ONNX Runtime team for inference framework