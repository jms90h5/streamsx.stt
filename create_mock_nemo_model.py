#!/usr/bin/env python3
"""
Create a mock NeMo ONNX model for testing the STT pipeline.
This creates a minimal ONNX model with the correct inputs/outputs
that the NeMoCacheAwareConformer implementation expects.
"""

import numpy as np
import os

try:
    import onnx
    from onnx import helper, TensorProto, numpy_helper
except ImportError:
    print("Installing required packages...")
    import subprocess
    subprocess.check_call(["pip3", "install", "--user", "onnx", "numpy"])
    import onnx
    from onnx import helper, TensorProto, numpy_helper

def create_mock_nemo_model(output_path):
    """Create a mock NeMo model with cache support"""
    
    # Define input tensors - matching NeMo cache-aware conformer
    inputs = [
        # Main audio features input
        helper.make_tensor_value_info('audio_signal', TensorProto.FLOAT, [1, 80, None]),  # [batch, features, time]
        helper.make_tensor_value_info('length', TensorProto.INT64, [1]),  # sequence length
        
        # Cache tensors for streaming
        helper.make_tensor_value_info('cache_last_channel', TensorProto.FLOAT, [12, 1, 128, 512]),  # [layers, batch, time, channels]
        helper.make_tensor_value_info('cache_last_time', TensorProto.FLOAT, [12, 1, 512, 128])      # [layers, batch, channels, time]
    ]
    
    # Define output tensors
    outputs = [
        # Log probabilities output
        helper.make_tensor_value_info('logprobs', TensorProto.FLOAT, [1, None, 1024]),  # [batch, time, vocab_size]
        
        # Updated cache tensors
        helper.make_tensor_value_info('cache_last_channel_next', TensorProto.FLOAT, [12, 1, 128, 512]),
        helper.make_tensor_value_info('cache_last_time_next', TensorProto.FLOAT, [12, 1, 512, 128])
    ]
    
    # Create a simple graph that passes through with minimal processing
    # In reality, this would be a complex Conformer model
    
    # Create constant for vocabulary projection
    vocab_proj = numpy_helper.from_array(
        np.random.randn(512, 1024).astype(np.float32),
        name='vocab_projection'
    )
    
    # Create nodes for the graph
    nodes = [
        # Simulate feature processing - just pass through for mock
        helper.make_node('Identity', ['audio_signal'], ['processed_features']),
        
        # Simulate cache update - for mock, just pass through
        helper.make_node('Identity', ['cache_last_channel'], ['cache_last_channel_next']),
        helper.make_node('Identity', ['cache_last_time'], ['cache_last_time_next']),
        
        # Create dummy logprobs output
        # In real model, this would involve attention, conformer blocks, etc.
        helper.make_node('ReduceMean', ['processed_features'], ['features_reduced'], axes=[1]),  # Reduce feature dim
        helper.make_node('MatMul', ['features_reduced', 'vocab_projection'], ['logprobs_raw']),
        helper.make_node('LogSoftmax', ['logprobs_raw'], ['logprobs'], axis=-1)
    ]
    
    # Create the graph
    graph_def = helper.make_graph(
        nodes,
        'NeMoMockConformer',
        inputs,
        outputs,
        [vocab_proj]  # initializers
    )
    
    # Create the model
    model_def = helper.make_model(graph_def, producer_name='mock_nemo_exporter')
    
    # Add metadata
    model_def.opset_import[0].version = 13
    
    # Add model properties that NeMo would add
    prop1 = model_def.metadata_props.add()
    prop1.key = "model_type"
    prop1.value = "FastConformer-Hybrid-Transducer-BPE"
    
    prop2 = model_def.metadata_props.add()
    prop2.key = "cache_aware"
    prop2.value = "true"
    
    prop3 = model_def.metadata_props.add()
    prop3.key = "streaming_latency"
    prop3.value = "80ms"
    
    # Save the model
    os.makedirs(os.path.dirname(output_path), exist_ok=True)
    onnx.save(model_def, output_path)
    
    # Verify the model
    try:
        onnx.checker.check_model(model_def)
        print(f"✓ Mock NeMo model created successfully: {output_path}")
        print(f"  Model inputs: {len(inputs)}")
        for inp in inputs:
            print(f"    - {inp.name}: {[d.dim_value if d.HasField('dim_value') else 'Dynamic' for d in inp.type.tensor_type.shape.dim]}")
        print(f"  Model outputs: {len(outputs)}")
        for out in outputs:
            print(f"    - {out.name}: {[d.dim_value if d.HasField('dim_value') else 'Dynamic' for d in out.type.tensor_type.shape.dim]}")
        return True
    except Exception as e:
        print(f"✗ Model verification failed: {e}")
        return False

def create_simple_tokenizer(output_path):
    """Create a simple character-based tokenizer file"""
    # Simple character vocabulary for testing
    vocab = ['<blank>', '<unk>'] + list(' abcdefghijklmnopqrstuvwxyz') + ['<space>']
    
    os.makedirs(os.path.dirname(output_path), exist_ok=True)
    with open(output_path, 'w') as f:
        for i, token in enumerate(vocab):
            f.write(f"{token}\n")
    
    print(f"✓ Created tokenizer with {len(vocab)} tokens: {output_path}")

def main():
    print("=== Creating Mock NeMo Model for Testing ===")
    
    base_dir = "models/nemo_fastconformer_streaming"
    
    # Create the mock ONNX model
    model_path = os.path.join(base_dir, "fastconformer_streaming.onnx")
    if create_mock_nemo_model(model_path):
        # Create tokenizer
        tokenizer_path = os.path.join(base_dir, "tokenizer.txt")
        create_simple_tokenizer(tokenizer_path)
        
        print("\n=== Mock Model Creation Complete ===")
        print(f"Model location: {model_path}")
        print(f"Tokenizer location: {tokenizer_path}")
        print("\nThis is a MOCK model for testing the pipeline.")
        print("For real transcription, export an actual NeMo model.")
        
        # Create a README in the model directory
        readme_path = os.path.join(base_dir, "README_MOCK.txt")
        with open(readme_path, 'w') as f:
            f.write("MOCK MODEL FOR TESTING\n")
            f.write("======================\n\n")
            f.write("This directory contains a mock NeMo model created for testing.\n")
            f.write("It has the correct input/output structure but will not produce\n")
            f.write("meaningful transcriptions.\n\n")
            f.write("To use a real model:\n")
            f.write("1. Export a NeMo FastConformer model to ONNX\n")
            f.write("2. Replace fastconformer_streaming.onnx with your model\n")
            f.write("3. Update tokenizer.txt with your model's vocabulary\n")
        
        return True
    return False

if __name__ == "__main__":
    import sys
    sys.exit(0 if main() else 1)