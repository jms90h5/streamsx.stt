#!/usr/bin/env python3
"""
Analyze differences between librosa and NeMo features to understand the correct approach
"""
import numpy as np
import librosa
import soundfile as sf

def analyze_feature_differences():
    print("=== Analyzing Librosa vs NeMo Feature Differences ===")
    
    # Load saved features
    librosa_features = np.load("librosa_mel_features.npy")  # Shape: [80, 874]
    nemo_features = np.load("nemo_features.npy")  # Shape: [1, 80, 874]
    
    # Remove batch dimension from NeMo
    nemo_features = nemo_features[0]  # Shape: [80, 874]
    
    print(f"Librosa shape: {librosa_features.shape}")
    print(f"NeMo shape: {nemo_features.shape}")
    
    # Calculate statistics
    print(f"\nLibrosa stats:")
    print(f"  Range: [{librosa_features.min():.4f}, {librosa_features.max():.4f}]")
    print(f"  Mean: {librosa_features.mean():.4f}, std: {librosa_features.std():.4f}")
    
    print(f"\nNeMo stats:")
    print(f"  Range: [{nemo_features.min():.4f}, {nemo_features.max():.4f}]")
    print(f"  Mean: {nemo_features.mean():.4f}, std: {nemo_features.std():.4f}")
    
    # Calculate correlation
    correlation = np.corrcoef(librosa_features.flatten(), nemo_features.flatten())[0, 1]
    print(f"\nCorrelation: {correlation:.6f}")
    
    # Calculate mean difference
    diff = librosa_features - nemo_features
    print(f"\nMean difference: {diff.mean():.4f}")
    print(f"Std difference: {diff.std():.4f}")
    
    # Check if there's a constant offset
    print(f"\nChecking for constant offset...")
    mean_diff_per_frame = np.mean(diff, axis=0)
    print(f"Mean difference per frame std: {mean_diff_per_frame.std():.4f}")
    
    # Check if there's a scaling factor
    print(f"\nChecking for scaling factor...")
    ratio = librosa_features / (nemo_features + 1e-10)
    ratio_mean = np.mean(ratio)
    ratio_std = np.std(ratio)
    print(f"Ratio mean: {ratio_mean:.4f}, std: {ratio_std:.4f}")
    
    # Try to find the transform that converts librosa to NeMo
    print(f"\nTrying to find transform...")
    
    # Simple offset
    offset_corrected = librosa_features + (nemo_features.mean() - librosa_features.mean())
    offset_corr = np.corrcoef(offset_corrected.flatten(), nemo_features.flatten())[0, 1]
    print(f"With mean offset correction: {offset_corr:.6f}")
    
    # Scale and offset
    scale = nemo_features.std() / librosa_features.std()
    offset = nemo_features.mean() - librosa_features.mean() * scale
    scaled_corrected = librosa_features * scale + offset
    scaled_corr = np.corrcoef(scaled_corrected.flatten(), nemo_features.flatten())[0, 1]
    print(f"With scale ({scale:.4f}) and offset ({offset:.4f}): {scaled_corr:.6f}")
    
    # Check frame-by-frame correlation
    print(f"\nFrame-by-frame analysis:")
    frame_correlations = []
    for i in range(min(10, librosa_features.shape[1])):
        frame_corr = np.corrcoef(librosa_features[:, i], nemo_features[:, i])[0, 1]
        frame_correlations.append(frame_corr)
        print(f"Frame {i}: correlation = {frame_corr:.6f}")
    
    print(f"Average frame correlation: {np.mean(frame_correlations):.6f}")

if __name__ == "__main__":
    analyze_feature_differences()