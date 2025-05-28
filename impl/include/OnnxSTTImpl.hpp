#ifndef ONNX_STT_IMPL_HPP
#define ONNX_STT_IMPL_HPP

#include <string>
#include <vector>
#include <memory>
#include <queue>
#include <mutex>
#include <atomic>
#include <chrono>
#include "ZipformerRNNT.hpp"
#include "simple_fbank.hpp"

namespace onnx_stt {

/**
 * ONNX-based Speech-to-Text implementation
 * Uses ZipformerRNNT for three-model pipeline with streaming support
 */
class OnnxSTTImpl {
public:
    struct Config {
        // Model paths
        std::string encoder_onnx_path;
        std::string vocab_path;
        std::string cmvn_stats_path;
        
        // Audio parameters
        int sample_rate = 16000;
        int chunk_size_ms = 100;
        
        // Feature parameters
        int num_mel_bins = 80;
        int frame_length_ms = 25;
        int frame_shift_ms = 10;
        
        // Decoding parameters
        int beam_size = 10;
        int blank_id = 0;
        
        // Performance tuning
        int num_threads = 4;
        bool use_gpu = false;
    };
    
    struct TranscriptionResult {
        std::string text;
        bool is_final;
        double confidence;
        uint64_t timestamp_ms;
        uint64_t latency_ms;
    };
    
    explicit OnnxSTTImpl(const Config& config);
    ~OnnxSTTImpl();
    
    // Initialize ONNX runtime and load models
    bool initialize();
    
    // Process audio chunk
    TranscriptionResult processAudioChunk(const int16_t* samples, 
                                         size_t num_samples, 
                                         uint64_t timestamp_ms);
    
    // Reset decoder state
    void reset();
    
    // Get performance stats
    struct Stats {
        uint64_t total_audio_ms = 0;
        uint64_t total_processing_ms = 0;
        double real_time_factor = 0.0;
    };
    Stats getStats() const { return stats_; }
    
private:
    Config config_;
    
    // Three-stage pipeline components
    std::unique_ptr<ZipformerRNNT> zipformer_;
    std::unique_ptr<simple_fbank::FbankComputer> fbank_;
    
    // Audio buffering for streaming
    std::vector<float> audio_buffer_;
    std::vector<float> feature_buffer_;
    
    // Performance tracking
    mutable Stats stats_;
    std::chrono::steady_clock::time_point last_process_time_;
    
    // Internal methods
    std::vector<float> extractFeatures(const std::vector<float>& audio);
    bool setupZipformerModel();
};

} // namespace onnx_stt

#endif // ONNX_STT_IMPL_HPP