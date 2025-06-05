// y5_6mono8k7mono16k7mono22k7mono44k7mono48k


#include "BYvQJG0olYstM6NNFkr0nvRyxjXELfXQ2X729nZMiYSE0lvY08jMqIMCiGkLNdjFnK5XlvQG7MqcO3aQjMR3NAg.h"

#define SELF BYvQJG0olYstM6NNFkr0nvRyxjXELfXQ2X729nZMiYSE0lvY08jMqIMCiGkLNdjFnK5XlvQG7MqcO3aQjMR3NAg

using namespace SPL;

EnumMappings* SELF::mappings_ = SELF::initMappings();


SELF SELF::mono8k(0);
SELF SELF::mono16k(1);
SELF SELF::mono22k(2);
SELF SELF::mono44k(3);
SELF SELF::mono48k(4);

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
      std::string s("mono8k");
      em->nameToIndex_.insert(std::make_pair(s, 0));
      em->indexToName_.push_back(s);
   }
   
   {
      std::string s("mono16k");
      em->nameToIndex_.insert(std::make_pair(s, 1));
      em->indexToName_.push_back(s);
   }
   
   {
      std::string s("mono22k");
      em->nameToIndex_.insert(std::make_pair(s, 2));
      em->indexToName_.push_back(s);
   }
   
   {
      std::string s("mono44k");
      em->nameToIndex_.insert(std::make_pair(s, 3));
      em->indexToName_.push_back(s);
   }
   
   {
      std::string s("mono48k");
      em->nameToIndex_.insert(std::make_pair(s, 4));
      em->indexToName_.push_back(s);
   }
   
   return em;
}
