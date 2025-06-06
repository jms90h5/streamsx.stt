# Systematic Plan to Fix C++ Implementation

## Current State: C++ Produces No Output

### Evidence
- Python: Features → Model → "it was the first great sorrow..."
- C++: Features → Model → "" (all blank tokens)
- Root cause: Feature mismatch (proven by different ranges and values)

## Step 1: Establish Ground Truth

### 1.1 Create Reference Test Data
```python
# save_reference_data.py
import numpy as np
import librosa

# Load audio
audio, sr = librosa.load("test_data/audio/librispeech-1995-1837-0001.wav", sr=16000)

# Save raw audio for C++ to load
audio.astype(np.float32).tofile("reference_audio_raw.bin")

# Extract features exactly as Python does
mel_spec = librosa.feature.melspectrogram(
    y=audio, sr=sr, n_fft=512, hop_length=160, 
    win_length=400, n_mels=80, fmin=0.0, fmax=8000
)
log_mel = np.log(mel_spec + 1e-10)

# Save intermediate values for debugging
np.save("reference_mel_spec.npy", mel_spec)
np.save("reference_log_mel.npy", log_mel)

# Save in format C++ can read
log_mel.astype(np.float32).tofile("reference_features.bin")

print(f"Saved {len(audio)} audio samples")
print(f"Saved {log_mel.shape} feature matrix")
```

### 1.2 Create C++ Feature Validator
```cpp
// validate_features.cpp
// Load reference features and compare with C++ extraction
bool validateFeatures() {
    // Load reference audio
    std::vector<float> audio = loadBinaryFile("reference_audio_raw.bin");
    
    // Extract with C++
    auto cpp_features = extractMelFeatures(audio);
    
    // Load Python reference
    auto ref_features = loadBinaryFile("reference_features.bin");
    
    // Compare
    float max_diff = 0;
    for (size_t i = 0; i < ref_features.size(); i++) {
        float diff = std::abs(cpp_features[i] - ref_features[i]);
        max_diff = std::max(max_diff, diff);
    }
    
    std::cout << "Max feature difference: " << max_diff << std::endl;
    return max_diff < 0.01;  // Tolerance
}
```

## Step 2: Test Model with Known Good Features

### 2.1 C++ Model Test with Python Features
```cpp
// test_model_only.cpp
// Bypass C++ feature extraction, use Python features directly
void testModelWithReferenceFeatures() {
    // Load Python features
    auto features = loadBinaryFile("reference_features.bin");
    
    // Reshape to [1, 80, 874]
    // Run through model
    // Should produce same output as Python
}
```

This will prove:
- If model loading is correct
- If CTC decoder is correct
- Isolate the problem to ONLY feature extraction

## Step 3: Debug Feature Extraction Systematically

### 3.1 Compare Each Step
```python
# debug_feature_steps.py
# Save intermediate values at each step

# 1. Raw audio
# 2. After windowing
# 3. After FFT
# 4. Power spectrum  
# 5. After mel filter bank
# 6. After log

# Save each step for C++ comparison
```

### 3.2 Find Exact Divergence Point
- Window function differences?
- FFT implementation differences?
- Mel filter bank calculation?
- Log scale differences?

## Step 4: Implementation Options

Based on findings, choose approach:

### Option A: Fix ProvenFeatureExtractor
- Match librosa exactly
- May require reimplementing FFT, window, mel filters

### Option B: Use Existing Library
- kaldi-native-fbank (already in codebase)
- SpeechBrain feature extraction
- torchaudio C++ API

### Option C: Pre-compute Features
- For testing/validation only
- Compute features in Python, save as binary
- Load in C++ for inference

## Step 5: Validation Protocol

### 5.1 Unit Tests
```cpp
TEST(Features, MatchesPythonExactly) {
    auto audio = loadTestAudio();
    auto cpp_features = extractFeatures(audio);
    auto ref_features = loadReferenceFeatures();
    ASSERT_NEAR_ARRAY(cpp_features, ref_features, 0.01);
}

TEST(Model, ProducesCorrectTokensWithReferenceFeatures) {
    auto features = loadReferenceFeatures();
    auto tokens = model.infer(features);
    ASSERT_EQ(tokens, {15, 34, 28, ...});  // Known good tokens
}

TEST(EndToEnd, ProducesCorrectTranscription) {
    auto audio = loadTestAudio();
    auto text = model.transcribe(audio);
    ASSERT_EQ(text, "it was the first great sorrow...");
}
```

## Step 6: Success Criteria

1. **Feature Match**: C++ features within 0.01 of Python features
2. **Token Match**: Same token sequence as Python
3. **Text Match**: Identical transcription
4. **Performance**: Real-time factor > 1.0

## Implementation Order

1. **Today**: Create reference data (Step 1)
2. **Next**: Test model with Python features (Step 2)
3. **Then**: Debug feature extraction (Step 3)
4. **Finally**: Fix or replace feature extractor (Step 4)

## No Guessing Policy

- Every change must be validated against reference data
- No "let's try this" without evidence
- Measure, compare, fix, verify
- Binary search to find exact problem

This approach will systematically identify and fix the exact issue without random attempts.