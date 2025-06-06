#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath>
#include "impl/include/KaldiFbankFeatureExtractor.hpp"

// Load reference Python features
std::vector<float> loadReferenceFeatures(const std::string& filename) {
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

// Load test audio (first 8192 samples like in BasicNeMoDemo)
std::vector<float> loadTestAudio() {
    // For now, create dummy audio that matches expected length
    // Real implementation would load from WAV file
    int sample_rate = 16000;
    float duration_sec = 0.512; // 8192 / 16000
    int num_samples = sample_rate * duration_sec;
    
    std::vector<float> audio(num_samples);
    for (int i = 0; i < num_samples; i++) {
        audio[i] = 0.1f * sin(2.0f * M_PI * 440.0f * i / sample_rate); // 440Hz sine wave
    }
    
    return audio;
}

void printStats(const std::vector<float>& features, const std::string& name) {
    if (features.empty()) {
        std::cout << name << ": EMPTY" << std::endl;
        return;
    }
    
    float min_val = *std::min_element(features.begin(), features.end());
    float max_val = *std::max_element(features.begin(), features.end());
    
    float sum = 0.0f;
    for (float f : features) sum += f;
    float mean = sum / features.size();
    
    std::cout << name << ":" << std::endl;
    std::cout << "  Size: " << features.size() << " values" << std::endl;
    std::cout << "  Range: [" << min_val << ", " << max_val << "]" << std::endl;
    std::cout << "  Mean: " << mean << std::endl;
    std::cout << "  First 5: ";
    for (int i = 0; i < 5 && i < features.size(); i++) {
        std::cout << features[i] << " ";
    }
    std::cout << std::endl << std::endl;
}

int main() {
    std::cout << "=== Feature Extraction Comparison Debug ===" << std::endl;
    
    try {
        // Load reference Python features (80 mels × 874 frames = 69920 values)
        std::cout << "Loading reference Python features..." << std::endl;
        auto python_features = loadReferenceFeatures("reference_data/log_mel_spectrogram.bin");
        printStats(python_features, "Python Features (Reference)");
        
        // Extract features using C++ Kaldi implementation
        std::cout << "Extracting features with C++ Kaldi..." << std::endl;
        KaldiFbankFeatureExtractor extractor;
        auto test_audio = loadTestAudio();
        auto cpp_features = extractor.extractMelSpectrogram(test_audio);
        printStats(cpp_features, "C++ Kaldi Features");
        
        // Compare formats
        std::cout << "Expected format: 80 mels × 874 frames = 69920 values" << std::endl;
        std::cout << "Python features: " << python_features.size() << " values" << std::endl;
        std::cout << "C++ features: " << cpp_features.size() << " values" << std::endl;
        
        // Calculate frames from C++ output
        if (!cpp_features.empty()) {
            int cpp_frames = cpp_features.size() / 80;
            std::cout << "C++ calculated frames: " << cpp_frames << std::endl;
        }
        
        // Check if dimensions make sense for test audio
        int test_audio_samples = test_audio.size();
        int expected_frames = (test_audio_samples - 400) / 160 + 1; // Using win_length=400, hop=160
        std::cout << "Test audio samples: " << test_audio_samples << std::endl;
        std::cout << "Expected frames for test audio: " << expected_frames << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}