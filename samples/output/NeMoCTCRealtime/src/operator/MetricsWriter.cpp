// XHBfeKwcabuzLtQ32cdLK5bWB7_1HwXTCehijeXkYFTwibXTAzHo16wTvHRxD7mLUV_0JrCxWkYlZ421bgLe0NDd




#include "./MetricsWriter.h"
using namespace SPL::_Operator;

#include <SPL/Runtime/Function/SPLFunctions.h>
#include <SPL/Runtime/Operator/Port/Punctuation.h>


#define MY_OPERATOR_SCOPE SPL::_Operator
#define MY_BASE_OPERATOR MetricsWriter_Base
#define MY_OPERATOR MetricsWriter$OP



#include <SPL/Runtime/Common/ApplicationRuntimeMessage.h>
#include <SPL/Runtime/Utility/LogTraceMessage.h>
#include <SPL/Runtime/Operator/OperatorMetrics.h>
#include <SPL/Toolkit/RuntimeException.h>
#include <filesystem>
#include <signal.h>



using namespace std;
using namespace boost::iostreams;

// substitute sequence numbers and date/time patterns in filename
std::string MY_OPERATOR_SCOPE::MY_OPERATOR::genFilename()
{
    // copy the pattern in preparation for creating the filename
    std::string pathName = _pattern;
    
    // convert the file sequence number into a string for the {id} substitution
    ostringstream s;
    s.imbue(locale::classic());
    s << _fileGeneration++;
    const string& id = s.str();
    
    // substitute the file sequence number for {id}
    string::size_type p;
    while ((p = pathName.find ("{id}")) != string::npos) {
        pathName.replace (p, 4, id);
    }

    bool needsLocaltime = ((pathName.find("{localtime:")) != string::npos);
    bool needsGMTime = ((pathName.find("{gmtime:")) != string::npos);

    if (needsLocaltime || needsGMTime) {
        // get the current time for the {localtime:format} and {gmtime:format} substitutions
        time_t now;
        time(&now);
    
        char buffer[1024*10] = "";

        if (needsLocaltime) {
            // substitute the time in the local timezone for the {localtime:format} pattern
            struct tm local_tm;
            localtime_r(&now, &local_tm);
            while ((p = pathName.find("{localtime:")) != string::npos) {
    	        string::size_type q = pathName.find ("}", p);
                if (q == string::npos) {
                    SPLTRACEMSGANDTHROW(SPLRuntimeFileSinkOperator, L_ERROR, SPL_APPLICATION_RUNTIME_FILESINK_BAD_PATTERN(_pattern, "{localtime:...}"), SPL_FUNC_DBG);
	        }
    	        string format = pathName.substr(p+11, q-p-11);
    	        strftime(buffer, sizeof(buffer), format.c_str(), &local_tm);
    	        pathName.replace(p, q-p+1, buffer);
            }
        }

        if (needsGMTime) {
            // substitute the time in the local timezone for the {gmtime:format} pattern
            struct tm gmt_tm;
            gmtime_r(&now, &gmt_tm);
            while ((p = pathName.find("{gmtime:")) != string::npos) {
    	        string::size_type q = pathName.find ("}", p);
                if (q == string::npos) {
                    SPLTRACEMSGANDTHROW(SPLRuntimeFileSinkOperator, L_ERROR, SPL_APPLICATION_RUNTIME_FILESINK_BAD_PATTERN(_pattern, "{gmtime:...}"), SPL_FUNC_DBG);
                }
    	        string format = pathName.substr(p+8, q-p-8);
    	        strftime(buffer, sizeof(buffer), format.c_str(), &gmt_tm);
    	        pathName.replace(p, q-p+1, buffer);
            }
        }
    }
    // Anything left?
    if (!pathName.size()) {
        SPLTRACEMSGANDTHROW(SPLRuntimeFileSinkOperator, L_ERROR, SPL_APPLICATION_RUNTIME_FILESINK_EMPTY_FILENAME, SPL_FUNC_DBG);
    }

    return makeAbsolute(pathName);
}



