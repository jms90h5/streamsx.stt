// t9s8filenames13transcriptioni9wordCounti9charCountF16audioDurationSecF17processingTimeSecF13speedupFactorT9startTimeT7endTime


#include "Bs5pNmw71y7PkapZ9lqBSwKrkbWCriwV7k_0ArlSoM1rLHrNgAEBvk5Tpn6YTnjdDdoEcBr6nreFdzTolVZ3wtD9.h"
#include <sstream>

#define SELF Bs5pNmw71y7PkapZ9lqBSwKrkbWCriwV7k_0ArlSoM1rLHrNgAEBvk5Tpn6YTnjdDdoEcBr6nreFdzTolVZ3wtD9

using namespace SPL;

TupleMappings* SELF::mappings_ = SELF::initMappings();

static void addMapping(TupleMappings & tm, TypeOffset & offset,
                       std::string const & name, uint32_t index)
{
    tm.nameToIndex_.insert(std::make_pair(name, index)); 
    tm.indexToName_.push_back(name);
    tm.indexToTypeOffset_.push_back(offset);    
}

static Tuple * initer() { return new SELF(); }

TupleMappings* SELF::initMappings()
{
    instantiators_.insert(std::make_pair("tuple<rstring filename,rstring transcription,int32 wordCount,int32 charCount,float64 audioDurationSec,float64 processingTimeSec,float64 speedupFactor,timestamp startTime,timestamp endTime>",&initer));
    TupleMappings * tm = new TupleMappings();
#define MY_OFFSETOF(member, base) \
    ((uintptr_t)&reinterpret_cast<Self*>(base)->member) - (uintptr_t)base
   
    // initialize the mappings 
    
    {
        std::string s("filename");
        TypeOffset t(MY_OFFSETOF(filename_, tm), 
                     Meta::Type::typeOf<SPL::rstring >(), 
                     &typeid(SPL::rstring));
        addMapping(*tm, t, s, 0);
    }
    
    {
        std::string s("transcription");
        TypeOffset t(MY_OFFSETOF(transcription_, tm), 
                     Meta::Type::typeOf<SPL::rstring >(), 
                     &typeid(SPL::rstring));
        addMapping(*tm, t, s, 1);
    }
    
    {
        std::string s("wordCount");
        TypeOffset t(MY_OFFSETOF(wordCount_, tm), 
                     Meta::Type::typeOf<SPL::int32 >(), 
                     &typeid(SPL::int32));
        addMapping(*tm, t, s, 2);
    }
    
    {
        std::string s("charCount");
        TypeOffset t(MY_OFFSETOF(charCount_, tm), 
                     Meta::Type::typeOf<SPL::int32 >(), 
                     &typeid(SPL::int32));
        addMapping(*tm, t, s, 3);
    }
    
    {
        std::string s("audioDurationSec");
        TypeOffset t(MY_OFFSETOF(audioDurationSec_, tm), 
                     Meta::Type::typeOf<SPL::float64 >(), 
                     &typeid(SPL::float64));
        addMapping(*tm, t, s, 4);
    }
    
    {
        std::string s("processingTimeSec");
        TypeOffset t(MY_OFFSETOF(processingTimeSec_, tm), 
                     Meta::Type::typeOf<SPL::float64 >(), 
                     &typeid(SPL::float64));
        addMapping(*tm, t, s, 5);
    }
    
    {
        std::string s("speedupFactor");
        TypeOffset t(MY_OFFSETOF(speedupFactor_, tm), 
                     Meta::Type::typeOf<SPL::float64 >(), 
                     &typeid(SPL::float64));
        addMapping(*tm, t, s, 6);
    }
    
    {
        std::string s("startTime");
        TypeOffset t(MY_OFFSETOF(startTime_, tm), 
                     Meta::Type::typeOf<SPL::timestamp >(), 
                     &typeid(SPL::timestamp));
        addMapping(*tm, t, s, 7);
    }
    
    {
        std::string s("endTime");
        TypeOffset t(MY_OFFSETOF(endTime_, tm), 
                     Meta::Type::typeOf<SPL::timestamp >(), 
                     &typeid(SPL::timestamp));
        addMapping(*tm, t, s, 8);
    }
    
    return tm;
}

