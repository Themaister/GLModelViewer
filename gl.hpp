
#ifndef GL_HPP__
#define GL_HPP__

#include "sgl/sgl.h"

#include <stdexcept>
#include <string>
#include <cstring>
#include <map>

// On-the-fly extension wrangler :o
#define GLSYM(sym) (::GL::sym_to_func<decltype(&sym)>(#sym))

#include "utils.hpp"

namespace GL
{
   class Exception : public std::exception
   {
      public:
         Exception();
         Exception(const std::string& in_msg);
         const char* what() const throw();
         ~Exception() throw();

      protected:
         std::string msg;
   };

   extern std::map<std::string, sgl_function_t> symbol_map;

   template <class Func, class T>
   inline Func sym_to_func(const T &gl_sym)
   {
      sgl_function_t symbol = symbol_map[gl_sym];
      if (!symbol)
      {
         symbol = sgl_get_proc_address(gl_sym);
         if (!symbol)
            throw Exception(GLU::join("GL Symbol ", gl_sym, " not found!"));

         symbol_map[gl_sym] = symbol;
      }

      return reinterpret_cast<Func>(symbol);
   }
}

#endif
