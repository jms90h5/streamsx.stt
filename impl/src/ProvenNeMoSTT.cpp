#include "ProvenNeMoSTT.hpp"
#include "ProvenFeatureExtractor.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <sndfile.h>
#include <unordered_map>

ProvenNeMoSTT::ProvenNeMoSTT() : initialized_(false) {
    try {
        ort_env_ = std::make_unique<Ort::Env>(ORT_LOGGING_LEVEL_WARNING, "ProvenNeMoSTT");
        session_options_ = std::make_unique<Ort::SessionOptions>();
        session_options_->SetIntraOpNumThreads(1);
        session_options_->SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_EXTENDED);
        memory_info_ = std::make_unique<Ort::MemoryInfo>(Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault));
        
        feature_extractor_ = std::make_unique<ProvenFeatureExtractor>();
    } catch (const std::exception& e) {
        std::cerr << "Error initializing ONNX Runtime: " << e.what() << std::endl;
    }
}

ProvenNeMoSTT::~ProvenNeMoSTT() = default;

bool ProvenNeMoSTT::initialize(const std::string& encoder_path, 
                               const std::string& decoder_path,
                               const std::string& /* vocab_path */) {
    try {
        std::cout << "Loading proven working ONNX models..." << std::endl;
        
        // Load encoder model
        encoder_session_ = std::make_unique<Ort::Session>(*ort_env_, encoder_path.c_str(), *session_options_);
        
        // Load decoder model  
        decoder_session_ = std::make_unique<Ort::Session>(*ort_env_, decoder_path.c_str(), *session_options_);
        
        // Get input/output names for encoder
        Ort::AllocatorWithDefaultOptions allocator;
        
        // Encoder input/output names
        size_t encoder_input_count = encoder_session_->GetInputCount();
        size_t encoder_output_count = encoder_session_->GetOutputCount();
        
        for (size_t i = 0; i < encoder_input_count; i++) {
            auto input_name = encoder_session_->GetInputNameAllocated(i, allocator);
            encoder_input_names_.push_back(std::string(input_name.get()));
        }
        
        for (size_t i = 0; i < encoder_output_count; i++) {
            auto output_name = encoder_session_->GetOutputNameAllocated(i, allocator);
            encoder_output_names_.push_back(std::string(output_name.get()));
        }
        
        // Decoder input/output names
        size_t decoder_input_count = decoder_session_->GetInputCount();
        size_t decoder_output_count = decoder_session_->GetOutputCount();
        
        for (size_t i = 0; i < decoder_input_count; i++) {
            auto input_name = decoder_session_->GetInputNameAllocated(i, allocator);
            decoder_input_names_.push_back(std::string(input_name.get()));
        }
        
        for (size_t i = 0; i < decoder_output_count; i++) {
            auto output_name = decoder_session_->GetOutputNameAllocated(i, allocator);
            decoder_output_names_.push_back(std::string(output_name.get()));
        }
        
        // Load vocabulary - use the real vocabulary file
        std::string real_vocab_path = "models/proven_onnx_export/real_vocabulary.txt";
        std::ifstream vocab_file(real_vocab_path);
        if (vocab_file.is_open()) {
            std::string line;
            while (std::getline(vocab_file, line)) {
                if (!line.empty() && line[0] != '#') {
                    // Parse "id\ttoken" format
                    size_t tab_pos = line.find('\t');
                    if (tab_pos != std::string::npos) {
                        int token_id = std::stoi(line.substr(0, tab_pos));
                        std::string token = line.substr(tab_pos + 1);
                        
                        if (static_cast<size_t>(token_id) >= vocabulary_.size()) {
                            vocabulary_.resize(token_id + 1);
                        }
                        vocabulary_[token_id] = token;
                        token_to_text_[token_id] = token;
                    }
                }
            }
            vocab_file.close();
            std::cout << "Loaded vocabulary with " << vocabulary_.size() << " tokens" << std::endl;
        } else {
            std::cerr << "Warning: Could not load vocabulary file: " << real_vocab_path << std::endl;
        }
        
        std::cout << "✓ Encoder loaded: " << encoder_input_names_.size() << " inputs, " 
                  << encoder_output_names_.size() << " outputs" << std::endl;
        std::cout << "✓ Decoder loaded: " << decoder_input_names_.size() << " inputs, " 
                  << decoder_output_names_.size() << " outputs" << std::endl;
        
        initialized_ = true;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error loading models: " << e.what() << std::endl;
        return false;
    }
}

