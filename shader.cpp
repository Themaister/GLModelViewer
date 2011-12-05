#include "shader.hpp"
#include "utils.hpp"

namespace GL
{
   ShaderException::ShaderException(GLuint shader)
   {
      int info_len;
      int max_len;
      std::vector<char> buf;

      if (glIsShader(shader) == GL_TRUE)
         glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &max_len);
      else
         glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &max_len);

      buf = std::vector<char>();
      buf.reserve(max_len);

      if (glIsShader(shader) == GL_TRUE)
         glGetShaderInfoLog(shader, max_len, &info_len, &buf[0]);
      else
         glGetProgramInfoLog(shader, max_len, &info_len, &buf[0]);

      msg = GLU::join("Shader error: ", &buf[0]);
   }

   Shader::Shader() : shader(0), type(Shader::Type::None)
   {}

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

      shader = glCreateShader(gl_type);
      const char *src = source.c_str();
      glShaderSource(shader, 1, &src, 0);
      glCompileShader(shader);

      GLint status;
      glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
      if (status != GL_TRUE)
         throw ShaderException(shader);
   }

   GLuint Shader::object() const
   {
      if (shader == 0)
         throw Exception("Invalid shader object.\n");

      return shader;
   }

   Shader& Shader::operator=(Shader&& in)
   {
      if (glIsShader(shader))
         glDeleteShader(shader);

      type = in.type;
      shader = in.shader;
      in.shader = 0;
      in.type = Shader::Type::None;
      return *this;
   }

   Shader::Shader(Shader&& in)
   {
      *this = std::move(in);
   }

   Shader::~Shader()
   {
      if (glIsShader(shader))
         glDeleteShader(shader);
   }

   Program::Program() : program(glCreateProgram()), m_linked(false)
   {
      if (program == 0)
         throw Exception("Failed to create program.\n");
   }

   void Program::use() const
   {
      if (!m_linked)
         throw Exception("Program is not linked!\n");

      glUseProgram(program);
   }

   void Program::link()
   {
      for (auto &shader : shaders)
         glAttachShader(program, shader.object());

      glLinkProgram(program);
      GLint status;
      glGetProgramiv(program, GL_LINK_STATUS, &status);
      if (status != GL_TRUE)
         throw ShaderException(program);

      m_linked = true;
   }

   void Program::add(const std::string &src, Shader::Type type)
   {
      shaders.push_back(Shader(src, type));
   }

   void Program::add(const Shader &shader)
   {
      glAttachShader(program, shader.object());
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
      if (glIsProgram(program))
      {
         for (auto &shader : shaders)
            glDetachShader(program, shader.object());
         glDeleteProgram(program);
      }
   }

   Program& Program::operator=(Program&& in)
   {
      if (glIsProgram(program))
         glDeleteProgram(program);

      program = in.program;
      shaders = std::move(in.shaders);
      in.shaders.clear();
      m_linked = in.m_linked;

      in.program = 0;
      in.m_linked = false;
      return *this;
   }

   Program::Program(Program&& in)
   {
      *this = std::move(in);
   }

   GLint Program::uniform(const std::string &key) const
   {
      if (!m_linked)
         throw Exception("Program not linked.\n");

      auto ret = glGetUniformLocation(program, key.c_str());
      return ret;
   }

   GLint Program::attrib(const std::string &key) const
   {
      if (!m_linked)
         throw Exception("Program not linked.\n");
      return glGetAttribLocation(program, key.c_str());
   }

   void Program::unbind()
   {
      glUseProgram(0);
   }
}
