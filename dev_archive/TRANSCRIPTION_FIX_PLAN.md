# Speech-to-Text Transcription Fix Plan

## Problem Summary
The NeMo model is outputting repetitive tokens ("that that that...") instead of proper transcription for the IBM culture audio file. Two main issues identified:
1. **Audio resampling issue**: 8kHz audio upsampled to 16kHz loses critical frequency information
2. **Model behavior issue**: Even with native 16kHz audio, the model outputs the same token

## Root Cause Analysis

### 1. Audio Frequency Mismatch
- **Input**: 8kHz audio (0-4kHz frequency content)
- **Model expects**: 16kHz audio (0-8kHz frequency content)
- **Current approach**: Simple upsampling (doesn't add missing frequencies)
- **Impact**: Model sees unusual patterns it wasn't trained on

### 2. Model Integration Issues
- CTC decoding using simple greedy approach (may need beam search)
- Possible feature normalization mismatch
- Model may need specific preprocessing not currently implemented

## Solution Plan

### Phase 1: Verify Model with Known-Good Audio (1-2 hours)

#### Step 1.1: Create Test Audio Set
```bash
# Download or create known-good 16kHz test audio
# Use LibriSpeech or other standard dataset samples
wget http://www.openslr.org/resources/12/test-clean.tar.gz
tar -xzf test-clean.tar.gz
# Convert FLAC to WAV at 16kHz
```

#### Step 1.2: Test with Multiple Models
- Test with Zipformer model (already in toolkit)
- Test with different NeMo model checkpoints
- Compare outputs to establish baseline

#### Step 1.3: Verify Feature Extraction
- Print mel-spectrogram statistics
- Compare with expected model input ranges
- Verify CMVN normalization is correct

### Phase 2: Fix Audio Preprocessing (2-3 hours)

#### Step 2.1: Implement Proper Audio Handling
```cpp
// Add to audio preprocessing
class AudioPreprocessor {
    // Detect sample rate
    // Apply appropriate filtering
    // Handle 8kHz audio differently than 16kHz
};
```

#### Step 2.2: Add Telephone Audio Support
- Implement narrow-band feature extraction for 8kHz
- Use different mel-filterbank settings
- Apply telephone-specific preprocessing

#### Step 2.3: Create Sample Rate Adapter
```python
# Create adapter to handle different sample rates
def create_sample_rate_adapter(source_rate, target_rate):
    if source_rate == 8000 and target_rate == 16000:
        # Use special handling for telephone audio
        return TelephoneAudioAdapter()
    else:
        return StandardResampler()
```

### Phase 3: Improve Model Integration (3-4 hours)

#### Step 3.1: Implement Beam Search CTC Decoding
```cpp
// Replace greedy decoding with beam search
class CTCBeamSearchDecoder {
    // Implement prefix beam search
    // Add language model support
    // Handle blank tokens properly
};
```

#### Step 3.2: Add Model Debugging
- Log raw model outputs before decoding
- Visualize attention/activation patterns
- Check for numerical issues (NaN/Inf)

#### Step 3.3: Test Alternative Models
- Try WeNet model (if available)
- Test SpeechBrain models
- Compare with working Python implementation

### Phase 4: Quick Workaround (30 minutes)

#### Step 4.1: Use External Tool for Verification
```bash
# Use existing tool to verify audio is transcribable
ffmpeg -i 11-ibm-culture-2min.wav -ar 16000 -ac 1 ibm-16k.wav
# Test with whisper or other known-working tool
```

#### Step 4.2: Create Bridge Solution
- Use Python binding to working model
- Create temporary file-based interface
- Validate transcription is possible

### Phase 5: Implement Robust Solution (4-6 hours)

#### Step 5.1: Create Multi-Model Pipeline
```cpp
class RobustSTTPipeline {
    // Try multiple models
    // Fall back gracefully
    // Combine results if needed
};
```

#### Step 5.2: Add Comprehensive Testing
- Unit tests for each component
- Integration tests with various audio types
- Performance benchmarks

#### Step 5.3: Documentation and Examples
- Clear usage examples
- Troubleshooting guide
- Model compatibility matrix

## Implementation Priority

### Immediate Actions (Today)
1. **Verify the model works with ANY audio**
   - Test with Zipformer model samples
   - Test with known-good 16kHz audio
   - Confirm vocabulary is being used

2. **Create working baseline**
   - Find one audio file that transcribes correctly
   - Document exact settings that work
   - Use as reference for debugging

3. **Test alternative approach**
   - Try the Sherpa-ONNX Zipformer model
   - Use its vocabulary and tokens
   - Compare results

### Short-term (This Week)
1. Implement proper 8kHz audio handling
2. Add beam search decoding
3. Create test suite with various audio types

### Long-term (Next Week)
1. Add multiple model support
2. Implement confidence scoring
3. Add real-time streaming optimizations

## Specific Code Changes Needed

### 1. Fix Model Factory (`ModelFactory.cpp`)
```cpp
// Add model-specific vocabulary paths
if (model_type == NEMO_CONFORMER) {
    vocab_path = model_dir + "/tokenizer.txt";
} else if (model_type == NEMO_FASTCONFORMER) {
    vocab_path = model_dir + "/vocab.txt";
}
```

### 2. Improve CTC Decoding (`NeMoCacheAwareConformer.cpp`)
```cpp
// Add beam search with blank handling
std::string decodeCTCBeamSearch(
    const float* log_probs, 
    int64_t seq_len, 
    int64_t num_classes,
    int beam_width = 10
);
```

### 3. Add Audio Validation (`STTPipeline.cpp`)
```cpp
// Validate audio properties
bool validateAudioForModel(
    const std::vector<float>& audio,
    int sample_rate,
    ModelType model_type
);
```

## Success Criteria
1. IBM culture audio file produces readable English transcript
2. Both 8kHz and 16kHz audio files work correctly
3. Processing remains real-time (< 0.1 RTF)
4. No repetitive token outputs
5. Confidence scores are meaningful

## Testing Plan
1. Test with original 8kHz IBM culture audio
2. Test with properly converted 16kHz version
3. Test with LibriSpeech samples
4. Test with various accents and audio qualities
5. Benchmark performance metrics

## Next Steps
1. Start with Phase 1 - verify model with known-good audio
2. If model works with some audio, proceed to Phase 2
3. If model doesn't work with any audio, focus on Phase 3
4. Document findings at each step

This plan provides a systematic approach to diagnose and fix the transcription issues. The key is to establish a working baseline first, then incrementally improve support for different audio types.