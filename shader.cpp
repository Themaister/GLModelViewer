#include "shader.hpp"
#include "utils.hpp"
#include <iostream>

namespace GL
{
   ShaderException::ShaderException(GLuint shader)
   {
      int info_len;
      int max_len;
      std::vector<char> buf;

      if (GLSYM(glIsShader)(shader) == GL_TRUE)
         GLSYM(glGetShaderiv)(shader, GL_INFO_LOG_LENGTH, &max_len);
      else
         GLSYM(glGetProgramiv)(shader, GL_INFO_LOG_LENGTH, &max_len);

      buf = std::vector<char>();
      buf.reserve(max_len);

      if (GLSYM(glIsShader)(shader) == GL_TRUE)
         GLSYM(glGetShaderInfoLog)(shader, max_len, &info_len, &buf[0]);
      else
         GLSYM(glGetProgramInfoLog)(shader, max_len, &info_len, &buf[0]);

      msg = GLU::join("Shader error: ", &buf[0]);
   }

   Shader::Shader(const std::string &source, Shader::Type type)
   {
      GLenum gl_type;
      switch (type)
      {
         case Shader::Type::Vertex:
            gl_type = GL_VERTEX_SHADER;
            break;

         case Shader::Type::Fragment:
            gl_type = GL_FRAGMENT_SHADER;
            break;

         default:
            throw Exception("Invalid shader type.\n");
      }

      shader = GLSYM(glCreateShader)(gl_type);
      const char *src = source.c_str();
      GLSYM(glShaderSource)(shader, 1, &src, 0);
      GLSYM(glCompileShader)(shader);

      GLint status;
      GLSYM(glGetShaderiv)(shader, GL_COMPILE_STATUS, &status);
      if (status != GL_TRUE)
         throw ShaderException(shader);
   }

   GLuint Shader::object() const
   {
      if (shader == 0)
         throw Exception("Invalid shader object.\n");

      return shader;
   }

   Shader::~Shader()
   {
      if (GLSYM(glIsShader)(shader))
         GLSYM(glDeleteShader)(shader);
   }

   Program::Program() : program(GLSYM(glCreateProgram)()), m_linked(false)
   {
      if (program == 0)
         throw Exception("Failed to create program.\n");
   }

   void Program::use() const
   {
      if (!m_linked)
         throw Exception("Program is not linked!\n");

      GLSYM(glUseProgram)(program);
   }

   void Program::link()
   {
      for (auto shader : shaders)
         GLSYM(glAttachShader)(program, shader->object());

      GLSYM(glLinkProgram)(program);
      GLint status;
      GLSYM(glGetProgramiv)(program, GL_LINK_STATUS, &status);
      if (status != GL_TRUE)
         throw ShaderException(program);

      GLSYM(glValidateProgram)(program);
      GLSYM(glGetProgramiv)(program, GL_VALIDATE_STATUS, &status);
      if (status != GL_TRUE)
         throw ShaderException(program);

      m_linked = true;
   }

   void Program::add(const std::string &src, Shader::Type type)
   {
      shaders.push_back(Shader::shared(src, type));
   }

   void Program::add(const Shader &shader)
   {
      GLSYM(glAttachShader)(program, shader.object());
   }

   GLuint Program::object() const
   {
      return program;
   }

   bool Program::linked() const
   {
      return m_linked;
   }

   Program::~Program()
   {
      if (GLSYM(glIsProgram)(program))
      {
         for (auto shader : shaders)
            GLSYM(glDetachShader)(program, shader->object());
         GLSYM(glDeleteProgram)(program);
      }
   }

   GLint Program::uniform(const std::string &key) const
   {
      if (!m_linked)
         throw Exception("Program not linked.\n");

      return GLSYM(glGetUniformLocation)(program, key.c_str());
   }

   GLuint Program::uniform_block_index(const std::string &key) const
   {
      if (!m_linked)
         throw Exception("Program not linked.\n");

      return GLSYM(glGetUniformBlockIndex)(program, key.c_str());
   }

   void Program::uniform_block_binding(unsigned block, unsigned index)
   {
      if (!m_linked)
         throw Exception("Program not linked.\n");

      GLSYM(glUniformBlockBinding)(program, block, index);
   }

   GLint Program::attrib(const std::string &key) const
   {
      if (!m_linked)
         throw Exception("Program not linked.\n");
      return GLSYM(glGetAttribLocation)(program, key.c_str());
   }

   void Program::unbind()
   {
      GLSYM(glUseProgram)(0);
   }
}
