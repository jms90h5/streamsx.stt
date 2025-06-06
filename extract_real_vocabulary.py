#!/usr/bin/env python3
"""
Extract the actual vocabulary from the working NeMo model
"""
import nemo.collections.asr as nemo_asr

def extract_vocabulary():
    try:
        # Load the proven working model
        model_path = "models/nemo_cache_aware_conformer/models--nvidia--stt_en_fastconformer_hybrid_large_streaming_multi/snapshots/ae98143333690bd7ced4bc8ec16769bcb8918374/stt_en_fastconformer_hybrid_large_streaming_multi.nemo"
        
        print(f"Loading model to extract vocabulary...")
        model = nemo_asr.models.EncDecHybridRNNTCTCBPEModel.restore_from(model_path)
        
        # Get the tokenizer/vocabulary
        tokenizer = model.tokenizer
        vocab_size = tokenizer.vocab_size
        
        print(f"Vocabulary size: {vocab_size}")
        
        # Extract all tokens
        vocabulary = []
        for i in range(vocab_size):
            token = tokenizer.ids_to_tokens([i])[0] if hasattr(tokenizer, 'ids_to_tokens') else str(i)
            vocabulary.append(token)
        
        # Save the real vocabulary
        output_file = "models/proven_onnx_export/real_vocabulary.txt"
        with open(output_file, 'w', encoding='utf-8') as f:
            for i, token in enumerate(vocabulary):
                f.write(f"{i}\t{token}\n")
        
        print(f"✅ Saved real vocabulary to: {output_file}")
        print(f"First 10 tokens: {vocabulary[:10]}")
        print(f"Last 10 tokens: {vocabulary[-10:]}")
        
        # Also check what special tokens exist
        if hasattr(tokenizer, 'blank_id'):
            print(f"Blank token ID: {tokenizer.blank_id}")
        if hasattr(tokenizer, 'unk_id'):
            print(f"UNK token ID: {tokenizer.unk_id}")
        if hasattr(tokenizer, 'bos_id'):
            print(f"BOS token ID: {tokenizer.bos_id}")
        if hasattr(tokenizer, 'eos_id'):
            print(f"EOS token ID: {tokenizer.eos_id}")
            
        return True
        
    except Exception as e:
        print(f"Error extracting vocabulary: {e}")
        import traceback
        traceback.print_exc()
        return False

if __name__ == "__main__":
    success = extract_vocabulary()
    exit(0 if success else 1)