#include <iostream>
#include <fstream>
#include <vector>
#include <onnxruntime_cxx_api.h>
#include <unordered_map>

// Simple direct inference test bypassing feature extraction

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

std::unordered_map<int, std::string> loadVocabulary(const std::string& path) {
    std::unordered_map<int, std::string> vocab;
    std::ifstream file(path);
    std::string line;
    
    while (std::getline(file, line)) {
        size_t space_pos = line.find(' ');
        if (space_pos != std::string::npos) {
            std::string token = line.substr(0, space_pos);
            int id = std::stoi(line.substr(space_pos + 1));
            vocab[id] = token;
        }
    }
    
    return vocab;
}

int main() {
    std::cout << "=== Direct Inference Test with Python Features ===" << std::endl;
    
    try {
        // Initialize ONNX Runtime
        Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "DirectTest");
        Ort::SessionOptions session_options;
        session_options.SetIntraOpNumThreads(1);
        
        // Load model
        std::cout << "Loading model..." << std::endl;
        Ort::Session session(env, "models/fastconformer_ctc_export/model.onnx", session_options);
        
        // Load Python features (80, 874)
        std::cout << "Loading Python features..." << std::endl;
        auto features = loadBinaryFile("reference_data/log_mel_spectrogram.bin");
        
        int n_mels = 80;
        int n_frames = 874;
        
        if (features.size() != n_mels * n_frames) {
            std::cerr << "Feature size mismatch! Expected " << n_mels * n_frames 
                      << ", got " << features.size() << std::endl;
            return 1;
        }
        
        std::cout << "Features loaded: " << features.size() << " values" << std::endl;
        std::cout << "First 5 features: ";
        for (int i = 0; i < 5; i++) {
            std::cout << features[i] << " ";
        }
        std::cout << std::endl;
        
        // Create input tensors
        Ort::MemoryInfo memory_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
        
        std::vector<int64_t> audio_shape = {1, n_mels, n_frames};
        auto audio_tensor = Ort::Value::CreateTensor<float>(
            memory_info, features.data(), features.size(),
            audio_shape.data(), audio_shape.size()
        );
        
        std::vector<int64_t> length_data = {n_frames};
        std::vector<int64_t> length_shape = {1};
        auto length_tensor = Ort::Value::CreateTensor<int64_t>(
            memory_info, length_data.data(), length_data.size(),
            length_shape.data(), length_shape.size()
        );
        
        // Run inference
        std::cout << "\nRunning inference..." << std::endl;
        
        std::vector<const char*> input_names = {"audio_signal", "length"};
        std::vector<const char*> output_names = {"logprobs"};
        std::vector<Ort::Value> inputs;
        inputs.push_back(std::move(audio_tensor));
        inputs.push_back(std::move(length_tensor));
        
        auto outputs = session.Run(Ort::RunOptions{nullptr}, 
                                  input_names.data(), inputs.data(), inputs.size(),
                                  output_names.data(), output_names.size());
        
        // Get output
        float* logits = outputs[0].GetTensorMutableData<float>();
        auto logits_shape = outputs[0].GetTensorTypeAndShapeInfo().GetShape();
        
        std::cout << "Output shape: [" << logits_shape[0] << ", " 
                  << logits_shape[1] << ", " << logits_shape[2] << "]" << std::endl;
        
        // Decode
        std::cout << "\nDecoding..." << std::endl;
        auto vocab = loadVocabulary("models/fastconformer_ctc_export/tokens.txt");
        int blank_id = vocab.size() - 1;  // Usually the last token
        
        std::vector<int> predictions;
        int vocab_size = logits_shape[2];
        int time_steps = logits_shape[1];
        
        // First 5 predictions
        std::cout << "First 5 frame predictions: ";
        for (int t = 0; t < 5 && t < time_steps; t++) {
            int max_idx = 0;
            float max_val = logits[t * vocab_size];
            
            for (int v = 1; v < vocab_size; v++) {
                if (logits[t * vocab_size + v] > max_val) {
                    max_val = logits[t * vocab_size + v];
                    max_idx = v;
                }
            }
            
            predictions.push_back(max_idx);
            std::cout << max_idx << " ";
        }
        std::cout << std::endl;
        
        // Full decode
        int prev_token = -1;
        std::string result;
        
        for (int t = 0; t < time_steps; t++) {
            int max_idx = 0;
            float max_val = logits[t * vocab_size];
            
            for (int v = 1; v < vocab_size; v++) {
                if (logits[t * vocab_size + v] > max_val) {
                    max_val = logits[t * vocab_size + v];
                    max_idx = v;
                }
            }
            
            if (max_idx != prev_token && max_idx != blank_id) {
                if (vocab.find(max_idx) != vocab.end()) {
                    std::string token = vocab[max_idx];
                    if (token.length() >= 3 && 
                        (unsigned char)token[0] == 0xE2 && 
                        (unsigned char)token[1] == 0x96 && 
                        (unsigned char)token[2] == 0x81) {
                        result += " " + token.substr(3);
                    } else {
                        result += token;
                    }
                }
            }
            
            prev_token = max_idx;
        }
        
        // Trim
        if (!result.empty() && result[0] == ' ') {
            result = result.substr(1);
        }
        
        std::cout << "\nFinal transcription: '" << result << "'" << std::endl;
        std::cout << "\nExpected: 'it was the first great sorrow of his life...'" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}