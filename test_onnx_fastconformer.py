#!/usr/bin/env python3
"""
Test script for FastConformer ONNX model
Tests the existing conformer_ctc_dynamic.onnx model
"""

import numpy as np
import onnxruntime as ort
import soundfile as sf
import os

def test_fastconformer_onnx():
    """Test the FastConformer ONNX model"""
    
    # Model and test file paths
    model_path = "models/nemo_fastconformer_streaming/conformer_ctc_dynamic.onnx"
    audio_path = "test_data/audio/librispeech-1995-1837-0001.wav"
    vocab_path = "models/nemo_fastconformer_streaming/tokenizer.txt"
    
    print(f"Testing FastConformer ONNX model: {model_path}")
    
    # Check if files exist
    if not os.path.exists(model_path):
        print(f"Error: Model not found at {model_path}")
        return
    
    if not os.path.exists(audio_path):
        print(f"Error: Audio file not found at {audio_path}")
        print("Using alternative test file...")
        audio_path = "test_data/audio/0.wav"
    
    # Load the ONNX model
    print("Loading ONNX model...")
    try:
        session = ort.InferenceSession(model_path)
        print("Model loaded successfully!")
        
        # Print model information
        print("\nModel inputs:")
        for input in session.get_inputs():
            print(f"  - {input.name}: {input.shape} ({input.type})")
        
        print("\nModel outputs:")
        for output in session.get_outputs():
            print(f"  - {output.name}: {output.shape} ({output.type})")
            
    except Exception as e:
        print(f"Error loading model: {e}")
        return
    
    # Load audio
    print(f"\nLoading audio: {audio_path}")
    try:
        audio_data, sample_rate = sf.read(audio_path)
        print(f"Audio loaded: {len(audio_data)} samples at {sample_rate} Hz")
        
        # Convert to mono if stereo
        if len(audio_data.shape) > 1:
            audio_data = audio_data.mean(axis=1)
            
        # Ensure float32
        audio_data = audio_data.astype(np.float32)
        
        print(f"Audio shape: {audio_data.shape}")
        
    except Exception as e:
        print(f"Error loading audio: {e}")
        return
    
    # Try to run inference
    print("\nAttempting inference...")
    try:
        # Prepare input - FastConformer expects batched audio
        # Shape should be [batch_size, num_samples]
        audio_input = audio_data.reshape(1, -1)
        
        # Create length tensor
        audio_lengths = np.array([len(audio_data)], dtype=np.int32)
        
        # Run inference
        outputs = session.run(None, {
            session.get_inputs()[0].name: audio_input,
            session.get_inputs()[1].name: audio_lengths
        })
        
        print("Inference successful!")
        print(f"Output shape: {outputs[0].shape}")
        
        # Decode output if vocab exists
        if os.path.exists(vocab_path):
            print("\nDecoding output...")
            # Basic greedy decoding
            logits = outputs[0][0]  # Remove batch dimension
            predicted_ids = np.argmax(logits, axis=-1)
            
            # Load vocabulary
            with open(vocab_path, 'r') as f:
                vocab = [line.strip() for line in f]
            
            # Decode tokens
            tokens = []
            for idx in predicted_ids:
                if idx < len(vocab):
                    tokens.append(vocab[idx])
            
            # Join tokens and clean up
            text = ''.join(tokens).replace('▁', ' ').strip()
            print(f"Transcription: {text}")
        
    except Exception as e:
        print(f"Error during inference: {e}")
        print(f"Error type: {type(e).__name__}")
        import traceback
        traceback.print_exc()

if __name__ == "__main__":
    test_fastconformer_onnx()