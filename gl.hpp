
#ifndef GL_HPP__
#define GL_HPP__

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glfw.h>

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

   extern std::map<std::string, void *> symbol_map;

   template <class Func, class T>
   inline Func sym_to_func(const T &gl_sym)
   {
      void *symbol = symbol_map[gl_sym];
      if (!symbol)
      {
         symbol = glfwGetProcAddress(gl_sym);
         if (!symbol)
            throw Exception(GLU::join("GL Symbol ", gl_sym, " not found!"));

         symbol_map[gl_sym] = symbol;
      }

      static_assert(sizeof(Func) == sizeof(void *), "Function pointer size does not match void* ...");
      Func out;
      std::memcpy(&out, &symbol, sizeof(out));
      return out;
   }
}

#endif
