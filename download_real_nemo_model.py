#!/usr/bin/env python3
"""
Download and export REAL NeMo ASR models
"""

import os
import sys

print("=== Real NeMo Model Download ===")
print("\nNeMo models require the NeMo toolkit to be installed.")
print("This is a large package with many dependencies including PyTorch.\n")

print("Option 1: Install NeMo and export a model")
print("-" * 40)
print("pip install nemo_toolkit[all]")
print("python -c \"")
print("import nemo.collections.asr as nemo_asr")
print("# Download and export FastConformer CTC model (smaller, faster)")
print("model = nemo_asr.models.EncDecCTCModelBPE.from_pretrained('nvidia/stt_en_fastconformer_ctc_large')")
print("model.export('fastconformer_ctc.onnx')")
print("\"")

print("\nOption 2: Download pre-exported ONNX models")
print("-" * 40)
print("Some community members have shared pre-exported ONNX models:")
print("- Hugging Face: https://huggingface.co/models?search=nemo+onnx")
print("- NVIDIA NGC: https://catalog.ngc.nvidia.com/models")

print("\nOption 3: Use Sherpa-ONNX pre-converted models")
print("-" * 40)
print("Sherpa-ONNX provides pre-converted streaming ASR models:")
print("wget https://github.com/k2-fsa/sherpa-onnx/releases/download/asr-models/sherpa-onnx-streaming-conformer-en-2023-08-18.tar.bz2")
print("tar xf sherpa-onnx-streaming-conformer-en-2023-08-18.tar.bz2")

print("\nWhich option would you like to use?")
print("1. Install NeMo toolkit (requires ~10GB disk space)")
print("2. Manual download from NGC/HuggingFace")
print("3. Use Sherpa-ONNX model (recommended for quick testing)")

choice = input("\nEnter choice (1-3) or 'q' to quit: ").strip()

if choice == '3':
    print("\nDownloading Sherpa-ONNX streaming model...")
    os.system("wget -q --show-progress https://github.com/k2-fsa/sherpa-onnx/releases/download/asr-models/sherpa-onnx-streaming-conformer-en-2023-08-18.tar.bz2")
    print("Extracting...")
    os.system("tar xf sherpa-onnx-streaming-conformer-en-2023-08-18.tar.bz2")
    print("\nModel downloaded. Files:")
    os.system("ls -la sherpa-onnx-streaming-conformer-en-2023-08-18/")
    
elif choice == '1':
    print("\nTo install NeMo toolkit:")
    print("pip install nemo_toolkit[all]")
    print("\nThis will take significant time and disk space.")
    
else:
    print("\nExiting without downloading.")
    sys.exit(0)