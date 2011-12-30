#ifndef BUFFER_HPP__
#define BUFFER_HPP__

#include "gl.hpp"
#include "utils.hpp"
#include "shader.hpp"
#include "window.hpp"

namespace GL
{
   class VAO : public GLResource
   {
      public:
         VAO();
         ~VAO();
         void bind();

         static void unbind();

      private:
         void operator=(const VAO&);
         GLuint obj;
   };

   class Buffer : public GLResource
   {
      public:
         Buffer(GLenum type);
         ~Buffer();
         void bind();

         static void unbind(GLenum type);

      private:
         void operator=(const Buffer&);
         GLenum type;
         GLuint obj;
   };

   class UniformBuffer : public GLResource
   {
      public:
         UniformBuffer();
         ~UniformBuffer();

         void bind();
         void bind(unsigned index);
         static void unbind();

         void bind_block(std::shared_ptr<Program> prog, GLuint block_index);

      private:
         void operator=(const UniformBuffer&);
         GLuint obj;
         unsigned bound_target;
   };
}

#endif
