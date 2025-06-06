#!/usr/bin/env python3
"""
Test NeMo Cache-Aware Streaming using the official conformer_stream_step method
Following the exact approach from NeMo documentation and examples
"""

import numpy as np
import torch
import torchaudio
import sys
import os

def test_nemo_streaming():
    """Test streaming inference using NeMo's official streaming interface"""
    
    print("=== NeMo Cache-Aware Streaming Test ===")
    print("Using official conformer_stream_step method")
    
    try:
        import nemo.collections.asr as nemo_asr
        
        # Load the exact model as specified
        model_name = "nvidia/stt_en_fastconformer_hybrid_large_streaming_multi"
        print(f"Loading model: {model_name}")
        
        asr_model = nemo_asr.models.EncDecHybridRNNTCTCBPEModel.from_pretrained(model_name)
        asr_model.eval()
        
        print("✓ Model loaded successfully")
        print(f"Model type: {type(asr_model).__name__}")
        
        # Setup streaming parameters (following official examples)
        # Each encoder frame = 40ms (4x downsampling * 10ms shift)
        # chunk_size=100 → 100*40ms = 4000ms in examples
        # For reasonable streaming, use smaller chunk like 40 frames = 1600ms
        
        chunk_size_frames = 40   # 40 encoder frames = 1600ms 
        left_chunks = 2          # 2 chunks of left context (from examples)
        shift_size = 40          # Same as chunk size for non-overlapping
        
        # Configure streaming parameters (following official examples)
        asr_model.encoder.setup_streaming_params(
            chunk_size=chunk_size_frames,
            left_chunks=left_chunks,
            shift_size=shift_size
        )
        
        # Calculate actual chunk size in milliseconds for audio processing
        # Each encoder frame = 40ms, so 40 frames = 1600ms
        chunk_size_ms = chunk_size_frames * 40
        
        print(f"Streaming configured: {chunk_size_ms}ms chunks ({chunk_size_frames} encoder frames)")
        
        # Load test audio
        audio_file = "test_data/audio/librispeech-1995-1837-0001.raw"
        print(f"Loading audio: {audio_file}")
        
        # Read raw audio (16-bit PCM, 16kHz)
        with open(audio_file, 'rb') as f:
            audio_data = np.frombuffer(f.read(), dtype=np.int16)
        
        # Convert to float32 and normalize
        audio_float = audio_data.astype(np.float32) / 32768.0
        
        print(f"Audio loaded: {len(audio_data)} samples, {len(audio_data)/16000:.2f} seconds")
        
        # Initialize cache state (following NeMo examples)
        print("Initializing cache state...")
        cache_state = asr_model.encoder.get_initial_cache_state(batch_size=1)
        cache_last_channel, cache_last_time, cache_last_channel_len = cache_state
        
        print(f"Cache initialized:")
        print(f"  Last channel cache: {cache_last_channel.shape}")
        print(f"  Last time cache: {cache_last_time.shape}")
        print(f"  Last channel len: {cache_last_channel_len.shape}")
        
        # Streaming inference
        print("\n=== Streaming Inference ===")
        
        # Calculate chunk size in samples
        chunk_samples = int(16000 * chunk_size_ms / 1000)  # 80ms = 1280 samples
        total_chunks = (len(audio_float) + chunk_samples - 1) // chunk_samples
        
        full_transcript = []
        
        for i in range(total_chunks):
            # Extract chunk
            start_idx = i * chunk_samples
            end_idx = min(start_idx + chunk_samples, len(audio_float))
            
            audio_chunk = audio_float[start_idx:end_idx]
            
            # Pad last chunk if needed
            if len(audio_chunk) < chunk_samples:
                audio_chunk = np.pad(audio_chunk, (0, chunk_samples - len(audio_chunk)))
            
            # Convert to torch tensor and extract features using model's preprocessor
            audio_tensor = torch.from_numpy(audio_chunk).unsqueeze(0)  # Add batch dimension
            audio_length = torch.tensor([len(audio_chunk)], dtype=torch.long)
            
            # Use model's preprocessor to extract features (mel-spectrograms)
            with torch.no_grad():
                processed_signal, processed_signal_length = asr_model.preprocessor(
                    input_signal=audio_tensor, length=audio_length
                )
            
            # Use official NeMo streaming method
            timestamp_ms = int(start_idx * 1000 / 16000)
            
            print(f"Processing chunk {i+1}/{total_chunks} [{timestamp_ms}ms]")
            
            try:
                # Call the official streaming method
                result = asr_model.conformer_stream_step(
                    processed_signal=processed_signal,
                    processed_signal_length=processed_signal_length,
                    cache_last_channel=cache_last_channel,
                    cache_last_time=cache_last_time,
                    cache_last_channel_len=cache_last_channel_len,
                    keep_all_outputs=(i == total_chunks - 1)  # Keep all for last chunk
                )
                
                # Extract results - debug what we actually get
                print(f"  Result type: {type(result)}")
                if isinstance(result, tuple):
                    print(f"  Result length: {len(result)}")
                    for j, item in enumerate(result):
                        if hasattr(item, 'shape'):
                            print(f"    Item {j}: {type(item)} - {item.shape}")
                        else:
                            print(f"    Item {j}: {type(item)} - {str(item)[:100]}")
                
                # Try to extract transcription
                if isinstance(result, tuple) and len(result) >= 1:
                    transcripts = result[0]
                    
                    # Update cache if available - handle format correctly
                    if len(result) >= 4:
                        # From debug output: Item 2-4 are the cache tensors
                        new_cache_channel = result[2]
                        new_cache_time = result[3] 
                        new_cache_len = result[4]
                        
                        # Ensure cache tensors have correct format for next iteration
                        # The error indicates a "nested depth" issue - cache tensors may be wrapped
                        if isinstance(new_cache_channel, torch.Tensor):
                            cache_last_channel = new_cache_channel
                        if isinstance(new_cache_time, torch.Tensor):
                            cache_last_time = new_cache_time
                        if isinstance(new_cache_len, torch.Tensor):
                            cache_last_channel_len = new_cache_len
                    
                    # Handle different transcript formats
                    if transcripts is not None:
                        if isinstance(transcripts, list) and len(transcripts) > 0:
                            tokens = transcripts[0]
                            if hasattr(tokens, 'cpu'):  # It's a tensor
                                token_ids = tokens.cpu().numpy()
                                # Decode using model's tokenizer
                                text = asr_model.tokenizer.ids_to_text(token_ids)
                            else:
                                text = str(tokens)
                        elif hasattr(transcripts, 'text'):
                            text = str(transcripts.text)
                        else:
                            text = str(transcripts)
                        
                        if text and text.strip() and not text.startswith('tensor'):
                            print(f"  → '{text}'")
                            full_transcript.append(text)
                        else:
                            print(f"  → (empty/raw tokens)")
                    else:
                        print(f"  → (no transcripts)")
                else:
                    print(f"  → Unexpected result format: {type(result)}")
                    
            except Exception as e:
                print(f"  → Error: {e}")
                
        # Final result
        final_text = ' '.join(full_transcript).strip()
        print(f"\n=== Final Transcription ===")
        print(f"Text: '{final_text}'")
        
        # Save result
        output_file = "test_results/nemo_streaming_official.txt"
        os.makedirs("test_results", exist_ok=True)
        with open(output_file, 'w') as f:
            f.write(f"NeMo Streaming Result:\n")
            f.write(f"Model: {model_name}\n")
            f.write(f"Chunk size: {chunk_size_ms}ms\n")
            f.write(f"Configuration: {chunk_size_frames} encoder frames\n")
            f.write(f"Transcription: {final_text}\n")
        
        print(f"Results saved to: {output_file}")
        
        # Check for success
        if final_text and len(final_text) > 10:
            print("✅ SUCCESS: Got meaningful transcription output!")
            return True
        else:
            print("⚠️  WARNING: Transcription is empty or very short")
            return False
            
    except Exception as e:
        print(f"❌ Error: {e}")
        import traceback
        traceback.print_exc()
        return False

if __name__ == "__main__":
    success = test_nemo_streaming()
    if success:
        print("\n✅ NeMo streaming test completed successfully!")
    else:
        print("\n❌ NeMo streaming test failed!")
        sys.exit(1)