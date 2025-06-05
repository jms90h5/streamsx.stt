// t9s8filenames13transcriptioni9wordCounti9charCountF16audioDurationSecF17processingTimeSecF13speedupFactorT9startTimeT7endTime


#ifndef BS5PNMW71Y7PKAPZ9LQBSWKRKBWCRIWV7K_0ARLSOM1RLHRNGAEBVK5TPN6YTNJDDDOECBR6NREFDZTOLVZ3WTD9_H_
#define BS5PNMW71Y7PKAPZ9LQBSWKRKBWCRIWV7K_0ARLSOM1RLHRNGAEBVK5TPN6YTNJDDDOECBR6NREFDZTOLVZ3WTD9_H_

#include <SPL/Runtime/Type/Tuple.h>
#include <SPL/Runtime/Type/PrimitiveType.h>
#include <SPL/Runtime/Type/CollectionType.h>
#include <SPL/Runtime/Serialization/NetworkByteBuffer.h>
#include <SPL/Runtime/Serialization/NativeByteBuffer.h>
#include <SPL/Runtime/Serialization/VirtualByteBuffer.h>
#include <SPL/Runtime/Type/Optional.h>



#define SELF Bs5pNmw71y7PkapZ9lqBSwKrkbWCriwV7k_0ArlSoM1rLHrNgAEBvk5Tpn6YTnjdDdoEcBr6nreFdzTolVZ3wtD9

namespace SPL {

class SELF : public Tuple
{
public:
    static const bool facade = false;

    typedef SELF Self;
    
    typedef SPL::rstring filename_type;
    typedef SPL::rstring transcription_type;
    typedef SPL::int32 wordCount_type;
    typedef SPL::int32 charCount_type;
    typedef SPL::float64 audioDurationSec_type;
    typedef SPL::float64 processingTimeSec_type;
    typedef SPL::float64 speedupFactor_type;
    typedef SPL::timestamp startTime_type;
    typedef SPL::timestamp endTime_type;

    enum { num_attributes = 9 } ;
    
    SELF() : Tuple(), filename_(), transcription_(), wordCount_(), charCount_(), audioDurationSec_(), processingTimeSec_(), speedupFactor_(), startTime_(), endTime_() {}
    SELF(const Self & ot) : Tuple(), filename_(ot.filename_), transcription_(ot.transcription_), wordCount_(ot.wordCount_), charCount_(ot.charCount_), audioDurationSec_(ot.audioDurationSec_), processingTimeSec_(ot.processingTimeSec_), speedupFactor_(ot.speedupFactor_), startTime_(ot.startTime_), endTime_(ot.endTime_) 
      { constructPayload(ot); }
    SELF(const filename_type & _filename, const transcription_type & _transcription, const wordCount_type & _wordCount, const charCount_type & _charCount, const audioDurationSec_type & _audioDurationSec, const processingTimeSec_type & _processingTimeSec, const speedupFactor_type & _speedupFactor, const startTime_type & _startTime, const endTime_type & _endTime) : Tuple(), filename_(_filename), transcription_(_transcription), wordCount_(_wordCount), charCount_(_charCount), audioDurationSec_(_audioDurationSec), processingTimeSec_(_processingTimeSec), speedupFactor_(_speedupFactor), startTime_(_startTime), endTime_(_endTime) { }
    
    SELF(const Tuple & ot, bool typesafe = true) : Tuple() { assignFrom(ot, typesafe); }
    SELF(const ConstValueHandle & ot) : Tuple() { const Tuple & o = ot; assignFrom(o); }

    virtual ~SELF() {}
    
