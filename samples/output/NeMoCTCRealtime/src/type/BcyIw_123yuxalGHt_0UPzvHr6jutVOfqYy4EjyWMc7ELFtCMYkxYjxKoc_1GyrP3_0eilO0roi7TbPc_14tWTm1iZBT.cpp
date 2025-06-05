// y2_12WindowMarker11FinalMarker


#include "BcyIw_123yuxalGHt_0UPzvHr6jutVOfqYy4EjyWMc7ELFtCMYkxYjxKoc_1GyrP3_0eilO0roi7TbPc_14tWTm1iZBT.h"

#define SELF BcyIw_123yuxalGHt_0UPzvHr6jutVOfqYy4EjyWMc7ELFtCMYkxYjxKoc_1GyrP3_0eilO0roi7TbPc_14tWTm1iZBT

using namespace SPL;

EnumMappings* SELF::mappings_ = SELF::initMappings();


SELF SELF::WindowMarker(0);
SELF SELF::FinalMarker(1);

SELF::SELF(const std::string & v)
: Enum(*mappings_)
{
    // initialize from a string value
    this->Enum::operator=(v);
}

SELF::SELF(const rstring & v)
: Enum(*mappings_)
{
    // initialize from a string value
    this->Enum::operator=(v);
}


EnumMappings* SELF::initMappings()
{
   EnumMappings* em = new EnumMappings();
   // initialize the mappings 
   {
      std::string s("WindowMarker");
      em->nameToIndex_.insert(std::make_pair(s, 0));
      em->indexToName_.push_back(s);
   }
   
   {
      std::string s("FinalMarker");
      em->nameToIndex_.insert(std::make_pair(s, 1));
      em->indexToName_.push_back(s);
   }
   
   return em;
}
