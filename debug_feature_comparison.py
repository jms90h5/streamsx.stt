#!/usr/bin/env python3
"""
Compare feature extraction between NeMo and our C++ implementation
"""
import numpy as np
import librosa
import nemo.collections.asr as nemo_asr
import torch

def extract_nemo_features():
    """Extract features exactly as NeMo does"""
    print("=== NeMo Feature Extraction ===")
    
    # Load the working model
    model_path = "models/nemo_cache_aware_conformer/models--nvidia--stt_en_fastconformer_hybrid_large_streaming_multi/snapshots/ae98143333690bd7ced4bc8ec16769bcb8918374/stt_en_fastconformer_hybrid_large_streaming_multi.nemo"
    model = nemo_asr.models.EncDecHybridRNNTCTCBPEModel.restore_from(model_path)
    model.eval()
    
    # Load the same audio file
    audio_file = "test_data/audio/librispeech-1995-1837-0001.wav"
    
    # Get NeMo's preprocessor
    preprocessor = model.preprocessor
    print(f"NeMo preprocessor config: {preprocessor._cfg}")
    
    # Load audio exactly as NeMo does
    audio, sr = librosa.load(audio_file, sr=16000)
    print(f"Audio shape: {audio.shape}, sample rate: {sr}")
    
    # Convert to tensor and add batch dimension
    audio_tensor = torch.tensor(audio).unsqueeze(0)  # [1, time]
    audio_length = torch.tensor([audio.shape[0]]).long()
    
    print(f"Audio tensor shape: {audio_tensor.shape}")
    print(f"Audio length: {audio_length}")
    
    # Extract features using NeMo's preprocessor
    with torch.no_grad():
        features, feature_length = preprocessor(input_signal=audio_tensor, length=audio_length)
    
    print(f"NeMo features shape: {features.shape}")
    print(f"NeMo feature length: {feature_length}")
    
    # Save features for comparison
    np.save("nemo_features.npy", features.cpu().numpy())
    np.save("nemo_feature_length.npy", feature_length.cpu().numpy())
    
    print("✅ Saved NeMo features to nemo_features.npy")
    
    # Print some statistics
    features_np = features.cpu().numpy()
    print(f"NeMo features min: {features_np.min():.6f}")
    print(f"NeMo features max: {features_np.max():.6f}")
    print(f"NeMo features mean: {features_np.mean():.6f}")
    print(f"NeMo features std: {features_np.std():.6f}")
    
    return features_np, feature_length.cpu().numpy()

def load_cpp_features():
    """Load the features our C++ implementation extracted"""
    print("\n=== C++ Feature Comparison ===")
    
    # We need to modify the C++ to save features
    print("Need to modify C++ to save extracted features for comparison")
    print("This will help identify the exact difference")
    
    return None

def compare_features(nemo_features, cpp_features=None):
    """Compare the feature extraction methods"""
    print(f"\n=== Feature Comparison ===")
    print(f"NeMo features shape: {nemo_features.shape}")
    
    if cpp_features is not None:
        print(f"C++ features shape: {cpp_features.shape}")
        
        # Compare statistics
        print(f"Shape match: {nemo_features.shape == cpp_features.shape}")
        if nemo_features.shape == cpp_features.shape:
            diff = np.abs(nemo_features - cpp_features)
            print(f"Max difference: {diff.max():.6f}")
            print(f"Mean difference: {diff.mean():.6f}")
            print(f"Correlation: {np.corrcoef(nemo_features.flatten(), cpp_features.flatten())[0,1]:.6f}")
    else:
        print("Need C++ features to compare")

if __name__ == "__main__":
    nemo_features, nemo_lengths = extract_nemo_features()
    cpp_features = load_cpp_features()
    compare_features(nemo_features, cpp_features)