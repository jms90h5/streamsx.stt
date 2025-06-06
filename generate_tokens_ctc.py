#!/usr/bin/env python3
"""
Generate tokens file for the CTC-exported NeMo model
"""
import nemo.collections.asr as nemo_asr

def generate_tokens():
    print("=== Generating Tokens File for CTC Model ===")
    
    print("Loading NeMo FastConformer model...")
    model_path = "models/nemo_cache_aware_conformer/models--nvidia--stt_en_fastconformer_hybrid_large_streaming_multi/snapshots/ae98143333690bd7ced4bc8ec16769bcb8918374/stt_en_fastconformer_hybrid_large_streaming_multi.nemo"
    
    # Load the model
    m = nemo_asr.models.EncDecHybridRNNTCTCBPEModel.restore_from(model_path)
    
    # Inspect the model structure
    print("Model structure inspection:")
    print(f"- Has tokenizer: {hasattr(m, 'tokenizer')}")
    print(f"- Has decoder: {hasattr(m, 'decoder')}")
    if hasattr(m, 'tokenizer'):
        print(f"- Tokenizer type: {type(m.tokenizer)}")
        print(f"- Tokenizer vocab_size: {m.tokenizer.vocab_size}")
    
    # Generate tokens file using multiple approaches
    output_file = "models/fastconformer_ctc_export/tokens.txt"
    
    try:
        # Method 1: Direct tokenizer access
        print("Attempting Method 1: Direct tokenizer access...")
        tokens = []
        for i in range(m.tokenizer.vocab_size):
            token = m.tokenizer.ids_to_tokens([i])[0]
            tokens.append(token)
        
        with open(output_file, 'w') as f:
            for i, token in enumerate(tokens):
                f.write(f"{token} {i}\n")
            # Add blank token for CTC
            f.write(f"<blk> {len(tokens)}\n")
        
        print(f"✅ Successfully generated tokens file: {output_file}")
        print(f"Total tokens: {len(tokens)}")
        print(f"Sample tokens: {tokens[:10]}")
        print(f"Special tokens: {[t for t in tokens if t.startswith('<') or t.startswith('▁')][:10]}")
        
    except Exception as e1:
        print(f"Method 1 failed: {e1}")
        
        try:
            # Method 2: Use sentencepiece directly
            print("Attempting Method 2: SentencePiece direct access...")
            sp = m.tokenizer.tokenizer
            
            with open(output_file, 'w') as f:
                for i in range(sp.vocab_size()):
                    token = sp.id_to_piece(i)
                    f.write(f"{token} {i}\n")
                # Add blank token for CTC
                f.write(f"<blk> {sp.vocab_size()}\n")
            
            print(f"✅ Successfully generated tokens file: {output_file}")
            print(f"Total tokens: {sp.vocab_size()}")
            
        except Exception as e2:
            print(f"Method 2 failed: {e2}")
            
            try:
                # Method 3: Use model's vocabulary property if available
                print("Attempting Method 3: Model vocabulary property...")
                if hasattr(m, 'decoder') and hasattr(m.decoder, 'vocabulary'):
                    vocab_list = m.decoder.vocabulary
                elif hasattr(m, 'ctc_decoder') and hasattr(m.ctc_decoder, 'vocabulary'):
                    vocab_list = m.ctc_decoder.vocabulary
                else:
                    raise AttributeError("No vocabulary found")
                
                with open(output_file, 'w') as f:
                    for i, token in enumerate(vocab_list):
                        f.write(f"{token} {i}\n")
                    f.write(f"<blk> {len(vocab_list)}\n")
                
                print(f"✅ Successfully generated tokens file: {output_file}")
                print(f"Total tokens: {len(vocab_list)}")
                
            except Exception as e3:
                print(f"Method 3 failed: {e3}")
                print("❌ All methods failed to generate tokens file.")
                print("Please check the model structure manually.")

if __name__ == "__main__":
    generate_tokens()