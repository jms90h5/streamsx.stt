# FastConformer Implementation Plan - Tensor Dimension Fix

**Date**: June 1, 2025  
**Goal**: Create working C++ Streams application using nvidia/stt_en_fastconformer_hybrid_large_streaming_multi  
**Root Issue**: Tensor broadcasting error "20 by 70" in attention mechanism  

## Implementation Steps (Detailed)

### Step 1: Update NeMoCacheAwareConformer.hpp (Header Changes)

**File**: `impl/include/NeMoCacheAwareConformer.hpp`  
**Lines to modify**: 38-46 (NeMoConfig struct)

#### Current Code (PROBLEMATIC):
```cpp
int chunk_frames = 160;         // Input frames per chunk
int last_channel_cache_size = 40;  // Output frames after subsampling (160/4) 
int last_time_cache_size = 70;     // Left context size for attention
```

#### New Code (FIXED):
```cpp
int chunk_frames = 160;         // Input frames per chunk (same as working model)
int max_cache_frames = 70;      // Maximum attention context (from NeMo docs)
int current_cache_frames = 0;   // Dynamic cache size (starts empty)
bool handle_short_context = true;  // Handle < 70 frames gracefully
```

#### Additional Variables to Add:
```cpp
// Add to private section around line 87
std::vector<std::vector<float>> frame_cache_;  // Store actual encoded frames
int total_frames_processed_;     // Track total frames for cache management
```

### Step 2: Fix Cache Initialization 

**File**: `impl/src/NeMoCacheAwareConformer.cpp`  
**Function**: `initializeCacheTensors()` (around line 156)

#### Key Changes:
1. **Remove fixed-size cache allocation**
2. **Initialize dynamic cache storage**
3. **Set cache counters to zero**

#### New initializeCacheTensors() Implementation:
```cpp
bool NeMoCacheAwareConformer::initializeCacheTensors() {
    try {
        // Initialize cache with empty state (no fixed size)
        cache_last_channel_.clear();
        cache_last_time_.clear();
        cache_last_channel_len_.clear();
        
        // Initialize frame cache for dynamic management
        frame_cache_.clear();
        current_cache_frames_ = 0;
        total_frames_processed_ = 0;
        
        // Cache will be sized dynamically based on actual encoder outputs
        cache_initialized_ = true;
        
        std::cout << "Cache tensors initialized (dynamic sizing)" << std::endl;
        std::cout << "Max cache frames: " << config_.max_cache_frames << std::endl;
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error initializing cache tensors: " << e.what() << std::endl;
        return false;
    }
}
```

### Step 3: Fix prepareEncoderInputs() - THE CRITICAL FIX

**File**: `impl/src/NeMoCacheAwareConformer.cpp`  
**Function**: `prepareEncoderInputs()` (line 315)

#### Current Problem (Lines 371-378):
```cpp
// WRONG: Fixed 70-frame cache regardless of actual content
std::vector<int64_t> time_cache_shape = {batch_size, num_cache_layers, hidden_size, 70};
```

#### New Implementation Strategy:
1. **Calculate actual cache size** based on current_cache_frames
2. **Handle first chunk case** (no cache available)
3. **Create properly sized tensors** for current cache content
4. **Add comprehensive debug logging**

