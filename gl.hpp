#ifndef GL_HPP__
#define GL_HPP__

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glfw.h>

#include <stdexcept>
#include <string>

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

#endif
