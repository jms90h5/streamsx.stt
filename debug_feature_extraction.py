#!/usr/bin/env python3
"""
Debug feature extraction differences between NeMo and C++
"""
import numpy as np
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt

# Load features
nemo_features = np.load("nemo_features.npy")  # Shape: [1, 80, 874]
cpp_features = []
with open("cpp_features.txt", 'r') as f:
    for line in f:
        if line.startswith('#'):
            continue
        values = [float(x) for x in line.strip().split()]
        if values:
            cpp_features.append(values)

cpp_features = np.array(cpp_features).T[np.newaxis, :, :]  # [1, 80, time]

print(f"NeMo shape: {nemo_features.shape}")
print(f"C++ shape: {cpp_features.shape}")

# Trim to same length
min_time = min(nemo_features.shape[2], cpp_features.shape[2])
nemo_trimmed = nemo_features[:, :, :min_time]
cpp_trimmed = cpp_features[:, :, :min_time]

# Plot first few frames
fig, axes = plt.subplots(3, 1, figsize=(12, 10))

# Plot NeMo features
im1 = axes[0].imshow(nemo_trimmed[0, :, :100], aspect='auto', origin='lower')
axes[0].set_title('NeMo Features (first 100 frames)')
axes[0].set_ylabel('Mel bin')
plt.colorbar(im1, ax=axes[0])

# Plot C++ features  
im2 = axes[1].imshow(cpp_trimmed[0, :, :100], aspect='auto', origin='lower')
axes[1].set_title('C++ Features (first 100 frames)')
axes[1].set_ylabel('Mel bin')
plt.colorbar(im2, ax=axes[1])

# Plot difference
diff = np.abs(nemo_trimmed[0, :, :100] - cpp_trimmed[0, :, :100])
im3 = axes[2].imshow(diff, aspect='auto', origin='lower')
axes[2].set_title('Absolute Difference')
axes[2].set_ylabel('Mel bin')
axes[2].set_xlabel('Frame')
plt.colorbar(im3, ax=axes[2])

plt.tight_layout()
plt.savefig('feature_comparison.png', dpi=150)
print("Saved feature comparison to feature_comparison.png")

# Analyze specific differences
print("\n=== Feature Analysis ===")
print(f"NeMo range: [{nemo_features.min():.2f}, {nemo_features.max():.2f}]")
print(f"C++ range: [{cpp_features.min():.2f}, {cpp_features.max():.2f}]")

# Check if features are normalized differently
nemo_mean = nemo_features.mean()
cpp_mean = cpp_features.mean()
print(f"\nNeMo mean: {nemo_mean:.2f}")
print(f"C++ mean: {cpp_mean:.2f}")

# Check specific bins
print("\n=== First frame, first 10 mel bins ===")
print(f"NeMo: {nemo_trimmed[0, :10, 0]}")
print(f"C++:  {cpp_trimmed[0, :10, 0]}")
print(f"Diff: {nemo_trimmed[0, :10, 0] - cpp_trimmed[0, :10, 0]}")

# Check if there's a systematic offset
mean_diff = (nemo_trimmed - cpp_trimmed).mean()
print(f"\nMean difference: {mean_diff:.2f}")

# Check if features are scaled differently
scale_ratio = nemo_features.std() / cpp_features.std()
print(f"Std ratio (NeMo/C++): {scale_ratio:.2f}")

# Save detailed analysis
with open("feature_analysis.txt", "w") as f:
    f.write("Feature Extraction Analysis\n")
    f.write("===========================\n\n")
    f.write(f"NeMo shape: {nemo_features.shape}\n")
    f.write(f"C++ shape: {cpp_features.shape}\n")
    f.write(f"NeMo range: [{nemo_features.min():.4f}, {nemo_features.max():.4f}]\n")
    f.write(f"C++ range: [{cpp_features.min():.4f}, {cpp_features.max():.4f}]\n")
    f.write(f"Mean difference: {mean_diff:.4f}\n")
    f.write(f"Correlation: {np.corrcoef(nemo_trimmed.flatten(), cpp_trimmed.flatten())[0, 1]:.4f}\n")
    
print("\nSaved detailed analysis to feature_analysis.txt")