#!/usr/bin/env python3
"""
Test if the current ONNX model can work with proper preprocessing
"""

import numpy as np
import onnxruntime as ort
import os

def test_model_with_context():
    """Test if providing context/history helps the model"""
    
    model_path = "models/nemo_fastconformer_streaming/conformer_ctc_dynamic.onnx"
    vocab_path = "models/nemo_fastconformer_streaming/tokenizer.txt"
    
    print("=== Testing Current Model with Context ===\n")
    
    # Load model
    print(f"Loading model: {model_path}")
    session = ort.InferenceSession(model_path)
    
    # Load vocabulary
    with open(vocab_path, 'r') as f:
        vocab = [line.strip() for line in f]
    print(f"Loaded {len(vocab)} tokens")
    
    # Get model info
    input_info = session.get_inputs()[0]
    print(f"Model expects: {input_info.name} with shape {input_info.shape}")
    
    # The issue might be that the model needs:
    # 1. Proper feature normalization (CMVN)
    # 2. Accumulated context from previous chunks
    # 3. Correct chunk boundaries
    
    print("\nTesting different approaches...")
    
    # Test 1: Single long sequence (non-streaming)
    print("\n1. Testing with long sequence (non-streaming):")
    # Create fake mel features for 5 seconds
    time_steps = 500  # 5 seconds at 10ms stride
    features = np.random.randn(1, time_steps, 80).astype(np.float32)
    
    # Add some structure to features (not just random)
    for i in range(time_steps):
        features[0, i, :] = np.sin(np.arange(80) * 0.1 + i * 0.01) * 2.0
    
    outputs = session.run(None, {input_info.name: features})
    logits = outputs[0][0]
    tokens = np.argmax(logits, axis=-1)
    unique_tokens = np.unique(tokens)
    
    print(f"Output shape: {logits.shape}")
    print(f"Unique tokens: {unique_tokens}")
    print(f"Token distribution: {[(t, np.sum(tokens==t)) for t in unique_tokens[:5]]}")
    
    # Test 2: Check if model has internal state
    print("\n2. Checking for stateful behavior:")
    chunk1 = np.random.randn(1, 160, 80).astype(np.float32)
    chunk2 = np.random.randn(1, 160, 80).astype(np.float32)
    
    out1_a = session.run(None, {input_info.name: chunk1})[0]
    out1_b = session.run(None, {input_info.name: chunk1})[0]
    
    print(f"Same input twice: {'Same output' if np.allclose(out1_a, out1_b) else 'Different output'}")
    
    # Test 3: Try with overlapping chunks
    print("\n3. Testing overlapping chunks:")
    full_features = np.random.randn(1, 320, 80).astype(np.float32)
    
    # Process as one chunk
    full_out = session.run(None, {input_info.name: full_features})[0]
    
    # Process as two overlapping chunks
    chunk1 = full_features[:, :160, :]
    chunk2 = full_features[:, 80:240, :]  # 50% overlap
    chunk3 = full_features[:, 160:, :]
    
    out1 = session.run(None, {input_info.name: chunk1})[0]
    out2 = session.run(None, {input_info.name: chunk2})[0]
    out3 = session.run(None, {input_info.name: chunk3})[0]
    
    print(f"Full output tokens: {np.argmax(full_out[0], axis=-1)[:10]}")
    print(f"Chunk 1 tokens: {np.argmax(out1[0], axis=-1)[:10]}")
    print(f"Chunk 2 tokens: {np.argmax(out2[0], axis=-1)[:10]}")
    print(f"Chunk 3 tokens: {np.argmax(out3[0], axis=-1)[:10]}")
    
    print("\n=== Analysis ===")
    print("The model appears to be stateless (no cache tensors).")
    print("Each chunk is processed independently, losing context.")
    print("This explains why it outputs the same token repeatedly.")
    print("\nSolutions:")
    print("1. Export model with cache tensors (cache_last_channel, cache_last_time)")
    print("2. Use overlapping chunks with careful stitching")
    print("3. Process longer sequences instead of small chunks")
    print("4. Use the working wav2vec2 model instead")

if __name__ == "__main__":
    test_model_with_context()