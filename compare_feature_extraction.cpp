#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include "impl/include/ProvenFeatureExtractor.hpp"

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

void compareArrays(const std::vector<float>& cpp_data, const std::vector<float>& ref_data, 
                   const std::string& name, int show_count = 10) {
    std::cout << "\n=== Comparing " << name << " ===" << std::endl;
    
    if (cpp_data.size() != ref_data.size()) {
        std::cout << "❌ Size mismatch! C++: " << cpp_data.size() 
                  << ", Python: " << ref_data.size() << std::endl;
        return;
    }
    
    // Find min/max for both
    float cpp_min = cpp_data[0], cpp_max = cpp_data[0];
    float ref_min = ref_data[0], ref_max = ref_data[0];
    
    float max_diff = 0;
    float total_diff = 0;
    int diff_count = 0;
    
    for (size_t i = 0; i < cpp_data.size(); i++) {
        if (cpp_data[i] < cpp_min) cpp_min = cpp_data[i];
        if (cpp_data[i] > cpp_max) cpp_max = cpp_data[i];
        if (ref_data[i] < ref_min) ref_min = ref_data[i];
        if (ref_data[i] > ref_max) ref_max = ref_data[i];
        
        float diff = std::abs(cpp_data[i] - ref_data[i]);
        if (diff > max_diff) max_diff = diff;
        total_diff += diff;
        if (diff > 0.01) diff_count++;
    }
    
    float avg_diff = total_diff / cpp_data.size();
    
    std::cout << "C++ range: [" << cpp_min << ", " << cpp_max << "]" << std::endl;
    std::cout << "Python range: [" << ref_min << ", " << ref_max << "]" << std::endl;
    std::cout << "Max diff: " << max_diff << std::endl;
    std::cout << "Avg diff: " << avg_diff << std::endl;
    std::cout << "Elements with diff > 0.01: " << diff_count << " (" 
              << (100.0 * diff_count / cpp_data.size()) << "%)" << std::endl;
    
    // Show first few values
    std::cout << "\nFirst " << show_count << " values comparison:" << std::endl;
    for (int i = 0; i < show_count && i < cpp_data.size(); i++) {
        std::cout << "  [" << i << "] C++: " << cpp_data[i] 
                  << ", Python: " << ref_data[i] 
                  << ", diff: " << (cpp_data[i] - ref_data[i]) << std::endl;
    }
}

int main() {
    std::cout << "=== Feature Extraction Comparison ===" << std::endl;
    
    try {
        // Load reference audio
        auto audio = loadBinaryFile("reference_data/audio_raw.bin");
        std::cout << "Loaded " << audio.size() << " audio samples" << std::endl;
        
        // Extract features with C++
        ProvenFeatureExtractor extractor;
        // Parameters from Python: n_fft=512, hop_length=160, win_length=400, n_mels=80
        auto cpp_mel = extractor.extractMelSpectrogram(audio, 16000, 80, 512, 160);
        
        // Convert to log scale like Python does
        std::vector<float> cpp_features;
        for (float val : cpp_mel) {
            cpp_features.push_back(std::log(val + 1e-10f));
        }
        
        std::cout << "\nC++ extracted " << cpp_features.size() << " features" << std::endl;
        
        // Load Python references
        auto ref_log_mel = loadBinaryFile("reference_data/log_mel_spectrogram.bin");
        
        // Compare final log mel features
        compareArrays(cpp_features, ref_log_mel, "Log Mel Spectrogram");
        
        // Also load and compare intermediate steps if available
        try {
            auto ref_mel = loadBinaryFile("reference_data/mel_spectrogram.bin");
            // We'd need to expose intermediate values from C++ to compare
            std::cout << "\nPython mel spectrogram has " << ref_mel.size() << " values" << std::endl;
        } catch (...) {
            std::cout << "Could not load intermediate mel spectrogram" << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}