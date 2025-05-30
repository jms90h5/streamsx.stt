# Speech-to-Text Toolkit Samples

This directory contains sample applications demonstrating speech-to-text using the TeraCloud Streams Speech-to-Text toolkit.

## Sample Applications

### 1. UnifiedSTTSample
**Main demonstration sample** with configurable runtime parameters and support for both ONNX and NeMo operators.

- **File**: `UnifiedSTTSample.spl`
- **Technology**: C++ with ONNX Runtime
- **Models Supported**: 
  - NVIDIA NeMo FastConformer (recommended)
  - Zipformer RNN-T
- **Features**:
  - Voice Activity Detection (Silero VAD)
  - Feature extraction (kaldifeat)
  - Cache-aware streaming
  - Runtime model configuration
- **Use Case**: Primary sample for production and development

### 2. CppONNX_OnnxSTT/IBMCultureTest
**Simple fixed demonstration** of NVIDIA FastConformer model using C++ ONNX implementation.

- **File**: `CppONNX_OnnxSTT/IBMCultureTest.spl`
- **Technology**: Pure C++ with ONNX Runtime
- **Model**: Fixed to NeMo FastConformer
- **Features**: Self-contained test with embedded audio data
- **Use Case**: Quick verification and testing

## Choosing the Right Sample

### Use UnifiedSTTSample when:
- Learning the toolkit capabilities
- Need configurable runtime parameters
- Want to switch between different models
- Building production applications

### Use IBMCultureTest when:
- Quick verification of toolkit installation
- Testing specific model performance
- Simple integration scenarios
- Fixed configuration requirements

## Real-time Design

This toolkit is optimized for real-time speech-to-text with minimal latency. Key principles are documented in the main README.md.

## Self-Contained Design

All samples use models and test data included with the toolkit:

- **Models**: Located in `../models/` relative to samples
- **Test Data**: Located in `../test_data/` 
- **No External Dependencies**: All required models and test audio files are included

## Building and Running Samples

### Prerequisites

Ensure your Streams environment is properly configured:

```bash
# Source the Streams environment
source /path/to/streams/bin/streamsprofile.sh

# Verify environment
echo $STREAMS_INSTALL
```

### Building Samples

1. **Build the toolkit first**:
   ```bash
   cd ..  # Go to toolkit root
   make
   ```

2. **Compile and run samples**:
   ```bash
   # UnifiedSTTSample
   sc -a -t ../../ -M UnifiedSTTSample --output-directory output
   streamtool submitjob output/UnifiedSTTSample.sab
   
   # IBMCultureTest
   cd CppONNX_OnnxSTT
   sc -a -t ../../../ -M IBMCultureTest --output-directory output
   streamtool submitjob output/IBMCultureTest.sab
   ```

## Model Notes

**Large model files (>100MB) are excluded from git** and must be shared separately:
- NVIDIA NeMo models (120MB - 440MB)
- ONNX models (50MB - 200MB) 
- Extracted model weights

Contact your team for access to the model files - they should be placed in the `../models/` directory relative to this samples folder.
