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
    srand(42); // Fixed seed for reproducibility
    for (size_t i = 0; i < dithered_audio.size(); i++) {
        // Add small random noise for dithering
        float noise = ((rand() / float(RAND_MAX)) - 0.5f) * 2.0f * dither_value;
        dithered_audio[i] += noise;
    }
    
    try {
        // Use exact librosa/NeMo parameters
        const int n_fft = 512;
        const int hop_length = 160;  // 10ms at 16kHz
        const int win_length = 400;  // 25ms at 16kHz
        
        // Calculate number of frames using librosa's method
        int num_frames = 1 + (dithered_audio.size() - n_fft) / hop_length;
        
        if (num_frames <= 0) {
            std::cerr << "Error: Not enough audio data for feature extraction" << std::endl;
            return {};
        }
        
        std::cout << "Processing " << num_frames << " frames with librosa-based method" << std::endl;
        
        // Create mel filterbank (same as librosa)
        auto mel_basis = createMelFilterbank(n_mels, n_fft, sample_rate);
        
        std::vector<float> mel_features;
        mel_features.reserve(num_frames * n_mels);
        
        // Process each frame using librosa's STFT approach
        for (int frame_idx = 0; frame_idx < num_frames; frame_idx++) {
            int start_sample = frame_idx * hop_length;
            
            // Extract frame with proper centering (like librosa)
            std::vector<float> frame = extractSTFTFrame(dithered_audio, start_sample, n_fft, win_length);
            
            // Apply window function
            auto windowed_frame = applyWindow(frame, win_length);
            
            // Compute FFT and power spectrum
            auto power_spectrum = computeLibrosaFFT(windowed_frame);
            
            // Apply mel filterbank
            auto mel_frame = applyMelFilterbank(power_spectrum, mel_basis);
            
            // Add to output
            mel_features.insert(mel_features.end(), mel_frame.begin(), mel_frame.end());
        }
        
        std::cout << "✓ Extracted " << num_frames << " frames with " << n_mels 
                  << " mel bins each (librosa-based)" << std::endl;
        
        return mel_features;
        
    } catch (const std::exception& e) {
        std::cerr << "Error in mel spectrogram extraction: " << e.what() << std::endl;
        return {};
    }
}

std::vector<float> ProvenFeatureExtractor::extractSTFTFrame(
    const std::vector<float>& audio_data, 
    int center, 
    int n_fft, 
    int win_length) {
    
    // Librosa-style frame extraction with centering
    int half_frame = n_fft / 2;
    int start = center - half_frame;
    
    std::vector<float> frame(n_fft, 0.0f);
    
    // Copy audio data with padding
    for (int i = 0; i < n_fft; i++) {
        int audio_idx = start + i;
        if (audio_idx >= 0 && audio_idx < static_cast<int>(audio_data.size())) {
            frame[i] = audio_data[audio_idx];
        }
        // else frame[i] remains 0.0f (padding)
    }
    
    return frame;
}

std::vector<float> ProvenFeatureExtractor::applyWindow(
    const std::vector<float>& frame, 
    int win_length) {
    
    std::vector<float> windowed(frame.size(), 0.0f);
    
    // Apply Hann window only to win_length samples (center of frame)
    int start_idx = (frame.size() - win_length) / 2;
    
    for (int i = 0; i < win_length; i++) {
        int frame_idx = start_idx + i;
        if (frame_idx < static_cast<int>(frame.size())) {
            float window_val = 0.5f * (1.0f - std::cos(2.0f * PI * i / (win_length - 1)));
            windowed[frame_idx] = frame[frame_idx] * window_val;
        }
    }
    
    return windowed;
}

std::vector<float> ProvenFeatureExtractor::computeLibrosaFFT(const std::vector<float>& windowed_frame) {
    const int N_FFT = 512;
    int fft_size = N_FFT / 2 + 1;  // Only positive frequencies
    
    std::vector<float> power_spectrum(fft_size);
    
    // Compute DFT for positive frequencies only
    for (int k = 0; k < fft_size; k++) {
        std::complex<float> sum(0.0f, 0.0f);
        
        for (int n = 0; n < N_FFT; n++) {
            float angle = -2.0f * PI * k * n / N_FFT;
            std::complex<float> exp_term(std::cos(angle), std::sin(angle));
            sum += windowed_frame[n] * exp_term;
        }
        
        // Power spectrum = |X(k)|^2
        power_spectrum[k] = std::norm(sum);
    }
    
    return power_spectrum;
}

