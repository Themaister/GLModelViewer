#ifndef MESH_HPP__
#define MESH_HPP__

#include "gl.hpp"
#include "buffer.hpp"
#include "shader.hpp"
#include "structure.hpp"
#include "texture.hpp"
#include <string>
#include <array>

namespace GL
{
   class Mesh
   {
      public:
         Mesh(const std::string &obj);
         virtual void render();
         void set_shader(Program::Ptr shader);

         void set_mvp(const float *matrix, bool reset = true, bool transpose = true);
         void set_mvp(const GLMatrix &matrix, bool reset = true, bool transpose = true);
         void set_normal(const float *matrix, bool reset = true, bool transpose = true);
         void set_normal(const GLMatrix &matrix, bool reset = true, bool transpose = true);
         void set_transform(const float *matrix, bool reset = true, bool transpose = true);
         void set_transform(const GLMatrix &matrix, bool reset = true, bool transpose = true);
         void set_camera(const float *matrix, bool reset = true, bool transpose = true);
         void set_camera(const GLMatrix &matrix, bool reset = true, bool transpose = true);
         void set_texture(Texture::Ptr tex);

         void set_light(unsigned index, const vec3 &pos, const vec3 &color);
         void unset_light(unsigned index);
         void set_ambient(const vec3 &color);

      private:
         unsigned num_vertices;
         Buffer vbo;
         VAO vao;

         bool m_mvp_trans, m_trans_trans, m_normal_trans, m_camera_trans;
         bool m_mvp_reset, m_trans_reset, m_normal_reset, m_camera_reset;
         GLMatrix m_mvp_matrix;
         GLMatrix m_trans_matrix;
         GLMatrix m_normal_matrix;
         GLMatrix m_camera_matrix;

         Program::Ptr shader;
         Texture::Ptr tex;

         enum { max_lights = 8 };
         std::array<bool, max_lights> light_enabled;
         vec3 light_pos[max_lights];
         vec3 light_color[max_lights];
         vec3 light_ambient;

         void bind();
         void unbind();
         void load_object(const std::string &obj);
         void set_uniforms();
         void set_lights();
   };
}

#endif
