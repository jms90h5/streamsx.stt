/**
 * Test program for NeMo FastConformer-Hybrid dual model with cache-aware streaming
 * 
 * This is THE critical test to verify the "sh sh sh" problem is fixed
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <cstring>

#include "impl/include/NeMoCacheAwareConformer.hpp"
#include "impl/include/ImprovedFbank.hpp"

using namespace onnx_stt;

// Load raw audio file (16kHz, 16-bit PCM)
std::vector<int16_t> loadAudioFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open audio file: " + filename);
    }
    
    file.seekg(0, std::ios::end);
    size_t file_size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    std::vector<int16_t> audio_data(file_size / sizeof(int16_t));
    file.read(reinterpret_cast<char*>(audio_data.data()), file_size);
    
    std::cout << "Loaded audio: " << filename << std::endl;
    std::cout << "  Samples: " << audio_data.size() << std::endl;
    std::cout << "  Duration: " << (audio_data.size() / 16000.0) << " seconds" << std::endl;
    
    return audio_data;
}

int main(int argc, char* argv[]) {
    try {
        std::cout << "=== NeMo Cache-Aware FastConformer-Hybrid Test ===" << std::endl;
        std::cout << "Testing solution for 'sh sh sh' repetition issue" << std::endl;
        
        // Parse simple command line
        std::string encoder_model = "models/nemo_fresh_export/encoder-fastconformer_cache_final.onnx";
        std::string decoder_model = "models/nemo_fresh_export/decoder_joint-fastconformer_cache_final.onnx";
        std::string audio_file = "test_data/audio/librispeech-1995-1837-0001.raw";
        std::string output_file = "test_results/nemo_cache_aware_test.txt";
        
        if (argc > 1 && std::string(argv[1]) == "--help") {
            std::cout << "Usage: " << argv[0] << " [audio_file] [output_file]" << std::endl;
            return 0;
        }
        if (argc > 1) audio_file = argv[1];
        if (argc > 2) output_file = argv[2];
        
        // Load audio
        auto audio_data = loadAudioFile(audio_file);
        
        // Configure NeMo model
        NeMoCacheAwareConformer::NeMoConfig config;
        config.encoder_model_path = encoder_model;
        config.decoder_model_path = decoder_model;
        config.num_threads = 4;
        config.batch_size = 1;
        config.feature_dim = 80;
        config.chunk_frames = 160;   // 160 frames to produce 20 output frames
        config.enable_caching = true;
        config.provider = "CPU";
        config.num_cache_layers = 17;  // FastConformer-Hybrid has 17 layers
        
        // Create model
        std::cout << "\nInitializing NeMo dual model..." << std::endl;
        NeMoCacheAwareConformer model(config);
        
        ModelInterface::ModelConfig model_config;
        model_config.encoder_path = encoder_model;
        model_config.decoder_path = decoder_model;
        model_config.sample_rate = 16000;
        model_config.chunk_frames = 160;
        model_config.feature_dim = 80;
        model_config.num_threads = 4;
        
        if (!model.initialize(model_config)) {
            throw std::runtime_error("Failed to initialize model");
        }
        
        // Create feature extractor
        std::cout << "\nInitializing feature extractor..." << std::endl;
        improved_fbank::FbankComputer::Options fbank_opts;
        fbank_opts.sample_rate = 16000;
        fbank_opts.num_mel_bins = 80;
        fbank_opts.frame_length_ms = 25;
        fbank_opts.frame_shift_ms = 10;
        improved_fbank::FbankComputer fbank(fbank_opts);
        
        // Process audio in chunks
        std::cout << "\n=== Processing Audio ===" << std::endl;
        
        // Use model's exported chunk size: 160 frames which produces 20 output frames
        // Model was exported with chunk_size=160, so use that
        int target_input_frames = 160;  // 160 / 8 = 20 output frames
        int frame_shift_samples = 160;  // 10ms at 16kHz  
        int chunk_samples = target_input_frames * frame_shift_samples;  // 160 * 160 = 25600 samples
        int total_chunks = (audio_data.size() + chunk_samples - 1) / chunk_samples;
        
        std::string full_transcript;
        std::ofstream output(output_file);
        
        for (int i = 0; i < total_chunks; ++i) {
            // Extract chunk
            int start = i * chunk_samples;
            int end = std::min(start + chunk_samples, (int)audio_data.size());
            
            std::vector<float> chunk_audio(end - start);
            for (int j = 0; j < end - start; ++j) {
                chunk_audio[j] = audio_data[start + j] / 32768.0f;
            }
            
            // Pad if needed
            if (chunk_audio.size() < chunk_samples) {
                chunk_audio.resize(chunk_samples, 0.0f);
            }
            
            // Extract features
            auto features = fbank.computeFeatures(chunk_audio);
            
            // THEORY 1: Add right context padding for model's expectation
            // Model expects right context of 13 encoder frames = 13 * 8 = 104 audio frames
            // This should make total attention window = 70 + current + 13 as expected
            int right_context_frames = 13;  // From attention context [70, 13]
            int right_padding_needed = right_context_frames;  // Already in encoder frame units
            
            // Pad features with zeros for right context
            while (features.size() < target_input_frames + right_padding_needed) {
                std::vector<float> zero_frame(80, 0.0f);
                features.push_back(zero_frame);
            }
            
            std::cout << "DEBUG: Added right context padding, total frames: " << features.size() << std::endl;
            
            // Process chunk
            uint64_t timestamp_ms = (start * 1000) / 16000;
            auto result = model.processChunk(features, timestamp_ms);
            
            // Display result
            std::cout << "[" << timestamp_ms << "ms] " << result.text 
                      << " (latency: " << result.latency_ms << "ms)" << std::endl;
            
            // Save to output
            output << "[" << timestamp_ms << "ms] " << result.text << std::endl;
            
            // Check for repeated "sh" tokens (the key test!)
            if (result.text.find("sh sh sh") != std::string::npos) {
                std::cout << "\n❌ FAILED: Still outputting repeated 'sh' tokens!" << std::endl;
                output << "FAILED: Repeated 'sh' tokens detected" << std::endl;
            }
            
            full_transcript += result.text + " ";
        }
        
        output << "\nFull transcript: " << full_transcript << std::endl;
        output.close();
        
        // Final result
        std::cout << "\n=== TRANSCRIPTION COMPLETE ===" << std::endl;
        std::cout << "Full transcript: " << full_transcript << std::endl;
        std::cout << "Results saved to: " << output_file << std::endl;
        
        // Check if we fixed the issue
        if (full_transcript.find("sh sh sh") == std::string::npos) {
            std::cout << "\n✅ SUCCESS: No repeated 'sh' tokens detected!" << std::endl;
            std::cout << "The cache-aware model appears to have fixed the issue." << std::endl;
        }
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}