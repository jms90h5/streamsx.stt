# Feature Mismatch Identified

## The Problem

**Python works perfectly, C++ produces only blank tokens**

### Evidence:
```
Python: "it was the first great sorrow of his life..."
C++: "" (all blank tokens)
```

### Feature Differences Found:

| Aspect | Python | C++ |
|--------|--------|-----|
| Mel Range | [-15.03, 3.06] | [-23.03, 6.68] |
| Frame Count | 874 | 870 |
| First 5 values | [-10.6, -13.1, -9.7, -11.3, -8.5] | [-23.0, -7.0, -7.6, -9.4, -8.0] |
| Result | Perfect transcription | All blanks |

## Root Cause

The C++ ProvenFeatureExtractor is producing different mel spectrograms than Python's librosa. This causes the model to output only blank tokens.

## Solution Needed

Either:
1. Fix the C++ feature extractor to match librosa exactly
2. Use a different feature extraction library
3. Save Python features and use them for testing

## Validation

When Python features are used:
- Model outputs diverse tokens: [1, 2, 4, 9, 15, 18, 20, 24, 28, 34...]
- Correct transcription produced

When C++ features are used:
- Model outputs only blanks: [1024, 1024, 1024, 1024, 1024...]
- Empty transcription

This proves the issue is purely in feature extraction, not in the model loading or CTC decoding.