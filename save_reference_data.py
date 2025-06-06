#!/usr/bin/env python3
"""
Save reference data for C++ debugging
This creates ground truth data to compare against
"""
import numpy as np
import librosa
import os

def save_reference_data():
    audio_file = "test_data/audio/librispeech-1995-1837-0001.wav"
    
    print(f"Loading audio from: {audio_file}")
    
    # Load audio
    audio, sr = librosa.load(audio_file, sr=16000)
    print(f"Loaded {len(audio)} samples at {sr}Hz")
    print(f"Audio range: [{audio.min():.4f}, {audio.max():.4f}]")
    
    # Create reference directory
    os.makedirs("reference_data", exist_ok=True)
    
    # Save raw audio for C++ to load
    audio.astype(np.float32).tofile("reference_data/audio_raw.bin")
    print(f"Saved raw audio to reference_data/audio_raw.bin")
    
    # Save audio metadata
    with open("reference_data/audio_info.txt", "w") as f:
        f.write(f"samples: {len(audio)}\n")
        f.write(f"sample_rate: {sr}\n")
        f.write(f"duration_seconds: {len(audio)/sr:.3f}\n")
    
    # Extract features step by step
    print("\nExtracting features step by step...")
    
    # 1. Compute STFT
    D = librosa.stft(audio, n_fft=512, hop_length=160, win_length=400, window='hann', center=True)
    print(f"STFT shape: {D.shape}")
    
    # 2. Power spectrogram
    S = np.abs(D)**2
    print(f"Power spectrum shape: {S.shape}")
    
    # 3. Mel spectrogram
    mel_spec = librosa.feature.melspectrogram(
        y=audio, sr=sr, n_fft=512, hop_length=160, 
        win_length=400, n_mels=80, fmin=0.0, fmax=8000,
        htk=False, norm='slaney'
    )
    print(f"Mel spectrogram shape: {mel_spec.shape}")
    print(f"Mel spec range: [{mel_spec.min():.6f}, {mel_spec.max():.6f}]")
    
    # 4. Log mel spectrogram
    log_mel = np.log(mel_spec + 1e-10)
    print(f"Log mel shape: {log_mel.shape}")
    print(f"Log mel range: [{log_mel.min():.4f}, {log_mel.max():.4f}]")
    
    # Save all intermediate steps
    np.save("reference_data/stft_complex.npy", D)
    np.save("reference_data/power_spectrum.npy", S)
    np.save("reference_data/mel_spectrogram.npy", mel_spec)
    np.save("reference_data/log_mel_spectrogram.npy", log_mel)
    
    # Save in binary format for C++
    S.astype(np.float32).tofile("reference_data/power_spectrum.bin")
    mel_spec.astype(np.float32).tofile("reference_data/mel_spectrogram.bin")
    log_mel.astype(np.float32).tofile("reference_data/log_mel_spectrogram.bin")
    
    # Save feature info
    with open("reference_data/feature_info.txt", "w") as f:
        f.write(f"n_fft: 512\n")
        f.write(f"hop_length: 160\n")
        f.write(f"win_length: 400\n")
        f.write(f"n_mels: 80\n")
        f.write(f"fmin: 0.0\n")
        f.write(f"fmax: 8000.0\n")
        f.write(f"feature_frames: {log_mel.shape[1]}\n")
        f.write(f"feature_dims: {log_mel.shape[0]}\n")
    
    # Print first few values for comparison
    print("\nFirst 5 log mel features:")
    print(log_mel.flatten()[:5])
    
    # Also save the mel filter bank for debugging
    mel_basis = librosa.filters.mel(sr=sr, n_fft=512, n_mels=80, fmin=0.0, fmax=8000, htk=False, norm='slaney')
    np.save("reference_data/mel_filterbank.npy", mel_basis)
    mel_basis.astype(np.float32).tofile("reference_data/mel_filterbank.bin")
    print(f"\nMel filterbank shape: {mel_basis.shape}")
    
    print("\n✅ Reference data saved to reference_data/")
    print("Use this data to debug C++ feature extraction")

if __name__ == "__main__":
    save_reference_data()