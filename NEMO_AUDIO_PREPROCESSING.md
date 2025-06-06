# NeMo Audio Preprocessing Pipeline

## Overview
Based on the analysis of NeMo model configurations and implementation files, here's how NeMo extracts and preprocesses audio features for speech recognition:

## Preprocessing Configuration (from model_config.yaml)
```yaml
preprocessor:
  _target_: nemo.collections.asr.modules.AudioToMelSpectrogramPreprocessor
  sample_rate: 16000
  normalize: NA
  window_size: 0.025      # 25ms window
  window_stride: 0.01     # 10ms stride  
  window: hann           # Hann window function
  features: 80           # 80 mel bins
  n_fft: 512            # FFT size
  frame_splicing: 1
  dither: 1.0e-05       # Small noise added to avoid numerical issues
  pad_to: 0
```

## Feature Extraction Pipeline

### 1. Audio Input
- Sample rate: 16000 Hz (16 kHz)
- Input format: Float32 normalized to [-1, 1] range

### 2. Dithering
- Adds small random noise (1e-05) to each sample
- Prevents numerical issues with silence/zeros
- Implementation: `audio += (random_noise - 0.5) * 2 * dither_value`

### 3. Framing
- Window size: 25ms (400 samples at 16kHz)
- Window stride: 10ms (160 samples at 16kHz)  
- Creates overlapping frames from continuous audio

### 4. Windowing
- Applies Hann window to each frame
- Formula: `window[i] = 0.5 * (1 - cos(2*pi*i/(N-1)))`
- Reduces spectral leakage at frame boundaries

### 5. FFT (Fast Fourier Transform)
- FFT size: 512
- Computes magnitude spectrum
- Only positive frequencies used (257 bins)

### 6. Mel Filterbank
- 80 triangular mel filters
- Frequency range: 0 Hz to 8000 Hz (Nyquist/2)
- Mel scale conversion: `mel = 2595 * log10(1 + freq/700)`
- Applies triangular filters to FFT magnitude

### 7. Log Mel Spectrogram
- Converts to log scale: `log_mel = log(mel_spectrogram + epsilon)`
- No explicit normalization in this model ("normalize: NA")
- Output shape: [num_frames, 80]

## Key Parameters Summary
| Parameter | Value | Description |
|-----------|-------|-------------|
| Sample Rate | 16000 Hz | Audio sampling frequency |
| Window Size | 25 ms (400 samples) | Frame length for FFT |
| Window Stride | 10 ms (160 samples) | Frame shift/hop length |
| Window Type | Hann | Window function |
| FFT Size | 512 | FFT computation size |
| Mel Bins | 80 | Number of mel filterbank channels |
| Frequency Range | 0-8000 Hz | Mel filterbank range |
| Dither | 1e-05 | Noise level for dithering |

## C++ Implementation Considerations

### Key Differences Found:
1. **Dithering**: NeMo applies dithering before framing, C++ implementations may miss this
2. **Window Function**: Must use Hann window, not Hamming or others
3. **FFT Size**: Fixed at 512, not dependent on window size
4. **Normalization**: This model uses "NA" (no normalization), but some models may normalize
5. **Frequency Range**: 0-8000 Hz for mel filterbank, not 0-Nyquist

### Common Issues:
1. Missing dithering step
2. Wrong window function
3. Incorrect FFT size
4. Different mel filterbank implementation
5. Missing or incorrect log transformation

## Validation Steps
To verify correct implementation:
1. Load same audio file in both Python (NeMo) and C++
2. Apply dithering with same seed
3. Extract one frame and compare window values
4. Compare FFT magnitude spectrum
5. Compare mel filterbank output
6. Check final log mel spectrogram values

## Example Python Code (NeMo-compatible)
```python
# Using librosa to match NeMo preprocessing
mel_spec = librosa.feature.melspectrogram(
    y=audio,          # with dithering applied
    sr=16000,
    n_fft=512,
    hop_length=160,   # 10ms at 16kHz
    win_length=400,   # 25ms at 16kHz  
    n_mels=80,
    fmin=0,
    fmax=8000,
    window='hann'
)
log_mel = librosa.power_to_db(mel_spec, ref=np.max)
```