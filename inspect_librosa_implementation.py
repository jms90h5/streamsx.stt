#!/usr/bin/env python3
"""
Inspect librosa's mel spectrogram implementation details
"""
import numpy as np
import librosa
import soundfile as sf

def inspect_librosa_details():
    print("=== Inspecting Librosa Implementation Details ===")
    
    # Load audio
    audio, sample_rate = sf.read("test_data/audio/librispeech-1995-1837-0001.wav")
    print(f"Audio shape: {audio.shape}, sample rate: {sample_rate}")
    
    # NeMo parameters
    n_fft = 512
    hop_length = 160  # 10ms * 16000 / 1000
    win_length = 400  # 25ms * 16000 / 1000
    n_mels = 80
    dither = 1e-05
    
    # Apply dithering
    np.random.seed(42)  # For reproducible results
    dithered_audio = audio + np.random.uniform(-dither, dither, size=audio.shape)
    
    print(f"\nStep-by-step analysis:")
    
    # 1. STFT
    print(f"\n1. Computing STFT...")
    stft = librosa.stft(
        y=dithered_audio,
        n_fft=n_fft,
        hop_length=hop_length,
        win_length=win_length,
        window='hann'
    )
    
    print(f"STFT shape: {stft.shape}")
    print(f"STFT dtype: {stft.dtype}")
    
    # 2. Power spectrum
    print(f"\n2. Computing power spectrum...")
    power_spec = np.abs(stft) ** 2
    print(f"Power spectrum shape: {power_spec.shape}")
    print(f"Power spectrum range: [{power_spec.min():.6f}, {power_spec.max():.6f}]")
    print(f"Power spectrum mean: {power_spec.mean():.6f}")
    
    # 3. Mel filterbank
    print(f"\n3. Computing mel filterbank...")
    mel_basis = librosa.filters.mel(
        sr=sample_rate,
        n_fft=n_fft,
        n_mels=n_mels,
        fmin=0.0,
        fmax=sample_rate/2
    )
    
    print(f"Mel basis shape: {mel_basis.shape}")
    print(f"Mel basis range: [{mel_basis.min():.6f}, {mel_basis.max():.6f}]")
    
    # 4. Apply mel filterbank
    print(f"\n4. Applying mel filterbank...")
    mel_spec = np.dot(mel_basis, power_spec)
    print(f"Mel spectrogram shape: {mel_spec.shape}")
    print(f"Mel spectrogram range: [{mel_spec.min():.6f}, {mel_spec.max():.6f}]")
    
    # 5. Log transform
    print(f"\n5. Applying log transform...")
    log_mel = np.log(mel_spec + 1e-10)
    print(f"Log mel shape: {log_mel.shape}")
    print(f"Log mel range: [{log_mel.min():.4f}, {log_mel.max():.4f}]")
    print(f"Log mel mean: {log_mel.mean():.4f}, std: {log_mel.std():.4f}")
    
    # Compare with librosa's direct method
    print(f"\n6. Comparing with librosa.feature.melspectrogram...")
    mel_direct = librosa.feature.melspectrogram(
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
    
    log_mel_direct = np.log(mel_direct + 1e-10)
    print(f"Direct method shape: {log_mel_direct.shape}")
    print(f"Direct method range: [{log_mel_direct.min():.4f}, {log_mel_direct.max():.4f}]")
    
    # Check if they match
    diff = np.abs(log_mel - log_mel_direct)
    print(f"Difference between methods: max={diff.max():.10f}, mean={diff.mean():.10f}")
    
    # Save intermediate results for C++ debugging
    np.save("librosa_power_spectrum.npy", power_spec)
    np.save("librosa_mel_basis.npy", mel_basis)
    np.save("librosa_mel_spec.npy", mel_spec)
    
    print(f"\nSaved intermediate results for debugging.")
    
    # Look at first frame in detail
    print(f"\n7. First frame analysis...")
    first_frame_power = power_spec[:, 0]
    first_frame_mel = mel_spec[:, 0]
    first_frame_log = log_mel[:, 0]
    
    print(f"First frame power spectrum (first 10 bins): {first_frame_power[:10]}")
    print(f"First frame mel spectrum (first 10 bins): {first_frame_mel[:10]}")
    print(f"First frame log mel (first 10 bins): {first_frame_log[:10]}")
    
    # Check mel filterbank details
    print(f"\n8. Mel filterbank analysis...")
    print(f"Mel basis first filter (first 10 values): {mel_basis[0, :10]}")
    print(f"Mel basis sum per filter (first 10): {np.sum(mel_basis, axis=1)[:10]}")

if __name__ == "__main__":
    inspect_librosa_details()