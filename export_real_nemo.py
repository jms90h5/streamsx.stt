#!/usr/bin/env python3
"""
Export the real NeMo model to ONNX
"""

import os
import sys

print("=== Exporting Real NeMo Model to ONNX ===")

try:
    import torch
    import nemo.collections.asr as nemo_asr
    
    # Path to the downloaded model
    model_path = "models/nemo_real/stt_en_conformer_ctc_small.nemo"
    output_dir = "models/nemo_fastconformer_streaming"
    os.makedirs(output_dir, exist_ok=True)
    
    print(f"Loading NeMo model from: {model_path}")
    
    # Load the model
    model = nemo_asr.models.EncDecCTCModel.restore_from(model_path)
    
    # Export to ONNX
    output_path = os.path.join(output_dir, "conformer_ctc.onnx")
    print(f"Exporting to ONNX: {output_path}")
    
    # NeMo's export method
    model.export(output_path)
    
    print("\n✓ Model successfully exported to ONNX!")
    
    # Copy vocabulary
    import shutil
    vocab_src = "models/nemo_real/vocab.txt"
    vocab_dst = os.path.join(output_dir, "tokenizer.txt")
    if os.path.exists(vocab_src):
        shutil.copy(vocab_src, vocab_dst)
        print(f"✓ Vocabulary copied to: {vocab_dst}")
    
    # Print model info
    print("\nModel Information:")
    print(f"  Model type: {type(model).__name__}")
    print(f"  Output path: {output_path}")
    print(f"  File size: {os.path.getsize(output_path) / 1024 / 1024:.1f} MB")
    
except Exception as e:
    print(f"\nError: {e}")
    import traceback
    traceback.print_exc()
    sys.exit(1)