void MY_OPERATOR_SCOPE::MY_OPERATOR::closeFile()
{
    if (!_f.get()) 
        return; // we don't want to do this twice

    delete _f.release();
    
    
    
    
}

MY_OPERATOR_SCOPE::MY_OPERATOR::Helper::Helper(const string& fName, bool restoring 
                            
                            )
: _fs(&_filt_str)


{
    SPLAPPTRC(L_DEBUG, "open '" << fName << "' as the output file...", SPL_OPER_DBG);

    
    
        _fd = ::open (fName.c_str(), O_WRONLY|O_CREAT|O_LARGEFILE|O_TRUNC, 0666);
    
    
    if (_fd < 0) 
        SPLTRACEMSGANDTHROW (SPLRuntimeFileSinkOperator, L_ERROR, SPL_APPLICATION_RUNTIME_FAILED_OPEN_OUTPUT_FILE(fName, RuntimeUtility::getErrorNoStr()), SPL_FUNC_DBG);

    _fd_sink.reset (new file_descriptor_sink (_fd, close_handle));
    
    _filt_str.push (*_fd_sink);
    _fs.imbue(locale::classic());
}

void MY_OPERATOR_SCOPE::MY_OPERATOR::openFile(bool restoring)
{
    if (!_shutdown) {
        SPLAPPTRC(L_DEBUG, "Opening file: " << _pathName, SPL_OPER_DBG);
        _f.reset (new Helper(_pathName, restoring
                             
                             ));
        _numFilesOpenedMetric.incrementValueNoLock();
        
    }
}

std::string MY_OPERATOR_SCOPE::MY_OPERATOR::makeAbsolute(const std::string & fileName)
{
    std::filesystem::path file(fileName);
    if (!file.is_absolute()) {
        // getDataDirectory will throw an exception if there is no data dir
        file = getPE().getDataDirectory();
        file /= fileName;
    }
    return file.string();
}

MY_OPERATOR_SCOPE::MY_OPERATOR::MY_OPERATOR()
: MY_BASE_OPERATOR(),
  _shutdown(false),
  _pattern((((((lit$6 + (::SPL::spl_cast<SPL::boolean, SPL::rstring >::cast(lit$3) ? lit$5 : lit$4)) + ::SPL::spl_cast<SPL::rstring, SPL::int32 >::cast(::SPL::spl_cast<SPL::int32, SPL::rstring >::cast(lit$2))) + lit$1) + ::SPL::spl_cast<SPL::rstring, SPL::float64 >::cast(::SPL::Functions::Time::getTimestampInSecs())) + lit$0)),
  _numFilesOpenedMetric(getContext().getMetrics().getCustomMetricByName("nFilesOpened")), 
  _numTupleWriteErrorsMetric(getContext().getMetrics().getCustomMetricByName("nTupleWriteErrors")),
  _fileGeneration(0)
  
  
  
  , _ckptOffset(0), _resetting(false)
{
    SPLAPPTRC(L_DEBUG, "FileSink startup...", SPL_OPER_DBG);

    _numFilesOpenedMetric.setValueNoLock(0);
    _numTupleWriteErrorsMetric.setValueNoLock(0);

    // Figure out output file and open it
    
         
                _pathName = genFilename();
               // Not restoring
               openFile(false);
           
        
    

     try {
         SPLAPPTRC(L_DEBUG, "Register tag data for governance", SPL_OPER_DBG);
         std::map<string,string> props;
         props["outputOperatorType"]="FileSink";
         std::filesystem::path absPath(makeAbsolute(_pathName));
         props["srcName"]=absPath.filename().string();
         props["srcType"]="data_file";
         props["p1srcName"]=absPath.parent_path().string();
         props["srcParent"]="p1";
         props["p1parentType"]="parent_folder_or_host";
         props["p1srcType"]="data_file_folder";
         props["registerType"] = "output";
         setTagData("OperatorIGC", props);
     } catch (std::exception const & e) {
         SPLAPPTRC(L_DEBUG, "Exception received when registering tag data: " << e.what(), SPL_OPER_DBG);		  
     }
 
    
    
    
    
    
    signal(SIGPIPE,SIG_IGN);

    
}


