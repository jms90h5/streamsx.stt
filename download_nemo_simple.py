#!/usr/bin/env python3
"""
Simple download of NVIDIA NeMo FastConformer model using Hugging Face Hub
"""
import os
import sys
import subprocess

def install_huggingface_hub():
    """Install Hugging Face Hub"""
    try:
        subprocess.run([sys.executable, "-m", "pip", "install", "huggingface_hub"], check=True)
        return True
    except subprocess.CalledProcessError:
        return False

def download_model():
    """Download the model using Hugging Face Hub"""
    try:
        from huggingface_hub import hf_hub_download
        
        model_name = "nvidia/stt_en_fastconformer_hybrid_large_streaming_multi"
        filename = "stt_en_fastconformer_hybrid_large_streaming_multi.nemo"
        
        print(f"Downloading {filename} from {model_name}...")
        
        # Create directory
        model_dir = "models/nemo_cache_aware_conformer"
        os.makedirs(model_dir, exist_ok=True)
        
        # Download the .nemo file
        file_path = hf_hub_download(
            repo_id=model_name,
            filename=filename,
            cache_dir=model_dir
        )
        
        print(f"✓ Model downloaded to {file_path}")
        return file_path
        
    except Exception as e:
        print(f"✗ Download failed: {e}")
        return None

def main():
    print("=== Simple NeMo FastConformer Download ===")
    
    # Install huggingface_hub if needed
    try:
        import huggingface_hub
    except ImportError:
        print("Installing Hugging Face Hub...")
        if not install_huggingface_hub():
            sys.exit(1)
    
    # Download model
    model_path = download_model()
    if model_path:
        print(f"\n✓ Download complete: {model_path}")
        print("\nNext: Export to ONNX using NeMo toolkit")
    else:
        sys.exit(1)

if __name__ == "__main__":
    main()