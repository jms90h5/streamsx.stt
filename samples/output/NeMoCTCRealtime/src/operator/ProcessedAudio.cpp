// m2nFWvhiCP8S93aLbaPfyDm7Ny5lKJfAXNU_0_1ecI3xJRLfmPzuGGhyVsIiU5XefphthMoKAyqGjsofAFjB_1HDF




#include <SPL/Toolkit/Delay.h>



#include "./ProcessedAudio.h"
using namespace SPL::_Operator;

#include <SPL/Runtime/Function/SPLFunctions.h>
#include <SPL/Runtime/Operator/Port/Punctuation.h>

#include <string>

#define MY_OPERATOR_SCOPE SPL::_Operator
#define MY_BASE_OPERATOR ProcessedAudio_Base
#define MY_OPERATOR ProcessedAudio$OP



#define NANOS_IN_SEC 1000000000LL

MY_OPERATOR_SCOPE::MY_OPERATOR::MY_OPERATOR()
: MY_BASE_OPERATOR()
{
    _last.time = 0;     // ensure first tuple processed correctly
    _rate = (::SPL::spl_cast<SPL::boolean, SPL::rstring >::cast(lit$0) ? (lit$3 / ::SPL::spl_cast<SPL::float64, SPL::int32 >::cast(::SPL::spl_cast<SPL::int32, SPL::rstring >::cast(lit$2))) : lit$1);
    double p = 10.0 * (1.0 / (_rate));
    _period = static_cast<uint64_t>(p * NANOS_IN_SEC);
    _periodDiv2 = _period / 2;
    _halfFull = static_cast<uint64_t>(_rate * (p / 2)); // assume half full
    _initTime = 0;
}

template<class T> void MY_OPERATOR_SCOPE::MY_OPERATOR::handleOne(T& o)
{
    AutoPortMutex apm(_mutex, *this);
    uint64 currentTime = SPL::Functions::Time::getCPUCounterInNanoSeconds() - _initTime;

    if (_last.time == 0) {
        _last.time = _periodDiv2;
        _last.count = _currentCount = _halfFull;
        _first.time = _first.count = 0;
        _initTime = currentTime - _periodDiv2;
        submit(o, 0);
        return;
    }

    uint64 newLastTime = _periodDiv2*(currentTime/_periodDiv2);
    uint64 newFirstTime = newLastTime - _periodDiv2;

    if (newFirstTime != _first.time) {
        _first.time = newFirstTime;
        _last.time = newLastTime;
        _first.count = _last.count;
        _last.count = _currentCount;
    }

    double timeInterval = (currentTime-_first.time)/static_cast<double>(NANOS_IN_SEC);
    double countInterval = 1+_currentCount-_first.count;
    double projectedRate = countInterval/timeInterval;

    _currentCount++;
    if (projectedRate < _rate) {
        submit(o, 0);
        return;
    } 

    double sleepTime = countInterval/_rate - timeInterval;
    
        SPL::preciseBlockUntilShutdownRequest(sleepTime, getPE());
    
    submit(o, 0);
}

void MY_OPERATOR_SCOPE::MY_OPERATOR::process(Tuple const & tuple, uint32_t port)
{
    handleOne (tuple);
}

void MY_OPERATOR_SCOPE::MY_OPERATOR::process(Punctuation const & punct, uint32_t port)
{
    if (punct == Punctuation::FinalMarker)    
        return;

    submit (punct, port);

}

void MY_OPERATOR_SCOPE::MY_OPERATOR::processWatermark(Punctuation const & wm, uint32_t port)
{

    submit (wm, port);

}

static SPL::Operator * initer() { return new MY_OPERATOR_SCOPE::MY_OPERATOR(); }
bool MY_BASE_OPERATOR::globalInit_ = MY_BASE_OPERATOR::globalIniter();
bool MY_BASE_OPERATOR::globalIniter() {
    instantiators_.insert(std::make_pair("ProcessedAudio",&initer));
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
    param$rate$0 = (::SPL::spl_cast<SPL::boolean, SPL::rstring >::cast(lit$0) ? (lit$3 / ::SPL::spl_cast<SPL::float64, SPL::int32 >::cast(::SPL::spl_cast<SPL::int32, SPL::rstring >::cast(lit$2))) : lit$1);
    addParameterValue ("rate", SPL::ConstValueHandle(param$rate$0));
    param$precise$0 = true;
    addParameterValue ("precise", SPL::ConstValueHandle(param$precise$0));
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



