// G_0dMTYXDO_1pj3L5kaxxzR85t_1prif2QHc8BJxjmsbKvzsqtiCrfsBBwJrE3dA0LsxEg0sbT3Dg2OJFUFqCGjAf


#include "./Display.h"
using namespace SPL::_Operator;

#include <SPL/Runtime/Function/SPLFunctions.h>
#include <SPL/Runtime/Operator/Port/Punctuation.h>


#define MY_OPERATOR_SCOPE SPL::_Operator
#define MY_BASE_OPERATOR Display_Base
#define MY_OPERATOR Display$OP




static SPL::Operator * initer() { return new MY_OPERATOR_SCOPE::MY_OPERATOR(); }
bool MY_BASE_OPERATOR::globalInit_ = MY_BASE_OPERATOR::globalIniter();
bool MY_BASE_OPERATOR::globalIniter() {
    instantiators_.insert(std::make_pair("Display",&initer));
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
    initRTC(*this, lit$10, "lit$10");
    initRTC(*this, lit$11, "lit$11");
    initRTC(*this, lit$12, "lit$12");
    initRTC(*this, lit$13, "lit$13");
    initRTC(*this, lit$14, "lit$14");
    initRTC(*this, lit$15, "lit$15");
    initRTC(*this, lit$16, "lit$16");
    initRTC(*this, lit$17, "lit$17");
    initRTC(*this, lit$18, "lit$18");
    initRTC(*this, lit$19, "lit$19");
    initRTC(*this, lit$20, "lit$20");
    initRTC(*this, lit$21, "lit$21");
    initRTC(*this, lit$22, "lit$22");
    initRTC(*this, lit$23, "lit$23");
    initRTC(*this, lit$24, "lit$24");
    initRTC(*this, lit$25, "lit$25");
    initRTC(*this, lit$26, "lit$26");
    state$totalProcessingMs = lit$25;
    SPLAPPTRC(L_DEBUG, "Variable: state$totalProcessingMs Value: " << state$totalProcessingMs,SPL_OPER_DBG);
    state$totalAudioMs = lit$26;
    SPLAPPTRC(L_DEBUG, "Variable: state$totalAudioMs Value: " << state$totalAudioMs,SPL_OPER_DBG);
    state$sessionStart = ::SPL::Functions::Time::getTimestamp();
    SPLAPPTRC(L_DEBUG, "Variable: state$sessionStart Value: " << state$sessionStart,SPL_OPER_DBG);
    (void) getParameters(); // ensure thread safety by initializing here
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
    IPort0Type const & iport$0 = static_cast<IPort0Type const  &>(tuple);
    AutoPortMutex $apm($svMutex, *this);
    
{
    ::SPL::Functions::Utility::printStringLn((((lit$1 + ::SPL::spl_cast<SPL::rstring, SPL::int32 >::cast(iport$0.get_chunkNumber())) + lit$0) + iport$0.get_transcription()));
    ::SPL::Functions::Utility::printStringLn(((((((lit$5 + ::SPL::spl_cast<SPL::rstring, SPL::float64 >::cast(iport$0.get_processingTimeMs())) + lit$4) + ::SPL::spl_cast<SPL::rstring, SPL::float64 >::cast(iport$0.get_audioChunkMs())) + lit$3) + ::SPL::spl_cast<SPL::rstring, SPL::float64 >::cast(iport$0.get_speedupFactor())) + lit$2));
    state$totalProcessingMs += iport$0.get_processingTimeMs();
    state$totalAudioMs += iport$0.get_audioChunkMs();
}

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
    AutoPortMutex $apm($svMutex, *this);
    
{
    if ((SPL::BcyIw_123yuxalGHt_0UPzvHr6jutVOfqYy4EjyWMc7ELFtCMYkxYjxKoc_1GyrP3_0eilO0roi7TbPc_14tWTm1iZBT(punct-1) == SPL::BcyIw_123yuxalGHt_0UPzvHr6jutVOfqYy4EjyWMc7ELFtCMYkxYjxKoc_1GyrP3_0eilO0roi7TbPc_14tWTm1iZBT::FinalMarker)) 
        {
            ::SPL::Functions::Utility::printStringLn(lit$6);
            ::SPL::Functions::Utility::printStringLn((lit$10 + (::SPL::spl_cast<SPL::boolean, SPL::rstring >::cast(lit$7) ? lit$9 : lit$8)));
            ::SPL::Functions::Utility::printStringLn(((lit$13 + ::SPL::spl_cast<SPL::rstring, SPL::int32 >::cast(::SPL::spl_cast<SPL::int32, SPL::rstring >::cast(lit$12))) + lit$11));
            ::SPL::Functions::Utility::printStringLn(((lit$16 + ::SPL::spl_cast<SPL::rstring, SPL::float64 >::cast((state$totalAudioMs / lit$15))) + lit$14));
            ::SPL::Functions::Utility::printStringLn(((lit$19 + ::SPL::spl_cast<SPL::rstring, SPL::float64 >::cast((state$totalProcessingMs / lit$18))) + lit$17));
            ::SPL::Functions::Utility::printStringLn(((lit$21 + ::SPL::spl_cast<SPL::rstring, SPL::float64 >::cast((state$totalAudioMs / state$totalProcessingMs))) + lit$20));
            ::SPL::Functions::Utility::printStringLn(((lit$23 + ::SPL::spl_cast<SPL::rstring, SPL::float64 >::cast(::SPL::Functions::Time::diffAsSecs(::SPL::Functions::Time::getTimestamp(), state$sessionStart))) + lit$22));
            ::SPL::Functions::Utility::printStringLn(lit$24);
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
    opstate << state$totalProcessingMs;
    opstate << state$totalAudioMs;
    opstate << state$sessionStart;
}

void MY_BASE_OPERATOR::restoreStateVariables(NetworkByteBuffer & opstate) {
    opstate >> state$totalProcessingMs;
    opstate >> state$totalAudioMs;
    opstate >> state$sessionStart;
}

void MY_BASE_OPERATOR::checkpointStateVariables(Checkpoint & ckpt) {
    ckpt << state$totalProcessingMs;
    ckpt << state$totalAudioMs;
    ckpt << state$sessionStart;
}

void MY_BASE_OPERATOR::resetStateVariables(Checkpoint & ckpt) {
    ckpt >> state$totalProcessingMs;
    ckpt >> state$totalAudioMs;
    ckpt >> state$sessionStart;
}

void MY_BASE_OPERATOR::resetStateVariablesToInitialState() {
    state$totalProcessingMs = lit$25;
    SPLAPPTRC(L_DEBUG, "Variable: state$totalProcessingMs Value: " << state$totalProcessingMs,SPL_OPER_DBG);
    state$totalAudioMs = lit$26;
    SPLAPPTRC(L_DEBUG, "Variable: state$totalAudioMs Value: " << state$totalAudioMs,SPL_OPER_DBG);
    state$sessionStart = ::SPL::Functions::Time::getTimestamp();
    SPLAPPTRC(L_DEBUG, "Variable: state$sessionStart Value: " << state$sessionStart,SPL_OPER_DBG);
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



