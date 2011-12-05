#include "gl.hpp"

namespace GL
{
   Exception::Exception(const std::string& in_msg) : msg(in_msg)
   {}

   Exception::Exception() : msg("Unknown exception")
   {}

   const char* Exception::what() const throw()
   {
      return msg.c_str();
   }

   Exception::~Exception() throw()
   {}
}
