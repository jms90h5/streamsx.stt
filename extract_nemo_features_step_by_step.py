#!/usr/bin/env python3
"""
Extract features from NeMo step by step to understand the preprocessing
"""
import numpy as np
import torch
import nemo.collections.asr as nemo_asr
import soundfile as sf
import librosa

# Load the working model
model_path = "models/nemo_fastconformer_direct/stt_en_fastconformer_hybrid_large_streaming_multi.nemo"
model = nemo_asr.models.EncDecHybridRNNTCTCBPEModel.restore_from(model_path)
model.eval()

# Load audio
audio_file = "test_data/audio/librispeech-1995-1837-0001.wav"
audio, sample_rate = sf.read(audio_file)
print(f"Audio shape: {audio.shape}, sample rate: {sample_rate}")

# Access the preprocessor
preprocessor = model.preprocessor
print(f"\nPreprocessor config:")
print(f"  sample_rate: {preprocessor.sample_rate}")
print(f"  window_size: {preprocessor.win_length / preprocessor.sample_rate * 1000:.1f}ms ({preprocessor.win_length} samples)")
print(f"  window_stride: {preprocessor.hop_length / preprocessor.sample_rate * 1000:.1f}ms ({preprocessor.hop_length} samples)")
print(f"  n_fft: {preprocessor.n_fft}")
print(f"  n_mels: {preprocessor.nfilt}")
print(f"  window: {preprocessor.window}")
print(f"  dither: {preprocessor.dither}")

# Process audio through preprocessor
audio_tensor = torch.tensor(audio).unsqueeze(0)
audio_len = torch.tensor([len(audio)])

# Get features
with torch.no_grad():
    features, feature_len = preprocessor(input_signal=audio_tensor, length=audio_len)

print(f"\nFeatures shape: {features.shape}")
print(f"Features range: [{features.min().item():.4f}, {features.max().item():.4f}]")
print(f"Features mean: {features.mean().item():.4f}, std: {features.std().item():.4f}")

# Save features
np.save("nemo_preprocessor_features.npy", features.cpu().numpy())

# Let's also manually compute mel spectrogram using librosa for comparison
print("\n\nManual computation using librosa:")

# Apply dithering
dithered_audio = audio + np.random.uniform(-preprocessor.dither, preprocessor.dither, size=audio.shape)

# Compute mel spectrogram
mel_spec = librosa.feature.melspectrogram(
    y=dithered_audio,
    sr=sample_rate,
    n_fft=preprocessor.n_fft,
    hop_length=preprocessor.hop_length,
    win_length=preprocessor.win_length,
    window='hann',
    n_mels=preprocessor.nfilt,
    fmin=0.0,
    fmax=sample_rate/2
)

# Convert to log scale
log_mel = np.log(mel_spec + 1e-10)

print(f"Librosa mel shape: {log_mel.shape}")
print(f"Librosa mel range: [{log_mel.min():.4f}, {log_mel.max():.4f}]")
print(f"Librosa mel mean: {log_mel.mean():.4f}, std: {log_mel.std():.4f}")

# Compare shapes
print(f"\nNeMo features: {features.shape}")
print(f"Librosa features: {log_mel.shape}")

# Save manual features
np.save("librosa_mel_features.npy", log_mel)

print("\nFeatures saved to nemo_preprocessor_features.npy and librosa_mel_features.npy")