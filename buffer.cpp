#include "buffer.hpp"

namespace GL
{
   VAO::VAO()
   {
      glGenVertexArrays(1, &obj);
   }

   VAO::~VAO()
   {
      glDeleteVertexArrays(1, &obj);
   }

   void VAO::unbind()
   {
      glBindVertexArray(0);
   }

   void VAO::bind()
   {
      glBindVertexArray(obj);
   }

   Buffer::Buffer(GLenum type_) : type(type_)
   {
      glGenBuffers(1, &obj);
   }

   Buffer::~Buffer()
   {
      glDeleteBuffers(1, &obj);
   }

   void Buffer::bind()
   {
      glBindBuffer(type, obj);
   }

   void Buffer::unbind(GLenum type)
   {
      glBindBuffer(type, 0);
   }
}
