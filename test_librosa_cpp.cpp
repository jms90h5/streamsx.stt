#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include "impl/include/ProvenFeatureExtractor.hpp"

// Simple WAV reader (assumes 16-bit PCM)
std::vector<float> readWav(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error: Cannot open " << filename << std::endl;
        return {};
    }
    
    // Skip WAV header (44 bytes)
    file.seekg(44);
    
    std::vector<int16_t> samples;
    int16_t sample;
    while (file.read(reinterpret_cast<char*>(&sample), sizeof(sample))) {
        samples.push_back(sample);
    }
    
    // Convert to float and normalize
    std::vector<float> audio;
    for (int16_t s : samples) {
        audio.push_back(s / 32768.0f);
    }
    
    std::cout << "Read " << audio.size() << " samples from " << filename << std::endl;
    return audio;
}

void saveFeatures(const std::vector<float>& features, int n_mels, const std::string& filename) {
    std::ofstream file(filename);
    if (!file) {
        std::cerr << "Error: Cannot write to " << filename << std::endl;
        return;
    }
    
    int num_frames = features.size() / n_mels;
    
    file << std::fixed << std::setprecision(6);
    for (int frame = 0; frame < num_frames; frame++) {
        for (int mel = 0; mel < n_mels; mel++) {
            if (mel > 0) file << " ";
            file << features[frame * n_mels + mel];
        }
        file << "\n";
    }
    
    std::cout << "Saved " << num_frames << "x" << n_mels << " features to " << filename << std::endl;
}

void compareWithPython(const std::vector<float>& cpp_features, int n_mels) {
    // Try to load Python features for comparison
    std::ifstream nemo_file("nemo_features.npy", std::ios::binary);
    if (!nemo_file) {
        std::cout << "Warning: nemo_features.npy not found, skipping comparison" << std::endl;
        return;
    }
    nemo_file.close();
    
    std::ifstream librosa_file("librosa_mel_features.npy", std::ios::binary);  
    if (!librosa_file) {
        std::cout << "Warning: librosa_mel_features.npy not found, skipping comparison" << std::endl;
        return;
    }
    librosa_file.close();
    
    std::cout << "\nCreating Python comparison script..." << std::endl;
    
    std::ofstream script("compare_cpp_features.py");
    script << "#!/usr/bin/env python3\n";
    script << "import numpy as np\n";
    script << "\n";
    script << "# Load features\n";
    script << "cpp_features = np.loadtxt('cpp_features.txt')\n";
    script << "nemo_features = np.load('nemo_features.npy')[0]  # Remove batch dim\n";
    script << "librosa_features = np.load('librosa_mel_features.npy')\n";
    script << "\n";
    script << "print('Feature shapes:')\n";
    script << "print(f'  C++:     {cpp_features.shape}')\n";
    script << "print(f'  NeMo:    {nemo_features.shape}')\n";
    script << "print(f'  Librosa: {librosa_features.shape}')\n";
    script << "\n";
    script << "# Transpose C++ features to match Python (mels, frames)\n";
    script << "cpp_features = cpp_features.T\n";
    script << "\n";
    script << "print('\\nFeature statistics:')\n";
    script << "print(f'C++ range:     [{cpp_features.min():.4f}, {cpp_features.max():.4f}]')\n";
    script << "print(f'C++ mean/std:  {cpp_features.mean():.4f} / {cpp_features.std():.4f}')\n";
    script << "print(f'NeMo range:    [{nemo_features.min():.4f}, {nemo_features.max():.4f}]')\n";
    script << "print(f'NeMo mean/std: {nemo_features.mean():.4f} / {nemo_features.std():.4f}')\n";
    script << "print(f'Librosa range: [{librosa_features.min():.4f}, {librosa_features.max():.4f}]')\n";
    script << "print(f'Librosa mean/std: {librosa_features.mean():.4f} / {librosa_features.std():.4f}')\n";
    script << "\n";
    script << "# Calculate correlations\n";
    script << "min_frames = min(cpp_features.shape[1], nemo_features.shape[1], librosa_features.shape[1])\n";
    script << "cpp_trimmed = cpp_features[:, :min_frames]\n";
    script << "nemo_trimmed = nemo_features[:, :min_frames]\n";
    script << "librosa_trimmed = librosa_features[:, :min_frames]\n";
    script << "\n";
    script << "cpp_nemo_corr = np.corrcoef(cpp_trimmed.flatten(), nemo_trimmed.flatten())[0, 1]\n";
    script << "cpp_librosa_corr = np.corrcoef(cpp_trimmed.flatten(), librosa_trimmed.flatten())[0, 1]\n";
    script << "nemo_librosa_corr = np.corrcoef(nemo_trimmed.flatten(), librosa_trimmed.flatten())[0, 1]\n";
    script << "\n";
    script << "print('\\nCorrelations:')\n";
    script << "print(f'  C++ vs NeMo:    {cpp_nemo_corr:.6f}')\n";
    script << "print(f'  C++ vs Librosa: {cpp_librosa_corr:.6f}')\n";
    script << "print(f'  NeMo vs Librosa: {nemo_librosa_corr:.6f}')\n";
    script << "\n";
    script << "# First frame comparison\n";
    script << "print('\\nFirst frame comparison (mel bins 0-9):')\n";
    script << "print(f'C++:     {cpp_trimmed[:10, 0]}')\n";
    script << "print(f'NeMo:    {nemo_trimmed[:10, 0]}')\n";
    script << "print(f'Librosa: {librosa_trimmed[:10, 0]}')\n";
    
    script.close();
    
    std::cout << "Comparison script created. Run 'python compare_cpp_features.py' to see results." << std::endl;
}

