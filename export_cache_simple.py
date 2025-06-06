#!/usr/bin/env python3
"""
Simple export script for NeMo FastConformer with cache support
Avoids TTS module conflicts
"""

import os
import sys

# Prevent TTS imports
os.environ['NEMO_DISABLE_TTS'] = '1'

def export_with_cache():
    print("=== NeMo Cache-Aware Export (Simple) ===\n")
    
    # Import only ASR modules
    try:
        import torch
        import nemo.collections.asr as nemo_asr
    except ImportError as e:
        print(f"Import error: {e}")
        return False
    
    # Model path
    model_path = "models/nemo_fastconformer_direct/stt_en_fastconformer_hybrid_large_streaming_multi.nemo"
    
    if not os.path.exists(model_path):
        # Try alternative path
        model_path = "models/nemo_real/stt_en_conformer_ctc_small.nemo"
        if not os.path.exists(model_path):
            print(f"Error: No model found")
            return False
    
    print(f"Using model: {model_path}")
    
    try:
        # Load model
        print("Loading model...")
        model = nemo_asr.models.ASRModel.restore_from(model_path)
        model.eval()
        print(f"✓ Model loaded: {type(model).__name__}")
        
        # Create output directory
        output_dir = "models/nemo_cache_fixed"
        os.makedirs(output_dir, exist_ok=True)
        
        # Check if model supports cache-aware export
        if hasattr(model, 'encoder') and hasattr(model.encoder, 'streaming_cfg'):
            print("✓ Model supports streaming/cache configuration")
        
        # Try to set cache configuration
        if hasattr(model, 'change_decoding_strategy'):
            print("Setting streaming configuration...")
            model.change_decoding_strategy(
                decoder_type='ctc',
                decoding_cfg={
                    'strategy': 'greedy',
                    'preserve_alignments': True,
                    'streaming': {
                        'enable': True,
                        'cache_aware': True,
                        'chunk_size': 160,
                        'left_context': 70,
                        'right_context': 0
                    }
                }
            )
        
        # Export with ONNX
        output_path = os.path.join(output_dir, "conformer_cache.onnx")
        print(f"Exporting to {output_path}...")
        
        # Method 1: Direct export with cache config
        try:
            export_config = {
                'export_format': 'onnx',
                'cache_support': True,
                'streaming': True,
                'chunk_size': 160,
                'context_size': [70, 0]  # left, right
            }
            
            if hasattr(model, 'set_export_config'):
                model.set_export_config(export_config)
            
            model.export(output_path, check_trace=False)
            print("✓ Export method 1 successful")
            
        except Exception as e1:
            print(f"Export method 1 failed: {e1}")
            
            # Method 2: Manual ONNX export
            try:
                print("\nTrying manual ONNX export...")
                
                # Prepare dummy inputs
                batch_size = 1
                time_steps = 160
                feat_dim = 80
                
                # Create inputs including cache tensors
                audio_signal = torch.randn(batch_size, time_steps, feat_dim)
                length = torch.tensor([time_steps])
                
                # Check model's expected inputs
                if hasattr(model, 'forward'):
                    import inspect
                    sig = inspect.signature(model.forward)
                    print(f"Model forward signature: {list(sig.parameters.keys())}")
                
                # Export encoder only
                encoder = model.encoder
                
                # For cache-aware models, we need cache tensors
                if hasattr(encoder, 'streaming_cfg'):
                    # Cache tensors based on model architecture
                    num_layers = getattr(encoder, 'num_layers', 17)
                    hidden_size = getattr(encoder, 'd_model', 512)
                    
                    cache_last_channel = torch.zeros(batch_size, num_layers, hidden_size)
                    cache_last_time = torch.zeros(batch_size, num_layers, hidden_size)
                    
                    inputs = (audio_signal, length, cache_last_channel, cache_last_time)
                    input_names = ['audio_signal', 'length', 'cache_last_channel', 'cache_last_time']
                    output_names = ['encoded', 'encoded_len', 'cache_last_channel_out', 'cache_last_time_out']
                else:
                    inputs = (audio_signal, length)
                    input_names = ['audio_signal', 'length'] 
                    output_names = ['encoded', 'encoded_len']
                
                torch.onnx.export(
                    encoder,
                    inputs,
                    output_path,
                    input_names=input_names,
                    output_names=output_names,
                    dynamic_axes={
                        'audio_signal': {0: 'batch', 1: 'time'},
                        'length': {0: 'batch'},
                        'encoded': {0: 'batch', 1: 'time'}
                    },
                    opset_version=14,
                    do_constant_folding=True
                )
                print("✓ Manual export successful")
                
            except Exception as e2:
                print(f"Manual export also failed: {e2}")
                return False
        
        # Verify export
        print("\nVerifying exported model...")
        import onnx
        onnx_model = onnx.load(output_path)
        
        print(f"Inputs ({len(onnx_model.graph.input)}):")
        for inp in onnx_model.graph.input:
            print(f"  - {inp.name}")
            
        print(f"\nOutputs ({len(onnx_model.graph.output)}):")  
        for out in onnx_model.graph.output:
            print(f"  - {out.name}")
        
        # Check for cache tensors
        has_cache = any('cache' in inp.name for inp in onnx_model.graph.input)
        print(f"\nCache support: {'✓ Yes' if has_cache else '✗ No'}")
        
        # Save vocabulary
        vocab_path = os.path.join(output_dir, "tokenizer.txt")
        if hasattr(model, 'tokenizer') and hasattr(model.tokenizer, 'vocab'):
            with open(vocab_path, 'w') as f:
                for token in model.tokenizer.vocab:
                    f.write(f"{token}\n")
            print(f"✓ Vocabulary saved to {vocab_path}")
        
        return True
        
    except Exception as e:
        print(f"Error: {e}")
        import traceback
        traceback.print_exc()
        return False

if __name__ == "__main__":
    success = export_with_cache()
    if success:
        print("\n✅ Export complete!")
    else:
        print("\n❌ Export failed")
        print("\nAlternative: The model might already be non-cache-aware.")
        print("Consider using the wav2vec2 model which is proven to work.")
        sys.exit(1)