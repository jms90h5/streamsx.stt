#ifndef NEMO_CTC_INTERFACE_HPP
#define NEMO_CTC_INTERFACE_HPP

#include <string>
#include <vector>
#include <memory>

/**
 * Pure interface for NeMo CTC speech recognition that completely hides ONNX Runtime headers
 * This prevents macro conflicts between ONNX Runtime and SPL/Streams
 */
class NeMoCTCInterface {
public:
    virtual ~NeMoCTCInterface() = default;
    
    // Initialize with CTC model and tokens paths
    virtual bool initialize(const std::string& model_path, const std::string& tokens_path) = 0;
    
    // Process audio samples and return transcription
    virtual std::string transcribe(const std::vector<float>& audio_samples) = 0;
    
    // Get model info
    virtual std::string getModelInfo() const = 0;
    virtual bool isInitialized() const = 0;
};

// Factory function - implementation in .cpp file to hide ONNX dependencies
std::unique_ptr<NeMoCTCInterface> createNeMoCTCImpl();

#endif // NEMO_CTC_INTERFACE_HPP