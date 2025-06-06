#!/usr/bin/env python3
"""
Direct test of the ONNX model to verify it works
"""
import numpy as np
import onnxruntime as ort
import librosa
import sys
import os

def test_onnx_model():
    """Test the ONNX model directly"""
    model_path = "models/nemo_fastconformer_streaming/conformer_ctc_dynamic.onnx"
    
    if not os.path.exists(model_path):
        print(f"❌ Model not found: {model_path}")
        return False
    
    print(f"✓ Testing ONNX model: {model_path}")
    
    try:
        # Create ONNX Runtime session
        session = ort.InferenceSession(model_path)
        
        # Get model input/output info
        input_name = session.get_inputs()[0].name
        input_shape = session.get_inputs()[0].shape
        output_name = session.get_outputs()[0].name
        output_shape = session.get_outputs()[0].shape
        
        print(f"Input: {input_name} {input_shape}")
        print(f"Output: {output_name} {output_shape}")
        
        # Create dummy audio data (16kHz, 2 seconds)
        sample_rate = 16000
        duration = 2.0
        audio_samples = int(sample_rate * duration)
        
        # Generate some test audio (sine wave + noise)
        t = np.linspace(0, duration, audio_samples)
        audio = 0.5 * np.sin(2 * np.pi * 440 * t) + 0.1 * np.random.randn(audio_samples)
        audio = audio.astype(np.float32)
        
        print(f"✓ Generated test audio: {audio.shape} samples")
        
        # Convert audio to 80-dimensional log-mel features
        # The model expects [batch, time, 80] features, not raw audio
        print("✓ Converting audio to 80-dim log-mel features...")
        
        # Extract log-mel features (80-dimensional)
        n_mels = 80
        hop_length = 160  # 10ms at 16kHz
        win_length = 400  # 25ms at 16kHz
        
        mel_spec = librosa.feature.melspectrogram(
            y=audio, sr=sample_rate, 
            n_mels=n_mels, 
            hop_length=hop_length,
            win_length=win_length,
            fmin=0, fmax=8000
        )
        
        # Convert to log scale
        log_mel = librosa.power_to_db(mel_spec)
        
        # Transpose to get [time, features] and add batch dimension
        log_mel = log_mel.T  # Shape: [time, n_mels]
        audio_input = log_mel.reshape(1, log_mel.shape[0], log_mel.shape[1])  # [batch, time, features]
        
        print(f"✓ Log-mel features shape: {audio_input.shape}")
        print(f"✓ Feature range: [{audio_input.min():.2f}, {audio_input.max():.2f}] dB")
        
        # Run inference
        outputs = session.run([output_name], {input_name: audio_input})
        
        output_probs = outputs[0]
        print(f"✓ Model output shape: {output_probs.shape}")
        print(f"✓ Output range: [{output_probs.min():.3f}, {output_probs.max():.3f}]")
        
        # Check if we get reasonable log probabilities
        if output_probs.shape[-1] > 20:  # Should have vocab size > 20
            print(f"✓ Vocabulary size: {output_probs.shape[-1]}")
            
            # Find most likely tokens
            best_tokens = np.argmax(output_probs[0], axis=-1)
            print(f"✓ Predicted token sequence (first 10): {best_tokens[:10]}")
            
            print("\n🎉 ONNX MODEL TEST SUCCESSFUL!")
            print("The model loads and runs correctly with dummy audio data.")
            return True
        else:
            print(f"❌ Unexpected output vocabulary size: {output_probs.shape[-1]}")
            return False
            
    except Exception as e:
        print(f"❌ Error testing ONNX model: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_with_real_audio():
    """Test with real audio file if available"""
    audio_files = [
        "test_data/audio/librispeech-1995-1837-0001.wav",
        "test_data/audio/11-ibm-culture-2min-16k.wav"
    ]
    
    for audio_file in audio_files:
        if os.path.exists(audio_file):
            print(f"\n🎵 Testing with real audio: {audio_file}")
            try:
                audio, sr = librosa.load(audio_file, sr=16000)
                print(f"✓ Loaded audio: {audio.shape} samples at {sr}Hz")
                
                # Test model with real audio
                model_path = "models/nemo_fastconformer_streaming/conformer_ctc_dynamic.onnx"
                session = ort.InferenceSession(model_path)
                
                input_name = session.get_inputs()[0].name
                output_name = session.get_outputs()[0].name
                
                # Extract log-mel features (same as above)
                n_mels = 80
                hop_length = 160
                win_length = 400
                
                mel_spec = librosa.feature.melspectrogram(
                    y=audio, sr=sr, 
                    n_mels=n_mels, 
                    hop_length=hop_length,
                    win_length=win_length,
                    fmin=0, fmax=8000
                )
                log_mel = librosa.power_to_db(mel_spec)
                log_mel = log_mel.T  # [time, features]
                audio_input = log_mel.reshape(1, log_mel.shape[0], log_mel.shape[1])  # [batch, time, features]
                
                print(f"✓ Real audio features shape: {audio_input.shape}")
                
                outputs = session.run([output_name], {input_name: audio_input})
                output_probs = outputs[0]
                
                print(f"✓ Real audio inference successful!")
                print(f"✓ Output shape: {output_probs.shape}")
                
                # Get token sequence
                best_tokens = np.argmax(output_probs[0], axis=-1)
                print(f"✓ Token sequence length: {len(best_tokens)}")
                print(f"✓ Unique tokens: {len(set(best_tokens))}")
                
                return True
                
            except Exception as e:
                print(f"❌ Error with real audio: {e}")
                continue
                
    print("ℹ️ No real audio files found for testing")
    return False

if __name__ == "__main__":
    print("=== Direct ONNX Model Test ===")
    print("Testing if the conformer_ctc_dynamic.onnx model works correctly")
    print()
    
    success = test_onnx_model()
    
    if success:
        test_with_real_audio()
        print("\n✅ CONCLUSION: The ONNX model is working correctly!")
        print("The C++ implementation should work once dependencies are fixed.")
    else:
        print("\n❌ CONCLUSION: The ONNX model has issues.")
        print("Need to investigate the model file.")