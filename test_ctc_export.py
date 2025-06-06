#!/usr/bin/env python3
"""
Test the CTC-exported ONNX model to verify it works
"""
import onnxruntime as ort
import numpy as np
import librosa
import os

def load_tokens(tokens_file):
    """Load vocabulary from tokens file"""
    vocab = {}
    with open(tokens_file, 'r') as f:
        for line in f:
            parts = line.strip().split(' ', 1)
            if len(parts) == 2:
                token, idx = parts
                vocab[int(idx)] = token
    return vocab

def ctc_decode_simple(logits, vocab, blank_id):
    """Simple CTC decoding - remove blanks and consecutive duplicates"""
    predicted_ids = np.argmax(logits, axis=-1)
    
    # Remove consecutive duplicates
    decoded_ids = []
    prev_id = None
    for token_id in predicted_ids:
        if token_id != prev_id:
            decoded_ids.append(token_id)
            prev_id = token_id
    
    # Remove blank tokens
    decoded_ids = [id for id in decoded_ids if id != blank_id]
    
    # Convert to text
    text = ""
    for token_id in decoded_ids:
        if token_id in vocab:
            token = vocab[token_id]
            if token.startswith('▁'):
                text += " " + token[1:]  # Remove SentencePiece prefix
            else:
                text += token
    
    return text.strip()

def test_ctc_model():
    print("=== Testing CTC-Exported ONNX Model ===")
    
    model_path = "models/fastconformer_ctc_export/model.onnx"
    tokens_path = "models/fastconformer_ctc_export/tokens.txt"
    
    if not os.path.exists(model_path):
        print(f"❌ Model file not found: {model_path}")
        return
    
    if not os.path.exists(tokens_path):
        print(f"❌ Tokens file not found: {tokens_path}")
        return
    
    print("Loading ONNX model...")
    try:
        session = ort.InferenceSession(model_path)
        print(f"✅ Model loaded successfully")
        
        # Print model info
        print("\nModel Input/Output Info:")
        for input_meta in session.get_inputs():
            print(f"  Input: {input_meta.name}, shape: {input_meta.shape}, type: {input_meta.type}")
        for output_meta in session.get_outputs():
            print(f"  Output: {output_meta.name}, shape: {output_meta.shape}, type: {output_meta.type}")
        
    except Exception as e:
        print(f"❌ Failed to load ONNX model: {e}")
        return
    
    print("\nLoading vocabulary...")
    vocab = load_tokens(tokens_path)
    blank_id = max(vocab.keys())  # Blank is typically the last token
    print(f"✅ Loaded {len(vocab)} tokens, blank_id: {blank_id}")
    
    # Test with a sample audio file
    test_audio_files = [
        "test_data/audio/librispeech-1995-1837-0001.wav",
        "test_data/audio/ibm_culture_16k.wav"
    ]
    
    for audio_file in test_audio_files:
        if os.path.exists(audio_file):
            print(f"\n--- Testing with: {audio_file} ---")
            
            try:
                # Load and preprocess audio
                print("Loading audio...")
                audio, sr = librosa.load(audio_file, sr=16000)
                print(f"Audio shape: {audio.shape}, duration: {len(audio)/sr:.2f}s")
                
                # Extract mel features - model expects [batch, features, time]
                print("Extracting features...")
                mel_spec = librosa.feature.melspectrogram(
                    y=audio, sr=sr, n_fft=512, hop_length=160, 
                    win_length=400, n_mels=80, fmin=0.0, fmax=8000
                )
                log_mel = np.log(mel_spec + 1e-10)  # [80, time]
                
                # Prepare input for ONNX model
                # Model expects [batch, features, time] for audio_signal
                audio_signal = log_mel[np.newaxis, :, :]  # [1, 80, time]
                length = np.array([log_mel.shape[1]], dtype=np.int64)  # [1] - length in frames
                
                print(f"Input shapes - audio_signal: {audio_signal.shape}, length: {length.shape}")
                
                # Run inference
                print("Running ONNX inference...")
                inputs = {
                    session.get_inputs()[0].name: audio_signal.astype(np.float32),
                    session.get_inputs()[1].name: length
                }
                
                outputs = session.run(None, inputs)
                logits = outputs[0]  # [batch, time, vocab_size]
                
                print(f"Output logits shape: {logits.shape}")
                
                # Decode CTC output
                print("Decoding CTC output...")
                text = ctc_decode_simple(logits[0], vocab, blank_id)
                
                print(f"✅ CTC Result: '{text}'")
                    
            except Exception as e:
                print(f"❌ Error processing {audio_file}: {e}")
                import traceback
                traceback.print_exc()
        else:
            print(f"⚠️ Test audio file not found: {audio_file}")
    
    print("\n=== CTC Test Complete ===")

if __name__ == "__main__":
    test_ctc_model()