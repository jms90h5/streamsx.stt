#ifndef KALDIFEAT_EXTRACTOR_HPP
#define KALDIFEAT_EXTRACTOR_HPP

#include "FeatureExtractor.hpp"
#include "simple_fbank.hpp"  // Fallback implementation
#include <memory>

// Forward declarations for kaldifeat types
namespace kaldifeat {
    class OnlineFbank;
    struct FbankOptions;
}

namespace onnx_stt {

/**
 * Kaldifeat-based feature extractor
 * Uses the C++17 static library version of kaldifeat
 * Falls back to simple_fbank if kaldifeat is not available
 */
class KaldifeatExtractor : public FeatureExtractor {
public:
    explicit KaldifeatExtractor(const Config& config);
    ~KaldifeatExtractor() override = default;
    
    bool initialize(const Config& config) override;
    
    std::vector<std::vector<float>> computeFeatures(const std::vector<float>& audio) override;
    std::vector<std::vector<float>> computeFeatures(const int16_t* samples, size_t num_samples) override;
    
    const Config& getConfig() const override { return config_; }
    int getFeatureDim() const override;
    
private:
    Config config_;
    bool kaldifeat_available_;
    
    // Fallback to simple_fbank if kaldifeat not available
    std::unique_ptr<simple_fbank::FbankComputer> simple_fbank_;
    
    // CMVN statistics
    std::vector<float> cmvn_mean_;
    std::vector<float> cmvn_var_;
    bool cmvn_loaded_;
    
    // Helper methods
    bool loadCmvnStats(const std::string& stats_path);
    void applyCmvn(std::vector<std::vector<float>>& features);
    std::vector<float> convertInt16ToFloat(const int16_t* samples, size_t num_samples);
    
    // Kaldifeat-specific members (conditionally compiled)
#ifdef HAVE_KALDIFEAT
    std::unique_ptr<kaldifeat::OnlineFbank> kaldifeat_fbank_;
    std::unique_ptr<kaldifeat::FbankOptions> kaldifeat_opts_;
#endif
};

/**
 * Simple filterbank extractor using the existing simple_fbank
 * This is a wrapper to make it compatible with the FeatureExtractor interface
 */
class SimpleFbankExtractor : public FeatureExtractor {
public:
    explicit SimpleFbankExtractor(const Config& config);
    ~SimpleFbankExtractor() override = default;
    
    bool initialize(const Config& config) override;
    
    std::vector<std::vector<float>> computeFeatures(const std::vector<float>& audio) override;
    std::vector<std::vector<float>> computeFeatures(const int16_t* samples, size_t num_samples) override;
    
    const Config& getConfig() const override { return config_; }
    int getFeatureDim() const override { return config_.num_mel_bins; }
    
private:
    Config config_;
    std::unique_ptr<simple_fbank::FbankComputer> fbank_;
    
    // Helper methods
    std::vector<float> convertInt16ToFloat(const int16_t* samples, size_t num_samples);
};

} // namespace onnx_stt

#endif // KALDIFEAT_EXTRACTOR_HPP