#### Detailed prepareEncoderInputs() Fix:
```cpp
std::vector<Ort::Value> NeMoCacheAwareConformer::prepareEncoderInputs(const std::vector<std::vector<float>>& features) {
    std::vector<Ort::Value> inputs;
    
    // 1. Audio signal tensor preparation (same as before)
    size_t batch_size = 1;
    size_t time_frames = features.size();
    size_t feature_dim = features[0].size();
    
    // Transpose to [batch, features, time] format
    std::vector<float> audio_signal;
    audio_signal.reserve(batch_size * time_frames * feature_dim);
    for (size_t f = 0; f < feature_dim; ++f) {
        for (size_t t = 0; t < time_frames; ++t) {
            audio_signal.push_back(features[t][f]);
        }
    }
    
    std::vector<int64_t> audio_shape = {static_cast<int64_t>(batch_size), 
                                       static_cast<int64_t>(feature_dim),
                                       static_cast<int64_t>(time_frames)};
    
    std::cout << "PREPARE_INPUTS: Audio shape [" << batch_size << ", " << feature_dim 
              << ", " << time_frames << "]" << std::endl;
    
    inputs.push_back(Ort::Value::CreateTensor<float>(memory_info_, audio_signal.data(), 
                                                     audio_signal.size(), audio_shape.data(), 
                                                     audio_shape.size()));
    
    // 2. Length tensor (same as before)
    std::vector<int64_t> length_data = {static_cast<int64_t>(time_frames)};
    std::vector<int64_t> length_shape = {static_cast<int64_t>(batch_size)};
    inputs.push_back(Ort::Value::CreateTensor<int64_t>(memory_info_, length_data.data(), 
                                                       length_data.size(), length_shape.data(), 
                                                       length_shape.size()));
    
    if (config_.enable_caching) {
        // 3. CRITICAL FIX: Dynamic cache tensor sizing
        
        // Calculate actual cache dimensions
        int actual_cache_frames = std::min(current_cache_frames_, config_.max_cache_frames);
        
        std::cout << "CACHE_STATE: current=" << current_cache_frames_ 
                  << " max=" << config_.max_cache_frames 
                  << " actual=" << actual_cache_frames << std::endl;
        
        if (actual_cache_frames == 0) {
            // FIRST CHUNK: No cache available
            std::cout << "FIRST_CHUNK: Initializing empty cache tensors" << std::endl;
            
            // Create minimal cache tensors (will be ignored by model)
            std::vector<int64_t> empty_channel_shape = {1, config_.num_cache_layers, 1, config_.hidden_size};
            std::vector<int64_t> empty_time_shape = {1, config_.num_cache_layers, config_.hidden_size, 1};
            
            std::vector<float> empty_channel(config_.num_cache_layers * config_.hidden_size, 0.0f);
            std::vector<float> empty_time(config_.num_cache_layers * config_.hidden_size, 0.0f);
            
            inputs.push_back(Ort::Value::CreateTensor<float>(memory_info_, empty_channel.data(),
                                                             empty_channel.size(), 
                                                             empty_channel_shape.data(),
                                                             empty_channel_shape.size()));
            
            inputs.push_back(Ort::Value::CreateTensor<float>(memory_info_, empty_time.data(),
                                                             empty_time.size(),
                                                             empty_time_shape.data(),
                                                             empty_time_shape.size()));
            
            // Cache length = 0 (no cache)
            std::vector<int64_t> cache_len_data = {0};
            std::vector<int64_t> len_shape = {1};
            inputs.push_back(Ort::Value::CreateTensor<int64_t>(memory_info_, cache_len_data.data(),
                                                               cache_len_data.size(),
                                                               len_shape.data(),
                                                               len_shape.size()));
        } else {
            // SUBSEQUENT CHUNKS: Use actual cache content
            std::cout << "SUBSEQUENT_CHUNK: Using " << actual_cache_frames << " cache frames" << std::endl;
            
            // Create cache tensors with actual dimensions
            std::vector<int64_t> channel_cache_shape = {1, config_.num_cache_layers, 
                                                       actual_cache_frames, config_.hidden_size};
            std::vector<int64_t> time_cache_shape = {1, config_.num_cache_layers, 
                                                    config_.hidden_size, actual_cache_frames};
            
            inputs.push_back(Ort::Value::CreateTensor<float>(memory_info_, cache_last_channel_.data(),
                                                             cache_last_channel_.size(), 
                                                             channel_cache_shape.data(),
                                                             channel_cache_shape.size()));
            
            inputs.push_back(Ort::Value::CreateTensor<float>(memory_info_, cache_last_time_.data(),
                                                             cache_last_time_.size(),
                                                             time_cache_shape.data(),
                                                             time_cache_shape.size()));
            
            // Cache length = actual cache frames
            std::vector<int64_t> cache_len_data = {static_cast<int64_t>(actual_cache_frames)};
            std::vector<int64_t> len_shape = {1};
            inputs.push_back(Ort::Value::CreateTensor<int64_t>(memory_info_, cache_len_data.data(),
                                                               cache_len_data.size(),
                                                               len_shape.data(),
                                                               len_shape.size()));
        }
    }
    
    std::cout << "PREPARE_INPUTS: Created " << inputs.size() << " input tensors" << std::endl;
    return inputs;
}
```

