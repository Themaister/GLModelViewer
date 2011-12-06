#include "mesh.hpp"
#include "object.hpp"
#include <vector>
#include <iostream>
#include <assert.h>

namespace GL
{
   Mesh::Mesh(const std::string &obj) : 
      num_vertices(0), vbo(GL_ARRAY_BUFFER),
      m_mvp_trans(false), m_trans_trans(false), 
      m_normal_trans(false), m_camera_trans(false),
      m_mvp_reset(false), m_trans_reset(false), 
      m_normal_reset(false), m_camera_reset(false)
   {
      load_object(obj);
   }

   Mesh::Mesh(const std::vector<Geo::Triangle> &triangles) :
      num_vertices(0), vbo(GL_ARRAY_BUFFER),
      m_mvp_trans(false), m_trans_trans(false), 
      m_normal_trans(false), m_camera_trans(false),
      m_mvp_reset(false), m_trans_reset(false), 
      m_normal_reset(false), m_camera_reset(false)
   {
      load_object(triangles);
   }

   void Mesh::set_shader(Program::Ptr shader_)
   {
      shader = shader_;
   }

   void Mesh::render()
   {
      if (shader)
         shader->use();
      set_uniforms();
      vao.bind();
      if (tex)
         tex->bind();

      GLSYM(glDrawArrays)(GL_TRIANGLES, 0, num_vertices);

      VAO::unbind();
      if (tex)
         tex->unbind();
      Program::unbind();
   }

   void Mesh::bind()
   {
      vao.bind();
      vbo.bind();
   }

   void Mesh::unbind()
   {
      VAO::unbind();
      Buffer::unbind(GL_ARRAY_BUFFER);
      Program::unbind();
   }

   void Mesh::load_object(const std::vector<Geo::Triangle> &triangles)
   {
      bind();
      num_vertices = triangles.size() * 3;

      GLSYM(glBufferData)(GL_ARRAY_BUFFER, triangles.size() * sizeof(Geo::Triangle), &triangles[0], GL_STATIC_DRAW);

      GLSYM(glVertexAttribPointer)(Program::VertexStream, 3, 
            GL_FLOAT, GL_FALSE, sizeof(Geo::Coord), (void*)Geo::VertexOffset);
      GLSYM(glEnableVertexAttribArray)(Program::VertexStream);

      GLSYM(glVertexAttribPointer)(Program::NormalStream, 3, 
            GL_FLOAT, GL_FALSE, sizeof(Geo::Coord), (void*)Geo::NormalOffset);
      GLSYM(glEnableVertexAttribArray)(Program::NormalStream);

      GLSYM(glVertexAttribPointer)(Program::TextureStream, 2, 
            GL_FLOAT, GL_FALSE, sizeof(Geo::Coord), (void*)Geo::TextureOffset);
      GLSYM(glEnableVertexAttribArray)(Program::TextureStream);

      unbind();
   }

   void Mesh::load_object(const std::string &obj)
   {
      load_object(GLU::LoadObject(obj));
   }

   void Mesh::set_mvp(const float *matrix, bool reset, bool transpose)
   {
      if (!shader)
         throw Exception("No shader bound!\n");

      shader->use();
      GLSYM(glUniformMatrix4fv)(shader->uniform("projection_matrix"), 1, 
            transpose ? GL_TRUE : GL_FALSE, matrix);
      Program::unbind();

      m_mvp_reset = reset;
      if (m_mvp_reset)
      {
         m_mvp_matrix = matrix;
         m_mvp_trans = transpose;
      }
   }

   void Mesh::set_transform(const float *matrix, bool reset, bool transpose)
   {
      if (!shader)
         throw Exception("No shader bound!\n");

      shader->use();
      GLSYM(glUniformMatrix4fv)(shader->uniform("trans_matrix"), 1, 
            transpose ? GL_TRUE : GL_FALSE, matrix);
      Program::unbind();

      m_trans_reset = reset;
      if (m_trans_reset)
      {
         m_trans_matrix = matrix;
         m_trans_trans = transpose;
      }
   }

