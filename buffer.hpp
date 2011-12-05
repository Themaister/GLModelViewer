#ifndef BUFFER_HPP__
#define BUFFER_HPP__

#include "gl.hpp"
#include "utils.hpp"

namespace GL
{
   class VAO : private GLU::SmartDefs<VAO>
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

   class Buffer : private GLU::SmartDefs<Buffer>
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
}

#endif
