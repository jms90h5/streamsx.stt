#!/usr/bin/env python3
"""Verify the saved features match what Python generates"""
import numpy as np
import librosa

# Load saved features
saved_features = np.load("reference_data/log_mel_spectrogram.npy")
print(f"Saved features shape: {saved_features.shape}")
print(f"Saved features range: [{saved_features.min():.4f}, {saved_features.max():.4f}]")
print(f"First 5 values: {saved_features.flat[:5]}")

# Load audio and recompute
audio, sr = librosa.load("test_data/audio/librispeech-1995-1837-0001.wav", sr=16000)
mel_spec = librosa.feature.melspectrogram(
    y=audio, sr=sr, n_fft=512, hop_length=160,
    win_length=400, n_mels=80, fmin=0.0, fmax=8000,
    htk=False, norm='slaney', center=True
)
log_mel = np.log(mel_spec + 1e-10)

print(f"\nRecomputed features shape: {log_mel.shape}")  
print(f"Recomputed range: [{log_mel.min():.4f}, {log_mel.max():.4f}]")
print(f"First 5 values: {log_mel.flat[:5]}")

# Check if they match
if np.allclose(saved_features, log_mel):
    print("\n✅ Features match!")
else:
    print("\n❌ Features don't match!")
    diff = np.abs(saved_features - log_mel)
    print(f"Max difference: {diff.max()}")

# The issue might be the transpose - NeMo expects [batch, mels, time]
# Let's check both orientations
print(f"\nFor NeMo we need shape [1, 80, {log_mel.shape[1]}]")
nemo_features = log_mel[np.newaxis, :, :]  # Add batch dimension
print(f"NeMo input shape: {nemo_features.shape}")