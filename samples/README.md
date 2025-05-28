# Speech-to-Text Toolkit Samples

This directory contains sample applications demonstrating different approaches to implementing real-time speech-to-text using the TeraCloud Streams Speech-to-Text toolkit.

## Sample Applications

### Python-based Implementations

#### PythonONNX_MultiOperator
- **Technology**: Python with ONNX Runtime (multi-operator approach)
- **Operator**: Custom Python operators
- **Dependencies**: Requires `com.teracloud.streams.python` and `com.teracloud.streams.onnx` toolkits
- **Use Case**: Research and prototyping where component flexibility is needed
- **Latency**: ~200-300ms
- **Key Features**: Separate operators for feature extraction, encoding, and decoding
- **Status**: Reference implementation (requires additional toolkits)

#### PythonONNX_SingleOperator
- **Technology**: Python with embedded ONNX Runtime (single operator)
- **Operator**: Custom Python operator with all processing embedded
- **Dependencies**: Requires `com.teracloud.streams.python` toolkit
- **Use Case**: Production scenarios where Python is acceptable
- **Latency**: ~150-200ms
- **Key Features**: Better performance through reduced inter-operator communication
- **Status**: Reference implementation (requires additional toolkits)

### C++-based Implementations

#### CppONNX_OnnxSTT (Recommended)
- **Technology**: Pure C++ with ONNX Runtime
- **Models Supported**: 
  - Zipformer RNN-T (legacy)
  - **NVidia NeMo Cache-Aware Conformer (new)**
- **Pipeline**: VAD → Feature Extraction → ASR Model
- **Operator**: `OnnxSTT` (toolkit primitive operator)
- **Use Case**: Production real-time systems requiring low latency
- **Latency**: ~100-150ms
- **Key Features**:
  - **New**: NVidia NeMo FastConformer support (114M parameters, 13k hours training)
  - Voice Activity Detection (Silero VAD + energy fallback)
  - Advanced feature extraction (kaldifeat + simple_fbank fallback)
  - Cache-aware streaming for minimal latency
  - Uses kaldi-native-fbank for feature extraction
  - Supports CPU/CUDA/TensorRT providers
  - Self-contained deployment (no WeNet runtime needed)
- **Sample Applications**:
  - `ONNXRealtime.spl` - Original Zipformer implementation
  - `NeMoRealtime.spl` - New NeMo pipeline with VAD and advanced features
  - `test_nemo_standalone` - C++ standalone test without SPL

#### CppWeNet_STT
- **Technology**: C++ with full WeNet runtime
- **Operator**: `WenetSTT` (toolkit primitive operator)
- **Use Case**: Applications requiring full WeNet features
- **Latency**: ~100-150ms
- **Key Features**:
  - Complete WeNet implementation
  - Advanced features like VAD
  - WebSocket I/O for streaming

## Choosing the Right Implementation

### Use Python implementations when:
- Prototyping new features
- Integration with Python ML ecosystem is needed
- Latency requirements are moderate (150-300ms)
- Development speed is prioritized

### Use CppONNX_OnnxSTT when:
- Low latency is critical (<150ms)
- Deployment simplicity is important
- Cross-platform compatibility is needed
- GPU acceleration is required

### Use CppWeNet_WenetSTT when:
- Full WeNet features are needed
- Already have WeNet infrastructure
- Need advanced features like built-in VAD
- Willing to manage WeNet dependencies

## Real-time Design

This toolkit is optimized for real-time speech-to-text with minimal latency. Key principles are documented in the main README.md, including why batching is inappropriate for real-time STT applications.

## Self-Contained Design

All samples are now **self-contained** within the toolkit directory:

- **Models**: Located in `../models/` relative to samples
- **Test Data**: Located in `../test_data/audio/` 
- **No External Dependencies**: All required models and test audio files are included

### Available Test Data
- Real audio files (WAV format) for testing
- WeNet model with real CMVN statistics  
- ONNX models for CPU/GPU acceleration

### Sample Applications (Updated)

#### BasicWorkingExample
- **BasicDemo.spl**: Simple demonstration with mock data
- **TestAudioDemo.spl**: Uses real test audio files from `test_data/`
- **Purpose**: Verify toolkit installation and basic functionality

## Building and Running Samples

### Prerequisites

Before building any samples, ensure your Streams environment is properly configured:

```bash
# Source the Streams environment
source ~/teracloud/streams/7.2.0.0/bin/streamsprofile.sh

# Verify environment
echo $STREAMS_INSTALL
```

### Building Samples

Each sample directory contains:
- `README.md`: Specific instructions for that sample
- `Makefile`: Build configuration
- `*.spl`: Streams application source

#### Ready-to-Build Samples (C++)
```bash
# Build the ONNX C++ sample (recommended)
cd CppONNX_OnnxSTT
make

# Build the WeNet C++ sample  
cd CppWeNet_WenetSTT
make
```

#### Reference Samples (Python)
The Python samples require additional toolkits and are provided as reference implementations:
- `PythonONNX_MultiOperator`: Requires `com.teracloud.streams.python` and `com.teracloud.streams.onnx`
- `PythonONNX_SingleOperator`: Requires `com.teracloud.streams.python`

### Running Samples
```bash
streamtool submitjob output/<app_name>.sab
```

Refer to individual sample READMEs for specific parameters and configuration.

## NVidia NeMo Cache-Aware Conformer (New!)

The toolkit now supports NVidia NeMo FastConformer-Hybrid models with advanced pipeline features.

### Quick Start with NeMo

1. **Download and export NeMo model**:
   ```bash
   # Download NeMo model setup script
   ../download_nemo_model.sh
   
   # Export NeMo model to ONNX (requires nemo_toolkit)
   python3 ../export_nemo_to_onnx.py
   ```

2. **Test with standalone C++ application**:
   ```bash
   cd CppONNX_OnnxSTT
   make -f Makefile.nemo
   ./test_nemo_standalone --help
   ```

3. **Run SPL application with NeMo**:
   ```bash
   # Build toolkit first
   cd CppONNX_OnnxSTT
   make
   
   # Compile NeMo SPL application
   sc -a -t ../../../ -M NeMoRealtime --output-directory output_nemo
   
   # Run with Streams
   streamtool submitjob output_nemo/NeMoRealtime.sab
   ```

### NeMo Features

- **114M parameter FastConformer-Hybrid model**
- **13,000+ hours of English training data**
- **Cache-aware streaming** for minimal latency
- **Voice Activity Detection** (Silero VAD + energy fallback)
- **Advanced feature extraction** (kaldifeat + simple_fbank)
- **Multiple latency modes**: 0ms, 80ms, 480ms, 1040ms
- **Real-time performance** with CPU inference

### Performance Comparison

| Model | Latency | Accuracy | Training Data | Parameters |
|-------|---------|----------|---------------|------------|
| Zipformer RNN-T | ~150ms | Good | LibriSpeech | 90M |
| **NeMo FastConformer** | **~100ms** | **Excellent** | **13k hours** | **114M** |

### Model Export Instructions

```bash
# Install NeMo toolkit
pip install nemo_toolkit[all]

# Export streaming model with cache support
python3 -c "
import nemo.collections.asr as nemo_asr
model = nemo_asr.models.EncDecRNNTBPEModel.from_pretrained('nvidia/stt_en_fastconformer_hybrid_large_streaming_multi')
model.set_export_config({'cache_support': True})
model.export('models/nemo_fastconformer_streaming/fastconformer_streaming.onnx')
"
```
