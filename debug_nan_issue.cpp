#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>

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
    std::cout << "=== Debug NaN Issue ===" << std::endl;
    
    // Load one frame of audio
    auto audio = loadBinaryFile("reference_data/audio_raw.bin");
    
    // Extract just first frame
    const int win_length = 400;
    std::vector<float> frame(win_length);
    
    for (int i = 0; i < win_length && i < audio.size(); i++) {
        frame[i] = audio[i];
    }
    
    // Apply Hann window
    std::cout << "\nApplying Hann window..." << std::endl;
    for (int n = 0; n < win_length; n++) {
        float window_val = 0.5f - 0.5f * cos(2.0f * M_PI * n / (win_length - 1));
        frame[n] *= window_val;
    }
    
    // Check for NaN in windowed frame
    bool has_nan = false;
    for (float val : frame) {
        if (std::isnan(val)) {
            has_nan = true;
            break;
        }
    }
    std::cout << "Frame has NaN after windowing: " << (has_nan ? "YES" : "NO") << std::endl;
    
    // Simple FFT test on first few bins
    const int n_fft = 512;
    std::vector<float> padded_frame(n_fft, 0.0f);
    
    // Center the frame in FFT window
    int start = (n_fft - win_length) / 2;
    for (int i = 0; i < win_length; i++) {
        padded_frame[start + i] = frame[i];
    }
    
    // Compute just first bin of FFT
    float real = 0.0f, imag = 0.0f;
    for (int n = 0; n < n_fft; n++) {
        real += padded_frame[n];  // k=0, so no complex exponential needed
    }
    
    float power = real * real + imag * imag;
    std::cout << "\nFirst FFT bin:" << std::endl;
    std::cout << "Real: " << real << ", Imag: " << imag << std::endl;
    std::cout << "Power: " << power << std::endl;
    
    // Test log of small values
    std::cout << "\nTesting log of small values:" << std::endl;
    std::cout << "log(0 + 1e-10) = " << std::log(0.0f + 1e-10f) << std::endl;
    std::cout << "log(1e-10) = " << std::log(1e-10f) << std::endl;
    std::cout << "log(1e-20) = " << std::log(1e-20f) << std::endl;
    
    // The issue might be with mel filterbank
    // If filter_output is 0 and we do log(0 + 1e-10), we get a very negative number
    // But if the filter has no overlap with spectrum, we might get exactly 0
    
    return 0;
}