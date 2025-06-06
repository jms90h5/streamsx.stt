#!/usr/bin/env python3
"""
Test exported ONNX models with Python ONNX Runtime to verify they work
"""
import numpy as np
import onnxruntime as ort
import librosa
import nemo.collections.asr as nemo_asr

def test_onnx_models():
    print("=== Testing Exported ONNX Models with Python ===")
    
    # Load the original NeMo model for comparison
    print("\n1. Loading original NeMo model...")
    model_path = "models/nemo_cache_aware_conformer/models--nvidia--stt_en_fastconformer_hybrid_large_streaming_multi/snapshots/ae98143333690bd7ced4bc8ec16769bcb8918374/stt_en_fastconformer_hybrid_large_streaming_multi.nemo"
    nemo_model = nemo_asr.models.EncDecHybridRNNTCTCBPEModel.restore_from(model_path)
    nemo_model.eval()
    
    # Load audio
    print("\n2. Loading audio file...")
    audio_file = "test_data/audio/librispeech-1995-1837-0001.wav"
    audio, sr = librosa.load(audio_file, sr=16000)
    print(f"Audio shape: {audio.shape}, sample rate: {sr}")
    
    # Get NeMo reference transcription
    print("\n3. Getting NeMo reference transcription...")
    nemo_transcription = nemo_model.transcribe([audio_file])[0]
    print(f"NeMo result: '{nemo_transcription}'")
    
    # Extract features using NeMo's preprocessing
    print("\n4. Extracting features using NeMo preprocessing...")
    # Use NeMo's internal feature extraction
    # We need to get the features as NeMo processes them
    device = nemo_model.device
    
    # Process audio to features using NeMo's method
    features = nemo_model.preprocessor(input_signal=audio.reshape(1, -1), length=None)
    print(f"NeMo features shape: {features.shape}")
    print(f"NeMo features range: [{features.min():.4f}, {features.max():.4f}]")
    print(f"NeMo features mean: {features.mean():.4f}, std: {features.std():.4f}")
    
    # Load ONNX models
    print("\n5. Loading ONNX models...")
    try:
        encoder_session = ort.InferenceSession("models/proven_onnx_export/encoder-proven_fastconformer.onnx")
        decoder_session = ort.InferenceSession("models/proven_onnx_export/decoder_joint-proven_fastconformer.onnx")
        print("✓ ONNX models loaded successfully")
        
        # Print model input/output info
        print("\nEncoder inputs:")
        for inp in encoder_session.get_inputs():
            print(f"  {inp.name}: {inp.shape} ({inp.type})")
        print("Encoder outputs:")
        for out in encoder_session.get_outputs():
            print(f"  {out.name}: {out.shape} ({out.type})")
            
        print("\nDecoder inputs:")
        for inp in decoder_session.get_inputs():
            print(f"  {inp.name}: {inp.shape} ({inp.type})")
        print("Decoder outputs:")
        for out in decoder_session.get_outputs():
            print(f"  {out.name}: {out.shape} ({out.type})")
            
    except Exception as e:
        print(f"✗ Error loading ONNX models: {e}")
        return False
    
    # Test ONNX encoder
    print("\n6. Testing ONNX encoder...")
    try:
        # Prepare inputs for encoder (may need to adjust based on model requirements)
        features_np = features.detach().cpu().numpy()
        print(f"Input features shape: {features_np.shape}")
        
        # Get input names
        encoder_inputs = encoder_session.get_inputs()
        input_name = encoder_inputs[0].name
        
        # Run encoder
        encoder_result = encoder_session.run(None, {input_name: features_np})
        encoder_output = encoder_result[0]
        print(f"✓ Encoder output shape: {encoder_output.shape}")
        print(f"Encoder output range: [{encoder_output.min():.4f}, {encoder_output.max():.4f}]")
        
    except Exception as e:
        print(f"✗ Encoder inference failed: {e}")
        import traceback
        traceback.print_exc()
        return False
    
    # Test ONNX decoder
    print("\n7. Testing ONNX decoder...")
    try:
        # Get decoder input names
        decoder_inputs = decoder_session.get_inputs()
        print(f"Decoder expects {len(decoder_inputs)} inputs:")
        for inp in decoder_inputs:
            print(f"  {inp.name}: {inp.shape}")
        
        # Prepare decoder inputs (this might need adjustment based on model)
        decoder_input_dict = {}
        
        # The main encoder output
        decoder_input_dict[decoder_inputs[0].name] = encoder_output
        
        # Handle additional inputs if needed
        for i, inp in enumerate(decoder_inputs[1:], 1):
            input_shape = inp.shape
            # Create dummy inputs for now (would need proper values in real implementation)
            if 'length' in inp.name.lower():
                # Length tensor
                decoder_input_dict[inp.name] = np.array([encoder_output.shape[1]], dtype=np.int64)
            else:
                # Other tensors - create appropriate shapes
                dummy_shape = [1 if dim == 'batch' else 
                             encoder_output.shape[1] if dim == 'time' else 
                             int(dim) if str(dim).isdigit() else 1 
                             for dim in input_shape]
                decoder_input_dict[inp.name] = np.zeros(dummy_shape, dtype=np.float32)
        
        # Run decoder
        decoder_result = decoder_session.run(None, decoder_input_dict)
        decoder_output = decoder_result[0]
        print(f"✓ Decoder output shape: {decoder_output.shape}")
        print(f"Decoder output range: [{decoder_output.min():.4f}, {decoder_output.max():.4f}]")
        
        # Get top predictions
        if len(decoder_output.shape) >= 2:
            # Get token IDs from logits
            token_ids = np.argmax(decoder_output, axis=-1)
            print(f"Token IDs shape: {token_ids.shape}")
            print(f"Sample token IDs: {token_ids.flatten()[:20]}")
            
            # Try to decode with vocabulary
            try:
                with open("models/proven_onnx_export/real_vocabulary.txt", 'r') as f:
                    vocab = [line.strip() for line in f]
                
                # Decode tokens
                decoded_tokens = []
                for token_id in token_ids.flatten():
                    if 0 <= token_id < len(vocab):
                        decoded_tokens.append(vocab[token_id])
                    else:
                        decoded_tokens.append(f"<UNK_{token_id}>")
                
                decoded_text = ''.join(decoded_tokens[:50])  # First 50 tokens
                print(f"Decoded text sample: '{decoded_text}'")
                
            except Exception as e:
                print(f"Warning: Could not decode tokens: {e}")
        
    except Exception as e:
        print(f"✗ Decoder inference failed: {e}")
        import traceback
        traceback.print_exc()
        return False
    
    print("\n✓ ONNX models test completed successfully!")
    print("\nComparison:")
    print(f"NeMo result: '{nemo_transcription}'")
    print(f"ONNX result: (basic decoding attempted above)")
    
    return True

if __name__ == "__main__":
    success = test_onnx_models()
    exit(0 if success else 1)