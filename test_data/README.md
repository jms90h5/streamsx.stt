# Test Data Directory

This directory contains audio test files for the Speech-to-Text toolkit.

## Audio Files

The `audio/` directory contains test WAV files:

### Primary Test Files
- **`11-ibm-culture-2min-16k.wav`** - Main test file (2 minutes, 16kHz)
  - IBM corporate presentation audio
  - Used by most samples as default
  
- **`11-ibm-culture-2min.wav`** - Original version (may be 8kHz)

### LibriSpeech Test Files
- `librispeech-1995-1837-0001.wav` - English speech sample
- `librispeech-1995-1837-0001.raw` - Raw PCM version

### Bilingual Test Files
- `0.wav`, `1.wav`, `2.wav`, `3.wav` - Short English/Chinese samples
- `8k.wav` - 8kHz sample for resampling tests
- `aishell-BAC009S0724W0121.wav` - Chinese AISHELL sample

### Other Test Files
- `test_16k.wav` - Generic 16kHz test audio
- `wav_to_raw.py` - Utility script for WAV to RAW conversion

## Usage

Samples reference these files with relative paths:
```spl
param
    expression<rstring> $audioFile : "../../test_data/audio/11-ibm-culture-2min-16k.wav";
```

## Audio Requirements

For best results, audio should be:
- **Format**: WAV (PCM)
- **Sample Rate**: 16000 Hz
- **Bit Depth**: 16-bit
- **Channels**: Mono (1 channel)

Use the included `wav_to_raw.py` script to convert files if needed.