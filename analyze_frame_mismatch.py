#!/usr/bin/env python3
"""
Analyze why C++ gets 870 frames but Python gets 874 frames
"""
import numpy as np
import librosa

def analyze_stft_frames():
    audio_file = "test_data/audio/librispeech-1995-1837-0001.wav"
    audio, sr = librosa.load(audio_file, sr=16000)
    
    print(f"Audio samples: {len(audio)}")
    print(f"Sample rate: {sr} Hz")
    print(f"Duration: {len(audio)/sr:.3f} seconds")
    
    # Parameters
    n_fft = 512
    hop_length = 160
    win_length = 400
    
    print(f"\nSTFT parameters:")
    print(f"n_fft: {n_fft}")
    print(f"hop_length: {hop_length}")
    print(f"win_length: {win_length}")
    
    # Calculate expected frames
    # With center=True (default), librosa pads the signal
    n_frames_center_true = 1 + (len(audio) + n_fft//2 * 2) // hop_length
    n_frames_center_false = 1 + (len(audio) - n_fft) // hop_length
    
    print(f"\nExpected frames:")
    print(f"With center=True: {n_frames_center_true}")
    print(f"With center=False: {n_frames_center_false}")
    
    # Actual STFT
    D_center_true = librosa.stft(audio, n_fft=n_fft, hop_length=hop_length, 
                                 win_length=win_length, center=True)
    D_center_false = librosa.stft(audio, n_fft=n_fft, hop_length=hop_length, 
                                  win_length=win_length, center=False)
    
    print(f"\nActual STFT shapes:")
    print(f"With center=True: {D_center_true.shape}")
    print(f"With center=False: {D_center_false.shape}")
    
    # Check mel spectrogram
    mel_center_true = librosa.feature.melspectrogram(
        y=audio, sr=sr, n_fft=n_fft, hop_length=hop_length,
        win_length=win_length, n_mels=80, center=True
    )
    mel_center_false = librosa.feature.melspectrogram(
        y=audio, sr=sr, n_fft=n_fft, hop_length=hop_length,
        win_length=win_length, n_mels=80, center=False
    )
    
    print(f"\nMel spectrogram shapes:")
    print(f"With center=True: {mel_center_true.shape}")
    print(f"With center=False: {mel_center_false.shape}")
    
    # C++ calculation
    # Looks like C++ is using center=False logic
    cpp_frames = (len(audio) - win_length) // hop_length + 1
    print(f"\nC++ calculation (no padding): {cpp_frames}")
    
    # How much padding does librosa add?
    pad_length = n_fft // 2
    padded_length = len(audio) + 2 * pad_length
    frames_with_padding = (padded_length - win_length) // hop_length + 1
    print(f"\nWith padding {pad_length} on each side:")
    print(f"Padded length: {padded_length}")
    print(f"Frames: {frames_with_padding}")

if __name__ == "__main__":
    analyze_stft_frames()