void SELF::deserialize(std::istream & istr, bool withSuffix)
{
   std::string s;
   char c;

   istr >> c; if (!istr) { return; }
   if (c != '{') { istr.setstate(std::ios_base::failbit); return; }
   
   if (!readAttributeIdentifier(istr, s)) { return; }
   if (s != "filename") { istr.setstate(std::ios_base::failbit); return; }
   istr >> c; if (!istr) { return; }
   if (c != '=') { istr.setstate(std::ios_base::failbit); return; }
   if (withSuffix)
     SPL::deserializeWithSuffix(istr, filename_);
   else
     istr >> filename_;
   if (!istr) { return; }  
   istr >> c; if (!istr) { return; }
   if (c != ',') { istr.setstate(std::ios_base::failbit); return; }

   if (!readAttributeIdentifier(istr, s)) { return; }
   if (s != "transcription") { istr.setstate(std::ios_base::failbit); return; }
   istr >> c; if (!istr) { return; }
   if (c != '=') { istr.setstate(std::ios_base::failbit); return; }
   if (withSuffix)
     SPL::deserializeWithSuffix(istr, transcription_);
   else
     istr >> transcription_;
   if (!istr) { return; }  
   istr >> c; if (!istr) { return; }
   if (c != ',') { istr.setstate(std::ios_base::failbit); return; }

   if (!readAttributeIdentifier(istr, s)) { return; }
   if (s != "wordCount") { istr.setstate(std::ios_base::failbit); return; }
   istr >> c; if (!istr) { return; }
   if (c != '=') { istr.setstate(std::ios_base::failbit); return; }
   if (withSuffix)
     SPL::deserializeWithSuffix(istr, wordCount_);
   else
     istr >> wordCount_;
   if (!istr) { return; }  
   istr >> c; if (!istr) { return; }
   if (c != ',') { istr.setstate(std::ios_base::failbit); return; }

   if (!readAttributeIdentifier(istr, s)) { return; }
   if (s != "charCount") { istr.setstate(std::ios_base::failbit); return; }
   istr >> c; if (!istr) { return; }
   if (c != '=') { istr.setstate(std::ios_base::failbit); return; }
   if (withSuffix)
     SPL::deserializeWithSuffix(istr, charCount_);
   else
     istr >> charCount_;
   if (!istr) { return; }  
   istr >> c; if (!istr) { return; }
   if (c != ',') { istr.setstate(std::ios_base::failbit); return; }

   if (!readAttributeIdentifier(istr, s)) { return; }
   if (s != "audioDurationSec") { istr.setstate(std::ios_base::failbit); return; }
   istr >> c; if (!istr) { return; }
   if (c != '=') { istr.setstate(std::ios_base::failbit); return; }
   if (withSuffix)
     SPL::deserializeWithSuffix(istr, audioDurationSec_);
   else
     istr >> audioDurationSec_;
   if (!istr) { return; }  
   istr >> c; if (!istr) { return; }
   if (c != ',') { istr.setstate(std::ios_base::failbit); return; }

   if (!readAttributeIdentifier(istr, s)) { return; }
   if (s != "processingTimeSec") { istr.setstate(std::ios_base::failbit); return; }
   istr >> c; if (!istr) { return; }
   if (c != '=') { istr.setstate(std::ios_base::failbit); return; }
   if (withSuffix)
     SPL::deserializeWithSuffix(istr, processingTimeSec_);
   else
     istr >> processingTimeSec_;
   if (!istr) { return; }  
   istr >> c; if (!istr) { return; }
   if (c != ',') { istr.setstate(std::ios_base::failbit); return; }

   if (!readAttributeIdentifier(istr, s)) { return; }
   if (s != "speedupFactor") { istr.setstate(std::ios_base::failbit); return; }
   istr >> c; if (!istr) { return; }
   if (c != '=') { istr.setstate(std::ios_base::failbit); return; }
   if (withSuffix)
     SPL::deserializeWithSuffix(istr, speedupFactor_);
   else
     istr >> speedupFactor_;
   if (!istr) { return; }  
   istr >> c; if (!istr) { return; }
   if (c != ',') { istr.setstate(std::ios_base::failbit); return; }

   if (!readAttributeIdentifier(istr, s)) { return; }
   if (s != "startTime") { istr.setstate(std::ios_base::failbit); return; }
   istr >> c; if (!istr) { return; }
   if (c != '=') { istr.setstate(std::ios_base::failbit); return; }
   if (withSuffix)
     SPL::deserializeWithSuffix(istr, startTime_);
   else
     istr >> startTime_;
   if (!istr) { return; }  
   istr >> c; if (!istr) { return; }
   if (c != ',') { istr.setstate(std::ios_base::failbit); return; }

   if (!readAttributeIdentifier(istr, s)) { return; }
   if (s != "endTime") { istr.setstate(std::ios_base::failbit); return; }
   istr >> c; if (!istr) { return; }
   if (c != '=') { istr.setstate(std::ios_base::failbit); return; }
   if (withSuffix)
     SPL::deserializeWithSuffix(istr, endTime_);
   else
     istr >> endTime_;
   if (!istr) { return; }  
   istr >> c; if (!istr) { return; }
   
   if (c != '}') { istr.setstate(std::ios_base::failbit); return; }
}

