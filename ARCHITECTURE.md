# Architecture and Implementation Details

## Overview

This document provides technical details about the TeraCloud Streams Speech-to-Text toolkit implementation, design decisions, and historical context.

## System Architecture

### Component Layers

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
│     C++ Wrapper Layer               │  ← SPL/C++ bridge
│   (OnnxSTTWrapper)                 │
└─────────────────┬───────────────────┘
                  │
┌─────────────────▼───────────────────┐
│     Implementation Layer            │  ← Business logic
│   (OnnxSTTImpl)                    │
└─────────────────┬───────────────────┘
                  │
┌─────────────────▼───────────────────┐
│     ASR Engine Layer                │  ← Core algorithm
│   (ZipformerRNNT)                  │
└─────────────────┬───────────────────┘
                  │
┌─────────────────▼───────────────────┐
│     Inference Layer                 │  ← ONNX Runtime
│   (libonnxruntime.so)              │
└─────────────────────────────────────┘
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