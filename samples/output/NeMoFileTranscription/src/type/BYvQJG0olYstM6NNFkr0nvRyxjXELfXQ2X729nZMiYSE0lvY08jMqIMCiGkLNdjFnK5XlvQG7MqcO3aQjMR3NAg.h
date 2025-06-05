// y5_6mono8k7mono16k7mono22k7mono44k7mono48k


#ifndef BYVQJG0OLYSTM6NNFKR0NVRYXJXELFXQ2X729NZMIYSE0LVY08JMQIMCIGKLNDJFNK5XLVQG7MQCO3AQJMR3NAG_H_
#define BYVQJG0OLYSTM6NNFKR0NVRYXJXELFXQ2X729NZMIYSE0LVY08JMQIMCIGKLNDJFNK5XLVQG7MQCO3AQJMR3NAG_H_

#include <SPL/Runtime/Type/Enum.h>

#define SELF BYvQJG0olYstM6NNFkr0nvRyxjXELfXQ2X729nZMiYSE0lvY08jMqIMCiGkLNdjFnK5XlvQG7MqcO3aQjMR3NAg

namespace SPL {

class SELF : public Enum
{
public:
   typedef SELF Self;

   static SELF mono8k;
   static SELF mono16k;
   static SELF mono22k;
   static SELF mono44k;
   static SELF mono48k;
   

   SELF() : Enum(*mappings_) { }
   SELF(uint32_t v) : Enum(*mappings_, v) { }
   SELF(const Self & ot) : Enum(ot) { }
   SELF(const Enum& ot) : Enum(*mappings_) { assignFrom(ot); }
   SELF(const ConstValueHandle & ot) : Enum(ot) { }
   SELF(const std::string & v);
   SELF(const rstring & v);

   virtual Enum * clone() const { return new Self(*this); }

   Self & operator=(const Self & ot) { index_ = ot.index_; return *this; }

   bool operator==(const Self & ot) const { return index_ == ot.index_; }
   bool operator!=(const Self & ot) const { return index_ != ot.index_; }
   bool operator>(const Self & ot) const { return index_ > ot.index_; }
   bool operator>=(const Self & ot) const { return index_ >= ot.index_; }
   bool operator<(const Self & ot) const { return index_ < ot.index_; }
   bool operator<=(const Self & ot) const { return index_ <= ot.index_; }

   bool operator==(const Enum & ot) const { return index_ == ot.getIndex(); }
   bool operator!=(const Enum & ot) const { return index_ != ot.getIndex(); }
   bool operator>(const Enum & ot) const { return index_ > ot.getIndex(); }
   bool operator>=(const Enum & ot) const { return index_ >= ot.getIndex(); }
   bool operator<(const Enum & ot) const { return index_ < ot.getIndex(); }
   bool operator<=(const Enum & ot) const { return index_ <= ot.getIndex(); }

   SELF& operator= (uint32_t v) { index_ = v; return *this; }

private:
   static EnumMappings* mappings_;
   static EnumMappings* initMappings();
};

// Helper for parsing CSV.
template <class T, unsigned char separator>
class CSVExtractor;

template <unsigned char separator>
class CSVExtractor<SELF, separator>
{
public:
    // Extract one token of type T from stream fs.
    static inline void extract(std::istream & fs, SELF& field) {
        field.deserialize(fs, separator);
    }
};

}  /* namespace SPL */

namespace std { 
        template<>
        struct hash<SPL::SELF> {
            inline size_t operator()(const SPL::SELF & self) const { return self.hashCode(); }
        };
}

#undef SELF
#endif // BYVQJG0OLYSTM6NNFKR0NVRYXJXELFXQ2X729NZMIYSE0LVY08JMQIMCIGKLNDJFNK5XLVQG7MQCO3AQJMR3NAG_H_
