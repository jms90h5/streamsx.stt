# Quick Start Guide - STT Toolkit

Get the NeMo Speech-to-Text toolkit running in under 10 minutes!

## Prerequisites

- IBM Streams 7.2.0+
- Python 3.9+
- 4GB free disk space
- 16GB RAM recommended

## 5-Minute Setup

### 1. Clone and Enter Directory
```bash
git clone <your-repo-url>
cd com.teracloud.streamsx.stt
```

### 2. Set Up Streams Environment
```bash
export STREAMS_INSTALL=/path/to/streams/7.2.0.0
source $STREAMS_INSTALL/bin/streamsprofile.sh
```

### 3. Get Required Model (One-time, ~5 minutes)

**⚠️ Required**: The 438MB ONNX model is not in Git and must be obtained:

```bash
# Install dependencies (requires ~16GB RAM + internet)
pip install -r requirements.txt

# Download and export model from HuggingFace
python export_model_ctc.py
# Creates: models/fastconformer_ctc_export/model.onnx (438MB)
# Vocabulary already included: models/fastconformer_ctc_export/tokens.txt ✅

# Verify model exists
ls -lh models/fastconformer_ctc_export/model.onnx
# Should show: ~438MB file
```

### 4. Build Toolkit (~2 minutes)
```bash
# Build C++ library
cd impl && make -f Makefile.nemo_interface && cd ..

# Generate toolkit
spl-make-toolkit -i . --no-mixed-mode -m

# Build samples
cd samples && make all && cd ..
```

### 5. Run Your First Transcription!
```bash
# Set library paths
export LD_LIBRARY_PATH=$PWD/deps/onnxruntime/lib:$PWD/deps/kaldi-native-fbank/lib:$LD_LIBRARY_PATH

# Run basic demo
cd samples/output/BasicNeMoDemo
./bin/standalone --data-directory=data
```

**Expected Output:**
```
BasicNeMoDemo Transcription: it was the first great sorrow of his life it was not so much the loss of the cotton itself but the fantasy the hopes the dreams built around it
```

## Try Different Modes

### Real-time Streaming (simulates live audio)
```bash
cd ../NeMoCTCRealtime
./bin/standalone --data-directory=data realtimePlayback="true"
```

### High-Speed Batch Processing
```bash
cd ../NeMoFileTranscription
./bin/standalone --data-directory=data realtimePlayback="false"
```

## What's Next?

- Read the full [README.md](README.md) for detailed documentation
- Check [samples/README.md](samples/README.md) for all sample applications
- See [ARCHITECTURE.md](ARCHITECTURE.md) for technical details

## Need Help?

Common issues:
- **"STREAMS_JAVA_HOME not set"** → Run `source $STREAMS_INSTALL/bin/streamsprofile.sh`
- **"library not found"** → Set `LD_LIBRARY_PATH` as shown above
- **"data directory"** → Always use `--data-directory=data`

## Performance

- **Real-time mode**: Processes audio at 1x speed (live streaming simulation)
- **Full speed mode**: 8-14x faster than real-time
- **Model**: NVIDIA NeMo FastConformer (114M parameters)
- **Accuracy**: Production-grade English transcription