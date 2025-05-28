# Architecture and Implementation Details

## Overview

This document provides technical details about the TeraCloud Streams Speech-to-Text toolkit implementation, design decisions, and historical context. As of 2025-05-28, the toolkit features a completely redesigned three-stage pipeline with VAD, advanced feature extraction, and model abstraction interfaces.

## System Architecture

### New Pipeline Architecture (2025-05-28)

```
┌─────────────────────────────────────┐
│     SPL Application Layer           │  ← User applications
│   (ONNXRealtime.spl)               │
└─────────────────┬───────────────────┘
                  │
┌─────────────────▼───────────────────┐
│     SPL Operator Layer              │  ← Toolkit operators  
│   (OnnxSTT, FileAudioSource)       │
└─────────────────┬───────────────────┘
                  │
┌─────────────────▼───────────────────┐
│     STTPipeline Layer               │  ← Modern pipeline
│   ┌─────────────────────────────────┐ │
│   │ 1. VAD Stage                    │ │
│   │   ├─ SileroVAD (ONNX)          │ │
│   │   └─ EnergyVAD (fallback)      │ │
│   └─────────────────────────────────┘ │
│   ┌─────────────────────────────────┐ │
│   │ 2. Feature Extraction Stage     │ │
│   │   ├─ KaldifeatExtractor        │ │
│   │   └─ SimpleFbank (fallback)    │ │
│   └─────────────────────────────────┘ │
│   ┌─────────────────────────────────┐ │
│   │ 3. ASR Model Stage              │ │
│   │   ├─ ZipformerRNNT (impl)      │ │
│   │   ├─ ConformerModel (intf)     │ │
│   │   ├─ WenetModel (intf)         │ │
│   │   └─ SpeechBrainModel (intf)   │ │
│   └─────────────────────────────────┘ │
└─────────────────┬───────────────────┘
                  │
┌─────────────────▼───────────────────┐
│     Cache Management Layer          │  ← Tensor orchestration
│   (CacheManager - multi-arch)      │
└─────────────────┬───────────────────┘
                  │
┌─────────────────▼───────────────────┐
│     Inference Layer                 │  ← ONNX Runtime
│   (libonnxruntime.so)              │
└─────────────────────────────────────┘
```

### Legacy Architecture (deprecated)

The previous architecture used a direct OnnxSTTImpl → ZipformerRNNT approach without VAD or proper feature extraction. This has been superseded by the modular pipeline design.

## Modern Pipeline Components (2025-05-28)

### 1. Voice Activity Detection (VAD) Stage

#### SileroVAD Implementation
```cpp
class SileroVAD : public VADInterface {
    // ONNX-based neural VAD with 64ms processing windows
    // Provides high accuracy speech/non-speech classification
    // Falls back to energy-based VAD if model unavailable
};
```

**Key Features:**
- **64ms processing windows** with configurable overlap
- **Adaptive thresholds** (default 0.5) for speech detection
- **Performance tracking** with timing metrics
- **Automatic fallback** to energy-based VAD
- **State persistence** across audio chunks

**Configuration:**
```cpp
VADInterface::Config vad_config;
vad_config.sample_rate = 16000;
vad_config.window_size_ms = 64;
vad_config.speech_threshold = 0.5f;
```

#### Energy-based VAD Fallback
Simple energy thresholding when Silero VAD model unavailable:
- RMS energy calculation with smoothing
- Configurable energy threshold
- Minimal processing overhead

### 2. Feature Extraction Stage

#### KaldifeatExtractor Implementation
```cpp
class KaldifeatExtractor : public FeatureExtractorInterface {
    // Production-quality feature extraction with kaldifeat C++17
    // Falls back to simple_fbank for compatibility
};
```

**Key Features:**
- **Kaldifeat C++17 integration** for production features
- **Simple filterbank fallback** for testing/compatibility
- **80-dimensional log-mel features** (configurable)
- **CMVN normalization support** (pending implementation)
- **Configurable parameters**: sample rate, window size, mel bins

**Configuration:**
```cpp
FeatureExtractorInterface::Config feat_config;
feat_config.sample_rate = 16000;
feat_config.num_mel_bins = 80;
feat_config.frame_length_ms = 25;
feat_config.frame_shift_ms = 10;
feat_config.use_kaldifeat = true;  // Falls back to simple_fbank if unavailable
```

### 3. ASR Model Stage

