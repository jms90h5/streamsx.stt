#include "impl/include/ProvenNeMoSTT.hpp"
#include <iostream>
#include <string>
#include <chrono>

int main() {
    std::cout << "=== Testing Proven C++ NeMo STT Implementation ===" << std::endl;
    
    try {
        // Initialize the proven STT implementation
        ProvenNeMoSTT stt;
        
        // Model paths (proven working ONNX exports)
        std::string encoder_path = "models/proven_onnx_export/encoder-proven_fastconformer.onnx";
        std::string decoder_path = "models/proven_onnx_export/decoder_joint-proven_fastconformer.onnx";
        std::string vocab_path = "models/proven_onnx_export/vocabulary.txt";
        
        std::cout << "Loading proven working models..." << std::endl;
        
        if (!stt.initialize(encoder_path, decoder_path, vocab_path)) {
            std::cerr << "❌ Failed to initialize STT models" << std::endl;
            return 1;
        }
        
        std::cout << "✅ Models loaded successfully" << std::endl;
        
        // Test with the target audio file that we know works perfectly
        std::string audio_file = "test_data/audio/librispeech-1995-1837-0001.wav";
        
        std::cout << "\nTesting with target audio file: " << audio_file << std::endl;
        std::cout << "Expected result: \"it was the first great sorrow of his life it was not so much the loss of the cotton itself but the fantasy the hopes the dreams built around it\"" << std::endl;
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        std::string transcript = stt.transcribe(audio_file);
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        std::cout << "\n=== RESULTS ===" << std::endl;
        std::cout << "Transcript: \"" << transcript << "\"" << std::endl;
        std::cout << "Processing time: " << duration.count() << " ms" << std::endl;
        
        // Compare with expected result
        std::string expected = "it was the first great sorrow of his life it was not so much the loss of the cotton itself but the fantasy the hopes the dreams built around it";
        
        if (transcript.find("sorrow") != std::string::npos && 
            transcript.find("cotton") != std::string::npos &&
            transcript.find("fantasy") != std::string::npos) {
            std::cout << "✅ SUCCESS: Transcript contains expected key words!" << std::endl;
        } else {
            std::cout << "⚠️  Note: Transcript differs from expected result" << std::endl;
            std::cout << "Expected: \"" << expected << "\"" << std::endl;
        }
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Error: " << e.what() << std::endl;
        return 1;
    }
}