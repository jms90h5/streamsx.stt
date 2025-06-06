#!/usr/bin/env python3
"""
Compare C++ features with Python references
"""
import numpy as np

def compare_features():
    print("=== Comparing C++ Features with Python References ===")
    
    # Load features
    try:
        cpp_features = np.loadtxt('cpp_features.txt')
        print(f"C++ features shape: {cpp_features.shape}")
    except:
        print("Error: cpp_features.txt not found. Run C++ test first.")
        return
    
    try:
        nemo_features = np.load('nemo_features.npy')[0]  # Remove batch dim
        print(f"NeMo features shape: {nemo_features.shape}")
    except:
        print("Error: nemo_features.npy not found")
        return
        
    try:
        librosa_features = np.load('librosa_mel_features.npy')
        print(f"Librosa features shape: {librosa_features.shape}")
    except:
        print("Warning: librosa_mel_features.npy not found")
        librosa_features = None
    
    # Transpose C++ features to match Python (mels, frames)
    cpp_features = cpp_features.T
    print(f"C++ features transposed: {cpp_features.shape}")
    
    print('\nFeature statistics:')
    print(f'C++ range:     [{cpp_features.min():.4f}, {cpp_features.max():.4f}]')
    print(f'C++ mean/std:  {cpp_features.mean():.4f} / {cpp_features.std():.4f}')
    print(f'NeMo range:    [{nemo_features.min():.4f}, {nemo_features.max():.4f}]')
    print(f'NeMo mean/std: {nemo_features.mean():.4f} / {nemo_features.std():.4f}')
    
    if librosa_features is not None:
        print(f'Librosa range: [{librosa_features.min():.4f}, {librosa_features.max():.4f}]')
        print(f'Librosa mean/std: {librosa_features.mean():.4f} / {librosa_features.std():.4f}')
    
    # Calculate correlations
    min_frames = min(cpp_features.shape[1], nemo_features.shape[1])
    cpp_trimmed = cpp_features[:, :min_frames]
    nemo_trimmed = nemo_features[:, :min_frames]
    
    cpp_nemo_corr = np.corrcoef(cpp_trimmed.flatten(), nemo_trimmed.flatten())[0, 1]
    print(f'\nCorrelations:')
    print(f'  C++ vs NeMo:    {cpp_nemo_corr:.6f}')
    
    if librosa_features is not None:
        librosa_trimmed = librosa_features[:, :min_frames]
        cpp_librosa_corr = np.corrcoef(cpp_trimmed.flatten(), librosa_trimmed.flatten())[0, 1]
        nemo_librosa_corr = np.corrcoef(nemo_trimmed.flatten(), librosa_trimmed.flatten())[0, 1]
        print(f'  C++ vs Librosa: {cpp_librosa_corr:.6f}')
        print(f'  NeMo vs Librosa: {nemo_librosa_corr:.6f}')
    
    # First frame comparison
    print('\nFirst frame comparison (mel bins 0-9):')
    print(f'C++:     {cpp_trimmed[:10, 0]}')
    print(f'NeMo:    {nemo_trimmed[:10, 0]}')
    
    if librosa_features is not None:
        print(f'Librosa: {librosa_trimmed[:10, 0]}')
    
    # Analysis
    print(f'\n=== ANALYSIS ===')
    if cpp_nemo_corr < 0.1:
        print(f'❌ POOR CORRELATION ({cpp_nemo_corr:.6f}): C++ feature extraction differs significantly from NeMo')
        print('🔧 SOLUTION: Fix C++ feature extraction to match NeMo preprocessing')
        
        if librosa_features is not None and nemo_librosa_corr > 0.7:
            print(f'💡 HINT: Librosa correlates well with NeMo ({nemo_librosa_corr:.6f})')
            print('📋 ACTION: Update C++ to match librosa implementation')
    
    elif cpp_nemo_corr > 0.7:
        print(f'✅ GOOD CORRELATION ({cpp_nemo_corr:.6f}): C++ features match NeMo well')
        print('🔧 NEXT: Check model inference or token decoding')
    
    else:
        print(f'⚠️  MODERATE CORRELATION ({cpp_nemo_corr:.6f}): C++ features partially match NeMo')
        print('🔧 ACTION: Fine-tune C++ feature extraction')

if __name__ == "__main__":
    compare_features()