// HmOB77jI7wg1Y76PuNw_1G_11Payc2udE7TliWiB42ArrERVquX_0GG4rcTxW5qp_1AcWOTXBVfcABkrzMHgADhWC8


/* Additional includes for NeMoSTT operator */
#include <KaldiFbankFeatureExtractor.hpp>
#include <iostream>
#include <cstring>
#include <chrono>


#include "./Transcription.h"
using namespace SPL::_Operator;

#include <SPL/Runtime/Function/SPLFunctions.h>
#include <SPL/Runtime/Operator/Port/Punctuation.h>

#include <string>

#define MY_OPERATOR_SCOPE SPL::_Operator
#define MY_BASE_OPERATOR Transcription_Base
#define MY_OPERATOR Transcription$OP





MY_OPERATOR_SCOPE::MY_OPERATOR::MY_OPERATOR()
    : sampleRate_(16000),
      channels_(1),
      modelPath_(lit$0),
      tokensPath_(lit$1),
      chunkDurationMs_(5000),
      minSpeechDurationMs_(500)
{
    // Parse audio format
    std::string format = "mono16k";
    if (format == "mono8k") {
        sampleRate_ = 8000;
    } else if (format == "mono16k") {
        sampleRate_ = 16000;
    } else if (format == "mono22k") {
        sampleRate_ = 22050;
    } else if (format == "mono44k") {
        sampleRate_ = 44100;
    } else if (format == "mono48k") {
        sampleRate_ = 48000;
    }
    
    SPLAPPTRC(L_DEBUG, "NeMoSTT constructor: modelPath=" << modelPath_ 
              << ", tokensPath=" << tokensPath_
              << ", sampleRate=" << sampleRate_ 
              << ", chunkDuration=" << chunkDurationMs_ << "ms"
              << ", minSpeechDuration=" << minSpeechDurationMs_ << "ms", 
              SPL_OPER_DBG);
}

MY_OPERATOR_SCOPE::MY_OPERATOR::~MY_OPERATOR() 
{
    SPLAPPTRC(L_DEBUG, "NeMoSTT destructor", SPL_OPER_DBG);
}

void MY_OPERATOR_SCOPE::MY_OPERATOR::allPortsReady() 
{
    SPLAPPTRC(L_DEBUG, "NeMoSTT allPortsReady", SPL_OPER_DBG);
    
    try {
        // Initialize feature extractor with Kaldi-native-fbank
        featureExtractor_.reset(new KaldiFbankFeatureExtractor());
        
        // Initialize NeMo CTC implementation via factory
        nemoSTT_ = createNeMoCTCImpl();
        if (!nemoSTT_->initialize(modelPath_, tokensPath_)) {
            throw std::runtime_error("Failed to initialize NeMo CTC model");
        }
        
        SPLAPPTRC(L_INFO, "NeMo CTC model and feature extractor initialized successfully", SPL_OPER_DBG);
        
    } catch (const std::exception& e) {
        SPLAPPTRC(L_ERROR, "Failed to initialize NeMo STT: " << e.what(), SPL_OPER_DBG);
        throw;
    }
}

void MY_OPERATOR_SCOPE::MY_OPERATOR::prepareToShutdown() 
{
    SPLAPPTRC(L_DEBUG, "NeMoSTT prepareToShutdown", SPL_OPER_DBG);
}

void MY_OPERATOR_SCOPE::MY_OPERATOR::process(Tuple const & tuple, uint32_t port)
{
    SPLAPPTRC(L_TRACE, "NeMoSTT process tuple", SPL_OPER_DBG);
    
    const IPort0Type & ituple = static_cast<const IPort0Type &>(tuple);
    
    // Extract audio data from input tuple
    // Assuming the tuple has a blob attribute containing audio data
    // Extract audio data from input tuple (assumes first blob attribute is audio)
    const SPL::blob& audioBlob = ituple.get_audioChunk();
    const void* audioData = audioBlob.getData();
    uint64_t audioSize = audioBlob.getSize();
    
    // Process audio data (assuming 16-bit samples)
    processAudioData(audioData, audioSize, 16);
    
    // For streaming mode, could check if we have enough audio for transcription
    // For now, accumulate audio and transcribe on punctuation
}

void MY_OPERATOR_SCOPE::MY_OPERATOR::process(Punctuation const & punct, uint32_t port)
{
    SPLAPPTRC(L_TRACE, "NeMoSTT process punctuation: " << punct, SPL_OPER_DBG);
    
    // On window marker, flush any remaining audio and get final transcription
    if (punct == Punctuation::WindowMarker) {
        // Process any accumulated audio
        if (!audioBuffer_.empty()) {
            std::string transcription = transcribeAudio(audioBuffer_);
            if (!transcription.empty()) {
                outputTranscription(transcription);
            }
            audioBuffer_.clear();
        }
    }
    
    // Forward punctuation
    submit(punct, 0);
}

