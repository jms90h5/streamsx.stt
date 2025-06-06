#include "ProvenFeatureExtractor.hpp"
#include <iostream>
#include <cstring>
#include <cstdlib>

ProvenFeatureExtractor::ProvenFeatureExtractor() {
    // Initialize random seed for dithering
    srand(42);
}

std::vector<float> ProvenFeatureExtractor::extractMelSpectrogram(
    const std::vector<float>& audio_data,
    int sample_rate,
    int n_mels,
    int frame_length,
    int frame_shift) {
    
    // Apply dithering like NeMo does
    const float dither_value = 0.00001f;
    std::vector<float> dithered_audio = audio_data;
    
    for (size_t i = 0; i < dithered_audio.size(); i++) {
        float noise = ((rand() / float(RAND_MAX)) - 0.5f) * 2.0f * dither_value;
        dithered_audio[i] += noise;
    }
    
    // Use exact librosa/NeMo parameters
    const int n_fft = 512;
    const int hop_length = 160;
    const int win_length = 400;
    
    // CRITICAL FIX: Apply center padding like librosa does
    const int pad_length = n_fft / 2;  // 256 samples
    std::vector<float> padded_audio;
    
    // Pad beginning with zeros
    for (int i = 0; i < pad_length; i++) {
        padded_audio.push_back(0.0f);
    }
    
    // Copy original audio
    for (float sample : dithered_audio) {
        padded_audio.push_back(sample);
    }
    
    // Pad end with zeros  
    for (int i = 0; i < pad_length; i++) {
        padded_audio.push_back(0.0f);
    }
    
    // Calculate number of frames (should now match librosa)
    int num_frames = 1 + (padded_audio.size() - win_length) / hop_length;
    
    std::cout << "Original audio: " << audio_data.size() << " samples" << std::endl;
    std::cout << "Padded audio: " << padded_audio.size() << " samples" << std::endl;
    std::cout << "Processing " << num_frames << " frames with librosa-based method" << std::endl;
    
    // Create mel filterbank once
    auto mel_basis = createMelFilterbank(n_mels, n_fft, sample_rate);
    
    // Process all frames
    std::vector<float> all_mel_features;
    all_mel_features.reserve(n_mels * num_frames);
    
    for (int frame_idx = 0; frame_idx < num_frames; frame_idx++) {
        int start_idx = frame_idx * hop_length;
        
        // Extract frame with proper windowing
        auto frame = extractSTFTFrame(padded_audio, start_idx, n_fft, win_length);
        
        // Apply window
        auto windowed_frame = applyWindow(frame, win_length);
        
        // Compute FFT
        auto fft_result = computeLibrosaFFT(windowed_frame);
        
        // Power spectrum
        std::vector<float> power_spectrum(n_fft / 2 + 1);
        for (int i = 0; i <= n_fft / 2; i++) {
            power_spectrum[i] = fft_result[i];
        }
        
        // Apply mel filterbank
        auto mel_frame = applyMelFilterbank(power_spectrum, mel_basis);
        
        // Append to output
        all_mel_features.insert(all_mel_features.end(), mel_frame.begin(), mel_frame.end());
    }
    
    std::cout << "✓ Extracted " << num_frames << " frames with " 
              << n_mels << " mel bins each (librosa-based)" << std::endl;
    
    return all_mel_features;
}

// Rest of the implementation remains the same...
std::vector<float> ProvenFeatureExtractor::extractSTFTFrame(
    const std::vector<float>& audio_data, int center, int n_fft, int win_length) {
    
    std::vector<float> frame(n_fft, 0.0f);
    
    int half_win = win_length / 2;
    int start = center - half_win;
    
    for (int i = 0; i < win_length; i++) {
        int audio_idx = start + i;
        if (audio_idx >= 0 && audio_idx < audio_data.size()) {
            int frame_idx = i + (n_fft - win_length) / 2;
            if (frame_idx >= 0 && frame_idx < n_fft) {
                frame[frame_idx] = audio_data[audio_idx];
            }
        }
    }
    
    return frame;
}

