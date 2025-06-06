#include <iostream>
#include <vector>
#include <cmath>
#include <complex>
#include <fstream>

// Simple but correct FFT using std::complex
std::vector<float> computePowerSpectrum(const std::vector<float>& signal, int n_fft) {
    // Zero-pad to n_fft
    std::vector<std::complex<float>> fft_input(n_fft, 0);
    for (size_t i = 0; i < signal.size() && i < n_fft; i++) {
        fft_input[i] = signal[i];
    }
    
    // Simple DFT (slow but correct)
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
    
    // Power spectrum (only positive frequencies)
    std::vector<float> power_spectrum(n_fft / 2 + 1);
    for (int i = 0; i <= n_fft / 2; i++) {
        float real = fft_output[i].real();
        float imag = fft_output[i].imag();
        power_spectrum[i] = real * real + imag * imag;
    }
    
    return power_spectrum;
}

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
    std::cout << "=== Test Simple FFT ===" << std::endl;
    
    // Load audio
    auto audio = loadBinaryFile("reference_data/audio_raw.bin");
    
    // Extract first frame
    const int win_length = 400;
    const int n_fft = 512;
    std::vector<float> frame(n_fft, 0.0f);
    
    // Center frame in FFT window
    int start = (n_fft - win_length) / 2;
    for (int i = 0; i < win_length && i < audio.size(); i++) {
        frame[start + i] = audio[i];
    }
    
    // Apply Hann window
    for (int i = 0; i < win_length; i++) {
        float window_val = 0.5f - 0.5f * cos(2.0f * M_PI * i / (win_length - 1));
        frame[start + i] *= window_val;
    }
    
    // Compute power spectrum
    auto power_spec = computePowerSpectrum(frame, n_fft);
    
    std::cout << "Power spectrum size: " << power_spec.size() << std::endl;
    std::cout << "First 10 bins: ";
    for (int i = 0; i < 10; i++) {
        std::cout << power_spec[i] << " ";
    }
    std::cout << std::endl;
    
    // Check for negative values
    int neg_count = 0;
    float min_val = power_spec[0];
    for (float val : power_spec) {
        if (val < 0) neg_count++;
        if (val < min_val) min_val = val;
    }
    
    std::cout << "Negative values: " << neg_count << std::endl;
    std::cout << "Min value: " << min_val << std::endl;
    
    return 0;
}