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

   class Shader : private GLU::SmartDefs<Shader>
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

   class Program : private GLU::SmartDefs<Program>
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
         GLint attrib(const std::string &key) const;

         void operator=(const Program&) = delete;

         enum
         {
            VertexStream = 0,
            NormalStream = 1,
            TextureStream = 2,
            ColorStream = 3
         };

      private:
         GLuint program;
         std::vector<Shader::Ptr> shaders;
         bool m_linked;
   };

}

#endif
