#include "ProvenFeatureExtractor.hpp"
#include <algorithm>
#include <complex>
#include <iostream>
#include <cmath>

ProvenFeatureExtractor::ProvenFeatureExtractor() {
}

std::vector<float> ProvenFeatureExtractor::extractMelSpectrogram(
    const std::vector<float>& audio_data,
    int sample_rate,
    int n_mels,
    int frame_length,
    int frame_shift) {
    
    if (audio_data.empty()) {
        std::cerr << "Error: Empty audio data" << std::endl;
        return {};
    }
    
    // Apply dithering like NeMo (dither: 1e-05)
    std::vector<float> dithered_audio = audio_data;
    const float dither_value = 1e-05f;
    for (size_t i = 0; i < dithered_audio.size(); i++) {
        // Add small random noise for dithering
        float noise = ((rand() / float(RAND_MAX)) - 0.5f) * 2.0f * dither_value;
        dithered_audio[i] += noise;
    }
    
    try {
        // Calculate number of frames using dithered audio
        int num_frames = std::max(0, static_cast<int>((dithered_audio.size() - frame_length) / frame_shift) + 1);
        
        if (num_frames <= 0) {
            std::cerr << "Error: Not enough audio data for feature extraction" << std::endl;
            return {};
        }
        
        std::vector<float> mel_features;
        mel_features.reserve(num_frames * n_mels);
        
        // Process each frame using dithered audio
        for (int frame_idx = 0; frame_idx < num_frames; frame_idx++) {
            int start_sample = frame_idx * frame_shift;
            int end_sample = std::min(start_sample + frame_length, static_cast<int>(dithered_audio.size()));
            
            // Extract frame from dithered audio
            std::vector<float> frame(dithered_audio.begin() + start_sample, 
                                   dithered_audio.begin() + end_sample);
            
            // Pad frame if necessary
            while (frame.size() < static_cast<size_t>(frame_length)) {
                frame.push_back(0.0f);
            }
            
            // Apply window function
            auto windowed_frame = applyWindow(frame);
            
            // Compute FFT
            auto power_spectrum = computeFFT(windowed_frame);
            
            // Apply mel filterbank
            auto mel_frame = computeMelFilterbank(power_spectrum, n_mels, sample_rate);
            
            // Add to output
            mel_features.insert(mel_features.end(), mel_frame.begin(), mel_frame.end());
        }
        
        std::cout << "✓ Extracted " << num_frames << " frames with " << n_mels 
                  << " mel bins each" << std::endl;
        
        return mel_features;
        
    } catch (const std::exception& e) {
        std::cerr << "Error in mel spectrogram extraction: " << e.what() << std::endl;
        return {};
    }
}

std::vector<float> ProvenFeatureExtractor::applyWindow(const std::vector<float>& frame) {
    std::vector<float> windowed(frame.size());
    
    // Apply Hann window (matching NeMo)
    for (size_t i = 0; i < frame.size(); i++) {
        float window_val = 0.5f * (1.0f - std::cos(2.0f * PI * i / (frame.size() - 1)));
        windowed[i] = frame[i] * window_val;
    }
    
    return windowed;
}

std::vector<float> ProvenFeatureExtractor::computeFFT(const std::vector<float>& windowed_frame) {
    // Use NeMo's exact FFT parameters: n_fft=512
    const int N_FFT = 512;
    int fft_size = N_FFT / 2 + 1;  // Only need positive frequencies
    
    // Pad or truncate frame to match n_fft size
    std::vector<float> padded_frame(N_FFT, 0.0f);
    size_t copy_size = std::min(windowed_frame.size(), static_cast<size_t>(N_FFT));
    std::copy(windowed_frame.begin(), windowed_frame.begin() + copy_size, padded_frame.begin());
    
    std::vector<float> power_spectrum(fft_size);
    
    // Simplified DFT for power spectrum using n_fft=512
    for (int k = 0; k < fft_size; k++) {
        std::complex<float> sum(0.0f, 0.0f);
        
        for (int n = 0; n < N_FFT; n++) {
            float angle = -2.0f * PI * k * n / N_FFT;
            std::complex<float> exp_term(std::cos(angle), std::sin(angle));
            sum += padded_frame[n] * exp_term;
        }
        
        // FIXED: Correct power spectrum calculation
        // Power = |X(k)|^2 (not divided by N_FFT)
        power_spectrum[k] = std::norm(sum);
    }
    
    return power_spectrum;
}

std::vector<float> ProvenFeatureExtractor::computeMelFilterbank(
    const std::vector<float>& power_spectrum, 
    int n_mels, 
    int sample_rate) {
    
    std::vector<float> mel_features(n_mels);
    
    // FIXED: Match NeMo's frequency range (0 Hz to Nyquist)
    float low_freq_mel = hzToMel(0.0f);  // Start at 0 Hz like NeMo
    float high_freq_mel = hzToMel(sample_rate / 2.0f);  // Nyquist frequency
    
    // Create mel filter bank
    std::vector<float> mel_points(n_mels + 2);
    for (int i = 0; i < n_mels + 2; i++) {
        float mel_freq = low_freq_mel + (high_freq_mel - low_freq_mel) * i / (n_mels + 1);
        mel_points[i] = melToHz(mel_freq);
    }
    
    // Convert to FFT bin indices using n_fft=512
    const int N_FFT = 512;
    std::vector<int> bin_indices(n_mels + 2);
    for (int i = 0; i < n_mels + 2; i++) {
        bin_indices[i] = static_cast<int>(std::round(mel_points[i] * N_FFT / sample_rate));
        bin_indices[i] = std::min(bin_indices[i], static_cast<int>(power_spectrum.size()) - 1);
    }
    
    // Apply triangular filters
    for (int m = 0; m < n_mels; m++) {
        float filter_output = 0.0f;
        
        int left_bin = bin_indices[m];
        int center_bin = bin_indices[m + 1];
        int right_bin = bin_indices[m + 2];
        
        // Left side of triangle
        for (int bin = left_bin; bin <= center_bin; bin++) {
            if (bin < static_cast<int>(power_spectrum.size()) && center_bin != left_bin) {
                float weight = static_cast<float>(bin - left_bin) / (center_bin - left_bin);
                filter_output += power_spectrum[bin] * weight;
            }
        }
        
        // Right side of triangle  
        for (int bin = center_bin + 1; bin <= right_bin; bin++) {
            if (bin < static_cast<int>(power_spectrum.size()) && right_bin != center_bin) {
                float weight = static_cast<float>(right_bin - bin) / (right_bin - center_bin);
                filter_output += power_spectrum[bin] * weight;
            }
        }
        
        // FIXED: Apply log transform correctly
        // Add small epsilon to avoid log(0)
        mel_features[m] = std::log(filter_output + 1e-10f);
    }
    
    return mel_features;
}

float ProvenFeatureExtractor::hzToMel(float hz) {
    // Standard mel scale conversion
    return MEL_HIGH_FREQUENCY_Q * std::log(1.0f + hz / MEL_BREAK_FREQUENCY_HERTZ);
}

float ProvenFeatureExtractor::melToHz(float mel) {
    // Standard mel scale conversion
    return MEL_BREAK_FREQUENCY_HERTZ * (std::exp(mel / MEL_HIGH_FREQUENCY_Q) - 1.0f);
}