int main() {
    std::cout << "=== Testing Librosa-Based C++ Feature Extractor ===" << std::endl;
    
    // Load audio
    auto audio_data = readWav("test_data/audio/librispeech-1995-1837-0001.wav");
    if (audio_data.empty()) {
        std::cerr << "Failed to load audio" << std::endl;
        return 1;
    }
    
    // Initialize feature extractor
    ProvenFeatureExtractor extractor;
    
    // Extract features with NeMo parameters
    const int sample_rate = 16000;
    const int n_mels = 80;
    const int frame_length = 400;  // 25ms
    const int frame_shift = 160;   // 10ms
    
    std::cout << "\nExtracting mel spectrogram features..." << std::endl;
    auto features = extractor.extractMelSpectrogram(
        audio_data, sample_rate, n_mels, frame_length, frame_shift);
    
    if (features.empty()) {
        std::cerr << "Feature extraction failed" << std::endl;
        return 1;
    }
    
    int num_frames = features.size() / n_mels;
    std::cout << "Extracted " << num_frames << " frames with " << n_mels << " mel bins each" << std::endl;
    
    // Display feature statistics
    float min_val = *std::min_element(features.begin(), features.end());
    float max_val = *std::max_element(features.begin(), features.end());
    
    double sum = 0.0;
    for (float f : features) sum += f;
    float mean = sum / features.size();
    
    double variance = 0.0;
    for (float f : features) variance += (f - mean) * (f - mean);
    float std_dev = std::sqrt(variance / features.size());
    
    std::cout << "\nC++ Feature Statistics:" << std::endl;
    std::cout << "  Range: [" << min_val << ", " << max_val << "]" << std::endl;
    std::cout << "  Mean: " << mean << ", std: " << std_dev << std::endl;
    
    // Show first frame
    std::cout << "\nFirst frame (mel bins 0-9):" << std::endl;
    std::cout << "  ";
    for (int i = 0; i < 10 && i < n_mels; i++) {
        std::cout << std::fixed << std::setprecision(6) << features[i] << " ";
    }
    std::cout << std::endl;
    
    // Save features for comparison
    saveFeatures(features, n_mels, "cpp_features.txt");
    
    // Create comparison with Python
    compareWithPython(features, n_mels);
    
    std::cout << "\n✓ C++ feature extraction completed successfully!" << std::endl;
    
    return 0;
}