void MY_OPERATOR_SCOPE::MY_OPERATOR::processAudioData(const void* data, size_t bytes, int bitsPerSample)
{
    size_t samples = bytes / (bitsPerSample / 8);
    
    // Convert to float samples and accumulate
    if (bitsPerSample == 16) {
        const int16_t* samples16 = static_cast<const int16_t*>(data);
        for (size_t i = 0; i < samples; i++) {
            audioBuffer_.push_back(samples16[i] / 32768.0f);
        }
    } else if (bitsPerSample == 8) {
        const uint8_t* samples8 = static_cast<const uint8_t*>(data);
        for (size_t i = 0; i < samples; i++) {
            audioBuffer_.push_back((samples8[i] - 128) / 128.0f);
        }
    }
    
    SPLAPPTRC(L_TRACE, "Accumulated " << samples << " samples, total: " << audioBuffer_.size(), SPL_OPER_DBG);
}

void MY_OPERATOR_SCOPE::MY_OPERATOR::processAudioChunk(const std::vector<float>& audioData)
{
    // For real-time streaming, this would process chunks as they arrive
    // For now, we accumulate in audioBuffer_ and process on window markers
    audioBuffer_.insert(audioBuffer_.end(), audioData.begin(), audioData.end());
}

std::string MY_OPERATOR_SCOPE::MY_OPERATOR::transcribeAudio(const std::vector<float>& audioData)
{
    auto start_time = std::chrono::high_resolution_clock::now();
    
    try {
        // Run inference with NeMo CTC model (handles feature extraction internally)
        std::string transcription = nemoSTT_->transcribe(audioData);
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        float audio_duration_ms = audioData.size() * 1000.0f / sampleRate_;
        float speedup = (duration.count() > 0) ? audio_duration_ms / duration.count() : 0.0f;
        
        SPLAPPTRC(L_INFO, "Transcribed " << audio_duration_ms << "ms audio in " 
                  << duration.count() << "ms (" << speedup << "x real-time)", SPL_OPER_DBG);
        
        return transcription;
        
    } catch (const std::exception& e) {
        SPLAPPTRC(L_ERROR, "Transcription failed: " << e.what(), SPL_OPER_DBG);
        return "";
    }
}

void MY_OPERATOR_SCOPE::MY_OPERATOR::outputTranscription(const std::string& text)
{
    if (text.empty()) return;
    
    SPLAPPTRC(L_INFO, "Transcription: " << text, SPL_OPER_DBG);
    
    // Create output tuple
    OPort0Type otuple;
    
    // Set transcription text (assumes output has 'transcription' attribute)
    otuple.set_transcription(text);
    
    // Submit output tuple
    submit(otuple, 0);
}

static SPL::Operator * initer() { return new MY_OPERATOR_SCOPE::MY_OPERATOR(); }
bool MY_BASE_OPERATOR::globalInit_ = MY_BASE_OPERATOR::globalIniter();
bool MY_BASE_OPERATOR::globalIniter() {
    instantiators_.insert(std::make_pair("Transcription",&initer));
    return true;
}

template<class T> static void initRTC (SPL::Operator& o, T& v, const char * n) {
    SPL::ValueHandle vh = v;
    o.getContext().getRuntimeConstantValue(vh, n);
}

MY_BASE_OPERATOR::MY_BASE_OPERATOR()
 : Operator()  {
    uint32_t index = getIndex();
    initRTC(*this, lit$0, "lit$0");
    initRTC(*this, lit$1, "lit$1");
    addParameterValue ("modelPath", SPL::ConstValueHandle(lit$0));
    addParameterValue ("tokensPath", SPL::ConstValueHandle(lit$1));
    param$audioFormat$0 = "mono16k";
    addParameterValue ("audioFormat", SPL::ConstValueHandle(param$audioFormat$0));
    (void) getParameters(); // ensure thread safety by initializing here
    $oportBitset = OPortBitsetType(std::string("01"));
    OperatorMetrics& om = getContext().getMetrics();
    metrics_[0] = &(om.createCustomMetric("nExceptionsCaughtPort0", "Number of exceptions caught on port 0", Metric::Counter));
}
MY_BASE_OPERATOR::~MY_BASE_OPERATOR()
{
    for (ParameterMapType::const_iterator it = paramValues_.begin(); it != paramValues_.end(); it++) {
        const ParameterValueListType& pvl = it->second;
        for (ParameterValueListType::const_iterator it2 = pvl.begin(); it2 != pvl.end(); it2++) {
            delete *it2;
        }
    }
}

