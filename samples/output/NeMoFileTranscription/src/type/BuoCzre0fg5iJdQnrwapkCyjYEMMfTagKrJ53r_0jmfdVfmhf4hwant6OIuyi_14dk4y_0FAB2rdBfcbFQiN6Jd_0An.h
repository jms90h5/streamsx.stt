// y5_3csv3txt3bin5block4line


#ifndef BUOCZRE0FG5IJDQNRWAPKCYJYEMMFTAGKRJ53R_0JMFDVFMHF4HWANT6OIUYI_14DK4Y_0FAB2RDBFCBFQIN6JD_0AN_H_
#define BUOCZRE0FG5IJDQNRWAPKCYJYEMMFTAGKRJ53R_0JMFDVFMHF4HWANT6OIUYI_14DK4Y_0FAB2RDBFCBFQIN6JD_0AN_H_

#include <SPL/Runtime/Type/Enum.h>

#define SELF BuoCzre0fg5iJdQnrwapkCyjYEMMfTagKrJ53r_0jmfdVfmhf4hwant6OIuyi_14dk4y_0FAB2rdBfcbFQiN6Jd_0An

namespace SPL {

class SELF : public Enum
{
public:
   typedef SELF Self;

   static SELF csv;
   static SELF txt;
   static SELF bin;
   static SELF block;
   static SELF line;
   

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
#endif // BUOCZRE0FG5IJDQNRWAPKCYJYEMMFTAGKRJ53R_0JMFDVFMHF4HWANT6OIUYI_14DK4Y_0FAB2RDBFCBFQIN6JD_0AN_H_
