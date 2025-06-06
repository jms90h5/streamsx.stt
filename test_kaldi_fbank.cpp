#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>
#include "kaldifeat/feature-fbank.h"
#include "kaldifeat/feature-mfcc.h"
#include "kaldifeat/feature-window.h"

// Load binary file
std::vector<float> loadBinaryFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename);
    }
    
    file.seekg(0, std::ios::end);
    size_t file_size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    size_t num_floats = file_size / sizeof(float);
    std::vector<float> data(num_floats);
    file.read(reinterpret_cast<char*>(data.data()), file_size);
    
    return data;
}

int main() {
    std::cout << "=== Testing Kaldi Fbank Feature Extraction ===" << std::endl;
    
    try {
        // Load test audio
        auto audio = loadBinaryFile("reference_data/audio_raw.bin");
        std::cout << "Loaded " << audio.size() << " audio samples" << std::endl;
        
        // Set up Kaldi Fbank options to match librosa
        kaldifeat::FbankOptions opts;
        opts.frame_opts.samp_freq = 16000;
        opts.frame_opts.frame_length_ms = 25.0;  // 400 samples at 16kHz
        opts.frame_opts.frame_shift_ms = 10.0;   // 160 samples at 16kHz
        opts.mel_opts.num_bins = 80;
        opts.mel_opts.low_freq = 0.0;
        opts.mel_opts.high_freq = 8000.0;
        opts.frame_opts.dither = 0.0;  // No dithering for exact comparison
        opts.frame_opts.preemph_coeff = 0.0;  // No pre-emphasis
        opts.frame_opts.window_type = "hanning";
        opts.use_energy = false;
        opts.use_log_fbank = true;  // Get log mel features directly
        
        // Create feature extractor
        kaldifeat::Fbank fbank(opts);
        
        // Extract features
        // Kaldi expects data in Matrix format
        int32_t num_samples = audio.size();
        kaldi::Matrix<kaldi::BaseFloat> wave_data(1, num_samples);
        
        for (int i = 0; i < num_samples; i++) {
            wave_data(0, i) = audio[i];
        }
        
        // Compute features
        kaldi::Matrix<kaldi::BaseFloat> features;
        try {
            fbank.ComputeFeatures(wave_data, opts.frame_opts.samp_freq, 1.0, &features);
        } catch (const std::exception& e) {
            std::cerr << "Feature extraction failed: " << e.what() << std::endl;
            return 1;
        }
        
        std::cout << "\nKaldi extracted features: " << features.NumRows() 
                  << " frames x " << features.NumCols() << " dims" << std::endl;
        
        // Print first few features
        std::cout << "\nFirst 5 features from first frame:" << std::endl;
        for (int i = 0; i < 5 && i < features.NumCols(); i++) {
            std::cout << features(0, i) << " ";
        }
        std::cout << std::endl;
        
        // Compare with Python reference
        auto ref_features = loadBinaryFile("reference_data/log_mel_spectrogram.bin");
        std::cout << "\nPython features: " << ref_features.size() << " values" << std::endl;
        std::cout << "Expected: 80 x 874 = " << (80 * 874) << std::endl;
        
        // Print first few Python features for comparison
        std::cout << "\nFirst 5 Python features: ";
        for (int i = 0; i < 5; i++) {
            std::cout << ref_features[i] << " ";
        }
        std::cout << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}