std::string ProvenNeMoSTT::transcribe(const std::string& audio_file_path) {
    if (!initialized_) {
        return "Error: Models not initialized";
    }
    
    try {
        std::cout << "Transcribing: " << audio_file_path << std::endl;
        
        // Load audio file
        auto audio_data = loadAudioFile(audio_file_path, SAMPLE_RATE);
        if (audio_data.empty()) {
            return "Error: Failed to load audio file";
        }
        
        std::cout << "✓ Loaded audio: " << audio_data.size() << " samples" << std::endl;
        
        return transcribe(audio_data, SAMPLE_RATE);
        
    } catch (const std::exception& e) {
        std::cerr << "Error in transcription: " << e.what() << std::endl;
        return "Error: Transcription failed";
    }
}

std::string ProvenNeMoSTT::transcribe(const std::vector<float>& audio_data, int sample_rate) {
    if (!initialized_) {
        return "Error: Models not initialized";
    }
    
    try {
        // Extract features (mel spectrograms)
        auto features = extractFeatures(audio_data, sample_rate);
        if (features.empty()) {
            return "Error: Feature extraction failed";
        }
        
        std::cout << "✓ Extracted features: " << features.size() << " values" << std::endl;
        
        // Save C++ features for comparison with NeMo
        std::ofstream cpp_features_file("cpp_features.txt");
        if (cpp_features_file.is_open()) {
            size_t time_steps = features.size() / N_MELS;
            cpp_features_file << "# C++ Features: " << time_steps << " time steps, " << N_MELS << " mel bins\n";
            cpp_features_file << "# Shape: [" << time_steps << ", " << N_MELS << "]\n";
            
            for (size_t t = 0; t < time_steps; t++) {
                for (size_t m = 0; m < N_MELS; m++) {
                    // Original layout before transposition
                    size_t idx = t * N_MELS + m;
                    cpp_features_file << features[idx];
                    if (m < N_MELS - 1) cpp_features_file << " ";
                }
                cpp_features_file << "\n";
            }
            cpp_features_file.close();
            std::cout << "✓ Saved C++ features to cpp_features.txt for comparison" << std::endl;
        }
        
        // Run encoder
        auto encoder_output = runEncoder(features);
        if (encoder_output.empty()) {
            return "Error: Encoder inference failed";
        }
        
        std::cout << "✓ Encoder output: " << encoder_output.size() << " values" << std::endl;
        
        // Run decoder
        auto transcript = runDecoder(encoder_output);
        
        std::cout << "✓ Transcription complete: " << transcript << std::endl;
        
        return transcript;
        
    } catch (const std::exception& e) {
        std::cerr << "Error in transcription pipeline: " << e.what() << std::endl;
        return "Error: Transcription pipeline failed";
    }
}

std::vector<float> ProvenNeMoSTT::extractFeatures(const std::vector<float>& audio_data, int sample_rate) {
    try {
        // Use the feature extractor to create mel spectrograms
        auto mel_features = feature_extractor_->extractMelSpectrogram(audio_data, sample_rate, N_MELS, FRAME_LENGTH, FRAME_SHIFT);
        
        // Transpose from [time, mel_bins] to [mel_bins, time] to match encoder expectations
        size_t time_steps = mel_features.size() / N_MELS;
        std::vector<float> transposed_features(mel_features.size());
        
        for (size_t t = 0; t < time_steps; t++) {
            for (size_t m = 0; m < N_MELS; m++) {
                // Source: [time, mel] -> Target: [mel, time]
                transposed_features[m * time_steps + t] = mel_features[t * N_MELS + m];
            }
        }
        
        return transposed_features;
    } catch (const std::exception& e) {
        std::cerr << "Error extracting features: " << e.what() << std::endl;
        return {};
    }
}