std::vector<float> ProvenFeatureExtractor::applyWindow(const std::vector<float>& frame, int win_length) {
    std::vector<float> windowed = frame;
    
    // Hann window
    for (int n = 0; n < win_length; n++) {
        float window_val = 0.5f - 0.5f * cos(2.0f * PI * n / (win_length - 1));
        int idx = n + (frame.size() - win_length) / 2;
        if (idx >= 0 && idx < windowed.size()) {
            windowed[idx] *= window_val;
        }
    }
    
    return windowed;
}

std::vector<float> ProvenFeatureExtractor::computeLibrosaFFT(const std::vector<float>& windowed_frame) {
    int n_fft = windowed_frame.size();
    std::vector<float> power_spectrum(n_fft / 2 + 1);
    
    // Simple DFT for now - in production use FFTW or similar
    for (int k = 0; k <= n_fft / 2; k++) {
        float real = 0.0f, imag = 0.0f;
        
        for (int n = 0; n < n_fft; n++) {
            float angle = -2.0f * PI * k * n / n_fft;
            real += windowed_frame[n] * cos(angle);
            imag += windowed_frame[n] * sin(angle);
        }
        
        power_spectrum[k] = real * real + imag * imag;
    }
    
    return power_spectrum;
}

float ProvenFeatureExtractor::hzToMel(float hz) {
    return MEL_HIGH_FREQUENCY_Q * std::log(1.0f + hz / MEL_BREAK_FREQUENCY_HERTZ);
}

float ProvenFeatureExtractor::melToHz(float mel) {
    return MEL_BREAK_FREQUENCY_HERTZ * (std::exp(mel / MEL_HIGH_FREQUENCY_Q) - 1.0f);
}

std::vector<std::vector<float>> ProvenFeatureExtractor::createMelFilterbank(int n_mels, int n_fft, int sample_rate) {
    int fft_bins = n_fft / 2 + 1;
    std::vector<std::vector<float>> mel_basis(n_mels, std::vector<float>(fft_bins, 0.0f));
    
    float fmin = 0.0f;
    float fmax = 8000.0f;
    
    float min_mel = hzToMel(fmin);
    float max_mel = hzToMel(fmax);
    
    std::vector<float> mel_points(n_mels + 2);
    for (int i = 0; i < n_mels + 2; i++) {
        mel_points[i] = min_mel + i * (max_mel - min_mel) / (n_mels + 1);
    }
    
    std::vector<float> hz_points(n_mels + 2);
    for (int i = 0; i < n_mels + 2; i++) {
        hz_points[i] = melToHz(mel_points[i]);
    }
    
    std::vector<int> bin_points(n_mels + 2);
    for (int i = 0; i < n_mels + 2; i++) {
        bin_points[i] = static_cast<int>((n_fft + 1) * hz_points[i] / sample_rate);
    }
    
    for (int m = 0; m < n_mels; m++) {
        int left = bin_points[m];
        int center = bin_points[m + 1];
        int right = bin_points[m + 2];
        
        for (int bin = left; bin < center && bin < fft_bins; bin++) {
            mel_basis[m][bin] = (float)(bin - left) / (center - left);
        }
        
        for (int bin = center; bin < right && bin < fft_bins; bin++) {
            mel_basis[m][bin] = (float)(right - bin) / (right - center);
        }
        
        // Normalize filter
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
    
    for (int m = 0; m < n_mels; m++) {
        float filter_output = 0.0f;
        
        for (size_t bin = 0; bin < power_spectrum.size() && bin < mel_basis[m].size(); bin++) {
            filter_output += mel_basis[m][bin] * power_spectrum[bin];
        }
        
        // Apply log transform
        mel_features[m] = std::log(filter_output + 1e-10f);
    }
    
    return mel_features;
}

// Legacy stubs
std::vector<float> ProvenFeatureExtractor::applyWindow(const std::vector<float>& frame) {
    return applyWindow(frame, frame.size());
}

std::vector<float> ProvenFeatureExtractor::computeFFT(const std::vector<float>& windowed_frame) {
    return computeLibrosaFFT(windowed_frame);
}

std::vector<float> ProvenFeatureExtractor::computeMelFilterbank(
    const std::vector<float>& power_spectrum, 
    int n_mels, 
    int sample_rate) {
    
    auto mel_basis = createMelFilterbank(n_mels, power_spectrum.size() * 2 - 2, sample_rate);
    return applyMelFilterbank(power_spectrum, mel_basis);
}