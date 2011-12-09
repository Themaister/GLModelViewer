#include "mesh.hpp"
#include "object.hpp"
#include <vector>
#include <iostream>
#include <assert.h>

namespace GL
{
   Mesh::Mesh(const std::string &obj) : 
      num_vertices(0), vbo(GL_ARRAY_BUFFER), local_transforms_changed(false)
   {
      load_object(obj);
      init_uniform_buffers();
   }

   Mesh::Mesh(const std::vector<Geo::Triangle> &triangles) :
      num_vertices(0), vbo(GL_ARRAY_BUFFER), local_transforms_changed(false)
   {
      load_object(triangles);
      init_uniform_buffers();
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
      local_transforms_changed = true;
      trans_matrix = matrix;
   }

   void Mesh::set_normal(const GLMatrix &matrix)
   {
      local_transforms_changed = true;
      normal_matrix = matrix;
   }

   void Mesh::set_light_transform(const GLMatrix &matrix)
   {
      transforms_changed = true;
      transforms.light_matrix = matrix;
   }

   void Mesh::set_projection(const GLMatrix &matrix)
   {
      transforms_changed = true;
      transforms.projection = matrix;
   }

   void Mesh::set_camera(const GLMatrix &matrix)
   {
      transforms_changed = true;
      transforms.camera = matrix;
   }

   void Mesh::set_uniforms()
   {
      set_transforms();
      set_lights();
   }

   void Mesh::set_viewport_size(unsigned width, unsigned height)
   {
      lights_changed = true;
      lights.viewport_size = {width, height};
   }

   void Mesh::set_light(unsigned index, const vec3 &pos, const vec3 &color)
   {
      lights_changed = true;
      if (index >= max_lights)
         throw Exception("Light index out of bounds ...\n");

      lights.light_pos[index] = vec_conv<3, 4>(pos);
      lights.light_color[index] = vec_conv<3, 4>(color);
      light_enabled[index] = true;
   }

   void Mesh::set_ambient(const vec3 &color)
   {
      lights_changed = true;
      lights.light_ambient = vec_conv<3, 4>(color);
   }

   void Mesh::unset_light(unsigned index)
   {
      lights_changed = true;
      light_enabled[index] = false;
   }

   void Mesh::set_transforms()
   {
      GLSYM(glUniform1i)(shader->uniform("texture"), 0);
      GLSYM(glUniform1i)(shader->uniform("shadow_texture"), 1);

      if (transforms_changed)
      {
         trans_unibuf->bind();

         void *data = GLSYM(glMapBufferRange)(GL_UNIFORM_BUFFER, 0, sizeof(transforms),
               GL_MAP_WRITE_BIT);
         if (!data)
            throw Exception("Failed to map Uniform transform buffer!");
         std::memcpy(data, &transforms, sizeof(transforms));
         GLSYM(glUnmapBuffer)(GL_UNIFORM_BUFFER);

         UniformBuffer::unbind();
         transforms_changed = false;
      }

      if (local_transforms_changed)
      {
         GLSYM(glUniformMatrix4fv)(shader->uniform("trans_matrix"), 1, 
               GL_FALSE, trans_matrix());
         GLSYM(glUniformMatrix4fv)(shader->uniform("normal_matrix"), 1, 
               GL_FALSE, normal_matrix());
         local_transforms_changed = false;
      }

      trans_unibuf->bind_block(shader, shader->uniform_block_index("Transforms"));
   }

   void Mesh::set_lights()
   {
      if (lights_changed)
      {
         Lights li;
         li.lights = 0;
         li.light_ambient = lights.light_ambient;
         li.viewport_size = lights.viewport_size;
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

         lights_unibuf->bind();
         void *data = GLSYM(glMapBufferRange)(GL_UNIFORM_BUFFER, 0, sizeof(lights), GL_MAP_WRITE_BIT);
         if (!data)
            throw Exception("Failed to map Uniform light buffer!");
         std::memcpy(data, &li, sizeof(li));
         GLSYM(glUnmapBuffer)(GL_UNIFORM_BUFFER);

         UniformBuffer::unbind();
         lights_changed = false;
      }

      lights_unibuf->bind_block(shader,
            shader->uniform_block_index("Lights"));
   }

   void Mesh::init_uniform_buffers()
   {
      if (trans_unibuf && lights_unibuf)
         return;

      transforms_changed = false;
      lights_changed = false;

      trans_unibuf = UniformBuffer::shared();
      lights_unibuf = UniformBuffer::shared();

      trans_unibuf->bind();
      GLSYM(glBufferData)(GL_UNIFORM_BUFFER, sizeof(transforms),
            nullptr, GL_DYNAMIC_DRAW);
      lights_unibuf->bind();
      GLSYM(glBufferData)(GL_UNIFORM_BUFFER, sizeof(lights),
            nullptr, GL_DYNAMIC_DRAW);
      UniformBuffer::unbind();

      trans_unibuf->bind(0);
      lights_unibuf->bind(1);
   }

   Program::Ptr Mesh::shader;
   UniformBuffer::Ptr Mesh::trans_unibuf;
   UniformBuffer::Ptr Mesh::lights_unibuf;
   Mesh::Transforms Mesh::transforms;
   Mesh::Lights Mesh::lights;
   bool Mesh::transforms_changed;
   bool Mesh::lights_changed;
   std::array<bool, Mesh::max_lights> Mesh::light_enabled;
}

