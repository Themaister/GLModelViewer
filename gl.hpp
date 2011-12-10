
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
}

#include "window.hpp"

namespace GL
{
   template <class Func, class T>
   inline Func sym_to_func(const T &gl_sym)
   {
      auto &map = Window::get()->symbol(gl_sym);

      sgl_function_t symbol = map;
      if (!symbol)
      {
         symbol = sgl_get_proc_address(gl_sym);
         if (!symbol)
            throw Exception(GLU::join("GL Symbol ", gl_sym, " not found!"));

         map = symbol;
      }

      return reinterpret_cast<Func>(symbol);
   }
}

#endif