std::vector<std::vector<float>> ProvenFeatureExtractor::createMelFilterbank(
    int n_mels, 
    int n_fft, 
    int sample_rate) {
    
    // Create mel filterbank matrix (n_mels x fft_bins)
    int fft_bins = n_fft / 2 + 1;
    std::vector<std::vector<float>> mel_basis(n_mels, std::vector<float>(fft_bins, 0.0f));
    
    // Create mel-spaced frequency points
    float fmin = 0.0f;
    float fmax = sample_rate / 2.0f;
    float mel_fmin = hzToMel(fmin);
    float mel_fmax = hzToMel(fmax);
    
    std::vector<float> mel_points(n_mels + 2);
    for (int i = 0; i < n_mels + 2; i++) {
        float mel_freq = mel_fmin + (mel_fmax - mel_fmin) * i / (n_mels + 1);
        mel_points[i] = melToHz(mel_freq);
    }
    
    // Convert to FFT bin indices
    std::vector<int> fft_bins_list(n_mels + 2);
    for (int i = 0; i < n_mels + 2; i++) {
        fft_bins_list[i] = static_cast<int>(std::floor(mel_points[i] * n_fft / sample_rate + 0.5f));
        fft_bins_list[i] = std::min(fft_bins_list[i], fft_bins - 1);
    }
    
    // Create triangular filters
    for (int m = 0; m < n_mels; m++) {
        int left_bin = fft_bins_list[m];
        int center_bin = fft_bins_list[m + 1];
        int right_bin = fft_bins_list[m + 2];
        
        // Left slope
        for (int bin = left_bin; bin < center_bin; bin++) {
            if (bin < fft_bins && center_bin != left_bin) {
                mel_basis[m][bin] = static_cast<float>(bin - left_bin) / (center_bin - left_bin);
            }
        }
        
        // Right slope
        for (int bin = center_bin; bin < right_bin; bin++) {
            if (bin < fft_bins && right_bin != center_bin) {
                mel_basis[m][bin] = static_cast<float>(right_bin - bin) / (right_bin - center_bin);
            }
        }
        
        // Normalize filter (librosa does this)
        float filter_sum = 0.0f;
        for (int bin = 0; bin < fft_bins; bin++) {
            filter_sum += mel_basis[m][bin];
        }
        
        if (filter_sum > 0.0f) {
            for (int bin = 0; bin < fft_bins; bin++) {
                mel_basis[m][bin] /= filter_sum;
            }
        }
    }
    
    return mel_basis;
}

std::vector<float> ProvenFeatureExtractor::applyMelFilterbank(
    const std::vector<float>& power_spectrum,
    const std::vector<std::vector<float>>& mel_basis) {
    
    int n_mels = mel_basis.size();
    std::vector<float> mel_features(n_mels);
    
    // Apply mel filterbank: mel_spec = mel_basis @ power_spectrum
    for (int m = 0; m < n_mels; m++) {
        float filter_output = 0.0f;
        
        for (size_t bin = 0; bin < power_spectrum.size(); bin++) {
            if (bin < mel_basis[m].size()) {
                filter_output += mel_basis[m][bin] * power_spectrum[bin];
            }
        }
        
        // Apply log transform
        mel_features[m] = std::log(filter_output + 1e-10f);
    }
    
    return mel_features;
}

float ProvenFeatureExtractor::hzToMel(float hz) {
    return MEL_HIGH_FREQUENCY_Q * std::log(1.0f + hz / MEL_BREAK_FREQUENCY_HERTZ);
}

float ProvenFeatureExtractor::melToHz(float mel) {
    return MEL_BREAK_FREQUENCY_HERTZ * (std::exp(mel / MEL_HIGH_FREQUENCY_Q) - 1.0f);
}