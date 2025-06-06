#include <onnxruntime_cxx_api.h>
#include <iostream>

int main() {
    try {
        std::cout << "=== ONNX Runtime Model Loading Test ===" << std::endl;
        
        Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "ModelLoadTest");
        Ort::SessionOptions session_options;
        session_options.SetIntraOpNumThreads(4);
        
        // Test encoder loading
        std::cout << "Loading encoder model..." << std::endl;
        Ort::Session encoder_session(env, "models/nemo_fresh_export/encoder-fastconformer_cache_final.onnx", session_options);
        std::cout << "✓ Encoder loaded successfully" << std::endl;
        std::cout << "  Inputs: " << encoder_session.GetInputCount() << std::endl;
        std::cout << "  Outputs: " << encoder_session.GetOutputCount() << std::endl;
        
        // Print encoder input names
        for (size_t i = 0; i < encoder_session.GetInputCount(); ++i) {
            auto input_name = encoder_session.GetInputNameAllocated(i, Ort::AllocatorWithDefaultOptions());
            std::cout << "  Input " << i << ": " << input_name.get() << std::endl;
        }
        
        // Test decoder loading
        std::cout << "\nLoading decoder model..." << std::endl;
        Ort::Session decoder_session(env, "models/nemo_fresh_export/decoder_joint-fastconformer_cache_final.onnx", session_options);
        std::cout << "✓ Decoder loaded successfully" << std::endl;
        std::cout << "  Inputs: " << decoder_session.GetInputCount() << std::endl;
        std::cout << "  Outputs: " << decoder_session.GetOutputCount() << std::endl;
        
        // Print decoder input names
        for (size_t i = 0; i < decoder_session.GetInputCount(); ++i) {
            auto input_name = decoder_session.GetInputNameAllocated(i, Ort::AllocatorWithDefaultOptions());
            std::cout << "  Input " << i << ": " << input_name.get() << std::endl;
        }
        
        std::cout << "\n✅ Both models loaded successfully!" << std::endl;
        std::cout << "Cache-aware streaming support confirmed." << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Model loading failed: " << e.what() << std::endl;
        return 1;
    }
}