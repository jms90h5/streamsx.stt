// t5s13transcriptionF16processingTimeMsF12audioChunkMsF13speedupFactori11chunkNumber


#ifndef B2RNRSKKRWYTTG0YXFKDRUYKBA8VL_1BHN_1BQQBQWMNZNKPMG1ALKG2HTZZPOUQ28LNRJGPRUEPHI34WV5PERWBO_H_
#define B2RNRSKKRWYTTG0YXFKDRUYKBA8VL_1BHN_1BQQBQWMNZNKPMG1ALKG2HTZZPOUQ28LNRJGPRUEPHI34WV5PERWBO_H_

#include <SPL/Runtime/Type/Tuple.h>
#include <SPL/Runtime/Type/PrimitiveType.h>
#include <SPL/Runtime/Type/CollectionType.h>
#include <SPL/Runtime/Serialization/NetworkByteBuffer.h>
#include <SPL/Runtime/Serialization/NativeByteBuffer.h>
#include <SPL/Runtime/Serialization/VirtualByteBuffer.h>
#include <SPL/Runtime/Type/Optional.h>



#define SELF B2rNrSkKrWYTTG0yXfkdrUykBA8vL_1Bhn_1bqqbqWmnzNkPmG1aLkG2HTzzPOuq28lNrJgpRUEPHI34Wv5perWBo

namespace SPL {

class SELF : public Tuple
{
public:
    static const bool facade = false;

    typedef SELF Self;
    
    typedef SPL::rstring transcription_type;
    typedef SPL::float64 processingTimeMs_type;
    typedef SPL::float64 audioChunkMs_type;
    typedef SPL::float64 speedupFactor_type;
    typedef SPL::int32 chunkNumber_type;

    enum { num_attributes = 5 } ;
    
    SELF() : Tuple(), transcription_(), processingTimeMs_(), audioChunkMs_(), speedupFactor_(), chunkNumber_() {}
    SELF(const Self & ot) : Tuple(), transcription_(ot.transcription_), processingTimeMs_(ot.processingTimeMs_), audioChunkMs_(ot.audioChunkMs_), speedupFactor_(ot.speedupFactor_), chunkNumber_(ot.chunkNumber_) 
      { constructPayload(ot); }
    SELF(const transcription_type & _transcription, const processingTimeMs_type & _processingTimeMs, const audioChunkMs_type & _audioChunkMs, const speedupFactor_type & _speedupFactor, const chunkNumber_type & _chunkNumber) : Tuple(), transcription_(_transcription), processingTimeMs_(_processingTimeMs), audioChunkMs_(_audioChunkMs), speedupFactor_(_speedupFactor), chunkNumber_(_chunkNumber) { }
    
    SELF(const Tuple & ot, bool typesafe = true) : Tuple() { assignFrom(ot, typesafe); }
    SELF(const ConstValueHandle & ot) : Tuple() { const Tuple & o = ot; assignFrom(o); }

    virtual ~SELF() {}
    
    transcription_type & get_transcription() { return transcription_; }
    const transcription_type & get_transcription() const { return transcription_; }
    void set_transcription(const transcription_type & _transcription) { transcription_ = _transcription; }
    processingTimeMs_type & get_processingTimeMs() { return processingTimeMs_; }
    const processingTimeMs_type & get_processingTimeMs() const { return processingTimeMs_; }
    void set_processingTimeMs(const processingTimeMs_type & _processingTimeMs) { processingTimeMs_ = _processingTimeMs; }
    audioChunkMs_type & get_audioChunkMs() { return audioChunkMs_; }
    const audioChunkMs_type & get_audioChunkMs() const { return audioChunkMs_; }
    void set_audioChunkMs(const audioChunkMs_type & _audioChunkMs) { audioChunkMs_ = _audioChunkMs; }
    speedupFactor_type & get_speedupFactor() { return speedupFactor_; }
    const speedupFactor_type & get_speedupFactor() const { return speedupFactor_; }
    void set_speedupFactor(const speedupFactor_type & _speedupFactor) { speedupFactor_ = _speedupFactor; }
    chunkNumber_type & get_chunkNumber() { return chunkNumber_; }
    const chunkNumber_type & get_chunkNumber() const { return chunkNumber_; }
    void set_chunkNumber(const chunkNumber_type & _chunkNumber) { chunkNumber_ = _chunkNumber; }
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
        buf << transcription_ << processingTimeMs_ << audioChunkMs_ << speedupFactor_ << chunkNumber_;
    }

    template <class BufferType>
    void serialize(ByteBuffer<BufferType> & buf) const
    {        
        buf << transcription_ << processingTimeMs_ << audioChunkMs_ << speedupFactor_ << chunkNumber_;
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
        buf >> transcription_ >> processingTimeMs_ >> audioChunkMs_ >> speedupFactor_ >> chunkNumber_;
    }

    template <class BufferType>
    void deserialize(ByteBuffer<BufferType> & buf)
    {        
        buf >> transcription_ >> processingTimeMs_ >> audioChunkMs_ >> speedupFactor_ >> chunkNumber_;
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
        s = 37 * s + std::hash<transcription_type >()(transcription_);
        s = 37 * s + std::hash<processingTimeMs_type >()(processingTimeMs_);
        s = 37 * s + std::hash<audioChunkMs_type >()(audioChunkMs_);
        s = 37 * s + std::hash<speedupFactor_type >()(speedupFactor_);
        s = 37 * s + std::hash<chunkNumber_type >()(chunkNumber_);
        return s;
    }
    
    size_t getSerializedSize() const
    {
        size_t size = sizeof(SPL::float64)+sizeof(SPL::float64)+sizeof(SPL::float64)+sizeof(SPL::int32);
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
        transcription_ = ot.transcription_;
        processingTimeMs_ = ot.processingTimeMs_;
        audioChunkMs_ = ot.audioChunkMs_;
        speedupFactor_ = ot.speedupFactor_;
        chunkNumber_ = ot.chunkNumber_; 
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
                transcription_ == ot.transcription_ && 
                processingTimeMs_ == ot.processingTimeMs_ && 
                audioChunkMs_ == ot.audioChunkMs_ && 
                speedupFactor_ == ot.speedupFactor_ && 
                chunkNumber_ == ot.chunkNumber_  
              ); 
    }
    bool operator==(const Tuple & ot) const { return equals(ot); }

    bool operator!=(const Self & ot) const { return !(*this == ot); }
    bool operator!=(const Tuple & ot) const { return !(*this == ot); }


    void swap(SELF & ot) 
    { 
        std::swap(transcription_, ot.transcription_);
        std::swap(processingTimeMs_, ot.processingTimeMs_);
        std::swap(audioChunkMs_, ot.audioChunkMs_);
        std::swap(speedupFactor_, ot.speedupFactor_);
        std::swap(chunkNumber_, ot.chunkNumber_);
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
    
    transcription_type transcription_;
    processingTimeMs_type processingTimeMs_;
    audioChunkMs_type audioChunkMs_;
    speedupFactor_type speedupFactor_;
    chunkNumber_type chunkNumber_;

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
#endif // B2RNRSKKRWYTTG0YXFKDRUYKBA8VL_1BHN_1BQQBQWMNZNKPMG1ALKG2HTZZPOUQ28LNRJGPRUEPHI34WV5PERWBO_H_ 


