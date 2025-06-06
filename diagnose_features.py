#!/usr/bin/env python3
"""
Diagnose feature differences between Python and C++
"""
import numpy as np
import librosa
import onnxruntime as ort

def extract_python_features(audio_file):
    """Extract features the Python way"""
    print("=== Python Feature Extraction ===")
    
    # Load audio
    audio, sr = librosa.load(audio_file, sr=16000)
    print(f"Audio shape: {audio.shape}")
    
    # Extract mel features
    mel_spec = librosa.feature.melspectrogram(
        y=audio, sr=sr, n_fft=512, hop_length=160, 
        win_length=400, n_mels=80, fmin=0.0, fmax=8000
    )
    log_mel = np.log(mel_spec + 1e-10)
    
    print(f"Mel spec shape: {mel_spec.shape}")
    print(f"Log mel shape: {log_mel.shape}")
    print(f"Log mel range: [{log_mel.min():.4f}, {log_mel.max():.4f}]")
    print(f"First 5 features (before batch): {log_mel.flatten()[:5]}")
    
    # Prepare for model [batch, features, time]
    audio_signal = log_mel[np.newaxis, :, :]
    
    print(f"Model input shape: {audio_signal.shape}")
    print(f"First 5 features (after batch): {audio_signal.flatten()[:5]}")
    
    # Save features for comparison
    np.save('python_features.npy', audio_signal)
    print("Saved Python features to python_features.npy")
    
    return audio_signal

def test_with_model(features):
    """Test features with the model"""
    print("\n=== Testing with Model ===")
    
    session = ort.InferenceSession("models/fastconformer_ctc_export/model.onnx")
    
    # Prepare inputs
    length = np.array([features.shape[2]], dtype=np.int64)
    
    print(f"Input shapes - features: {features.shape}, length: {length}")
    
    # Run inference
    outputs = session.run(None, {
        "audio_signal": features.astype(np.float32),
        "length": length
    })
    
    logits = outputs[0]
    print(f"Output shape: {logits.shape}")
    
    # Check predictions
    predictions = np.argmax(logits[0], axis=-1)
    print(f"First 5 predictions: {predictions[:5]}")
    print(f"Unique predictions: {np.unique(predictions)[:10]}")
    
    # Load vocab and decode
    vocab = {}
    with open("models/fastconformer_ctc_export/tokens.txt", 'r') as f:
        for line in f:
            parts = line.strip().split(' ', 1)
            if len(parts) == 2:
                token, idx = parts
                vocab[int(idx)] = token
    
    # Simple decode
    prev_token = -1
    result = []
    for token_id in predictions:
        if token_id != prev_token and token_id != 1024:  # Skip blanks and duplicates
            if token_id in vocab:
                result.append(vocab[token_id])
        prev_token = token_id
    
    text = ""
    for token in result:
        if token.startswith('▁'):
            text += " " + token[1:]
        else:
            text += token
    
    print(f"Decoded text: '{text.strip()}'")
    
    return logits

if __name__ == "__main__":
    audio_file = "test_data/audio/librispeech-1995-1837-0001.wav"
    
    # Extract and test
    features = extract_python_features(audio_file)
    logits = test_with_model(features)
    
    print("\n=== Summary ===")
    print("1. Check if python_features.npy matches C++ features")
    print("2. Python should produce good transcription")
    print("3. If Python works but C++ doesn't, it's a feature mismatch")