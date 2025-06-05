// kbSb6HZSwBasvltBrp_0Gs0kayEyaOSTP7E2J62j1UJKoEgvhjZXt7rFD4bumhcXrDfzyAChfCV26xwamk2YDC8


#include "./Analysis.h"
using namespace SPL::_Operator;

#include <SPL/Runtime/Function/SPLFunctions.h>
#include <SPL/Runtime/Operator/Port/Punctuation.h>

#include <string>

#define MY_OPERATOR_SCOPE SPL::_Operator
#define MY_BASE_OPERATOR Analysis_Base
#define MY_OPERATOR Analysis$OP




static SPL::Operator * initer() { return new MY_OPERATOR_SCOPE::MY_OPERATOR(); }
bool MY_BASE_OPERATOR::globalInit_ = MY_BASE_OPERATOR::globalIniter();
bool MY_BASE_OPERATOR::globalIniter() {
    instantiators_.insert(std::make_pair("Analysis",&initer));
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
    initRTC(*this, lit$2, "lit$2");
    initRTC(*this, lit$3, "lit$3");
    initRTC(*this, lit$4, "lit$4");
    initRTC(*this, lit$5, "lit$5");
    initRTC(*this, lit$6, "lit$6");
    initRTC(*this, lit$7, "lit$7");
    initRTC(*this, lit$8, "lit$8");
    initRTC(*this, lit$9, "lit$9");
    state$fullTranscription = lit$8;
    SPLAPPTRC(L_DEBUG, "Variable: state$fullTranscription Value: " << state$fullTranscription,SPL_OPER_DBG);
    state$chunkCount = lit$9;
    SPLAPPTRC(L_DEBUG, "Variable: state$chunkCount Value: " << state$chunkCount,SPL_OPER_DBG);
    state$processStart = ::SPL::Functions::Time::getTimestamp();
    SPLAPPTRC(L_DEBUG, "Variable: state$processStart Value: " << state$processStart,SPL_OPER_DBG);
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

void MY_BASE_OPERATOR::tupleLogic(Tuple & tuple, uint32_t port) {
    IPort0Type & iport$0 = static_cast<IPort0Type  &>(tuple);
    AutoPortMutex $apm($svMutex, *this);
    
{
    if ((state$chunkCount == lit$0)) 
        {
            state$processStart = ::SPL::Functions::Time::getTimestamp();
        }
    state$fullTranscription += (iport$0.get_transcription() + lit$1);
    state$chunkCount++;
}

}


void MY_BASE_OPERATOR::processRaw(Tuple & tuple, uint32_t port) {
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
    AutoPortMutex $apm($svMutex, *this);
    
{
    if ((SPL::BcyIw_123yuxalGHt_0UPzvHr6jutVOfqYy4EjyWMc7ELFtCMYkxYjxKoc_1GyrP3_0eilO0roi7TbPc_14tWTm1iZBT(punct-1) == SPL::BcyIw_123yuxalGHt_0UPzvHr6jutVOfqYy4EjyWMc7ELFtCMYkxYjxKoc_1GyrP3_0eilO0roi7TbPc_14tWTm1iZBT::FinalMarker)) 
        {
            const SPL::timestamp id$processEnd = ::SPL::Functions::Time::getTimestamp();
            const SPL::float64 id$processingTimeSec = ::SPL::Functions::Time::diffAsSecs(id$processEnd, state$processStart);
            state$fullTranscription = ::SPL::Functions::String::trim(state$fullTranscription, lit$2);
            const SPL::list<SPL::rstring > id$words = ::SPL::Functions::String::tokenize(state$fullTranscription, lit$3, (SPL::boolean)lit$4);
            const SPL::int32 id$wordCount = ::SPL::Functions::Collections::size(id$words);
            const SPL::int32 id$charCount = ::SPL::Functions::String::length(state$fullTranscription);
            const SPL::float64 id$estimatedAudioDuration = (::SPL::spl_cast<SPL::float64, SPL::int32 >::cast(state$chunkCount) * lit$5);
            const SPL::float64 id$speedupFactor = (id$estimatedAudioDuration / id$processingTimeSec);
            do { SPL::Bs5pNmw71y7PkapZ9lqBSwKrkbWCriwV7k_0ArlSoM1rLHrNgAEBvk5Tpn6YTnjdDdoEcBr6nreFdzTolVZ3wtD9 temp = SPL::Bs5pNmw71y7PkapZ9lqBSwKrkbWCriwV7k_0ArlSoM1rLHrNgAEBvk5Tpn6YTnjdDdoEcBr6nreFdzTolVZ3wtD9(lit$6, state$fullTranscription, id$wordCount, id$charCount, id$estimatedAudioDuration, id$processingTimeSec, id$speedupFactor, state$processStart, id$processEnd); submit (temp, lit$7); } while(0);
        }
}

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
    opstate << state$fullTranscription;
    opstate << state$chunkCount;
    opstate << state$processStart;
}

void MY_BASE_OPERATOR::restoreStateVariables(NetworkByteBuffer & opstate) {
    opstate >> state$fullTranscription;
    opstate >> state$chunkCount;
    opstate >> state$processStart;
}

void MY_BASE_OPERATOR::checkpointStateVariables(Checkpoint & ckpt) {
    ckpt << state$fullTranscription;
    ckpt << state$chunkCount;
    ckpt << state$processStart;
}

void MY_BASE_OPERATOR::resetStateVariables(Checkpoint & ckpt) {
    ckpt >> state$fullTranscription;
    ckpt >> state$chunkCount;
    ckpt >> state$processStart;
}

void MY_BASE_OPERATOR::resetStateVariablesToInitialState() {
    state$fullTranscription = lit$8;
    SPLAPPTRC(L_DEBUG, "Variable: state$fullTranscription Value: " << state$fullTranscription,SPL_OPER_DBG);
    state$chunkCount = lit$9;
    SPLAPPTRC(L_DEBUG, "Variable: state$chunkCount Value: " << state$chunkCount,SPL_OPER_DBG);
    state$processStart = ::SPL::Functions::Time::getTimestamp();
    SPLAPPTRC(L_DEBUG, "Variable: state$processStart Value: " << state$processStart,SPL_OPER_DBG);
}

bool MY_BASE_OPERATOR::hasStateVariables() const {
    return true;
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



