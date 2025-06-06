#pragma once

#include <string>
#include <vector>
#include <memory>
#include <stdexcept>

namespace stt {

/**
 * C++ Wrapper for Python NeMo streaming implementation
 * 
 * This wrapper calls our working Python NeMo streaming script
 * to perform actual speech-to-text transcription using the
 * nvidia/stt_en_fastconformer_hybrid_large_streaming_multi model.
 */
class PythonNeMoWrapper {
public:
    /**
     * Configuration for NeMo streaming
     */
    struct Config {
        std::string model_name = "nvidia/stt_en_fastconformer_hybrid_large_streaming_multi";
        int chunk_size_ms = 1600;  // 1.6 seconds chunks (40 encoder frames)
        int left_chunks = 2;       // Left context chunks  
        int shift_size = 40;       // Shift size (same as chunk for non-overlapping)
        int sample_rate = 16000;   // Audio sample rate
    };

    /**
     * Result from streaming inference
     */
    struct StreamingResult {
        std::string transcript;
        bool success;
        std::string error_message;
        double processing_time_ms;
    };

    /**
     * Constructor
     */
    explicit PythonNeMoWrapper(const Config& config = Config{});
    
    /**
     * Destructor
     */
    ~PythonNeMoWrapper();

    /**
     * Initialize the streaming system
     * @return true if successful
     */
    bool initialize();

    /**
     * Process audio file and return complete transcription
     * @param audio_file_path Path to raw audio file (16-bit PCM, 16kHz)
     * @return StreamingResult with transcript and status
     */
    StreamingResult processAudioFile(const std::string& audio_file_path);

    /**
     * Process raw audio data and return transcription
     * @param audio_data Raw audio samples (float32, normalized)
     * @param sample_count Number of samples
     * @return StreamingResult with transcript and status  
     */
    StreamingResult processAudioData(const float* audio_data, size_t sample_count);

    /**
     * Get current configuration
     */
    const Config& getConfig() const { return config_; }

private:
    Config config_;
    bool initialized_;
    std::string python_script_path_;
    std::string temp_dir_;

    /**
     * Execute Python script with given audio file
     */
    StreamingResult executePythonScript(const std::string& audio_file);
    
    /**
     * Save audio data to temporary file
     */
    std::string saveAudioToTempFile(const float* audio_data, size_t sample_count);
    
    /**
     * Parse output from Python script
     */
    StreamingResult parseScriptOutput(const std::string& output, double processing_time);
    
    /**
     * Cleanup temporary files
     */
    void cleanup();
};

} // namespace stt