std::vector<float> ProvenNeMoSTT::runEncoder(const std::vector<float>& features) {
    try {
        // Calculate dimensions based on features
        // Fix: Encoder expects [batch, mel_bins, time] not [batch, time, mel_bins]
        size_t batch_size = 1;
        size_t time_steps = features.size() / N_MELS;
        size_t mel_bins = N_MELS;
        
        std::vector<int64_t> input_shape = {static_cast<int64_t>(batch_size), 
                                           static_cast<int64_t>(mel_bins), 
                                           static_cast<int64_t>(time_steps)};
        
        // Create audio signal input tensor
        Ort::Value audio_tensor = Ort::Value::CreateTensor<float>(
            *memory_info_, 
            const_cast<float*>(features.data()),
            features.size(),
            input_shape.data(),
            input_shape.size()
        );
        
        // Create length input tensor - encoder needs sequence length
        std::vector<int64_t> length_data = {static_cast<int64_t>(time_steps)};
        std::vector<int64_t> length_shape = {static_cast<int64_t>(batch_size)};
        
        Ort::Value length_tensor = Ort::Value::CreateTensor<int64_t>(
            *memory_info_,
            length_data.data(),
            length_data.size(),
            length_shape.data(),
            length_shape.size()
        );
        
        // Prepare input tensors in correct order
        std::vector<Ort::Value> input_tensors;
        input_tensors.push_back(std::move(audio_tensor));
        input_tensors.push_back(std::move(length_tensor));
        
        // Prepare input/output names
        std::vector<const char*> input_names;
        for (const auto& name : encoder_input_names_) {
            input_names.push_back(name.c_str());
        }
        
        std::vector<const char*> output_names;
        for (const auto& name : encoder_output_names_) {
            output_names.push_back(name.c_str());
        }
        
        // Run inference with both inputs
        auto output_tensors = encoder_session_->Run(
            Ort::RunOptions{nullptr},
            input_names.data(),
            input_tensors.data(),
            input_tensors.size(),
            output_names.data(),
            output_names.size()
        );
        
        // Extract output data
        if (output_tensors.empty()) {
            std::cerr << "Error: No encoder output" << std::endl;
            return {};
        }
        
        const float* output_data = output_tensors[0].GetTensorData<float>();
        auto output_shape = output_tensors[0].GetTensorTypeAndShapeInfo().GetShape();
        
        size_t output_size = 1;
        for (auto dim : output_shape) {
            output_size *= dim;
        }
        
        return std::vector<float>(output_data, output_data + output_size);
        
    } catch (const std::exception& e) {
        std::cerr << "Error in encoder inference: " << e.what() << std::endl;
        return {};
    }
}

