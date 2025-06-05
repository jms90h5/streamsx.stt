// fjD9VHyaQfdW35FCpDsYvJX5PHmZibgNymMauouozSU2KJfviiddo0yj2_19jLXNvn_0CUtKWhzuE0AkHALKmVAR


/* Additional includes for NeMoSTT operator */
#include <iostream>
#include <cstring>


#include "./NeMoOut.h"
using namespace SPL::_Operator;

#include <SPL/Runtime/Function/SPLFunctions.h>
#include <SPL/Runtime/Operator/Port/Punctuation.h>

#include <string>

#define MY_OPERATOR_SCOPE SPL::_Operator
#define MY_BASE_OPERATOR NeMoOut_Base
#define MY_OPERATOR NeMoOut$OP





MY_OPERATOR_SCOPE::MY_OPERATOR::MY_OPERATOR()
    : sampleRate_(16000),
      channels_(1),
      modelPath_(SPL::rstring("/homes/jsharpe/teracloud/com.teracloud.streamsx.stt/models/fastconformer_ctc_export/model.onnx")),
      tokensPath_(),
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
    
    // Derive tokens path from model path
    std::string modelDir = modelPath_;
    size_t lastSlash = modelDir.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        modelDir = modelDir.substr(0, lastSlash);
        tokensPath_ = modelDir + "/tokens.txt";
    } else {
        tokensPath_ = "tokens.txt";
    }
    
    // Initialize NeMo CTC implementation
    nemoSTT_ = createNeMoCTCImpl();
    
    if (!nemoSTT_->initialize(modelPath_, tokensPath_)) {
        SPLAPPTRC(L_ERROR, "Failed to initialize NeMo CTC model: " << modelPath_, SPL_OPER_DBG);
        throw std::runtime_error("Failed to initialize NeMo CTC model");
    }
    
    SPLAPPTRC(L_INFO, "NeMo model initialized successfully", SPL_OPER_DBG);
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
    // Expecting tuple with audioChunk (blob) and audioTimestamp (uint64) attributes
    const SPL::blob& audioBlob = ituple.get_audioChunk();
    const void* audioData = audioBlob.getData();
    uint64_t audioSize = audioBlob.getSize();
    
    // Convert 16-bit audio to float samples
    const int16_t* samples = static_cast<const int16_t*>(audioData);
    size_t numSamples = audioSize / sizeof(int16_t);
    
    std::vector<float> floatSamples(numSamples);
    for (size_t i = 0; i < numSamples; ++i) {
        floatSamples[i] = static_cast<float>(samples[i]) / 32768.0f;
    }
    
    // Transcribe audio chunk
    std::string transcription = nemoSTT_->transcribe(floatSamples);
    
    // Output transcription if not empty
    if (!transcription.empty()) {
        outputTranscription(transcription);
    }
}

void MY_OPERATOR_SCOPE::MY_OPERATOR::process(Punctuation const & punct, uint32_t port)
{
    SPLAPPTRC(L_TRACE, "NeMoSTT process punctuation: " << punct, SPL_OPER_DBG);
    
    // Forward punctuation processing (CTC model handles each chunk independently)
    
    // Forward punctuation
    submit(punct, 0);
}


void MY_OPERATOR_SCOPE::MY_OPERATOR::outputTranscription(const std::string& text)
{
    if (text.empty()) return;
    
    SPLAPPTRC(L_INFO, "Transcription: " << text, SPL_OPER_DBG);
    
    // Create output tuple
    OPort0Type otuple;
    
    // Set transcription attribute (expecting rstring transcription)
    otuple.set_transcription(text);
    
    // Submit output tuple
    submit(otuple, 0);
}

static SPL::Operator * initer() { return new MY_OPERATOR_SCOPE::MY_OPERATOR(); }
bool MY_BASE_OPERATOR::globalInit_ = MY_BASE_OPERATOR::globalIniter();
bool MY_BASE_OPERATOR::globalIniter() {
    instantiators_.insert(std::make_pair("NeMoOut",&initer));
    return true;
}

template<class T> static void initRTC (SPL::Operator& o, T& v, const char * n) {
    SPL::ValueHandle vh = v;
    o.getContext().getRuntimeConstantValue(vh, n);
}

MY_BASE_OPERATOR::MY_BASE_OPERATOR()
 : Operator()  {
    uint32_t index = getIndex();
    param$modelPath$0 = SPL::rstring("/homes/jsharpe/teracloud/com.teracloud.streamsx.stt/models/fastconformer_ctc_export/model.onnx");
    addParameterValue ("modelPath", SPL::ConstValueHandle(param$modelPath$0));
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


