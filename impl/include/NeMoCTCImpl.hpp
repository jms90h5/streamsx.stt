#pragma once

#include "onnx_compat.hpp"
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include "KaldiFbankFeatureExtractor.hpp"
#include "NeMoCTCInterface.hpp"

class NeMoCTCImpl : public NeMoCTCInterface {
public:
    NeMoCTCImpl();
    ~NeMoCTCImpl();
    
    // Initialize with CTC model and tokens
    bool initialize(const std::string& model_path, const std::string& tokens_path) override;
    
    // Process audio and return transcription
    std::string transcribe(const std::vector<float>& audio_samples) override;
    
    // Get model info
    std::string getModelInfo() const override;
    bool isInitialized() const override { return initialized_; }
    
private:
    // ONNX Runtime components
    std::unique_ptr<Ort::Env> env_;
    std::unique_ptr<Ort::Session> session_;
    std::unique_ptr<Ort::SessionOptions> session_options_;
    std::unique_ptr<Ort::MemoryInfo> memory_info_;
    
    // Model metadata
    std::vector<std::string> input_names_;
    std::vector<std::string> output_names_;
    std::vector<std::vector<int64_t>> input_shapes_;
    std::vector<std::vector<int64_t>> output_shapes_;
    
    // Vocabulary
    std::unordered_map<int, std::string> vocab_;
    int blank_id_;
    
    // State
    bool initialized_;
    
    // Feature extractor
    KaldiFbankFeatureExtractor feature_extractor_;
    
    // Helper methods
    bool loadVocabulary(const std::string& tokens_path);
    std::vector<float> extractMelFeatures(const std::vector<float>& audio_samples);
    std::vector<float> loadWorkingFeatures(const std::string& filename);
    std::string ctcDecode(const std::vector<float>& logits, const std::vector<int64_t>& shape);
};