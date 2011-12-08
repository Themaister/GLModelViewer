#ifndef BUFFER_HPP__
#define BUFFER_HPP__

#include "gl.hpp"
#include "utils.hpp"
#include "shader.hpp"
#include "window.hpp"

namespace GL
{
   class VAO : private GLU::SmartDefs<VAO>, public GLResource
   {
      public:
         DECL_SHARED(VAO);
         VAO();
         ~VAO();
         void bind();

         static void unbind();

         void operator=(const VAO&) = delete;

      private:
         GLuint obj;
   };

   class Buffer : private GLU::SmartDefs<Buffer>, public GLResource
   {
      public:
         DECL_SHARED(Buffer);
         Buffer(GLenum type);
         ~Buffer();
         void bind();

         static void unbind(GLenum type);

         void operator=(const Buffer&) = delete;

      private:
         GLenum type;
         GLuint obj;
   };

   class UniformBuffer : private GLU::SmartDefs<UniformBuffer>, public GLResource
   {
      public:
         DECL_SHARED(UniformBuffer);
         UniformBuffer();
         ~UniformBuffer();

         void bind();
         void bind(unsigned index);
         static void unbind();

         void bind_block(Program::Ptr prog, unsigned block_index);

         void operator=(const UniformBuffer&) = delete;

      private:
         GLuint obj;
         unsigned bound_target;
         Window::Ptr win_hold;
   };
}

#endif
