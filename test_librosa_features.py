#!/usr/bin/env python3
"""
Test librosa mel spectrogram extraction to match NeMo
"""
import numpy as np
import librosa
import soundfile as sf

def extract_librosa_features():
    print("=== Testing Librosa Mel Spectrogram ===")
    
    # Load audio
    audio_file = "test_data/audio/librispeech-1995-1837-0001.wav"
    audio, sample_rate = sf.read(audio_file)
    print(f"Audio shape: {audio.shape}, sample rate: {sample_rate}")
    
    # NeMo parameters from config
    n_fft = 512
    hop_length = 160  # 10ms * 16000 / 1000
    win_length = 400  # 25ms * 16000 / 1000
    n_mels = 80
    dither = 1e-05
    
    print(f"\nParameters:")
    print(f"  n_fft: {n_fft}")
    print(f"  hop_length: {hop_length}")
    print(f"  win_length: {win_length}")
    print(f"  n_mels: {n_mels}")
    print(f"  dither: {dither}")
    
    # Apply dithering
    np.random.seed(42)  # For reproducible results
    dithered_audio = audio + np.random.uniform(-dither, dither, size=audio.shape)
    
    # Compute mel spectrogram using librosa
    mel_spec = librosa.feature.melspectrogram(
        y=dithered_audio,
        sr=sample_rate,
        n_fft=n_fft,
        hop_length=hop_length,
        win_length=win_length,
        window='hann',
        n_mels=n_mels,
        fmin=0.0,
        fmax=sample_rate/2
    )
    
    # Convert to log scale
    log_mel = np.log(mel_spec + 1e-10)
    
    print(f"\nLibrosa results:")
    print(f"  Shape: {log_mel.shape}")
    print(f"  Range: [{log_mel.min():.4f}, {log_mel.max():.4f}]")
    print(f"  Mean: {log_mel.mean():.4f}, std: {log_mel.std():.4f}")
    
    # Save librosa features
    np.save("librosa_mel_features.npy", log_mel)
    
    # Load NeMo features for comparison
    nemo_features = np.load("nemo_features.npy")  # Shape: [1, 80, 874]
    
    print(f"\nNeMo features:")
    print(f"  Shape: {nemo_features.shape}")
    print(f"  Range: [{nemo_features.min():.4f}, {nemo_features.max():.4f}]")
    print(f"  Mean: {nemo_features.mean():.4f}, std: {nemo_features.std():.4f}")
    
    # Compare shapes
    min_time = min(log_mel.shape[1], nemo_features.shape[2])
    librosa_trimmed = log_mel[:, :min_time]
    nemo_trimmed = nemo_features[0, :, :min_time]
    
    # Calculate correlation
    correlation = np.corrcoef(librosa_trimmed.flatten(), nemo_trimmed.flatten())[0, 1]
    print(f"\nLibrosa vs NeMo correlation: {correlation:.6f}")
    
    # First frame comparison
    print(f"\nFirst frame comparison (mel bin 0-9):")
    print(f"Librosa: {librosa_trimmed[:10, 0]}")
    print(f"NeMo:    {nemo_trimmed[:10, 0]}")
    print(f"Diff:    {librosa_trimmed[:10, 0] - nemo_trimmed[:10, 0]}")
    
    return log_mel

if __name__ == "__main__":
    extract_librosa_features()