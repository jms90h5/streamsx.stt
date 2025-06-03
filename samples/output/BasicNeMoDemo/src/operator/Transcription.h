// HmOB77jI7wg1Y76PuNw_1G_11Payc2udE7TliWiB42ArrERVquX_0GG4rcTxW5qp_1AcWOTXBVfcABkrzMHgADhWC8


/* Additional includes for NeMoSTT operator */
#include <vector>
#include <memory>
#include <string>
#include <NeMoCTCInterface.hpp>

// Forward declarations
class KaldiFbankFeatureExtractor;


#ifndef SPL_OPER_INSTANCE_TRANSCRIPTION_H_
#define SPL_OPER_INSTANCE_TRANSCRIPTION_H_

#include <SPL/Runtime/Operator/Operator.h>
#include <SPL/Runtime/Operator/ParameterValue.h>
#include <SPL/Runtime/Operator/OperatorContext.h>
#include <SPL/Runtime/Operator/OperatorMetrics.h>
#include <SPL/Runtime/Operator/Port/AutoPortMutex.h>
#include <SPL/Runtime/Operator/State/StateHandler.h>
#include <SPL/Runtime/ProcessingElement/PE.h>
#include <SPL/Runtime/Type/SPLType.h>
#include <SPL/Runtime/Utility/CV.h>
using namespace UTILS_NAMESPACE;

#include "../type/BJnZ5CG7dYuCzMUhZJwzAL3_0GdbQo3dSshe140tlHizvLTKJjeJwGLYVXIY0saSuxtDflHAbgU4Wsd74BTRfIBC.h"
#include "../type/BYvQJG0olYstM6NNFkr0nvRyxjXELfXQ2X729nZMiYSE0lvY08jMqIMCiGkLNdjFnK5XlvQG7MqcO3aQjMR3NAg.h"
#include "../type/BkuXToNx2yM5B7AXTZjTgjOLzXMlOCn1pQWayey0EwsqXG1SqvxIw28l3vJN_0vo7wI_0qsqY0D9dJBd8Uot8ZGCL.h"

#include <bitset>

#define MY_OPERATOR Transcription$OP
#define MY_BASE_OPERATOR Transcription_Base
#define MY_OPERATOR_SCOPE SPL::_Operator

namespace SPL {
namespace _Operator {

class MY_BASE_OPERATOR : public Operator
{
public:
    
    typedef SPL::BJnZ5CG7dYuCzMUhZJwzAL3_0GdbQo3dSshe140tlHizvLTKJjeJwGLYVXIY0saSuxtDflHAbgU4Wsd74BTRfIBC IPort0Type;
    typedef SPL::BkuXToNx2yM5B7AXTZjTgjOLzXMlOCn1pQWayey0EwsqXG1SqvxIw28l3vJN_0vo7wI_0qsqY0D9dJBd8Uot8ZGCL OPort0Type;
    
    MY_BASE_OPERATOR();
    
    ~MY_BASE_OPERATOR();
    
    inline void tupleLogic(Tuple const & tuple, uint32_t port);
    void processRaw(Tuple const & tuple, uint32_t port);
    
    inline void punctLogic(Punctuation const & punct, uint32_t port);
    void processRaw(Punctuation const & punct, uint32_t port);
    void punctPermitProcessRaw(Punctuation const & punct, uint32_t port);
    void punctNoPermitProcessRaw(Punctuation const & punct, uint32_t port);
    
    inline void submit(Tuple & tuple, uint32_t port)
    {
        Operator::submit(tuple, port);
    }
    inline void submit(Punctuation const & punct, uint32_t port)
    {
        Operator::submit(punct, port);
    }
    
    
    
    SPL::rstring lit$0;
    SPL::rstring lit$1;
    
    
protected:
    Mutex $svMutex;
    typedef std::bitset<2> OPortBitsetType;
    OPortBitsetType $oportBitset;
    Mutex $fpMutex;
    SPL::rstring param$audioFormat$0;
    void checkpointStateVariables(NetworkByteBuffer & opstate) const;
    void restoreStateVariables(NetworkByteBuffer & opstate);
    void checkpointStateVariables(Checkpoint & ckpt);
    void resetStateVariables(Checkpoint & ckpt);
    void resetStateVariablesToInitialState();
    bool hasStateVariables() const;
    void resetToInitialStateRaw();
    void checkpointRaw(Checkpoint & ckpt);
    void resetRaw(Checkpoint & ckpt);

private:
    static bool globalInit_;
    static bool globalIniter();
    ParameterMapType paramValues_;
    Metric* metrics_[1];
    ParameterMapType& getParameters() { return paramValues_;}
    void addParameterValue(std::string const & param, ConstValueHandle const& value)
    {
        ParameterMapType::iterator it = paramValues_.find(param);
        if (it == paramValues_.end())
            it = paramValues_.insert (std::make_pair (param, ParameterValueListType())).first;
        it->second.push_back(&ParameterValue::create(value));
    }
    void addParameterValue(std::string const & param)
    {
        ParameterMapType::iterator it = paramValues_.find(param);
        if (it == paramValues_.end())
            it = paramValues_.insert (std::make_pair (param, ParameterValueListType())).first;
        it->second.push_back(&ParameterValue::create());
    }
};


class MY_OPERATOR : public MY_BASE_OPERATOR 
{
public:
    MY_OPERATOR();
    virtual ~MY_OPERATOR(); 

    void allPortsReady(); 
    void prepareToShutdown(); 
    
    void process(Tuple const & tuple, uint32_t port);
    void process(Punctuation const & punct, uint32_t port);
    
private:
    // Working NeMo CTC implementation via interface
    std::unique_ptr<NeMoCTCInterface> nemoSTT_;
    std::unique_ptr<KaldiFbankFeatureExtractor> featureExtractor_;
    
    // Audio parameters
    int sampleRate_;
    int channels_;
    
    // Model paths
    std::string modelPath_;
    std::string tokensPath_;
    
    // Configuration
    int chunkDurationMs_;
    int minSpeechDurationMs_;
    
    // Audio buffer for streaming
    std::vector<float> audioBuffer_;
    
    // Helper methods
    void processAudioData(const void* data, size_t bytes, int bitsPerSample);
    void outputTranscription(const std::string& text);
    int getSampleRate() const;
    
    // Working implementation methods
    void processAudioChunk(const std::vector<float>& audioData);
    std::string transcribeAudio(const std::vector<float>& audioData);
}; 

} // namespace _Operator
} // namespace SPL

#undef MY_OPERATOR_SCOPE
#undef MY_BASE_OPERATOR
#undef MY_OPERATOR
#endif // SPL_OPER_INSTANCE_TRANSCRIPTION_H_

