#ifndef PROVEN_NEMO_STT_HPP
#define PROVEN_NEMO_STT_HPP

#include <string>
#include <vector>
#include <memory>
#include <onnxruntime_cxx_api.h>
#include "ProvenFeatureExtractor.hpp"

/**
 * Proven NeMo Speech-to-Text implementation using validated ONNX models
 * Based on the working checkpoint 3 solution that produced perfect transcriptions
 */
class ProvenNeMoSTT {
public:
    ProvenNeMoSTT();
    ~ProvenNeMoSTT();

    // Initialize with the proven working ONNX models
    bool initialize(const std::string& encoder_path, 
                   const std::string& decoder_path,
                   const std::string& vocab_path);

    // Transcribe audio file using proven approach
    std::string transcribe(const std::string& audio_file_path);
    
    // Transcribe audio data using proven approach
    std::string transcribe(const std::vector<float>& audio_data, int sample_rate);

    // Check if models are loaded
    bool isInitialized() const { return initialized_; }

private:
    // Core inference methods
    std::vector<float> extractFeatures(const std::vector<float>& audio_data, int sample_rate);
    std::vector<float> runEncoder(const std::vector<float>& features);
    std::string runDecoder(const std::vector<float>& encoder_output);
    
    // Utility methods
    std::vector<float> loadAudioFile(const std::string& file_path, int target_sample_rate = 16000);
    std::string decodeTokens(const std::vector<int64_t>& tokens);
    
    // ONNX Runtime components
    std::unique_ptr<Ort::Env> ort_env_;
    std::unique_ptr<Ort::Session> encoder_session_;
    std::unique_ptr<Ort::Session> decoder_session_;
    std::unique_ptr<Ort::SessionOptions> session_options_;
    std::unique_ptr<Ort::MemoryInfo> memory_info_;
    
    // Feature extraction
    std::unique_ptr<ProvenFeatureExtractor> feature_extractor_;
    
    // Vocabulary and tokenization
    std::vector<std::string> vocabulary_;
    std::unordered_map<int64_t, std::string> token_to_text_;
    
    // Model metadata
    std::vector<std::string> encoder_input_names_;
    std::vector<std::string> encoder_output_names_;
    std::vector<std::string> decoder_input_names_;
    std::vector<std::string> decoder_output_names_;
    
    // State
    bool initialized_;
    
    // Configuration matching NeMo's exact parameters
    static constexpr int SAMPLE_RATE = 16000;
    static constexpr int N_MELS = 80;
    static constexpr int FRAME_LENGTH = 400;  // 25ms at 16kHz (was 1024)
    static constexpr int FRAME_SHIFT = 160;   // 10ms at 16kHz (was 256)
};

#endif // PROVEN_NEMO_STT_HPP