#include "buffer.hpp"

namespace GL
{
   VAO::VAO()
   {
      GLSYM(glGenVertexArrays)(1, &obj);
   }

   VAO::~VAO()
   {
      GLSYM(glDeleteVertexArrays)(1, &obj);
   }

   void VAO::unbind()
   {
      GLSYM(glBindVertexArray)(0);
   }

   void VAO::bind()
   {
      GLSYM(glBindVertexArray)(obj);
   }

   Buffer::Buffer(GLenum type_) : type(type_)
   {
      GLSYM(glGenBuffers)(1, &obj);
   }

   Buffer::~Buffer()
   {
      GLSYM(glDeleteBuffers)(1, &obj);
   }

   void Buffer::bind()
   {
      GLSYM(glBindBuffer)(type, obj);
   }

   void Buffer::unbind(GLenum type)
   {
      GLSYM(glBindBuffer)(type, 0);
   }
}
