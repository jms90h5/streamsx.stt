#include "../include/ModelInterface.hpp"
#include "../include/NeMoCacheAwareConformer.hpp"
#include <iostream>
#include <string>

namespace onnx_stt {

std::unique_ptr<ModelInterface> createZipformerModel(const ModelInterface::ModelConfig& /* config */) {
    std::cerr << "ZipformerModel temporarily disabled due to interface issues" << std::endl;
    return nullptr;
}

std::unique_ptr<ModelInterface> createConformerModel(const ModelInterface::ModelConfig& /* config */) {
    std::cerr << "ConformerModel not yet implemented" << std::endl;
    return nullptr;
}

std::unique_ptr<ModelInterface> createWenetModel(const ModelInterface::ModelConfig& /* config */) {
    std::cerr << "WenetModel not yet implemented" << std::endl;
    return nullptr;
}

std::unique_ptr<ModelInterface> createSpeechBrainModel(const ModelInterface::ModelConfig& /* config */) {
    std::cerr << "SpeechBrainModel not yet implemented" << std::endl;
    return nullptr;
}

std::unique_ptr<ModelInterface> createNeMoModel(const ModelInterface::ModelConfig& config) {
    // Create NeMo cache-aware Conformer model
    NeMoCacheAwareConformer::NeMoConfig nemo_config;
    nemo_config.model_path = config.encoder_path;  // NeMo uses single model file
    nemo_config.num_threads = config.num_threads;
    nemo_config.chunk_frames = config.chunk_frames;
    nemo_config.feature_dim = config.feature_dim;
    nemo_config.batch_size = 1;
    
    // Default cache configuration for NeMo FastConformer
    nemo_config.last_channel_cache_size = 64;
    nemo_config.last_time_cache_size = 64;
    nemo_config.num_cache_layers = 12;  // Typical for FastConformer
    nemo_config.hidden_size = 512;     // FastConformer hidden size
    
    // Set attention context for latency (0ms by default)
    nemo_config.att_context_size_left = 70;
    nemo_config.att_context_size_right = 0;
    
    auto nemo_model = std::make_unique<NeMoCacheAwareConformer>(nemo_config);
    
    if (!nemo_model->initialize(config)) {
        std::cerr << "Failed to initialize NeMo model" << std::endl;
        return nullptr;
    }
    
    return std::move(nemo_model);
}

std::unique_ptr<ModelInterface> createModel(const ModelInterface::ModelConfig& config) {
    // Factory function that automatically detects model type based on config
    
    ModelInterface::ModelConfig::ModelType model_type = config.model_type;
    
    std::cout << "Creating model of type: " << static_cast<int>(model_type) << std::endl;
    
    switch (model_type) {
        case ModelInterface::ModelConfig::ZIPFORMER_RNNT:
            return createZipformerModel(config);
        case ModelInterface::ModelConfig::NVIDIA_NEMO:
            return createNeMoModel(config);
        case ModelInterface::ModelConfig::CONFORMER_RNNT:
            return createConformerModel(config);
        case ModelInterface::ModelConfig::WENET_CONFORMER:
            return createWenetModel(config);
        case ModelInterface::ModelConfig::SPEECHBRAIN_CRDNN:
            return createSpeechBrainModel(config);
        default:
            std::cerr << "Unknown model type: " << static_cast<int>(model_type) << std::endl;
            return nullptr;
    }
}

} // namespace onnx_stt