void SELF::deserializeWithNanAndInfs(std::istream & istr, bool withSuffix)
{
   std::string s;
   char c;

   istr >> c; if (!istr) { return; }
   if (c != '{') { istr.setstate(std::ios_base::failbit); return; }
   
   if (!readAttributeIdentifier(istr, s)) { return; }
   if (s != "filename") { istr.setstate(std::ios_base::failbit); return; }
   istr >> c; if (!istr) { return; }
   if (c != '=') { istr.setstate(std::ios_base::failbit); return; }
   SPL::deserializeWithNanAndInfs(istr, filename_, withSuffix);
   if (!istr) { return; }  
   istr >> c; if (!istr) { return; }
   if (c != ',') { istr.setstate(std::ios_base::failbit); return; }

   if (!readAttributeIdentifier(istr, s)) { return; }
   if (s != "transcription") { istr.setstate(std::ios_base::failbit); return; }
   istr >> c; if (!istr) { return; }
   if (c != '=') { istr.setstate(std::ios_base::failbit); return; }
   SPL::deserializeWithNanAndInfs(istr, transcription_, withSuffix);
   if (!istr) { return; }  
   istr >> c; if (!istr) { return; }
   if (c != ',') { istr.setstate(std::ios_base::failbit); return; }

   if (!readAttributeIdentifier(istr, s)) { return; }
   if (s != "wordCount") { istr.setstate(std::ios_base::failbit); return; }
   istr >> c; if (!istr) { return; }
   if (c != '=') { istr.setstate(std::ios_base::failbit); return; }
   SPL::deserializeWithNanAndInfs(istr, wordCount_, withSuffix);
   if (!istr) { return; }  
   istr >> c; if (!istr) { return; }
   if (c != ',') { istr.setstate(std::ios_base::failbit); return; }

   if (!readAttributeIdentifier(istr, s)) { return; }
   if (s != "charCount") { istr.setstate(std::ios_base::failbit); return; }
   istr >> c; if (!istr) { return; }
   if (c != '=') { istr.setstate(std::ios_base::failbit); return; }
   SPL::deserializeWithNanAndInfs(istr, charCount_, withSuffix);
   if (!istr) { return; }  
   istr >> c; if (!istr) { return; }
   if (c != ',') { istr.setstate(std::ios_base::failbit); return; }

   if (!readAttributeIdentifier(istr, s)) { return; }
   if (s != "audioDurationSec") { istr.setstate(std::ios_base::failbit); return; }
   istr >> c; if (!istr) { return; }
   if (c != '=') { istr.setstate(std::ios_base::failbit); return; }
   SPL::deserializeWithNanAndInfs(istr, audioDurationSec_, withSuffix);
   if (!istr) { return; }  
   istr >> c; if (!istr) { return; }
   if (c != ',') { istr.setstate(std::ios_base::failbit); return; }

   if (!readAttributeIdentifier(istr, s)) { return; }
   if (s != "processingTimeSec") { istr.setstate(std::ios_base::failbit); return; }
   istr >> c; if (!istr) { return; }
   if (c != '=') { istr.setstate(std::ios_base::failbit); return; }
   SPL::deserializeWithNanAndInfs(istr, processingTimeSec_, withSuffix);
   if (!istr) { return; }  
   istr >> c; if (!istr) { return; }
   if (c != ',') { istr.setstate(std::ios_base::failbit); return; }

   if (!readAttributeIdentifier(istr, s)) { return; }
   if (s != "speedupFactor") { istr.setstate(std::ios_base::failbit); return; }
   istr >> c; if (!istr) { return; }
   if (c != '=') { istr.setstate(std::ios_base::failbit); return; }
   SPL::deserializeWithNanAndInfs(istr, speedupFactor_, withSuffix);
   if (!istr) { return; }  
   istr >> c; if (!istr) { return; }
   if (c != ',') { istr.setstate(std::ios_base::failbit); return; }

   if (!readAttributeIdentifier(istr, s)) { return; }
   if (s != "startTime") { istr.setstate(std::ios_base::failbit); return; }
   istr >> c; if (!istr) { return; }
   if (c != '=') { istr.setstate(std::ios_base::failbit); return; }
   SPL::deserializeWithNanAndInfs(istr, startTime_, withSuffix);
   if (!istr) { return; }  
   istr >> c; if (!istr) { return; }
   if (c != ',') { istr.setstate(std::ios_base::failbit); return; }

   if (!readAttributeIdentifier(istr, s)) { return; }
   if (s != "endTime") { istr.setstate(std::ios_base::failbit); return; }
   istr >> c; if (!istr) { return; }
   if (c != '=') { istr.setstate(std::ios_base::failbit); return; }
   SPL::deserializeWithNanAndInfs(istr, endTime_, withSuffix);
   if (!istr) { return; }  
   istr >> c; if (!istr) { return; }
   
   if (c != '}') { istr.setstate(std::ios_base::failbit); return; }
}