   void Mesh::set_normal(const float *matrix, bool reset, bool transpose)
   {
      if (!shader)
         throw Exception("No shader bound!\n");

      shader->use();
      GLSYM(glUniformMatrix4fv)(shader->uniform("normal_matrix"), 1, 
            transpose ? GL_TRUE : GL_FALSE, matrix);
      Program::unbind();

      m_normal_reset = reset;
      if (m_normal_reset)
      {
         m_normal_matrix = matrix;
         m_normal_trans = transpose;
      }
   }

   void Mesh::set_camera(const float *matrix, bool reset, bool transpose)
   {
      if (!shader)
         throw Exception("No shader bound!\n");

      shader->use();
      GLSYM(glUniformMatrix4fv)(shader->uniform("camera_matrix"), 1, 
            transpose ? GL_TRUE : GL_FALSE, matrix);
      Program::unbind();

      m_camera_reset = reset;
      if (m_camera_reset)
      {
         m_camera_matrix = matrix;
         m_camera_trans = transpose;
      }
   }

   void Mesh::set_texture(Texture::Ptr tex)
   {
      this->tex = tex;
   }

   void Mesh::set_mvp(const GLMatrix &matrix, bool reset, bool transpose)
   {
      set_mvp(matrix(), reset, transpose);
   }

   void Mesh::set_transform(const GLMatrix &matrix, bool reset, bool transpose)
   {
      set_transform(matrix(), reset, transpose);
   }

   void Mesh::set_normal(const GLMatrix &matrix, bool reset, bool transpose)
   {
      set_normal(matrix(), reset, transpose);
   }

   void Mesh::set_camera(const GLMatrix &matrix, bool reset, bool transpose)
   {
      set_camera(matrix(), reset, transpose);
   }

   void Mesh::set_uniforms()
   {
      if (m_mvp_reset)
      {
         GLSYM(glUniformMatrix4fv)(shader->uniform("projection_matrix"), 1, 
               m_mvp_trans ? GL_TRUE : GL_FALSE, m_mvp_matrix());
      }
      if (m_trans_reset)
      {
         GLSYM(glUniformMatrix4fv)(shader->uniform("trans_matrix"), 1, 
               m_trans_trans ? GL_TRUE : GL_FALSE, m_trans_matrix());
      }
      if (m_normal_reset)
      {
         GLSYM(glUniformMatrix4fv)(shader->uniform("normal_matrix"), 1, 
               m_normal_trans ? GL_TRUE : GL_FALSE, m_normal_matrix());
      }
      if (m_camera_reset)
      {
         GLSYM(glUniformMatrix4fv)(shader->uniform("camera_matrix"), 1, 
               m_camera_trans ? GL_TRUE : GL_FALSE, m_camera_matrix());
      }

      GLSYM(glUniform1i)(shader->uniform("texture"), 0);

      set_lights();
   }

   void Mesh::set_light(unsigned index, const vec3 &pos, const vec3 &color)
   {
      if (index >= max_lights)
         throw Exception("Light index out of bounds ...\n");

      light_pos[index] = pos;
      light_color[index] = color;
      light_enabled[index] = true;
   }

   void Mesh::set_ambient(const vec3 &color)
   {
      light_ambient = color;
   }

   void Mesh::unset_light(unsigned index)
   {
      light_enabled[index] = false;
   }

   void Mesh::set_lights()
   {
      unsigned lights = 0;
      std::array<GLfloat, max_lights * 3> light;
      std::array<GLfloat, max_lights * 3> color;
      for (unsigned i = 0; i < max_lights; i++)
      {
         if (!light_enabled[i])
            continue;

         std::copy(light_pos[i](), light_pos[i]() + 3, light.begin() + 3 * lights);
         std::copy(light_color[i](), light_color[i]() + 3, color.begin() + 3 * lights);
         lights++;
      }

      GLSYM(glUniform3fv)(shader->uniform("light_ambient"), 1, light_ambient()); 
      GLSYM(glUniform3fv)(shader->uniform("lights_color"), lights, &color[0]);
      GLSYM(glUniform3fv)(shader->uniform("lights_pos"), lights, &light[0]);
      GLSYM(glUniform1i)(shader->uniform("lights_count"), lights);
   }
}
