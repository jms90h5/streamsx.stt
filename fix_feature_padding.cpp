#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

// Fixed feature extractor that implements librosa's center=True padding
class FixedFeatureExtractor {
public:
    std::vector<float> extractMelSpectrogramWithPadding(
        const std::vector<float>& audio_data,
        int sample_rate = 16000) {
        
        // Parameters matching Python/librosa exactly
        const int n_fft = 512;
        const int hop_length = 160;
        const int win_length = 400;
        const int n_mels = 80;
        
        // Apply center padding like librosa does
        int pad_length = n_fft / 2;
        std::vector<float> padded_audio;
        
        // Pad with zeros at the beginning
        for (int i = 0; i < pad_length; i++) {
            padded_audio.push_back(0.0f);
        }
        
        // Copy original audio
        for (float sample : audio_data) {
            padded_audio.push_back(sample);
        }
        
        // Pad with zeros at the end
        for (int i = 0; i < pad_length; i++) {
            padded_audio.push_back(0.0f);
        }
        
        std::cout << "Original audio: " << audio_data.size() << " samples" << std::endl;
        std::cout << "Padded audio: " << padded_audio.size() << " samples" << std::endl;
        
        // Calculate number of frames (should match librosa)
        int num_frames = 1 + (padded_audio.size() - win_length) / hop_length;
        std::cout << "Expected frames: " << num_frames << std::endl;
        
        // For now, just return dummy data of the right size
        // In real implementation, would do STFT, mel filterbank, etc.
        std::vector<float> mel_features(n_mels * num_frames, 0.0f);
        
        return mel_features;
    }
};

int main() {
    // Test with the same audio length as Python
    std::vector<float> test_audio(139680, 0.0f);
    
    FixedFeatureExtractor extractor;
    auto features = extractor.extractMelSpectrogramWithPadding(test_audio);
    
    std::cout << "\nExtracted " << features.size() << " features" << std::endl;
    std::cout << "This should be 80 * 874 = " << (80 * 874) << std::endl;
    
    return 0;
}