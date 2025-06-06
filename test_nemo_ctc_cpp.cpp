#include "impl/include/NeMoCTCImpl.hpp"
#include <vector>
#include <iostream>
#include <fstream>
#include <chrono>

// Simple WAV file reader (assuming 16-bit PCM)
std::vector<float> readWAVFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Cannot open WAV file: " << filename << std::endl;
        return {};
    }
    
    // Skip WAV header (44 bytes for basic PCM)
    file.seekg(44);
    
    std::vector<float> samples;
    int16_t sample;
    
    while (file.read(reinterpret_cast<char*>(&sample), sizeof(sample))) {
        // Convert 16-bit to float [-1.0, 1.0]
        samples.push_back(static_cast<float>(sample) / 32768.0f);
    }
    
    return samples;
}

int main() {
    std::cout << "=== Testing NeMo CTC C++ Implementation ===" << std::endl;
    
    // Initialize model
    NeMoCTCImpl ctc_model;
    
    std::string model_path = "models/fastconformer_ctc_export/model.onnx";
    std::string tokens_path = "models/fastconformer_ctc_export/tokens.txt";
    
    std::cout << "Initializing CTC model..." << std::endl;
    if (!ctc_model.initialize(model_path, tokens_path)) {
        std::cerr << "❌ Failed to initialize CTC model" << std::endl;
        return 1;
    }
    
    std::cout << "\nModel Information:" << std::endl;
    std::cout << ctc_model.getModelInfo() << std::endl;
    
    // Test with audio file
    std::vector<std::string> test_files = {
        "test_data/audio/librispeech-1995-1837-0001.wav",
        "test_data/audio/11-ibm-culture-2min-16k.wav"
    };
    
    for (const auto& audio_file : test_files) {
        std::cout << "\n--- Testing with: " << audio_file << " ---" << std::endl;
        
        // Read audio file
        auto audio_samples = readWAVFile(audio_file);
        if (audio_samples.empty()) {
            std::cout << "⚠️ Could not read audio file: " << audio_file << std::endl;
            continue;
        }
        
        std::cout << "Audio loaded: " << audio_samples.size() << " samples, "
                  << audio_samples.size() / 16000.0 << " seconds" << std::endl;
        
        // Transcribe
        auto start_time = std::chrono::high_resolution_clock::now();
        std::string transcript = ctc_model.transcribe(audio_samples);
        auto end_time = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
            end_time - start_time).count();
        
        std::cout << "✅ C++ CTC Result: '" << transcript << "'" << std::endl;
        std::cout << "Processing time: " << duration << " ms" << std::endl;
    }
    
    std::cout << "\n=== C++ CTC Test Complete ===" << std::endl;
    return 0;
}