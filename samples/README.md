# Sample Applications

This directory contains working sample applications demonstrating complete Streams SPL integration with NeMo FastConformer speech recognition.

## Available Samples

### 1. BasicNeMoDemo ✅ WORKING
**Complete demonstration** of NeMo FastConformer CTC speech recognition.

- **File**: `BasicNeMoDemo.spl`
- **Technology**: Native C++ with interface library pattern
- **Model**: NeMo FastConformer CTC (ONNX export)
- **Features**:
  - Audio file input via FileAudioSource
  - Real speech recognition processing
  - Text output with transcription results
  - File saving for transcript results
- **Status**: ✅ **Built and tested successfully**

### 2. Sample Build System
- **Makefile**: Unified build system for all samples
- **Dependencies**: Automatic toolkit dependency checking
- **Outputs**: Complete `.sab` bundles ready for deployment

## Quick Start

### Prerequisites
```bash
# Source Streams environment
source /path/to/streams/7.2.0.0/bin/streamsprofile.sh

# Ensure toolkit is built
cd ..
make -f impl/Makefile.nemo_interface
spl-make-toolkit -i . --no-mixed-mode -m
```

### Build and Run
```bash
cd samples

# Build BasicNeMoDemo
make BasicNeMoDemo

# Run the sample
cd output/BasicNeMoDemo
./bin/standalone
```

## Expected Output

The sample processes `test_data/audio/11-ibm-culture-2min-16k.wav` and produces transcription output:

```
BasicNeMoDemo Transcription: we are expanding together shoulder to shoulder all working for one common good...
```

Transcript files are saved to `output/BasicNeMoDemo/data/` with timestamps.

## Sample Architecture

```spl
// Audio input from file
stream<blob audioChunk, uint64 audioTimestamp> AudioStream = FileAudioSource() {
    param
        filename: "/absolute/path/to/audio.wav";
        blockSize: 16384u;
        sampleRate: 16000;
}

// NeMo speech recognition  
stream<rstring transcription> Transcription = NeMoSTT(AudioStream) {
    param
        modelPath: "/absolute/path/to/model.onnx";
        tokensPath: "/absolute/path/to/tokens.txt";
        audioFormat: mono16k;
}

// Display and save results
() as Display = Custom(Transcription) {
    logic onTuple Transcription: {
        printStringLn("Transcription: " + transcription);
    }
}
```

## Technical Implementation

### Interface Library Pattern
- **Problem**: ONNX Runtime headers contain C++17 `noexcept` keyword incompatible with Streams compiler
- **Solution**: Interface library (`libnemo_ctc_interface.so`) isolates ONNX headers from SPL compilation
- **Result**: Clean compilation and successful execution

### Key Components
- `NeMoCTCInterface.hpp`: Pure virtual interface
- `NeMoCTCInterface.cpp`: Factory implementation with ONNX dependencies
- `impl/lib/libnemo_ctc_interface.so`: Built library containing implementation

## Troubleshooting

### Common Issues

**"Model file not found"**
- Use absolute paths in SPL files
- Verify model exists at specified path

**"libnemo_ctc_interface.so not found"**
- Rebuild interface library: `make -f impl/Makefile.nemo_interface`
- Check `impl/lib/` directory

**"undefined symbol"**
- Ensure all dependencies built correctly
- Rebuild toolkit: `spl-make-toolkit -i . --no-mixed-mode -m`

### Debug Mode
```bash
export STREAMS_LOG_LEVEL=debug
./bin/standalone
```

## Creating New Samples

Use BasicNeMoDemo as template:
1. Copy SPL structure
2. Update audio file paths
3. Modify model paths as needed
4. Add to samples/Makefile

## Performance

- **Model**: FastConformer CTC (114M parameters)
- **Processing**: Real-time capable 
- **Memory**: ~200MB during inference
- **Accuracy**: Production-grade English speech recognition

## Requirements

- Teracloud Streams 7.2.0+
- NeMo FastConformer CTC model (exported to ONNX)
- Test audio files (16kHz, mono, WAV format)
- Built interface library and toolkit