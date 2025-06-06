#include <iostream>
#include <vector>
#include <fstream>
#include <onnxruntime_cxx_api.h>

int main() {
    try {
        std::cout << "=== Testing Simple ONNX Model ===" << std::endl;
        
        // Initialize ONNX Runtime
        Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "SimpleTest");
        Ort::SessionOptions session_options;
        
        // Load simple working model
        std::string model_path = "models/nemo_fastconformer_streaming/conformer_ctc_dynamic.onnx";
        Ort::Session session(env, model_path.c_str(), session_options);
        
        std::cout << "✓ Model loaded successfully" << std::endl;
        
        // Check inputs/outputs
        size_t num_inputs = session.GetInputCount();
        size_t num_outputs = session.GetOutputCount();
        
        std::cout << "Model has " << num_inputs << " inputs, " << num_outputs << " outputs" << std::endl;
        
        for (size_t i = 0; i < num_inputs; ++i) {
            auto input_name = session.GetInputNameAllocated(i, Ort::AllocatorWithDefaultOptions());
            auto input_shape = session.GetInputTypeInfo(i).GetTensorTypeAndShapeInfo().GetShape();
            std::cout << "Input " << i << ": " << input_name.get() << " shape: [";
            for (size_t j = 0; j < input_shape.size(); ++j) {
                std::cout << input_shape[j];
                if (j < input_shape.size() - 1) std::cout << ",";
            }
            std::cout << "]" << std::endl;
        }
        
        // Create simple test input
        Ort::MemoryInfo memory_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
        
        // Create dummy audio features [batch=1, time=160, features=80]
        // Need 160 frames because model subsamples by 4x: 160/4 = 40 expected
        std::vector<float> audio_features(1 * 160 * 80, 0.1f);  // Small constant values
        std::vector<int64_t> audio_shape = {1, 160, 80};
        
        std::vector<int64_t> lengths = {40};  // Sequence length
        std::vector<int64_t> length_shape = {1};
        
        // Create input tensors - only audio_signal needed
        std::vector<Ort::Value> inputs;
        inputs.push_back(Ort::Value::CreateTensor<float>(memory_info, audio_features.data(), 
                                                        audio_features.size(), 
                                                        audio_shape.data(), audio_shape.size()));
        
        // Prepare input/output names
        std::vector<const char*> input_names = {"audio_signal"};
        std::vector<const char*> output_names = {"log_probs"};
        
        std::cout << "Running inference..." << std::endl;
        
        // Run inference
        auto outputs = session.Run(Ort::RunOptions{nullptr}, input_names.data(), inputs.data(), 
                                  inputs.size(), output_names.data(), output_names.size());
        
        std::cout << "✓ Inference completed successfully" << std::endl;
        
        // Check output
        const float* output_data = outputs[0].GetTensorData<float>();
        auto output_shape = outputs[0].GetTensorTypeAndShapeInfo().GetShape();
        
        std::cout << "Output shape: [";
        for (size_t i = 0; i < output_shape.size(); ++i) {
            std::cout << output_shape[i];
            if (i < output_shape.size() - 1) std::cout << ",";
        }
        std::cout << "]" << std::endl;
        
        // Show first few outputs
        std::cout << "First 10 output values: ";
        for (int i = 0; i < 10 && i < output_shape[1] * output_shape[2]; ++i) {
            std::cout << output_data[i] << " ";
        }
        std::cout << std::endl;
        
        std::cout << "✅ SUCCESS: Simple model works!" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Error: " << e.what() << std::endl;
        return 1;
    }
}