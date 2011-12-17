#include "mesh.hpp"
#include "object.hpp"
#include <vector>
#include <iostream>
#include <assert.h>

namespace GL
{
   Mesh::Mesh(const std::string &obj) : 
      num_vertices(0), vbo(GL_ARRAY_BUFFER)
   {
      load_object(obj);
   }

   Mesh::Mesh(const std::vector<Geo::Triangle> &triangles) :
      num_vertices(0), vbo(GL_ARRAY_BUFFER)
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

   void Mesh::load_object(const std::vector<Geo::Triangle> &triangles)
   {
      vao.bind();
      vbo.bind();
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

      VAO::unbind();
      Buffer::unbind(GL_ARRAY_BUFFER);
   }

   void Mesh::load_object(const std::string &obj)
   {
      load_object(GLU::LoadObject(obj));
   }

   void Mesh::set_texture(Texture::Ptr tex)
   {
      this->tex = tex;
   }

   void Mesh::set_transform(const GLMatrix &matrix)
   {
      trans_matrix = matrix;
   }

   void Mesh::set_normal(const GLMatrix &matrix)
   {
      normal_matrix = matrix;
   }

   void Mesh::set_light_transform(const GLMatrix &matrix)
   {
      transforms.light_matrix = matrix;
   }

   void Mesh::set_projection(const GLMatrix &matrix)
   {
      transforms.projection = matrix;
   }

   void Mesh::set_camera(const GLMatrix &matrix)
   {
      transforms.camera = matrix;
   }

   void Mesh::set_uniforms()
   {
      set_transforms();
      set_lights();
   }

   void Mesh::set_light(unsigned index, const vec3 &pos, const vec3 &color)
   {
      if (index >= max_lights)
         throw Exception("Light index out of bounds ...\n");

      lights.light_pos[index] = vec_conv<3, 4>(pos);
      lights.light_color[index] = vec_conv<3, 4>(color);
      light_enabled[index] = true;
   }

   void Mesh::set_ambient(const vec3 &color)
   {
      lights.light_ambient = vec_conv<3, 4>(color);
   }

   void Mesh::set_player_pos(const vec3 &pos)
   {
      player_pos = pos;
   }

   void Mesh::unset_light(unsigned index)
   {
      light_enabled[index] = false;
   }

   void Mesh::set_transforms()
   {
      GLSYM(glUniform1i)(shader->uniform("texture"), 0);
      GLSYM(glUniform1i)(shader->uniform("shadow_texture"), 1);

      auto proj = transforms.projection * transforms.camera * trans_matrix;
      auto light = transforms.light_matrix * trans_matrix;

      GLSYM(glUniformMatrix4fv)(shader->uniform("projection_matrix"), 1,
            GL_TRUE, proj());
      GLSYM(glUniformMatrix4fv)(shader->uniform("light_matrix"), 1,
            GL_TRUE, light());
      GLSYM(glUniformMatrix4fv)(shader->uniform("trans_matrix"), 1, 
            GL_TRUE, trans_matrix());
      GLSYM(glUniformMatrix4fv)(shader->uniform("normal_matrix"), 1, 
            GL_TRUE, normal_matrix());
   }

   void Mesh::set_lights()
   {
      Lights li;
      li.lights = 0;
      li.light_ambient = lights.light_ambient;
      for (unsigned i = 0; i < max_lights; i++)
      {
         if (!light_enabled[i])
            continue;

         std::copy(lights.light_pos[i](), lights.light_pos[i]() + 4,
               li.light_pos[li.lights]());
         std::copy(lights.light_color[i](), lights.light_color[i]() + 4,
               li.light_color[li.lights]());
         li.lights++;
      }

      GLSYM(glUniform1i)(shader->uniform("lights_count"), li.lights);
      GLSYM(glUniform3f)(shader->uniform("light_ambient"),
            li.light_ambient(0), li.light_ambient(1), li.light_ambient(2));
      GLSYM(glUniform3fv)(shader->uniform("lights_pos"), li.lights,
            li.light_pos[0]());
      GLSYM(glUniform3fv)(shader->uniform("lights_color"), li.lights,
            li.light_color[0]());
      GLSYM(glUniform3f)(shader->uniform("player_pos"),
            player_pos(0), player_pos(1), player_pos(2));
   }

   Program::Ptr Mesh::shader;
   Mesh::Transforms Mesh::transforms;
   Mesh::Lights Mesh::lights;
   std::array<bool, Mesh::max_lights> Mesh::light_enabled;
   GL::vec3 Mesh::player_pos;
}