void SELF::serialize(std::ostream & ostr) const
{
    ostr << '{'
         << "filename=" << get_filename()  << ","
         << "transcription=" << get_transcription()  << ","
         << "wordCount=" << get_wordCount()  << ","
         << "charCount=" << get_charCount()  << ","
         << "audioDurationSec=" << get_audioDurationSec()  << ","
         << "processingTimeSec=" << get_processingTimeSec()  << ","
         << "speedupFactor=" << get_speedupFactor()  << ","
         << "startTime=" << get_startTime()  << ","
         << "endTime=" << get_endTime()  
         << '}';
}

void SELF::serializeWithPrecision(std::ostream & ostr) const
{
    ostr << '{';
    SPL::serializeWithPrecision(ostr << "filename=", get_filename()) << ",";
    SPL::serializeWithPrecision(ostr << "transcription=", get_transcription()) << ",";
    SPL::serializeWithPrecision(ostr << "wordCount=", get_wordCount()) << ",";
    SPL::serializeWithPrecision(ostr << "charCount=", get_charCount()) << ",";
    SPL::serializeWithPrecision(ostr << "audioDurationSec=", get_audioDurationSec()) << ",";
    SPL::serializeWithPrecision(ostr << "processingTimeSec=", get_processingTimeSec()) << ",";
    SPL::serializeWithPrecision(ostr << "speedupFactor=", get_speedupFactor()) << ",";
    SPL::serializeWithPrecision(ostr << "startTime=", get_startTime()) << ",";
    SPL::serializeWithPrecision(ostr << "endTime=", get_endTime()) ;
    ostr << '}';
}

SELF& SELF::clear()
{
    SPL::rstring().swap(get_filename());
    SPL::rstring().swap(get_transcription());
    get_wordCount() = 0;
    get_charCount() = 0;
    get_audioDurationSec() = 0;
    get_processingTimeSec() = 0;
    get_speedupFactor() = 0;
    get_startTime() = SPL::timestamp(0);
    get_endTime() = SPL::timestamp(0);

    return *this;
}

void SELF::normalizeBoundedSetsAndMaps()
{
    SPL::normalizeBoundedSetsAndMaps(*this);
}