void MY_BASE_OPERATOR::tupleLogic(Tuple const & tuple, uint32_t port) {
}


void MY_BASE_OPERATOR::processRaw(Tuple const & tuple, uint32_t port) {
    try {
            tupleLogic (tuple, port);
            static_cast<MY_OPERATOR_SCOPE::MY_OPERATOR*>(this)->MY_OPERATOR::process(tuple, port);
    } catch (const SPL::SPLRuntimeException& e) {
        metrics_[port]->incrementValue();
        unrecoverableExceptionShutdown(e.getExplanation());
    } catch (const std::exception& e) {
        metrics_[port]->incrementValue();
        unrecoverableExceptionShutdown(e.what());
    } catch (...) {
        metrics_[port]->incrementValue();
        unrecoverableExceptionShutdown("Unknown exception");
    }
}


void MY_BASE_OPERATOR::punctLogic(Punctuation const & punct, uint32_t port) {
}

void MY_BASE_OPERATOR::punctPermitProcessRaw(Punctuation const & punct, uint32_t port) {
    {
        punctNoPermitProcessRaw(punct, port);
    }
}

void MY_BASE_OPERATOR::punctNoPermitProcessRaw(Punctuation const & punct, uint32_t port) {
    switch(punct) {
    case Punctuation::WindowMarker:
        try {
            punctLogic(punct, port);
            process(punct, port);
        } catch (const SPL::SPLRuntimeException& e) {
            metrics_[port]->incrementValue();
            unrecoverableExceptionShutdown(e.getExplanation());
        } catch (const std::exception& e) {
            metrics_[port]->incrementValue();
            unrecoverableExceptionShutdown(e.what());
        } catch (...) {
            metrics_[port]->incrementValue();
            unrecoverableExceptionShutdown("Unknown exception");
        }
        break;
    case Punctuation::FinalMarker:
        {
            try {
                punctLogic(punct, port);
                process(punct, port);
            } catch (const SPL::SPLRuntimeException& e) {
                metrics_[port]->incrementValue();
                unrecoverableExceptionShutdown(e.getExplanation());
            } catch (const std::exception& e) {
                metrics_[port]->incrementValue();
                unrecoverableExceptionShutdown(e.what());
            } catch (...) {
                metrics_[port]->incrementValue();
                unrecoverableExceptionShutdown("Unknown exception");
            }
            bool forward = false;
            {
                AutoPortMutex $apm($fpMutex, *this);
                $oportBitset.reset(port);
                if ($oportBitset.none()) {
                    $oportBitset.set(1);
                    forward=true;
                }
            }
            if(forward) {
                if (0 < 1) submit(punct, 0);
                else submitException(punct, 0 - 1);
            }
            return;
        }
    case Punctuation::DrainMarker:
    case Punctuation::ResetMarker:
    case Punctuation::ResumeMarker:
        break;
    case Punctuation::SwitchMarker:
        break;
    case Punctuation::WatermarkMarker:
        break;
    default:
        break;
    }
}

void MY_BASE_OPERATOR::processRaw(Punctuation const & punct, uint32_t port) {
    switch(port) {
    case 0:
        punctNoPermitProcessRaw(punct, port);
        break;
    }
}



void MY_BASE_OPERATOR::checkpointStateVariables(NetworkByteBuffer & opstate) const {
}

void MY_BASE_OPERATOR::restoreStateVariables(NetworkByteBuffer & opstate) {
}

void MY_BASE_OPERATOR::checkpointStateVariables(Checkpoint & ckpt) {
}

void MY_BASE_OPERATOR::resetStateVariables(Checkpoint & ckpt) {
}

void MY_BASE_OPERATOR::resetStateVariablesToInitialState() {
}

bool MY_BASE_OPERATOR::hasStateVariables() const {
    return false;
}

void MY_BASE_OPERATOR::resetToInitialStateRaw() {
    AutoMutex $apm($svMutex);
    StateHandler *sh = getContext().getStateHandler();
    if (sh != NULL) {
        sh->resetToInitialState();
    }
    resetStateVariablesToInitialState();
}

void MY_BASE_OPERATOR::checkpointRaw(Checkpoint & ckpt) {
    AutoMutex $apm($svMutex);
    StateHandler *sh = getContext().getStateHandler();
    if (sh != NULL) {
        sh->checkpoint(ckpt);
    }
    checkpointStateVariables(ckpt);
}

void MY_BASE_OPERATOR::resetRaw(Checkpoint & ckpt) {
    AutoMutex $apm($svMutex);
    StateHandler *sh = getContext().getStateHandler();
    if (sh != NULL) {
        sh->reset(ckpt);
    }
    resetStateVariables(ckpt);
}