    filename_type & get_filename() { return filename_; }
    const filename_type & get_filename() const { return filename_; }
    void set_filename(const filename_type & _filename) { filename_ = _filename; }
    transcription_type & get_transcription() { return transcription_; }
    const transcription_type & get_transcription() const { return transcription_; }
    void set_transcription(const transcription_type & _transcription) { transcription_ = _transcription; }
    wordCount_type & get_wordCount() { return wordCount_; }
    const wordCount_type & get_wordCount() const { return wordCount_; }
    void set_wordCount(const wordCount_type & _wordCount) { wordCount_ = _wordCount; }
    charCount_type & get_charCount() { return charCount_; }
    const charCount_type & get_charCount() const { return charCount_; }
    void set_charCount(const charCount_type & _charCount) { charCount_ = _charCount; }
    audioDurationSec_type & get_audioDurationSec() { return audioDurationSec_; }
    const audioDurationSec_type & get_audioDurationSec() const { return audioDurationSec_; }
    void set_audioDurationSec(const audioDurationSec_type & _audioDurationSec) { audioDurationSec_ = _audioDurationSec; }
    processingTimeSec_type & get_processingTimeSec() { return processingTimeSec_; }
    const processingTimeSec_type & get_processingTimeSec() const { return processingTimeSec_; }
    void set_processingTimeSec(const processingTimeSec_type & _processingTimeSec) { processingTimeSec_ = _processingTimeSec; }
    speedupFactor_type & get_speedupFactor() { return speedupFactor_; }
    const speedupFactor_type & get_speedupFactor() const { return speedupFactor_; }
    void set_speedupFactor(const speedupFactor_type & _speedupFactor) { speedupFactor_ = _speedupFactor; }
    startTime_type & get_startTime() { return startTime_; }
    const startTime_type & get_startTime() const { return startTime_; }
    void set_startTime(const startTime_type & _startTime) { startTime_ = _startTime; }
    endTime_type & get_endTime() { return endTime_; }
    const endTime_type & get_endTime() const { return endTime_; }
    void set_endTime(const endTime_type & _endTime) { endTime_ = _endTime; }
    virtual bool equals(const Tuple & ot) const
    {

        if (typeid(*this) != typeid(ot)) { return false; }
        const SELF & o = static_cast<const SELF &>(ot);
        return (*this == o);

    }

    virtual SELF& clear();

    Tuple* clone() const { return new Self(*this); }
    
    void serialize(VirtualByteBuffer & buf) const
    {
        buf << filename_ << transcription_ << wordCount_ << charCount_ << audioDurationSec_ << processingTimeSec_ << speedupFactor_ << startTime_ << endTime_;
    }

    template <class BufferType>
    void serialize(ByteBuffer<BufferType> & buf) const
    {        
        buf << filename_ << transcription_ << wordCount_ << charCount_ << audioDurationSec_ << processingTimeSec_ << speedupFactor_ << startTime_ << endTime_;
    } 
    
    void serialize(NativeByteBuffer & buf) const
    {
        this->serialize<NativeByteBuffer>(buf);
    }

    void serialize(NetworkByteBuffer & buf) const
    {
        this->serialize<NetworkByteBuffer>(buf);
    }
    
    void deserialize(VirtualByteBuffer & buf)
    {
        buf >> filename_ >> transcription_ >> wordCount_ >> charCount_ >> audioDurationSec_ >> processingTimeSec_ >> speedupFactor_ >> startTime_ >> endTime_;
    }

    template <class BufferType>
    void deserialize(ByteBuffer<BufferType> & buf)
    {        
        buf >> filename_ >> transcription_ >> wordCount_ >> charCount_ >> audioDurationSec_ >> processingTimeSec_ >> speedupFactor_ >> startTime_ >> endTime_;
    } 

    void deserialize(NativeByteBuffer & buf)
    {
        this->deserialize<NativeByteBuffer>(buf);
    }    

    void deserialize(NetworkByteBuffer & buf)
    {
        this->deserialize<NetworkByteBuffer>(buf);
    }    

    void serialize(std::ostream & ostr) const;

    void serializeWithPrecision(std::ostream & ostr) const;

    void deserialize(std::istream & istr, bool withSuffix = false);
    
    void deserializeWithNanAndInfs(std::istream & istr, bool withSuffix = false);
    
    size_t hashCode() const
    {
        size_t s = 17;
        s = 37 * s + std::hash<filename_type >()(filename_);
        s = 37 * s + std::hash<transcription_type >()(transcription_);
        s = 37 * s + std::hash<wordCount_type >()(wordCount_);
        s = 37 * s + std::hash<charCount_type >()(charCount_);
        s = 37 * s + std::hash<audioDurationSec_type >()(audioDurationSec_);
        s = 37 * s + std::hash<processingTimeSec_type >()(processingTimeSec_);
        s = 37 * s + std::hash<speedupFactor_type >()(speedupFactor_);
        s = 37 * s + std::hash<startTime_type >()(startTime_);
        s = 37 * s + std::hash<endTime_type >()(endTime_);
        return s;
    }
    
