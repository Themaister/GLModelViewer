#ifndef SHADER_HPP__
#define SHADER_HPP__

#include "gl.hpp"
#include <vector>
#include "utils.hpp"

namespace GL
{
   class ShaderException : public Exception
   {
      public:
         ShaderException(GLuint shader);
   };

   class Shader : private GLU::SmartDefs<Shader>, public GLResource
   {
      public:
         DECL_SHARED(Shader);
         enum class Type : unsigned
         {
            None,
            Vertex,
            Fragment
         };

         Shader(const std::string &source, Type type);

         ~Shader();

         GLuint object() const;
         void operator=(const Shader&) = delete;

      private:
         GLuint shader;
         Type type;
   };

   class Program : private GLU::SmartDefs<Program>, public GLResource
   {
      public:
         DECL_SHARED(Program);
         Program();
         ~Program();

         void add(const std::string &src, Shader::Type type);
         void add(const Shader &shader);
         void link();

         static void unbind();

         void use() const;
         GLuint object() const;
         bool linked() const;

         GLint uniform(const std::string &key) const;
         GLuint uniform_block_index(const std::string &key) const;
         void uniform_block_binding(unsigned block, unsigned index);
         GLint attrib(const std::string &key) const;

         void operator=(const Program&) = delete;

         enum
         {
            VertexStream = 0,
            TextureStream = 1,
            NormalStream = 2
         };

      private:
         GLuint program;
         std::vector<Shader::Ptr> shaders;
         bool m_linked;
   };
}

#endif

