#include "KaldifeatExtractor.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

// Include kaldifeat headers if available
#ifdef HAVE_KALDIFEAT
    #include <kaldifeat/online-feature.h>
    #include <kaldifeat/fbank.h>
    #include <kaldifeat/mel-computations.h>
#else
    // Fallback to minimal kaldifeat compatibility
    #ifdef __has_include
        #if __has_include("kaldifeat_minimal.h")
            #include "kaldifeat_minimal.h"
            #define HAVE_KALDIFEAT_MINIMAL 1
        #endif
    #endif
#endif

namespace onnx_stt {

// KaldifeatExtractor Implementation
KaldifeatExtractor::KaldifeatExtractor(const Config& config) 
    : config_(config), kaldifeat_available_(false), cmvn_loaded_(false) {
    
    // Check if kaldifeat is available
#ifdef HAVE_KALDIFEAT
    kaldifeat_available_ = true;
    std::cout << "Info: Using kaldifeat C++17 library for high-quality feature extraction" << std::endl;
#elif defined(HAVE_KALDIFEAT_MINIMAL)
    kaldifeat_available_ = false;
    std::cout << "Info: Using kaldifeat minimal compatibility layer, falling back to simple_fbank" << std::endl;
#else
    kaldifeat_available_ = false;
    std::cout << "Info: Kaldifeat not available, falling back to simple_fbank" << std::endl;
#endif
}

bool KaldifeatExtractor::initialize(const Config& config) {
    config_ = config;
    
    if (kaldifeat_available_) {
        // Initialize kaldifeat
#ifdef HAVE_KALDIFEAT
        try {
            // Create kaldifeat options
            kaldifeat_opts_ = std::make_unique<kaldifeat::FbankOptions>();
            kaldifeat_opts_->frame_opts.samp_freq = config_.sample_rate;
            kaldifeat_opts_->frame_opts.frame_length_ms = config_.frame_length_ms;
            kaldifeat_opts_->frame_opts.frame_shift_ms = config_.frame_shift_ms;
            kaldifeat_opts_->mel_opts.num_bins = config_.num_mel_bins;
            kaldifeat_opts_->mel_opts.low_freq = config_.low_freq;
            kaldifeat_opts_->mel_opts.high_freq = config_.high_freq;
            kaldifeat_opts_->use_energy = config_.use_energy;
            kaldifeat_opts_->use_log_fbank = config_.use_log_fbank;
            
            // Create online feature extractor
            kaldifeat_fbank_ = std::make_unique<kaldifeat::OnlineFbank>(*kaldifeat_opts_);
            
            // Load CMVN statistics if provided
            if (!config_.cmvn_stats_path.empty() && config_.apply_cmvn) {
                cmvn_loaded_ = loadCmvnStats(config_.cmvn_stats_path);
                if (!cmvn_loaded_) {
                    std::cerr << "Warning: Failed to load CMVN stats from " 
                             << config_.cmvn_stats_path << std::endl;
                }
            }
            
            std::cout << "✓ Kaldifeat initialized successfully" << std::endl;
            return true;
            
        } catch (const std::exception& e) {
            std::cerr << "Error initializing kaldifeat: " << e.what() << std::endl;
            std::cerr << "Falling back to simple_fbank" << std::endl;
            kaldifeat_available_ = false;
            // Fall through to simple_fbank initialization
        }
#else
        // This shouldn't happen if kaldifeat_available_ is set correctly
        std::cerr << "Error: kaldifeat_available_ is true but HAVE_KALDIFEAT not defined" << std::endl;
        kaldifeat_available_ = false;
        // Fall through to simple_fbank initialization
#endif
    }
    
    // Initialize simple_fbank (either as fallback or primary)
    {
        // Initialize simple_fbank as fallback
        simple_fbank::FbankComputer::Options opts;
        opts.sample_rate = config_.sample_rate;
        opts.frame_length_ms = config_.frame_length_ms;
        opts.frame_shift_ms = config_.frame_shift_ms;
        opts.num_mel_bins = config_.num_mel_bins;
        
        simple_fbank_ = std::make_unique<simple_fbank::FbankComputer>(opts);
        
        // Load CMVN statistics if provided
        if (!config_.cmvn_stats_path.empty() && config_.apply_cmvn) {
            cmvn_loaded_ = loadCmvnStats(config_.cmvn_stats_path);
            if (!cmvn_loaded_) {
                std::cerr << "Warning: Failed to load CMVN stats from " 
                         << config_.cmvn_stats_path << std::endl;
            }
        }
        
        return true;
    }
}

std::vector<std::vector<float>> KaldifeatExtractor::computeFeatures(const std::vector<float>& audio) {
    if (kaldifeat_available_) {
        // Use kaldifeat for high-quality feature extraction
#ifdef HAVE_KALDIFEAT
        try {
            // Reset the feature extractor for new utterance
            kaldifeat_fbank_->Reset();
            
            // Feed audio data to kaldifeat
            // Note: kaldifeat expects data in chunks, we'll process the entire audio
            // Convert vector<float> to the format expected by kaldifeat
            std::vector<float> audio_copy = audio;  // kaldifeat may modify the input
            
            // Kaldifeat processes audio in frames, so we need to call AcceptWaveform
            kaldifeat_fbank_->AcceptWaveform(config_.sample_rate, audio_copy);
            
            // Signal end of input
            kaldifeat_fbank_->InputFinished();
            
            // Extract features
            std::vector<std::vector<float>> features;
            int num_frames = kaldifeat_fbank_->NumFramesReady();
            
            for (int i = 0; i < num_frames; ++i) {
                std::vector<float> frame_features(kaldifeat_fbank_->Dim());
                kaldifeat_fbank_->GetFrame(i, frame_features.data());
                features.push_back(std::move(frame_features));
            }
            
            // Apply CMVN if available
            if (cmvn_loaded_ && config_.apply_cmvn) {
                applyCmvn(features);
            }
            
            return features;
            
        } catch (const std::exception& e) {
            std::cerr << "Error in kaldifeat feature extraction: " << e.what() << std::endl;
            std::cerr << "Falling back to simple_fbank for this chunk" << std::endl;
            // Fall through to simple_fbank
        }
#endif
    }
    
    // Use simple_fbank (either as fallback or primary)
    {
        // Use simple_fbank fallback
        auto features = simple_fbank_->computeFeatures(audio);
        
        // Apply CMVN if available
        if (cmvn_loaded_ && config_.apply_cmvn) {
            applyCmvn(features);
        }
        
        return features;
    }
}

std::vector<std::vector<float>> KaldifeatExtractor::computeFeatures(const int16_t* samples, size_t num_samples) {
    auto audio_float = convertInt16ToFloat(samples, num_samples);
    return computeFeatures(audio_float);
}

int KaldifeatExtractor::getFeatureDim() const {
    if (kaldifeat_available_) {
        return config_.num_mel_bins;
    } else {
        return config_.num_mel_bins;  // simple_fbank dimension
    }
}

bool KaldifeatExtractor::loadCmvnStats(const std::string& stats_path) {
    std::ifstream file(stats_path);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        std::string line;
        std::vector<std::vector<float>> stats;
        
        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '#') continue;
            
