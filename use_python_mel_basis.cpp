#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>
#include <complex>

// Load binary file
std::vector<float> loadBinaryFile(const std::string& filename, size_t expected_size = 0) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename);
    }
    
    file.seekg(0, std::ios::end);
    size_t file_size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    size_t num_floats = file_size / sizeof(float);
    if (expected_size > 0 && num_floats != expected_size) {
        std::cerr << "Warning: Expected " << expected_size << " floats, got " << num_floats << std::endl;
    }
    
    std::vector<float> data(num_floats);
    file.read(reinterpret_cast<char*>(data.data()), file_size);
    
    return data;
}

// Compute power spectrum using std::complex for correctness
std::vector<float> computePowerSpectrum(const std::vector<float>& signal) {
    const int n_fft = 512;
    std::vector<std::complex<float>> fft_input(n_fft, 0);
    
    // Copy signal to complex array
    for (size_t i = 0; i < signal.size() && i < n_fft; i++) {
        fft_input[i] = signal[i];
    }
    
    // DFT
    std::vector<std::complex<float>> fft_output(n_fft);
    for (int k = 0; k < n_fft; k++) {
        std::complex<float> sum(0, 0);
        for (int n = 0; n < n_fft; n++) {
            float angle = -2.0f * M_PI * k * n / n_fft;
            std::complex<float> w(cos(angle), sin(angle));
            sum += fft_input[n] * w;
        }
        fft_output[k] = sum;
    }
    
    // Power spectrum
    std::vector<float> power(n_fft / 2 + 1);
    for (int i = 0; i <= n_fft / 2; i++) {
        power[i] = std::norm(fft_output[i]);  // |z|^2
    }
    
    return power;
}

int main() {
    std::cout << "=== Test with Python Mel Basis ===" << std::endl;
    
    try {
        // Load Python's mel filterbank
        auto mel_basis_flat = loadBinaryFile("reference_data/mel_basis_librosa.bin", 80 * 257);
        
        // Reshape to 2D (80 x 257)
        std::vector<std::vector<float>> mel_basis(80, std::vector<float>(257));
        for (int i = 0; i < 80; i++) {
            for (int j = 0; j < 257; j++) {
                mel_basis[i][j] = mel_basis_flat[i * 257 + j];
            }
        }
        
        std::cout << "Loaded mel basis: 80 x 257" << std::endl;
        
        // Load audio
        auto audio = loadBinaryFile("reference_data/audio_raw.bin");
        std::cout << "Loaded audio: " << audio.size() << " samples" << std::endl;
        
        // Process first frame
        const int win_length = 400;
        const int n_fft = 512;
        const int pad_length = n_fft / 2;
        
        // Apply padding like librosa
        std::vector<float> padded_audio;
        for (int i = 0; i < pad_length; i++) {
            padded_audio.push_back(0.0f);
        }
        for (float s : audio) {
            padded_audio.push_back(s);
        }
        for (int i = 0; i < pad_length; i++) {
            padded_audio.push_back(0.0f);
        }
        
        // Extract first frame (centered)
        std::vector<float> frame(n_fft, 0.0f);
        int start = (n_fft - win_length) / 2;
        for (int i = 0; i < win_length; i++) {
            frame[start + i] = padded_audio[i];
        }
        
        // Apply Hann window
        auto hann_window = loadBinaryFile("reference_data/hann_window_400.bin", 400);
        for (int i = 0; i < win_length; i++) {
            frame[start + i] *= hann_window[i];
        }
        
        // Compute power spectrum
        auto power_spec = computePowerSpectrum(frame);
        std::cout << "Power spectrum computed: " << power_spec.size() << " bins" << std::endl;
        
        // Apply mel filterbank
        std::vector<float> mel_energies(80);
        for (int m = 0; m < 80; m++) {
            float energy = 0.0f;
            for (int b = 0; b < 257; b++) {
                energy += mel_basis[m][b] * power_spec[b];
            }
            mel_energies[m] = energy;
        }
        
        // Apply log
        std::vector<float> log_mel(80);
        for (int i = 0; i < 80; i++) {
            log_mel[i] = std::log(mel_energies[i] + 1e-10f);
        }
        
        // Compare with Python
        auto python_features = loadBinaryFile("reference_data/log_mel_spectrogram.bin");
        
        std::cout << "\nFirst frame comparison:" << std::endl;
        std::cout << "C++ first 5 values: ";
        for (int i = 0; i < 5; i++) {
            std::cout << log_mel[i] << " ";
        }
        std::cout << std::endl;
        
        std::cout << "Python first 5 values: ";
        for (int i = 0; i < 5; i++) {
            std::cout << python_features[i] << " ";
        }
        std::cout << std::endl;
        
        // Calculate difference
        float max_diff = 0;
        for (int i = 0; i < 80; i++) {
            float diff = std::abs(log_mel[i] - python_features[i]);
            if (diff > max_diff) max_diff = diff;
        }
        std::cout << "Max difference in first frame: " << max_diff << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}