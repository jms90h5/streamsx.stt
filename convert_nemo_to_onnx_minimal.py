#!/usr/bin/env python3
"""
Convert NeMo model to ONNX using minimal dependencies
This requires installing nemo_toolkit but we'll do it step by step
"""

import os
import sys

print("=== Converting Real NeMo Model to ONNX ===")
print("\nThis will install the minimum required packages to convert the model.")
print("Note: This requires significant disk space and time.\n")

# First, let's try to install just the core requirements
print("Step 1: Installing core dependencies...")
import subprocess

try:
    # Install core dependencies one by one to isolate issues
    deps = [
        "torch>=2.0.0",
        "torchaudio",
        "pytorch-lightning>=2.0.0",
        "hydra-core>=1.3.0",
        "omegaconf>=2.3.0",
        "transformers>=4.36.0",
        "sentencepiece",
        "numpy",
        "scikit-learn",
        "librosa>=0.10.0",
        "soundfile",
        "editdistance",
        "jiwer",
        "braceexpand",
        "webdataset>=0.1.48,<=0.1.62",
        "pyannote.core",
        "pyannote.metrics",
        "torchmetrics>=0.11.0",
        "tensorboard",
        "pandas",
        "lhotse>=1.20.0",
        "marshmallow",
        "packaging",
        "g2p_en",
        "pydub",
        "kaldiio",
        "kaldi-python-io",
        "pesq",
        "pystoi",
        "matplotlib",
        "inflect",
        "pypinyin",
        "distance",
        "rapidfuzz",
        "sacremoses",
        "sacrebleu",
        "ipywidgets",
        "braceexpand"
    ]
    
    for dep in deps:
        print(f"Installing {dep}...")
        try:
            subprocess.check_call([sys.executable, "-m", "pip", "install", "--user", "--quiet", dep])
        except Exception as e:
            print(f"  Warning: Could not install {dep}: {e}")
            # Continue anyway, some deps might be optional
    
    print("\nStep 2: Installing NeMo toolkit (this is large)...")
    # Try to install nemo without problematic dependencies
    subprocess.check_call([sys.executable, "-m", "pip", "install", "--user", "--no-deps", "nemo_toolkit"])
    
    print("\nStep 3: Converting model to ONNX...")
    
    # Now try to import and convert
    import torch
    import nemo.collections.asr as nemo_asr
    from omegaconf import OmegaConf
    
    # Load the model from checkpoint
    model_path = "models/nemo_real/stt_en_conformer_ctc_small.nemo"
    
    print(f"Loading model from {model_path}")
    model = nemo_asr.models.EncDecCTCModel.restore_from(model_path)
    
    # Export to ONNX
    output_path = "models/nemo_fastconformer_streaming/conformer_ctc.onnx"
    os.makedirs(os.path.dirname(output_path), exist_ok=True)
    
    print(f"Exporting to {output_path}")
    model.export(output_path)
    
    print("\n✓ Model successfully converted to ONNX!")
    print(f"  Output: {output_path}")
    
    # Copy vocabulary
    import shutil
    shutil.copy("models/nemo_real/vocab.txt", "models/nemo_fastconformer_streaming/tokenizer.txt")
    print("✓ Vocabulary file copied")
    
except Exception as e:
    print(f"\nError during conversion: {e}")
    print("\nAlternative: You need to install the full NeMo toolkit:")
    print("pip install nemo_toolkit[asr]")
    print("\nThen run this script again.")
    sys.exit(1)