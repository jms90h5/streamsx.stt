#include <iostream>
#include <fstream>
#include <vector>
#include <onnxruntime_cxx_api.h>
#include "impl/include/NeMoCTCImpl.hpp"

// Load binary file
std::vector<float> loadBinaryFile(const std::string& filename, size_t expected_size = 0) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Cannot open file: " << filename << std::endl;
        return {};
    }
    
    // Get file size
    file.seekg(0, std::ios::end);
    size_t file_size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    size_t num_floats = file_size / sizeof(float);
    if (expected_size > 0 && num_floats != expected_size) {
        std::cerr << "File size mismatch. Expected " << expected_size 
                  << " floats, got " << num_floats << std::endl;
    }
    
    std::vector<float> data(num_floats);
    file.read(reinterpret_cast<char*>(data.data()), file_size);
    
    std::cout << "Loaded " << num_floats << " floats from " << filename << std::endl;
    return data;
}

int main() {
    std::cout << "=== Testing Model with Python Features ===" << std::endl;
    
    // Load Python-generated features
    // Shape should be (80, 874) = 69,920 floats
    auto python_features = loadBinaryFile("reference_data/log_mel_spectrogram.bin", 80 * 874);
    
    if (python_features.empty()) {
        std::cerr << "Failed to load reference features" << std::endl;
        return 1;
    }
    
    // Print first few values to verify
    std::cout << "First 5 Python features: ";
    for (int i = 0; i < 5; i++) {
        std::cout << python_features[i] << " ";
    }
    std::cout << std::endl;
    
    // Initialize model
    NeMoCTCImpl model;
    if (!model.initialize("models/fastconformer_ctc_export/model.onnx",
                          "models/fastconformer_ctc_export/tokens.txt")) {
        std::cerr << "Failed to initialize model" << std::endl;
        return 1;
    }
    
    // Now we need to run inference with these exact features
    // This requires exposing the internal inference method or modifying NeMoCTCImpl
    
    // For now, let's at least verify the features match what Python reported
    float min_val = python_features[0];
    float max_val = python_features[0];
    for (auto val : python_features) {
        if (val < min_val) min_val = val;
        if (val > max_val) max_val = val;
    }
    
    std::cout << "Python features range: [" << min_val << ", " << max_val << "]" << std::endl;
    std::cout << "Expected range: [-15.0317, 3.0605]" << std::endl;
    
    // Load raw audio for comparison
    auto raw_audio = loadBinaryFile("reference_data/audio_raw.bin", 139680);
    std::cout << "\nProcessing same audio with C++ feature extraction..." << std::endl;
    
    // Process with C++ to compare
    std::string result = model.transcribe(raw_audio);
    std::cout << "C++ transcription result: '" << result << "'" << std::endl;
    
    return 0;
}