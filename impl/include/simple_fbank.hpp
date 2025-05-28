#ifndef SIMPLE_FBANK_HPP
#define SIMPLE_FBANK_HPP

#include <vector>
#include <cmath>
#include <algorithm>

namespace simple_fbank {

class FbankComputer {
public:
    struct Options {
        int sample_rate = 16000;
        int num_mel_bins = 80;
        int frame_length_ms = 25;
        int frame_shift_ms = 10;
    };
    
    explicit FbankComputer(const Options& opts)
        : opts_(opts) {
        
        // Convert ms to samples
        frame_length_samples_ = (opts_.sample_rate * opts_.frame_length_ms) / 1000;
        frame_shift_samples_ = (opts_.sample_rate * opts_.frame_shift_ms) / 1000;
        
        // Initialize window function (Hamming)
        window_.resize(frame_length_samples_);
        for (int i = 0; i < frame_length_samples_; ++i) {
            window_[i] = 0.54f - 0.46f * std::cos(2.0f * M_PI * i / (frame_length_samples_ - 1));
        }
    }
    
    std::vector<std::vector<float>> computeFeatures(const std::vector<float>& audio) {
        // Simple placeholder implementation
        // In real implementation, this would compute log mel filterbank features
        
        int num_frames = (audio.size() - frame_length_samples_) / frame_shift_samples_ + 1;
        if (num_frames <= 0) {
            return std::vector<std::vector<float>>();
        }
        
        // Return 2D vector where each inner vector is one frame
        std::vector<std::vector<float>> features;
        features.reserve(num_frames);
        
        // Fill with normalized audio energy as placeholder
        for (int frame = 0; frame < num_frames; ++frame) {
            int start = frame * frame_shift_samples_;
            float energy = 0.0f;
            
            // Compute frame energy
            for (int i = 0; i < frame_length_samples_ && start + i < audio.size(); ++i) {
                float sample = audio[start + i] * window_[i];
                energy += sample * sample;
            }
            energy = std::log(energy + 1e-10f);
            
            // Create feature vector for this frame
            std::vector<float> frame_features(opts_.num_mel_bins);
            for (int mel = 0; mel < opts_.num_mel_bins; ++mel) {
                frame_features[mel] = energy * 0.1f * (1.0f + 0.1f * mel);
            }
            
            features.push_back(frame_features);
        }
        
        return features;
    }
    
    int getFeatureDim() const { return opts_.num_mel_bins; }

private:
    Options opts_;
    int frame_length_samples_;
    int frame_shift_samples_;
    std::vector<float> window_;
};

} // namespace simple_fbank

#endif // SIMPLE_FBANK_HPP