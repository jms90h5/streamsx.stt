#include "OnnxSTTImpl.hpp"
#include <iostream>
#include <chrono>
#include <cmath>

namespace onnx_stt {

OnnxSTTImpl::OnnxSTTImpl(const Config& config)
    : config_(config) {
    last_process_time_ = std::chrono::steady_clock::now();
}

OnnxSTTImpl::~OnnxSTTImpl() = default;

bool OnnxSTTImpl::initialize() {
    try {
        // Map OnnxSTT paths to ZipformerRNNT three-model architecture
        std::string model_dir = config_.encoder_onnx_path;
        
        // Remove filename if provided to get directory
        size_t last_slash = model_dir.find_last_of("/");
        size_t onnx_pos = model_dir.find(".onnx");
        if (onnx_pos != std::string::npos && last_slash != std::string::npos) {
            model_dir = model_dir.substr(0, last_slash);
        }
        
        // Configure ZipformerRNNT with three-model pipeline
        ZipformerRNNT::Config zipformer_config;
        zipformer_config.encoder_path = model_dir + "/encoder-epoch-99-avg-1.onnx";
        zipformer_config.decoder_path = model_dir + "/decoder-epoch-99-avg-1.onnx";
        zipformer_config.joiner_path = model_dir + "/joiner-epoch-99-avg-1.onnx";
        zipformer_config.tokens_path = config_.vocab_path;
        zipformer_config.num_threads = config_.num_threads;
        zipformer_config.chunk_size = 39;  // Zipformer expects 39 frames
        zipformer_config.beam_size = config_.beam_size;
        
        // Create and initialize ZipformerRNNT
        zipformer_ = std::make_unique<ZipformerRNNT>(zipformer_config);
        if (!zipformer_->initialize()) {
            std::cerr << "Failed to initialize ZipformerRNNT" << std::endl;
            return false;
        }
        
        // Initialize feature extraction (kaldifeat)
        simple_fbank::FbankComputer::Options fbank_opts;
        fbank_opts.sample_rate = config_.sample_rate;
        fbank_opts.num_mel_bins = config_.num_mel_bins;
        fbank_opts.frame_length_ms = config_.frame_length_ms;
        fbank_opts.frame_shift_ms = config_.frame_shift_ms;
        fbank_ = std::make_unique<simple_fbank::FbankComputer>(fbank_opts);
        
        std::cout << "OnnxSTTImpl initialized with ZipformerRNNT pipeline" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Exception during initialization: " << e.what() << std::endl;
        return false;
    }
}

OnnxSTTImpl::TranscriptionResult OnnxSTTImpl::processAudioChunk(
    const int16_t* samples, 
    size_t num_samples, 
    uint64_t timestamp_ms) {
    
    auto start_time = std::chrono::steady_clock::now();
    TranscriptionResult result;
    result.timestamp_ms = timestamp_ms;
    result.is_final = false;
    result.confidence = 0.0;
    
    try {
        // Stage 1: Voice Activity Detection (optional - for now process all audio)
        
        // Stage 2: Convert int16 to float and buffer
        std::vector<float> float_samples(num_samples);
        for (size_t i = 0; i < num_samples; ++i) {
            float_samples[i] = samples[i] / 32768.0f;
        }
        
        // Add to audio buffer
        audio_buffer_.insert(audio_buffer_.end(), 
                           float_samples.begin(), 
                           float_samples.end());
        
        // Update stats
        stats_.total_audio_ms += (num_samples * 1000) / config_.sample_rate;
        
        // Process if we have enough samples for a chunk (39 frames * 160 samples/frame)
        const size_t samples_per_chunk = 39 * (config_.sample_rate * config_.frame_shift_ms / 1000);
        
        while (audio_buffer_.size() >= samples_per_chunk) {
            // Extract chunk
            std::vector<float> chunk(audio_buffer_.begin(), 
                                   audio_buffer_.begin() + samples_per_chunk);
            audio_buffer_.erase(audio_buffer_.begin(), 
                              audio_buffer_.begin() + samples_per_chunk);
            
            // Stage 3: Feature extraction using kaldifeat
            auto features_2d = fbank_->computeFeatures(chunk);
            
            // Flatten features for ZipformerRNNT (expects 1D vector)
            std::vector<float> features_flat;
            for (const auto& frame : features_2d) {
                features_flat.insert(features_flat.end(), frame.begin(), frame.end());
            }
            
            // Ensure we have exactly 39 frames worth of features (39 * 80 = 3120 floats)
            const size_t expected_size = 39 * 80;
            if (features_flat.size() < expected_size) {
                // Pad with zeros if needed
                features_flat.resize(expected_size, 0.0f);
            }
            
            // Stage 4: Speech recognition using ZipformerRNNT
            auto zipformer_result = zipformer_->processChunk(features_flat);
            
            // Update result
            result.text = zipformer_result.text;
            result.confidence = zipformer_result.confidence;
            result.is_final = zipformer_result.is_final;
        }
        
        // Calculate latency
        auto end_time = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
            end_time - start_time).count();
        result.latency_ms = duration;
        
        // Update stats
        stats_.total_processing_ms += duration;
        if (stats_.total_audio_ms > 0) {
            stats_.real_time_factor = 
                static_cast<double>(stats_.total_processing_ms) / stats_.total_audio_ms;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error processing audio chunk: " << e.what() << std::endl;
    }
    
    return result;
}

void OnnxSTTImpl::reset() {
    if (zipformer_) {
        zipformer_->reset();
    }
    audio_buffer_.clear();
    feature_buffer_.clear();
    stats_ = Stats{};
}

std::vector<float> OnnxSTTImpl::extractFeatures(const std::vector<float>& audio) {
    // This is handled by fbank_ in processAudioChunk
    // The FbankComputer returns 2D features, we need to flatten for compatibility
    auto features_2d = fbank_->computeFeatures(audio);
    std::vector<float> features_flat;
    for (const auto& frame : features_2d) {
        features_flat.insert(features_flat.end(), frame.begin(), frame.end());
    }
    return features_flat;
}

bool OnnxSTTImpl::setupZipformerModel() {
    // This is handled in initialize()
    return true;
}

} // namespace onnx_stt