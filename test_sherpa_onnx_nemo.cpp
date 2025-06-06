#include <iostream>
#include <fstream>
#include <vector>
#include <sndfile.h>
#include "sherpa-onnx/c-api/c-api.h"

// Test sherpa-onnx with NeMo model to see if it works

int main() {
    std::cout << "=== Testing Sherpa-ONNX with NeMo Model ===" << std::endl;
    
    // Configure for NeMo model
    SherpaOnnxOfflineRecognizerConfig config;
    memset(&config, 0, sizeof(config));
    
    // Set up NeMo model paths
    config.model_config.transducer.encoder = "models/proven_onnx_export/encoder-proven_fastconformer.onnx";
    config.model_config.transducer.decoder = "models/proven_onnx_export/decoder_joint-proven_fastconformer.onnx";
    config.model_config.tokens = "models/proven_onnx_export/real_vocabulary.txt";
    config.model_config.num_threads = 4;
    
    // Create recognizer
    const SherpaOnnxOfflineRecognizer* recognizer = SherpaOnnxCreateOfflineRecognizer(&config);
    if (!recognizer) {
        std::cerr << "Failed to create recognizer" << std::endl;
        return 1;
    }
    
    // Load audio
    const char* audio_file = "test_data/audio/librispeech-1995-1837-0001.wav";
    SF_INFO sf_info;
    SNDFILE* sndfile = sf_open(audio_file, SFM_READ, &sf_info);
    if (!sndfile) {
        std::cerr << "Failed to open audio file: " << audio_file << std::endl;
        return 1;
    }
    
    std::vector<float> audio_data(sf_info.frames);
    sf_read_float(sndfile, audio_data.data(), sf_info.frames);
    sf_close(sndfile);
    
    std::cout << "Audio loaded: " << sf_info.frames << " samples at " 
              << sf_info.samplerate << " Hz" << std::endl;
    
    // Create stream
    const SherpaOnnxOfflineStream* stream = SherpaOnnxCreateOfflineStream(recognizer);
    
    // Process audio
    SherpaOnnxAcceptWaveformOffline(stream, sf_info.samplerate, 
                                   audio_data.data(), audio_data.size());
    
    // Decode
    SherpaOnnxDecodeOfflineStream(recognizer, stream);
    
    // Get result
    const SherpaOnnxOfflineRecognizerResult* result = 
        SherpaOnnxGetOfflineStreamResult(stream);
    
    std::cout << "\nTranscript: \"" << result->text << "\"" << std::endl;
    
    // Expected result
    std::string expected = "it was the first great sorrow of his life it was not so much the loss of the cotton itself but the fantasy the hopes the dreams built around it";
    std::cout << "\nExpected: \"" << expected << "\"" << std::endl;
    
    // Cleanup
    SherpaOnnxDestroyOfflineRecognizerResult(result);
    SherpaOnnxDestroyOfflineStream(stream);
    SherpaOnnxDestroyOfflineRecognizer(recognizer);
    
    return 0;
}