void MY_OPERATOR_SCOPE::MY_OPERATOR::process(Punctuation const & punct, uint32_t port) {
    AutoMutex am(_mutex);
    if (_shutdown) {
        return;
    }
    try {
        
        
            
                    _f->flush();
        
    } catch (const std::exception& e) {
        SPLAPPTRC(L_ERROR, SPL_APPLICATION_RUNTIME_EXCEPTION(e.what()), SPL_OPER_DBG);
    } catch (...) {
        SPLAPPTRC(L_ERROR, SPL_APPLICATION_RUNTIME_UNKNOWN_EXCEPTION, SPL_OPER_DBG);
    }
    
    
}


void MY_OPERATOR_SCOPE::MY_OPERATOR::prepareToShutdown()
{
    AutoMutex am(_mutex);
    _shutdown = true;
    
        delete _f.release();
    
}

void MY_OPERATOR_SCOPE::MY_OPERATOR::process(Tuple const & tuple$, uint32_t port)
{
    AutoMutex am(_mutex);
    if (_shutdown) {
        return;
    } 
    const IPort0Type iport$0 = static_cast<const IPort0Type&>(tuple$);

    

    

    // Write to output
    try {
        
        SPL::serializeWithPrecision(_f->writeTo(), iport$0.get_transcription()) << ',';
	SPL::serializeWithPrecision(_f->writeTo(), iport$0.get_processingTimeMs()) << ',';
	SPL::serializeWithPrecision(_f->writeTo(), iport$0.get_audioChunkMs()) << ',';
	SPL::serializeWithPrecision(_f->writeTo(), iport$0.get_speedupFactor()) << ',';
	SPL::serializeWithPrecision(_f->writeTo(), iport$0.get_chunkNumber()) << '\n';

        
        if (!_f->fs()) {
            // The write failed for some reason.  Mark this as a failed write
            _numTupleWriteErrorsMetric.incrementValueNoLock();
            
        }
    } catch (const std::exception& e) {
        SPLAPPTRC(L_ERROR, SPL_APPLICATION_RUNTIME_EXCEPTION(e.what()), SPL_OPER_DBG);
    } catch (...) {
        SPLAPPTRC(L_ERROR, SPL_APPLICATION_RUNTIME_UNKNOWN_EXCEPTION, SPL_OPER_DBG);
    }
    

    
}




static SPL::Operator * initer() { return new MY_OPERATOR_SCOPE::MY_OPERATOR(); }
bool MY_BASE_OPERATOR::globalInit_ = MY_BASE_OPERATOR::globalIniter();
bool MY_BASE_OPERATOR::globalIniter() {
    instantiators_.insert(std::make_pair("MetricsWriter",&initer));
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
    param$file$0 = (((((lit$6 + (::SPL::spl_cast<SPL::boolean, SPL::rstring >::cast(lit$3) ? lit$5 : lit$4)) + ::SPL::spl_cast<SPL::rstring, SPL::int32 >::cast(::SPL::spl_cast<SPL::int32, SPL::rstring >::cast(lit$2))) + lit$1) + ::SPL::spl_cast<SPL::rstring, SPL::float64 >::cast(::SPL::Functions::Time::getTimestampInSecs())) + lit$0);
    addParameterValue ("file", SPL::ConstValueHandle(param$file$0));
    param$format$0 = "csv";
    addParameterValue ("format", SPL::ConstValueHandle(param$format$0));
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