std::string ProvenNeMoSTT::runDecoder(const std::vector<float>& encoder_output) {
    try {
        // For now, implement a simple greedy decoding approach
        // This is simplified compared to full beam search but should work for testing
        
        // Calculate encoder output shape - decoder expects 3D: [batch, feature_dim, time]
        // Encoder output = 35328 values, feature_dim = 512, so time = 35328 / 512 = 69
        // But decoder expects [batch, feature_dim, time] not [batch, time, feature_dim]
        size_t batch_size = 1;
        size_t feature_dim = 512;  // FastConformer feature dimension
        size_t time_steps = encoder_output.size() / feature_dim;
        
        std::vector<int64_t> input_shape = {static_cast<int64_t>(batch_size), 
                                           static_cast<int64_t>(feature_dim),
                                           static_cast<int64_t>(time_steps)};
        
        // Transpose encoder output from [batch, time, feature_dim] to [batch, feature_dim, time]
        std::vector<float> transposed_encoder_output(encoder_output.size());
        for (size_t t = 0; t < time_steps; t++) {
            for (size_t f = 0; f < feature_dim; f++) {
                // Source: [batch=0, time=t, feature=f] -> Target: [batch=0, feature=f, time=t]
                transposed_encoder_output[f * time_steps + t] = encoder_output[t * feature_dim + f];
            }
        }
        
        // Create encoder outputs tensor (first input)
        Ort::Value encoder_outputs_tensor = Ort::Value::CreateTensor<float>(
            *memory_info_,
            transposed_encoder_output.data(),
            transposed_encoder_output.size(),
            input_shape.data(),
            input_shape.size()
        );
        
        // For RNN-T decoder, we typically need:
        // 1. encoder_outputs (what we have)
        // 2. encoded_lengths 
        // 3. targets (for training, can be empty for inference)
        // 4. target_lengths (for training, can be empty for inference)  
        // 5. Maybe additional parameters
        
        // Create encoded_lengths tensor
        std::vector<int64_t> encoded_lengths_data = {static_cast<int64_t>(time_steps)};
        std::vector<int64_t> lengths_shape = {static_cast<int64_t>(batch_size)};
        
        Ort::Value encoded_lengths_tensor = Ort::Value::CreateTensor<int64_t>(
            *memory_info_,
            encoded_lengths_data.data(),
            encoded_lengths_data.size(),
            lengths_shape.data(),
            lengths_shape.size()
        );
        
        // Create decoder inputs based on actual model specification from ONNX analysis
        
        // Input 1: targets (int32, [batch, seq_len]) - start with blank token for inference
        std::vector<int32_t> targets_data = {0};  // Blank token to start inference
        std::vector<int64_t> targets_shape = {static_cast<int64_t>(batch_size), 1};
        
        Ort::Value targets_tensor = Ort::Value::CreateTensor<int32_t>(
            *memory_info_,
            targets_data.data(),
            targets_data.size(),
            targets_shape.data(),
            targets_shape.size()
        );
        
        // Input 2: target_length (int32, [batch])
        std::vector<int32_t> target_length_data = {1};  // Length of targets sequence
        std::vector<int64_t> target_length_shape = {static_cast<int64_t>(batch_size)};
        
        Ort::Value target_length_tensor = Ort::Value::CreateTensor<int32_t>(
            *memory_info_,
            target_length_data.data(),
            target_length_data.size(),
            target_length_shape.data(),
            target_length_shape.size()
        );
        
        // Input 3: input_states_1 (float32, [1, 1, 640]) - LSTM hidden state for single timestep
        std::vector<float> input_states_1_data(1 * 1 * 640, 0.0f);
        std::vector<int64_t> states_1_shape = {1, 1, 640};
        
        Ort::Value states_1_tensor = Ort::Value::CreateTensor<float>(
            *memory_info_,
            input_states_1_data.data(),
            input_states_1_data.size(),
            states_1_shape.data(),
            states_1_shape.size()
        );
        
        // Input 4: input_states_2 (float32, [1, 1, 640]) - LSTM cell state for single timestep  
        std::vector<float> input_states_2_data(1 * 1 * 640, 0.0f);
        std::vector<int64_t> states_2_shape = {1, 1, 640};
        
        Ort::Value states_2_tensor = Ort::Value::CreateTensor<float>(
            *memory_info_,
            input_states_2_data.data(),
            input_states_2_data.size(),
            states_2_shape.data(),
            states_2_shape.size()
        );
        
        // Prepare all input tensors in correct order: encoder_outputs, targets, target_length, input_states_1, input_states_2
        std::vector<Ort::Value> input_tensors;
        input_tensors.push_back(std::move(encoder_outputs_tensor));
        input_tensors.push_back(std::move(targets_tensor));
        input_tensors.push_back(std::move(target_length_tensor));
        input_tensors.push_back(std::move(states_1_tensor));
        input_tensors.push_back(std::move(states_2_tensor));
        
        // Prepare input/output names
        std::vector<const char*> input_names;
        for (const auto& name : decoder_input_names_) {
            input_names.push_back(name.c_str());
        }
        
        std::vector<const char*> output_names;
        for (const auto& name : decoder_output_names_) {
            output_names.push_back(name.c_str());
        }
        
        std::cout << "Debug: Decoder expects " << decoder_input_names_.size() 
                  << " inputs, providing " << input_tensors.size() << std::endl;
        for (size_t i = 0; i < decoder_input_names_.size(); i++) {
            std::cout << "Input " << i << ": " << decoder_input_names_[i] << std::endl;
        }
        
        // Run decoder inference with all inputs
        auto output_tensors = decoder_session_->Run(
            Ort::RunOptions{nullptr},
            input_names.data(),
            input_tensors.data(),
            std::min(input_tensors.size(), decoder_input_names_.size()),
            output_names.data(),
            output_names.size()
        );
        
        if (output_tensors.empty()) {
            std::cerr << "Error: No decoder output" << std::endl;
            return "Error: Decoder failed";
        }
        
        // Analyze decoder output first
        auto output_shape = output_tensors[0].GetTensorTypeAndShapeInfo().GetShape();
        std::cout << "Decoder output shape: [";
        for (size_t i = 0; i < output_shape.size(); i++) {
            std::cout << output_shape[i];
            if (i < output_shape.size() - 1) std::cout << ", ";
        }
        std::cout << "]" << std::endl;
        
        // From ONNX analysis: outputs shape ['dim_outputs_dynamic_axes_1', 'dim_outputs_dynamic_axes_2', 'dim_outputs_dynamic_axes_3', 1025]
        // This is likely [batch, time, vocab_size+1] where last dim is logits over vocabulary
        
        const float* output_data = output_tensors[0].GetTensorData<float>();
        size_t total_elements = 1;
        for (auto dim : output_shape) {
            total_elements *= dim;
        }
        
        std::cout << "Decoder output total elements: " << total_elements << std::endl;
        
        // The output is logits, not token IDs - need to find argmax
        // Shape is [batch, time, 1, vocab_size] = [1, 69, 1, 1025]
        if (output_shape.size() == 4) {
            size_t batch_size = output_shape[0];
            size_t time_steps = output_shape[1]; 
            size_t middle_dim = output_shape[2];
            size_t vocab_size = output_shape[3];
            
            std::cout << "Interpreting as: batch=" << batch_size << ", time=" << time_steps << ", middle=" << middle_dim << ", vocab=" << vocab_size << std::endl;
            
            std::vector<int64_t> predicted_tokens;
            
            // For each time step, find the token with highest probability
            for (size_t t = 0; t < time_steps; t++) {
                float max_prob = -1e9f;
                int64_t best_token = 0;
                
                for (size_t v = 0; v < vocab_size && v < 1024; v++) { // Limit to our vocab size
                    // Index calculation for [batch, time, 1, vocab]
                    size_t idx = t * middle_dim * vocab_size + 0 * vocab_size + v;
                    if (idx < total_elements) {
                        float prob = output_data[idx];
                        if (prob > max_prob) {
                            max_prob = prob;
                            best_token = static_cast<int64_t>(v);
                        }
                    }
                }
                
                predicted_tokens.push_back(best_token);
            }
            
            std::cout << "Predicted " << predicted_tokens.size() << " tokens via argmax" << std::endl;
            return decodeTokens(predicted_tokens);
        } else {
            return "Error: Unexpected decoder output dimensions";
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error in decoder inference: " << e.what() << std::endl;
        return "Error: Decoder inference failed";
    }
}

std::vector<float> ProvenNeMoSTT::loadAudioFile(const std::string& file_path, int target_sample_rate) {
    SF_INFO sfinfo;
    memset(&sfinfo, 0, sizeof(sfinfo));
    
    SNDFILE* file = sf_open(file_path.c_str(), SFM_READ, &sfinfo);
    if (!file) {
        std::cerr << "Error opening audio file: " << file_path << std::endl;
        return {};
    }
    
    // Read all samples
    std::vector<float> audio_data(sfinfo.frames * sfinfo.channels);
    sf_count_t frames_read = sf_readf_float(file, audio_data.data(), sfinfo.frames);
    sf_close(file);
    
    if (frames_read != sfinfo.frames) {
        std::cerr << "Error reading audio data" << std::endl;
        return {};
    }
    
    // Convert to mono if needed
    if (sfinfo.channels > 1) {
        std::vector<float> mono_data(sfinfo.frames);
        for (int i = 0; i < sfinfo.frames; i++) {
            float sum = 0.0f;
            for (int ch = 0; ch < sfinfo.channels; ch++) {
                sum += audio_data[i * sfinfo.channels + ch];
            }
            mono_data[i] = sum / sfinfo.channels;
        }
        audio_data = std::move(mono_data);
    }
    
    // Resample if needed (simple implementation)
    if (sfinfo.samplerate != target_sample_rate) {
        std::cout << "Note: Resampling from " << sfinfo.samplerate 
                  << " Hz to " << target_sample_rate << " Hz" << std::endl;
        // For now, just return the data as-is
        // A proper implementation would use a resampling library
    }
    
    return audio_data;
}

std::string ProvenNeMoSTT::decodeTokens(const std::vector<int64_t>& tokens) {
    std::stringstream result;
    
    std::cout << "Debug: Decoding " << tokens.size() << " tokens" << std::endl;
    
    for (size_t i = 0; i < tokens.size() && i < 20; i++) {
        int64_t token = tokens[i];
        std::cout << "Token " << i << ": " << token;
        
        if (token == 0) {
            std::cout << " (<unk>/blank)" << std::endl;
            continue; // Skip blank/unknown tokens
        }
        
        auto it = token_to_text_.find(token);
        if (it != token_to_text_.end()) {
            std::string token_text = it->second;
            std::cout << " -> '" << token_text << "'" << std::endl;
            
            // Handle SentencePiece format: ▁ indicates word boundary
            if (token_text.length() >= 3 && token_text.substr(0, 3) == "▁") {
                // Remove ▁ and add space before word
                if (!result.str().empty()) {
                    result << " ";
                }
                result << token_text.substr(3);
            } else {
                // Regular subword token, append directly
                result << token_text;
            }
        } else {
            std::cout << " (not found in vocab)" << std::endl;
        }
    }
    
    std::string text = result.str();
    
    std::cout << "Final decoded text: '" << text << "'" << std::endl;
    
    return text.empty() ? "Error: Could not decode tokens" : text;
}