#!/usr/bin/env python3
"""
Simple test of NeMo without any mock dependencies
"""
def test_nemo_import():
    """Test basic NeMo import"""
    try:
        print("Testing NeMo import...")
        import nemo.collections.asr as nemo_asr
        print("✓ NeMo ASR imported successfully")
        
        # Load the model
        model_path = "models/nemo_cache_aware_conformer/models--nvidia--stt_en_fastconformer_hybrid_large_streaming_multi/snapshots/ae98143333690bd7ced4bc8ec16769bcb8918374/stt_en_fastconformer_hybrid_large_streaming_multi.nemo"
        
        print(f"Loading model: {model_path}")
        model = nemo_asr.models.EncDecHybridRNNTCTCBPEModel.restore_from(model_path)
        model.eval()
        print("✓ Model loaded successfully")
        
        # Test transcription
        audio_file = "test_data/audio/11-ibm-culture-2min-16k.wav"
        print(f"Transcribing: {audio_file}")
        
        transcriptions = model.transcribe([audio_file])
        
        print(f"\n🎤 REAL NEMO TRANSCRIPTION:")
        print(f"📝 {transcriptions[0]}")
        
        # Save result
        with open("nemo_real_transcription_success.txt", 'w') as f:
            f.write(f"NeMo FastConformer Real Transcription\n")
            f.write(f"Model: stt_en_fastconformer_hybrid_large_streaming_multi\n")
            f.write(f"Audio: {audio_file}\n")
            f.write(f"{'='*60}\n\n")
            f.write(transcriptions[0] if isinstance(transcriptions[0], str) else transcriptions[0][0])
        
        print(f"💾 Saved to: nemo_real_transcription_success.txt")
        return True
        
    except Exception as e:
        print(f"✗ Error: {e}")
        import traceback
        traceback.print_exc()
        return False

if __name__ == "__main__":
    success = test_nemo_import()
    exit(0 if success else 1)