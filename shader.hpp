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

   class Shader : public GLResource
   {
      public:
         enum Type
         {
            None,
            Vertex,
            Fragment
         };

         Shader(const std::string &source, Type type);

         ~Shader();

         GLuint object() const;

      private:
         void operator=(const Shader&);
         GLuint shader;
         Type type;
   };

   class Program : public GLResource
   {
      public:
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

         enum
         {
            VertexStream = 0,
            TextureStream = 1,
            NormalStream = 2
         };

      private:
         void operator=(const Program&);
         GLuint program;
         std::vector<std::shared_ptr<Shader>> shaders;
         bool m_linked;
   };
}

#endif

