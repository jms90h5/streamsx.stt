#!/usr/bin/env python3
"""
Detailed comparison of NeMo vs C++ features
"""
import numpy as np

def compare_features():
    print("=== Detailed Feature Comparison ===")
    
    # Load NeMo features
    nemo_features = np.load("nemo_features.npy")  # Shape: [1, 80, 874]
    print(f"NeMo features shape: {nemo_features.shape}")
    
    # Load C++ features
    try:
        cpp_features = []
        with open("cpp_features.txt", 'r') as f:
            lines = f.readlines()
            for line in lines:
                if line.startswith('#'):
                    continue
                values = [float(x) for x in line.strip().split()]
                if values:
                    cpp_features.append(values)
        
        cpp_features = np.array(cpp_features)  # Shape: [time, 80]
        print(f"C++ features shape: {cpp_features.shape}")
        
        # Transpose to match NeMo format [1, 80, time]
        cpp_features_transposed = cpp_features.T[np.newaxis, :, :]  # [1, 80, time]
        print(f"C++ features transposed shape: {cpp_features_transposed.shape}")
        
        # Compare shapes
        print(f"\nShape comparison:")
        print(f"NeMo: {nemo_features.shape}")
        print(f"C++:  {cpp_features_transposed.shape}")
        
        # Trim to same time length for comparison
        min_time = min(nemo_features.shape[2], cpp_features_transposed.shape[2])
        nemo_trimmed = nemo_features[:, :, :min_time]
        cpp_trimmed = cpp_features_transposed[:, :, :min_time]
        
        print(f"Trimmed to common time length: {min_time}")
        
        # Compare statistics
        print(f"\nStatistical comparison:")
        print(f"NeMo - min: {nemo_trimmed.min():.6f}, max: {nemo_trimmed.max():.6f}")
        print(f"NeMo - mean: {nemo_trimmed.mean():.6f}, std: {nemo_trimmed.std():.6f}")
        print(f"C++  - min: {cpp_trimmed.min():.6f}, max: {cpp_trimmed.max():.6f}")
        print(f"C++  - mean: {cpp_trimmed.mean():.6f}, std: {cpp_trimmed.std():.6f}")
        
        # Check correlation
        nemo_flat = nemo_trimmed.flatten()
        cpp_flat = cpp_trimmed.flatten()
        correlation = np.corrcoef(nemo_flat, cpp_flat)[0, 1]
        print(f"Correlation: {correlation:.6f}")
        
        # Check differences
        diff = np.abs(nemo_trimmed - cpp_trimmed)
        print(f"Max absolute difference: {diff.max():.6f}")
        print(f"Mean absolute difference: {diff.mean():.6f}")
        
        # Sample comparison (first few frames)
        print(f"\nFirst frame comparison (mel bin 0-9):")
        print(f"NeMo: {nemo_trimmed[0, :10, 0]}")
        print(f"C++:  {cpp_trimmed[0, :10, 0]}")
        
        # Save difference for analysis
        np.save("feature_difference.npy", diff)
        print(f"Saved feature difference to feature_difference.npy")
        
    except Exception as e:
        print(f"Error loading C++ features: {e}")

if __name__ == "__main__":
    compare_features()