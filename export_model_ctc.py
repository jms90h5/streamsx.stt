#!/usr/bin/env python3
"""
Properly export NeMo FastConformer model in CTC mode
"""
import nemo.collections.asr as nemo_asr
import os

def export_ctc_model():
    print("=== Exporting NeMo FastConformer Model in CTC Mode ===")
    
    # Create output directory
    output_dir = "models/fastconformer_ctc_export"
    os.makedirs(output_dir, exist_ok=True)
    
    print("Loading NeMo FastConformer model...")
    model_path = "models/nemo_cache_aware_conformer/models--nvidia--stt_en_fastconformer_hybrid_large_streaming_multi/snapshots/ae98143333690bd7ced4bc8ec16769bcb8918374/stt_en_fastconformer_hybrid_large_streaming_multi.nemo"
    
    # Load the model
    m = nemo_asr.models.EncDecHybridRNNTCTCBPEModel.restore_from(model_path)
    
    print("Setting CTC export configuration...")
    # CRITICAL: Export as CTC only, not hybrid RNNT
    m.set_export_config({'decoder_type': 'ctc'})
    
    print("Exporting to ONNX...")
    output_path = f"{output_dir}/model.onnx"
    m.export(output_path)
    
    print("Generating tokens file...")
    # For hybrid models, vocabulary is accessed through the tokenizer
    try:
        # Try multiple ways to access vocabulary
        if hasattr(m, 'tokenizer') and hasattr(m.tokenizer, 'vocab'):
            vocab = m.tokenizer.vocab
            vocab_list = [vocab.id_to_piece(i) for i in range(vocab.vocab_size())]
        elif hasattr(m, 'decoder') and hasattr(m.decoder, 'vocabulary'):
            vocab_list = m.decoder.vocabulary
        elif hasattr(m, 'ctc_decoder') and hasattr(m.ctc_decoder, 'vocabulary'):
            vocab_list = m.ctc_decoder.vocabulary
        else:
            # Fallback: use the tokenizer directly
            vocab_list = [m.tokenizer.ids_to_tokens([i])[0] for i in range(m.tokenizer.vocab_size)]
        
        with open(f'{output_dir}/tokens.txt', 'w') as f:
            for i, s in enumerate(vocab_list):
                f.write(f"{s} {i}\n")
            f.write(f"<blk> {i+1}\n")
        
        print("✅ CTC export completed successfully")
        print(f"Model: {output_path}")
        print(f"Tokens: {output_dir}/tokens.txt")
        
        # Print some model info
        print(f"\nModel Information:")
        print(f"Vocabulary size: {len(vocab_list)}")
        print(f"Sample tokens: {vocab_list[:10]}")
        
    except Exception as e:
        print(f"⚠️ Warning: Could not generate tokens file: {e}")
        print("Model export completed but tokens file generation failed.")
        print("You may need to generate tokens manually or use a different approach.")

if __name__ == "__main__":
    export_ctc_model()