#### Model Abstraction Interface
```cpp
class ModelInterface {
public:
    struct TranscriptionResult {
        std::string text;
        float confidence;
        uint64_t timestamp_ms;
        bool is_final;
        int processing_time_ms;
    };
    
    virtual bool initialize(const ModelConfig& config) = 0;
    virtual TranscriptionResult processChunk(const std::vector<std::vector<float>>& features, 
                                           uint64_t timestamp_ms) = 0;
    virtual void reset() = 0;
    virtual std::map<std::string, double> getStats() = 0;
    virtual bool supportsStreaming() const = 0;
    virtual int getFeatureDim() const = 0;
    virtual int getChunkFrames() const = 0;
};
```

#### Supported Model Architectures
1. **ZipformerRNNT** (fully implemented)
2. **ConformerModel** (interface ready)
3. **WenetModel** (interface ready) 
4. **SpeechBrainModel** (interface ready)

### 4. Cache Management Layer

#### Multi-Architecture Cache Support
```cpp
class CacheManager {
public:
    enum CacheType {
        ZIPFORMER_35_CACHE,      // 35 cache tensors (7 types × 5 layers)
        CONFORMER_3_CACHE,       // encoder_out_cache, cnn_cache, att_cache
        WENET_SINGLE_CACHE,      // Single "cache" tensor
        SPEECHBRAIN_H0_CACHE,    // LSTM hidden states (h0, c0)
        CUSTOM_CACHE             // User-defined cache architecture
    };
};
```

**Features:**
- **Multi-architecture support** for different streaming ASR models
- **Configurable cache sizes** and tensor shapes
- **Automatic cache initialization** and reset
- **Performance tracking** with cache hit rates
- **Memory-efficient management** with reuse patterns

### 5. Integrated STTPipeline

#### Pipeline Orchestration
```cpp
class STTPipeline {
public:
    struct PipelineResult {
        std::string transcription;
        float confidence;
        uint64_t timestamp_ms;
        bool is_speech;          // VAD result
        bool is_final;           // Final transcription
        
        // Performance metrics
        int vad_time_ms;
        int feature_time_ms;
        int model_time_ms;
        int total_time_ms;
    };
    
    PipelineResult processAudio(const std::vector<float>& audio, uint64_t timestamp_ms);
};
```

**Pipeline Flow:**
1. **VAD Processing**: Filter non-speech audio to reduce computation
2. **Feature Extraction**: Convert audio to 80-dim log-mel features
3. **Model Inference**: Process features through ASR model
4. **Result Integration**: Combine results with timing and confidence

**Factory Functions:**
```cpp
// Create preconfigured pipelines for different models
std::unique_ptr<STTPipeline> createZipformerPipeline(const std::string& model_dir, bool enable_vad = true);
std::unique_ptr<STTPipeline> createConformerPipeline(const std::string& model_dir, bool enable_vad = true);
std::unique_ptr<STTPipeline> createWenetPipeline(const std::string& model_dir, bool enable_vad = true);
```

## Zipformer RNN-T Implementation

### Three-Model Architecture

The toolkit uses a Zipformer RNN-T architecture split into three ONNX models:

1. **Encoder** (`encoder-epoch-99-avg-1.onnx`)
   - Input: 39×80 acoustic features + 35 cache tensors
   - Output: Encoded representations + updated cache
   - Maintains streaming state across chunks

2. **Decoder** (`decoder-epoch-99-avg-1.onnx`)
   - Input: Previous tokens + hypothesis state
   - Output: Decoder embeddings
   - Implements RNN-T prediction network

3. **Joiner** (`joiner-epoch-99-avg-1.onnx`)
   - Input: Encoder output + Decoder output
   - Output: Next token probabilities
   - Combines acoustic and linguistic information

### Cache Tensor Management

The Zipformer encoder maintains 35 cache tensors for streaming:

```cpp
struct CacheState {
    // 5 layers × 7 cache types = 35 tensors
    std::array<std::vector<int64_t>, 5> cached_len;    // Processed frame counts
    std::array<std::vector<float>, 5> cached_avg;      // Running averages
    std::array<std::vector<float>, 5> cached_key;      // Attention keys
    std::array<std::vector<float>, 5> cached_val;      // Attention values  
    std::array<std::vector<float>, 5> cached_val2;     // Secondary values
    std::array<std::vector<float>, 5> cached_conv1;    // Conv layer 1 state
    std::array<std::vector<float>, 5> cached_conv2;    // Conv layer 2 state
};
```