    size_t getSerializedSize() const
    {
        size_t size = sizeof(SPL::int32)+sizeof(SPL::int32)+sizeof(SPL::float64)+sizeof(SPL::float64)+sizeof(SPL::float64)+sizeof(SPL::timestamp)+sizeof(SPL::timestamp);
           size += filename_.getSerializedSize();
   size += transcription_.getSerializedSize();

        return size;

    }
    
    uint32_t getNumberOfAttributes() const 
        { return num_attributes; }

    TupleIterator getBeginIterator() 
        { return TupleIterator(*this, 0); }
    
    ConstTupleIterator getBeginIterator() const 
        { return ConstTupleIterator(*this, 0); }

    TupleIterator getEndIterator() 
        { return TupleIterator(*this, num_attributes); }

    ConstTupleIterator getEndIterator() const 
        { return ConstTupleIterator(*this, num_attributes); }
    
    TupleIterator findAttribute(const std::string & attrb)
    {
        std::unordered_map<std::string, uint32_t>::const_iterator it = mappings_->nameToIndex_.find(attrb);
        if ( it == mappings_->nameToIndex_.end() ) {
            return this->getEndIterator();
        }
        return TupleIterator(*this, it->second);
    }
    
    ConstTupleIterator findAttribute(const std::string & attrb) const
        { return const_cast<Self*>(this)->findAttribute(attrb); }
    
    TupleAttribute getAttribute(uint32_t index)
    {
        if (index >= num_attributes)
            invalidIndex (index, num_attributes);
        return TupleAttribute(mappings_->indexToName_[index], index, *this);
    }
    
    ConstTupleAttribute getAttribute(uint32_t index) const
        { return const_cast<Self*>(this)->getAttribute(index); }

    ValueHandle getAttributeValue(const std::string & attrb)
        { return getAttributeValueRaw(lookupAttributeName(*mappings_, attrb)->second); }


    ConstValueHandle getAttributeValue(const std::string & attrb) const
        { return const_cast<Self*>(this)->getAttributeValue(attrb); }

    ValueHandle getAttributeValue(uint32_t index) 
        { return getAttributeValueRaw(index); }

    ConstValueHandle getAttributeValue(uint32_t index) const
        { return const_cast<Self*>(this)->getAttributeValue(index); }

    Self & operator=(const Self & ot) 
    { 
        filename_ = ot.filename_;
        transcription_ = ot.transcription_;
        wordCount_ = ot.wordCount_;
        charCount_ = ot.charCount_;
        audioDurationSec_ = ot.audioDurationSec_;
        processingTimeSec_ = ot.processingTimeSec_;
        speedupFactor_ = ot.speedupFactor_;
        startTime_ = ot.startTime_;
        endTime_ = ot.endTime_; 
        assignPayload(ot);
        return *this; 
    }

    Self & operator=(const Tuple & ot) 
    { 
        assignFrom(ot); 
        return *this; 
    }

    void assign(Tuple const & tuple)
    {
        *this = static_cast<SELF const &>(tuple);
    }


    bool operator==(const Self & ot) const 
    {  
       return ( 
                filename_ == ot.filename_ && 
                transcription_ == ot.transcription_ && 
                wordCount_ == ot.wordCount_ && 
                charCount_ == ot.charCount_ && 
                audioDurationSec_ == ot.audioDurationSec_ && 
                processingTimeSec_ == ot.processingTimeSec_ && 
                speedupFactor_ == ot.speedupFactor_ && 
                startTime_ == ot.startTime_ && 
                endTime_ == ot.endTime_  
              ); 
    }
    bool operator==(const Tuple & ot) const { return equals(ot); }

    bool operator!=(const Self & ot) const { return !(*this == ot); }
    bool operator!=(const Tuple & ot) const { return !(*this == ot); }


    void swap(SELF & ot) 
    { 
        std::swap(filename_, ot.filename_);
        std::swap(transcription_, ot.transcription_);
        std::swap(wordCount_, ot.wordCount_);
        std::swap(charCount_, ot.charCount_);
        std::swap(audioDurationSec_, ot.audioDurationSec_);
        std::swap(processingTimeSec_, ot.processingTimeSec_);
        std::swap(speedupFactor_, ot.speedupFactor_);
        std::swap(startTime_, ot.startTime_);
        std::swap(endTime_, ot.endTime_);
      std::swap(payload_, ot.payload_);
    }

