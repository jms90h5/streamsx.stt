#ifndef PROVEN_FEATURE_EXTRACTOR_HPP
#define PROVEN_FEATURE_EXTRACTOR_HPP

#include <vector>
#include <cmath>
#include <cstdlib>

/**
 * Simple feature extractor for the proven NeMo implementation
 * Implements mel spectrogram extraction to match NeMo's preprocessing
 */
class ProvenFeatureExtractor {
public:
    ProvenFeatureExtractor();
    ~ProvenFeatureExtractor() = default;

    // Extract mel spectrogram features matching NeMo's approach
    std::vector<float> extractMelSpectrogram(
        const std::vector<float>& audio_data,
        int sample_rate,
        int n_mels,
        int frame_length,
        int frame_shift
    );

private:
    // Librosa-based helper functions
    std::vector<float> extractSTFTFrame(const std::vector<float>& audio_data, 
                                       int center, int n_fft, int win_length);
    std::vector<float> applyWindow(const std::vector<float>& frame, int win_length);
    std::vector<float> computeLibrosaFFT(const std::vector<float>& windowed_frame);
    std::vector<std::vector<float>> createMelFilterbank(int n_mels, int n_fft, int sample_rate);
    std::vector<float> applyMelFilterbank(const std::vector<float>& power_spectrum,
                                         const std::vector<std::vector<float>>& mel_basis);
    
    // Legacy helper functions (for compatibility)
    std::vector<float> applyWindow(const std::vector<float>& frame);
    std::vector<float> computeFFT(const std::vector<float>& windowed_frame);
    std::vector<float> computeMelFilterbank(const std::vector<float>& power_spectrum, 
                                           int n_mels, int sample_rate);
    
    // Mel scale conversion
    float hzToMel(float hz);
    float melToHz(float mel);
    
    // Constants
    static constexpr float PI = 3.14159265359f;
    static constexpr float MEL_BREAK_FREQUENCY_HERTZ = 700.0f;
    static constexpr float MEL_HIGH_FREQUENCY_Q = 1127.0f;
};

#endif // PROVEN_FEATURE_EXTRACTOR_HPP