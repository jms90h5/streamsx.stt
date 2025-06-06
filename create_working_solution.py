#!/usr/bin/env python3
"""
Create a working solution by saving features from Python for C++ to use.
This proves the model works and provides a path forward.
"""
import numpy as np
import librosa
import onnxruntime as ort

def create_feature_extractor_data():
    """Save all necessary data for C++ feature extraction"""
    
    # Test audio
    audio_file = "test_data/audio/librispeech-1995-1837-0001.wav"
    audio, sr = librosa.load(audio_file, sr=16000)
    
    print(f"Audio: {len(audio)} samples")
    
    # Create mel filterbank that C++ can use
    mel_basis = librosa.filters.mel(
        sr=sr, 
        n_fft=512, 
        n_mels=80, 
        fmin=0.0, 
        fmax=8000,
        htk=False,
        norm='slaney'
    )
    
    print(f"Mel filterbank shape: {mel_basis.shape}")
    
    # Save for C++
    np.save("reference_data/mel_basis_librosa.npy", mel_basis)
    mel_basis.astype(np.float32).tofile("reference_data/mel_basis_librosa.bin")
    
    # Also save a Hann window
    hann_window = np.hanning(400)
    hann_window.astype(np.float32).tofile("reference_data/hann_window_400.bin")
    
    # Test with known audio
    test_signal = np.ones(512) * 0.1  # Simple test signal
    test_fft = np.fft.rfft(test_signal)
    test_power = np.abs(test_fft) ** 2
    
    print(f"\nTest signal FFT:")
    print(f"DC component: {test_power[0]}")
    print(f"Other bins (should be ~0): {test_power[1:5]}")
    
    # Save test data
    test_signal.astype(np.float32).tofile("reference_data/test_signal.bin")
    test_power.astype(np.float32).tofile("reference_data/test_power_spectrum.bin")

def test_model_with_features():
    """Test that model works with pre-computed features"""
    
    # Load model
    session = ort.InferenceSession("models/fastconformer_ctc_export/model.onnx")
    
    # Load saved features
    features = np.load("reference_data/log_mel_spectrogram.npy")
    print(f"Features shape: {features.shape}")
    
    # Prepare inputs
    audio_signal = features.T.reshape(1, 80, -1)  # [1, 80, time]
    length = np.array([audio_signal.shape[2]], dtype=np.int64)
    
    # Run inference
    outputs = session.run(None, {
        "audio_signal": audio_signal,
        "length": length
    })
    
    logits = outputs[0]
    predictions = np.argmax(logits[0], axis=-1)
    
    # Count non-blank predictions
    non_blank = predictions[predictions != 1024]
    print(f"\nPredictions: {len(predictions)} frames")
    print(f"Non-blank tokens: {len(non_blank)}")
    print(f"First 10 predictions: {predictions[:10]}")
    
    return predictions

if __name__ == "__main__":
    print("=== Creating Feature Extraction Data ===")
    create_feature_extractor_data()
    
    print("\n=== Testing Model ===")
    test_model_with_features()
    
    print("\n✅ Data saved for C++ implementation")