### Step 4: Implement Proper Cache Management

**File**: `impl/src/NeMoCacheAwareConformer.cpp`  
**Function**: `updateCacheFromEncoderOutputs()` (around line 400)

#### Current Problem:
- Cache is not properly updated with actual encoder outputs
- No frame accumulation logic

#### New Implementation:
```cpp
void NeMoCacheAwareConformer::updateCacheFromEncoderOutputs(std::vector<Ort::Value>& outputs) {
    try {
        if (!config_.enable_caching || outputs.size() < 5) {
            return;
        }
        
        // Extract new cache tensors from encoder outputs
        auto new_channel_cache = outputs[2].GetTensorData<float>();
        auto new_time_cache = outputs[3].GetTensorData<float>();
        auto new_cache_len = outputs[4].GetTensorData<int64_t>();
        
        // Get tensor shapes
        auto channel_shape = outputs[2].GetTensorTypeAndShapeInfo().GetShape();
        auto time_shape = outputs[3].GetTensorTypeAndShapeInfo().GetShape();
        
        int new_frames = static_cast<int>(channel_shape[2]);  // frames dimension
        
        std::cout << "CACHE_UPDATE: Received " << new_frames << " new frames" << std::endl;
        
        // Calculate cache size after adding new frames
        int total_cache_frames = current_cache_frames_ + new_frames;
        
        if (total_cache_frames > config_.max_cache_frames) {
            // Trim old frames to stay within limit
            int frames_to_remove = total_cache_frames - config_.max_cache_frames;
            std::cout << "CACHE_TRIM: Removing " << frames_to_remove << " old frames" << std::endl;
            
            // TODO: Implement cache trimming logic
            current_cache_frames_ = config_.max_cache_frames;
        } else {
            current_cache_frames_ = total_cache_frames;
        }
        
        // Update cache tensors (simplified - copy new data)
        size_t channel_cache_size = channel_shape[0] * channel_shape[1] * channel_shape[2] * channel_shape[3];
        size_t time_cache_size = time_shape[0] * time_shape[1] * time_shape[2] * time_shape[3];
        
        cache_last_channel_.assign(new_channel_cache, new_channel_cache + channel_cache_size);
        cache_last_time_.assign(new_time_cache, new_time_cache + time_cache_size);
        
        total_frames_processed_ += new_frames;
        
        std::cout << "CACHE_UPDATE: Current cache=" << current_cache_frames_ 
                  << " total_processed=" << total_frames_processed_ << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error updating cache: " << e.what() << std::endl;
    }
}
```

### Step 5: Add Comprehensive Debug Logging

**Throughout all functions**, add logging to track:
1. **Input tensor shapes** before ONNX inference
2. **Cache state** at each step
3. **Error context** if broadcasting fails
4. **Memory usage** to prevent heap crashes

### Step 6: Test and Validate

**Create minimal test that**:
1. **Loads the ONNX models** successfully
2. **Processes 3-4 chunks** of librispeech audio
3. **Verifies cache growth** (0 → 20 → 40 → 60 frames)
4. **Confirms no broadcasting errors**
5. **Produces meaningful text output**

## Expected Outcome

After these changes:
1. **First chunk**: 0 cache + 20 current = handled gracefully
2. **Second chunk**: 20 cache + 20 current = 40 total context
3. **Fourth chunk**: 60 cache + 20 current = 80 → trim to 70
4. **Steady state**: 50 cache + 20 current = 70 total (perfect)

## Risk Mitigation

1. **Memory monitoring**: Add debug prints for tensor sizes
2. **Gradual testing**: Test one chunk at a time initially
3. **Fallback plan**: Keep original code commented for quick rollback
4. **Documentation**: Update this file if any crashes occur

---

**This plan addresses the exact root cause identified through systematic analysis and should resolve the tensor broadcasting error while maintaining memory safety.**