#include "buffer.hpp"
#include <iostream>

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

   UniformBuffer::UniformBuffer() : bound_target(0)
   {
      GLSYM(glGenBuffers)(1, &obj);
   }

   UniformBuffer::~UniformBuffer()
   {
      GLSYM(glDeleteBuffers)(1, &obj);
   }

   void UniformBuffer::bind()
   {
      GLSYM(glBindBuffer)(GL_UNIFORM_BUFFER, obj);
   }

   void UniformBuffer::unbind()
   {
      GLSYM(glBindBuffer)(GL_UNIFORM_BUFFER, 0);
   }

   void UniformBuffer::bind(unsigned index)
   {
      GLSYM(glBindBufferBase)(GL_UNIFORM_BUFFER, index, obj);
      bound_target = index;
   }

   void UniformBuffer::bind_block(Program::Ptr prog, unsigned block_index)
   {
      prog->uniform_block_binding(block_index, bound_target);
   }
}

