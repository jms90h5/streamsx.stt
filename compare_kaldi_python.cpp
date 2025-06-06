#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include "impl/include/KaldiFbankFeatureExtractor.hpp"

// Load binary file
std::vector<float> loadBinaryFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename);
    }
    
    file.seekg(0, std::ios::end);
    size_t file_size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    size_t num_floats = file_size / sizeof(float);
    std::vector<float> data(num_floats);
    file.read(reinterpret_cast<char*>(data.data()), file_size);
    
    return data;
}

int main() {
    std::cout << "=== Kaldi vs Python Feature Comparison ===" << std::endl;
    
    try {
        // Load audio
        auto audio = loadBinaryFile("reference_data/audio_raw.bin");
        std::cout << "Audio samples: " << audio.size() << std::endl;
        
        // Extract with Kaldi
        KaldiFbankFeatureExtractor extractor;
        auto kaldi_features = extractor.extractMelSpectrogram(audio);
        
        // Load Python features
        auto python_features = loadBinaryFile("reference_data/log_mel_spectrogram.bin");
        
        std::cout << "\nComparison:" << std::endl;
        std::cout << "Kaldi features: " << kaldi_features.size() << " values" << std::endl;
        std::cout << "Python features: " << python_features.size() << " values" << std::endl;
        
        // Determine dimensions
        int kaldi_frames = kaldi_features.size() / 80;
        int python_frames = python_features.size() / 80;
        
        std::cout << "Kaldi: " << kaldi_frames << " frames x 80 features" << std::endl;
        std::cout << "Python: " << python_frames << " frames x 80 features" << std::endl;
        
        // Check data layout: Kaldi might be [time, features], Python is [features, time]
        // Convert Kaldi to [features, time] format if needed
        std::vector<float> kaldi_reordered(kaldi_features.size());
        
        // Kaldi format: [time, features] -> Need [features, time]
        for (int t = 0; t < kaldi_frames; t++) {
            for (int f = 0; f < 80; f++) {
                int kaldi_idx = t * 80 + f;           // [time, features]
                int python_idx = f * kaldi_frames + t; // [features, time]
                kaldi_reordered[python_idx] = kaldi_features[kaldi_idx];
            }
        }
        
        // Compare first few values
        std::cout << "\nFirst 10 values comparison:" << std::endl;
        std::cout << "Python: ";
        for (int i = 0; i < 10; i++) {
            std::cout << python_features[i] << " ";
        }
        std::cout << std::endl;
        
        std::cout << "Kaldi (reordered): ";
        for (int i = 0; i < 10; i++) {
            std::cout << kaldi_reordered[i] << " ";
        }
        std::cout << std::endl;
        
        // Calculate differences for overlapping region
        int min_size = std::min(kaldi_reordered.size(), python_features.size());
        float max_diff = 0;
        float total_diff = 0;
        int significant_diffs = 0;
        
        for (int i = 0; i < min_size; i++) {
            float diff = std::abs(kaldi_reordered[i] - python_features[i]);
            max_diff = std::max(max_diff, diff);
            total_diff += diff;
            if (diff > 0.1) significant_diffs++;
        }
        
        std::cout << "\nDifferences:" << std::endl;
        std::cout << "Max difference: " << max_diff << std::endl;
        std::cout << "Average difference: " << (total_diff / min_size) << std::endl;
        std::cout << "Significant diffs (>0.1): " << significant_diffs << "/" << min_size << std::endl;
        
        if (max_diff < 0.1) {
            std::cout << "✅ Features match well!" << std::endl;
        } else {
            std::cout << "❌ Features have significant differences" << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}