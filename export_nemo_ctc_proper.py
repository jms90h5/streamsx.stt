#!/usr/bin/env python3
"""
Export NeMo FastConformer Hybrid model in CTC mode for simpler ONNX inference
Based on official documentation and working examples
"""

import os
import sys

def export_nemo_ctc():
    """Export model in CTC mode for ONNX inference"""
    
    print("=== Export NeMo FastConformer in CTC Mode ===\n")
    
    try:
        import nemo.collections.asr as nemo_asr
        import torch
        
        # Model to use
        model_name = "nvidia/stt_en_fastconformer_hybrid_large_streaming_multi"
        
        print(f"Loading model: {model_name}")
        print("This may take a few minutes...")
        
        # Load model
        asr_model = nemo_asr.models.EncDecHybridRNNTCTCBPEModel.from_pretrained(model_name)
        
        print("✓ Model loaded successfully")
        print(f"Model type: {type(asr_model).__name__}")
        
        # Set to eval mode
        asr_model.eval()
        
        # Create output directory
        output_dir = "models/nemo_ctc_export"
        os.makedirs(output_dir, exist_ok=True)
        
        print("\n=== Configuring for CTC Export ===")
        
        # CRITICAL: Set decoder_type to CTC for simpler ONNX export
        print("Setting decoder_type to CTC...")
        asr_model.set_export_config({'decoder_type': 'ctc'})
        
        # Change decoding strategy to CTC (this is key!)
        print("Changing decoding strategy to CTC...")
        asr_model.change_decoding_strategy(decoder_type='ctc')
        
        print("✓ Model configured for CTC export")
        
        # Export to ONNX
        onnx_path = os.path.join(output_dir, "fastconformer_ctc.onnx")
        print(f"\nExporting to: {onnx_path}")
        
        try:
            # Export with CTC configuration
            asr_model.export(onnx_path, check_trace=False)
            print("✅ CTC Export successful!")
            
            # Generate tokens file for CTC decoding
            tokens_path = os.path.join(output_dir, "tokens.txt")
            print(f"Generating tokens file: {tokens_path}")
            
            with open(tokens_path, 'w', encoding='utf-8') as f:
                # Extract vocabulary from the model
                if hasattr(asr_model, 'decoder') and hasattr(asr_model.decoder, 'vocabulary'):
                    vocab = asr_model.decoder.vocabulary
                elif hasattr(asr_model, 'joint') and hasattr(asr_model.joint, 'vocabulary'):
                    vocab = asr_model.joint.vocabulary
                else:
                    # Try to get from tokenizer
                    vocab = list(asr_model.tokenizer.vocab.keys())
                
                print(f"Vocabulary size: {len(vocab)}")
                
                for i, token in enumerate(vocab):
                    f.write(f"{token} {i}\n")
                
                # Add blank token for CTC
                f.write(f"<blk> {len(vocab)}\n")
            
            print("✅ Tokens file generated successfully!")
            
            print(f"\n=== Export Complete ===")
            print(f"ONNX Model: {onnx_path}")
            print(f"Tokens: {tokens_path}")
            print(f"Output directory: {output_dir}")
            
            # Print model info
            try:
                import onnx
                model = onnx.load(onnx_path)
                print(f"\nModel inputs: {len(model.graph.input)}")
                for i, inp in enumerate(model.graph.input):
                    print(f"  Input {i}: {inp.name} - {[d.dim_value for d in inp.type.tensor_type.shape.dim]}")
                
                print(f"Model outputs: {len(model.graph.output)}")
                for i, out in enumerate(model.graph.output):
                    print(f"  Output {i}: {out.name} - {[d.dim_value for d in out.type.tensor_type.shape.dim]}")
                    
            except ImportError:
                print("(Install onnx package to see model details)")
            
        except Exception as e:
            print(f"❌ Export failed: {e}")
            return False
            
    except Exception as e:
        print(f"❌ Error: {e}")
        return False
    
    return True

if __name__ == "__main__":
    success = export_nemo_ctc()
    if success:
        print("\n✅ Export completed successfully!")
        print("You can now use the CTC model for inference with standard ONNX runtime.")
    else:
        print("\n❌ Export failed!")
        sys.exit(1)