Cache dimensions by layer:
- Layer 0-4: avg[512], key[128×192], val[128×192], val2[128×256], conv[768×3]

### Chunk Processing

Fixed chunk size of 39 frames (390ms):
- Frame size: 25ms
- Frame shift: 10ms  
- Chunk duration: 39 × 10ms = 390ms
- Features per chunk: 39 × 80 = 3120 floats

## SPL Operator Design

### OnnxSTT Operator

The main speech-to-text operator with:

**Parameters**:
- `encoderModel`: Path to encoder ONNX file
- `decoderModel`: Path to decoder ONNX file  
- `joinerModel`: Path to joiner ONNX file
- `vocabFile`: Path to vocabulary
- `sampleRate`: Audio sample rate (default 16000)
- `provider`: ONNX execution provider (CPU/CUDA)

**Input Port**: Audio stream with samples
**Output Port**: Transcription results (text, isFinal, confidence)

### Implementation Bridge

The C++ implementation follows SPL conventions:

1. **OnnxSTTWrapper**: Generated C++ operator class
2. **OnnxSTTImpl**: Actual implementation
3. **Parameter passing**: Via SPL parameter framework
4. **Tuple processing**: One chunk at a time
5. **Memory management**: RAII with smart pointers

## Historical Context

### Evolution of the Toolkit

1. **Initial Goal**: Three-stage pipeline (VAD → Features → ASR)
2. **WeNet Integration**: First attempt using WeNet C++ API
3. **ONNX Migration**: Moved to ONNX for portability
4. **Model Issues**: WeNet ONNX models incompatible
5. **Sherpa-ONNX**: Found compatible Zipformer models
6. **Current State**: Working but needs improvements

### Design Decisions

**Why Zipformer RNN-T?**
- Designed for streaming ASR
- Good accuracy/latency tradeoff
- Available pre-trained models

**Why ONNX Runtime?**
- No PyTorch dependency
- Cross-platform support
- Optimized inference

**Why fixed chunks?**
- Model trained with specific size
- Simplifies cache management
- Predictable latency

**Why no feature extraction?**
- Complexity/time tradeoff
- Models work with simple features
- Future improvement planned

### Lessons Learned

1. **Model compatibility is critical** - Wrong schema = complete failure
2. **Start simple** - Complex pipelines can wait
3. **Test everything** - Each component separately
4. **Document working state** - Essential for recovery
5. **WeNet wasn't needed** - Simplified significantly

## Performance Characteristics

### Latency Breakdown (390ms chunk)
- Feature extraction: ~10ms
- Encoder forward: ~100ms
- Beam search: ~40ms
- Total: ~150ms (0.4x real-time)

### Memory Usage
- Models: ~200MB
- Cache tensors: ~10MB
- Beam search: ~5MB
- Total: ~220MB per stream

### CPU Usage
- Single-threaded processing
- ~40% of one core for real-time
- Scales linearly with streams

## Future Architecture

### Planned Three-Stage Pipeline

```
Audio → VAD → Feature Extraction → ASR
         ↓           ↓                ↓
    Silero VAD   kaldifeat      Zipformer
```

Benefits:
- Skip silence (reduce compute)
- Proper features (better accuracy)
- Modular design (easy updates)

### Improvements Needed

1. **Feature Extraction**
   - Replace simple_fbank with kaldifeat
   - Add CMVN normalization
   - Support different feature types

2. **Streaming Support**
   - Handle continuous audio
   - Dynamic chunk boundaries
   - Proper timestamp alignment

3. **Model Flexibility**
   - Support different architectures
   - Dynamic model loading
   - Multi-language support

## Code Organization

### Directory Structure Rationale

- `impl/include/`: Headers only (C++ convention)
- `impl/src/`: Source files separate from headers
- `impl/build/`: Keep artifacts out of source tree
- `impl/lib/`: Clear output location

### Build System Design

- Simple Makefile (no CMake complexity)
- Automatic dependency detection
- Clear build/clean targets
- Proper RPATH for libraries

### Why No WeNet Code?

Originally included because:
- Copied from existing toolkit
- Thought SPL needed C wrappers
- Makefile listed them

Removed because:
- Never actually called
- No runtime dependency
- Simplified maintenance
- Saved 100MB+ space