    void reset()
    { 
        *this = SELF(); 
    }

    void normalizeBoundedSetsAndMaps(); 

    const std::string & getAttributeName(uint32_t index) const
    {
        if (index >= num_attributes)
            invalidIndex (index, num_attributes);
        return mappings_->indexToName_[index];
    }

    const std::unordered_map<std::string, uint32_t> & getAttributeNames() const 
        { return mappings_->nameToIndex_; }


protected:

    ValueHandle getAttributeValueRaw(const uint32_t index)
    {
        if (index >= num_attributes)
            invalidIndex(index, num_attributes);
        const TypeOffset & t = mappings_->indexToTypeOffset_[index];
        return ValueHandle((char*)this + t.getOffset(), t.getMetaType(), &t.getTypeId());
    }

private:
    
    filename_type filename_;
    transcription_type transcription_;
    wordCount_type wordCount_;
    charCount_type charCount_;
    audioDurationSec_type audioDurationSec_;
    processingTimeSec_type processingTimeSec_;
    speedupFactor_type speedupFactor_;
    startTime_type startTime_;
    endTime_type endTime_;

    static TupleMappings* mappings_;
    static TupleMappings* initMappings();
};

inline VirtualByteBuffer & operator>>(VirtualByteBuffer & sbuf, SELF & tuple) 
    { tuple.deserialize(sbuf); return sbuf; }
inline VirtualByteBuffer & operator<<(VirtualByteBuffer & sbuf, SELF const & tuple) 
    { tuple.serialize(sbuf); return sbuf; }

template <class BufferType>
inline ByteBuffer<BufferType> & operator>>(ByteBuffer<BufferType> & sbuf, SELF & tuple) 
    { tuple.deserialize(sbuf); return sbuf; }
template <class BufferType>
inline ByteBuffer<BufferType> & operator<<(ByteBuffer<BufferType> & sbuf, SELF const & tuple) 
    { tuple.serialize(sbuf); return sbuf; }

inline NetworkByteBuffer & operator>>(NetworkByteBuffer & sbuf, SELF & tuple) 
    { tuple.deserialize(sbuf); return sbuf; }
inline NetworkByteBuffer & operator<<(NetworkByteBuffer & sbuf, SELF const & tuple) 
    { tuple.serialize(sbuf); return sbuf; }

inline NativeByteBuffer & operator>>(NativeByteBuffer & sbuf, SELF & tuple) 
    { tuple.deserialize(sbuf); return sbuf; }
inline NativeByteBuffer & operator<<(NativeByteBuffer & sbuf, SELF const & tuple) 
    { tuple.serialize(sbuf); return sbuf; }

template<>
inline std::ostream & serializeWithPrecision(std::ostream & ostr, SELF const & tuple) 
    { tuple.serializeWithPrecision(ostr); return ostr; }
inline std::ostream & operator<<(std::ostream & ostr, SELF const & tuple) 
    { tuple.serialize(ostr); return ostr; }
inline std::istream & operator>>(std::istream & istr, SELF & tuple) 
    { tuple.deserialize(istr); return istr; }
template<>
inline void deserializeWithSuffix(std::istream & istr, SELF  & tuple) 
{ tuple.deserialize(istr,true);  }
inline void deserializeWithNanAndInfs(std::istream & istr, SELF  & tuple, bool withSuffix = false) 
{ tuple.deserializeWithNanAndInfs(istr,withSuffix);  }



}  // namespace SPL

namespace std
{
    inline void swap(SPL::SELF & a, SPL::SELF & b)
    {
        a.swap(b);
    }
}

namespace std { 
        template <>
        struct hash<SPL::SELF> {
            inline size_t operator()(const SPL::SELF & self) const 
                { return self.hashCode(); }
        };
}

#undef SELF
#endif // BS5PNMW71Y7PKAPZ9LQBSWKRKBWCRIWV7K_0ARLSOM1RLHRNGAEBVK5TPN6YTNJDDDOECBR6NREFDZTOLVZ3WTD9_H_ 