            std::istringstream iss(line);
            std::vector<float> row;
            float value;
            
            while (iss >> value) {
                row.push_back(value);
            }
            
            if (!row.empty()) {
                stats.push_back(row);
            }
        }
        
        // Expecting: [mean_vector, var_vector, count]
        if (stats.size() >= 2 && stats[0].size() == stats[1].size()) {
            cmvn_mean_ = stats[0];
            cmvn_var_ = stats[1];
            
            // Convert variance to standard deviation
            for (auto& var : cmvn_var_) {
                var = std::sqrt(var);
                if (var == 0.0f) var = 1.0f;  // Avoid division by zero
            }
            
            return true;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error parsing CMVN stats: " << e.what() << std::endl;
    }
    
    return false;
}

void KaldifeatExtractor::applyCmvn(std::vector<std::vector<float>>& features) {
    if (cmvn_mean_.empty() || cmvn_var_.empty()) return;
    
    for (auto& frame : features) {
        for (size_t i = 0; i < frame.size() && i < cmvn_mean_.size(); ++i) {
            frame[i] = (frame[i] - cmvn_mean_[i]) / cmvn_var_[i];
        }
    }
}

std::vector<float> KaldifeatExtractor::convertInt16ToFloat(const int16_t* samples, size_t num_samples) {
    std::vector<float> result(num_samples);
    for (size_t i = 0; i < num_samples; ++i) {
        result[i] = static_cast<float>(samples[i]) / 32768.0f;
    }
    return result;
}

// SimpleFbankExtractor Implementation
SimpleFbankExtractor::SimpleFbankExtractor(const Config& config) : config_(config) {}

bool SimpleFbankExtractor::initialize(const Config& config) {
    config_ = config;
    
    simple_fbank::FbankComputer::Options opts;
    opts.sample_rate = config_.sample_rate;
    opts.frame_length_ms = config_.frame_length_ms;
    opts.frame_shift_ms = config_.frame_shift_ms;
    opts.num_mel_bins = config_.num_mel_bins;
    
    fbank_ = std::make_unique<simple_fbank::FbankComputer>(opts);
    
    return true;
}

std::vector<std::vector<float>> SimpleFbankExtractor::computeFeatures(const std::vector<float>& audio) {
    return fbank_->computeFeatures(audio);
}

std::vector<std::vector<float>> SimpleFbankExtractor::computeFeatures(const int16_t* samples, size_t num_samples) {
    auto audio_float = convertInt16ToFloat(samples, num_samples);
    return computeFeatures(audio_float);
}

std::vector<float> SimpleFbankExtractor::convertInt16ToFloat(const int16_t* samples, size_t num_samples) {
    std::vector<float> result(num_samples);
    for (size_t i = 0; i < num_samples; ++i) {
        result[i] = static_cast<float>(samples[i]) / 32768.0f;
    }
    return result;
}

// Factory functions
std::unique_ptr<FeatureExtractor> createSimpleFbank(const FeatureExtractor::Config& config) {
    auto extractor = std::make_unique<SimpleFbankExtractor>(config);
    if (extractor->initialize(config)) {
        return extractor;
    }
    return nullptr;
}

std::unique_ptr<FeatureExtractor> createKaldifeat(const FeatureExtractor::Config& config) {
    auto extractor = std::make_unique<KaldifeatExtractor>(config);
    if (extractor->initialize(config)) {
        return extractor;
    }
    return nullptr;
}

} // namespace onnx_stt