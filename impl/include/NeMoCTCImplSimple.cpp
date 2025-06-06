#include "NeMoCTCImpl.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>
#include <algorithm>

NeMoCTCImpl::NeMoCTCImpl() : initialized_(false), blank_id_(-1) {
}

NeMoCTCImpl::~NeMoCTCImpl() {
}

bool NeMoCTCImpl::initialize(const std::string& model_path, const std::string& tokens_path) {
    try {
        // Initialize ONNX Runtime
        env_ = std::make_unique<Ort::Env>(ORT_LOGGING_LEVEL_WARNING, "NeMoCTC");
        session_options_ = std::make_unique<Ort::SessionOptions>();
        session_options_->SetIntraOpNumThreads(1);
        session_options_->SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_EXTENDED);
        
        memory_info_ = std::make_unique<Ort::MemoryInfo>(
            Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault)
        );
        
        // Load model
        session_ = std::make_unique<Ort::Session>(*env_, model_path.c_str(), *session_options_);
        
        // Load vocabulary
        if (!loadVocabulary(tokens_path)) {
            return false;
        }
        
        initialized_ = true;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Initialization error: " << e.what() << std::endl;
        return false;
    }
}

bool NeMoCTCImpl::loadVocabulary(const std::string& vocab_path) {
    std::ifstream file(vocab_path);
    if (!file.is_open()) {
        std::cerr << "Cannot open vocabulary file: " << vocab_path << std::endl;
        return false;
    }
    
    vocab_.clear();
    std::string line;
    int max_id = -1;
    
    while (std::getline(file, line)) {
        size_t space_pos = line.find(' ');
        if (space_pos != std::string::npos) {
            std::string token = line.substr(0, space_pos);
            int id = std::stoi(line.substr(space_pos + 1));
            vocab_[id] = token;
            max_id = std::max(max_id, id);
        }
    }
    
    blank_id_ = max_id;
    return !vocab_.empty();
}

std::vector<float> NeMoCTCImpl::extractMelFeatures(const std::vector<float>& audio_samples) {
    // TEMPORARY: Return dummy features of the right size
    // This is just to prove the pipeline works
    // In production, use a proper mel spectrogram library
    
    const int n_mels = 80;
    const int hop_length = 160;
    const int n_fft = 512;
    const int pad_length = n_fft / 2;
    
    // Calculate number of frames with padding
    int padded_length = audio_samples.size() + 2 * pad_length;
    int n_frames = 1 + (padded_length - n_fft) / hop_length;
    
    // For now, return zeros - this will produce blank output
    // But proves the pipeline is correct
    std::vector<float> features(n_mels * n_frames, -10.0f);  // Log scale
    
    std::cout << "TEMPORARY: Using dummy features - implement proper mel spectrogram" << std::endl;
    std::cout << "Audio samples: " << audio_samples.size() << std::endl;
    std::cout << "Expected frames: " << n_frames << std::endl;
    
    return features;
}

std::string NeMoCTCImpl::transcribe(const std::vector<float>& audio_samples) {
    if (!initialized_) {
        return "ERROR: Model not initialized";
    }
    
    try {
        // Extract mel features
        auto mel_features = extractMelFeatures(audio_samples);
        
        // Calculate dimensions
        int n_mels = 80;
        int n_frames = mel_features.size() / n_mels;
        
        // Create input tensors
        std::vector<int64_t> audio_shape = {1, n_mels, n_frames};
        auto audio_tensor = Ort::Value::CreateTensor<float>(
            *memory_info_, mel_features.data(), mel_features.size(),
            audio_shape.data(), audio_shape.size()
        );
        
        std::vector<int64_t> length_data = {n_frames};
        std::vector<int64_t> length_shape = {1};
        auto length_tensor = Ort::Value::CreateTensor<int64_t>(
            *memory_info_, length_data.data(), length_data.size(),
            length_shape.data(), length_shape.size()
        );
        
        // Run inference
        std::vector<const char*> input_names = {"audio_signal", "length"};
        std::vector<const char*> output_names = {"logprobs"};
        std::vector<Ort::Value> inputs;
        inputs.push_back(std::move(audio_tensor));
        inputs.push_back(std::move(length_tensor));
        
        auto outputs = session_->Run(Ort::RunOptions{nullptr}, 
                                   input_names.data(), inputs.data(), inputs.size(),
                                   output_names.data(), output_names.size());
        
        // Decode CTC output
        float* logits = outputs[0].GetTensorMutableData<float>();
        auto logits_shape = outputs[0].GetTensorTypeAndShapeInfo().GetShape();
        
        int batch_size = logits_shape[0];
        int time_steps = logits_shape[1];
        int vocab_size = logits_shape[2];
        
        // Simple CTC decoding: argmax + remove blanks and duplicates
        std::vector<int> tokens;
        int prev_token = -1;
        
        for (int t = 0; t < time_steps; t++) {
            int max_idx = 0;
            float max_val = logits[t * vocab_size];
            
            for (int v = 1; v < vocab_size; v++) {
                if (logits[t * vocab_size + v] > max_val) {
                    max_val = logits[t * vocab_size + v];
                    max_idx = v;
                }
            }
            
            // Skip blanks and repeated tokens
            if (max_idx != blank_id_ && max_idx != prev_token) {
                tokens.push_back(max_idx);
            }
            prev_token = max_idx;
        }
        
        // Convert tokens to text
        std::string result = tokensToText(tokens);
        return result;
        
    } catch (const std::exception& e) {
        return "ERROR: " + std::string(e.what());
    }
}

std::string NeMoCTCImpl::tokensToText(const std::vector<int>& tokens) {
    std::string result;
    
    for (int token_id : tokens) {
        if (vocab_.find(token_id) != vocab_.end()) {
            std::string token = vocab_[token_id];
            
            // Handle SentencePiece tokens
            if (token.length() >= 3 && 
                (unsigned char)token[0] == 0xE2 && 
                (unsigned char)token[1] == 0x96 && 
                (unsigned char)token[2] == 0x81) {
                // Token starts with ▁ (UTF-8: E2 96 81)
                if (!result.empty()) {
                    result += " ";
                }
                result += token.substr(3);
            } else {
                result += token;
            }
        }
    }
    
    return result;
}

ModelInfo NeMoCTCImpl::getModelInfo() const {
    ModelInfo info;
    info.name = "NeMo CTC Model";
    
    if (initialized_ && session_) {
        Ort::AllocatorWithDefaultOptions allocator;
        
        // Get input info
        size_t num_inputs = session_->GetInputCount();
        info.input_names.resize(num_inputs);
        info.input_shapes.resize(num_inputs);
        
        for (size_t i = 0; i < num_inputs; i++) {
            auto input_name = session_->GetInputNameAllocated(i, allocator);
            info.input_names[i] = input_name.get();
            
            auto type_info = session_->GetInputTypeInfo(i);
            auto tensor_info = type_info.GetTensorTypeAndShapeInfo();
            info.input_shapes[i] = tensor_info.GetShape();
        }
        
        // Get output info
        size_t num_outputs = session_->GetOutputCount();
        info.output_names.resize(num_outputs);
        info.output_shapes.resize(num_outputs);
        
        for (size_t i = 0; i < num_outputs; i++) {
            auto output_name = session_->GetOutputNameAllocated(i, allocator);
            info.output_names[i] = output_name.get();
            
            auto type_info = session_->GetOutputTypeInfo(i);
            auto tensor_info = type_info.GetTensorTypeAndShapeInfo();
            info.output_shapes[i] = tensor_info.GetShape();
        }
    }
    
    info.description = "Inputs: " + std::to_string(info.input_names.size()) + 
                      ", Outputs: " + std::to_string(info.output_names.size()) +
                      ", Vocab size: " + std::to_string(vocab_.size()) +
                      ", Blank ID: " + std::to_string(blank_id_);
    
    return info;
}