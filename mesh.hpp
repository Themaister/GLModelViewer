#ifndef MESH_HPP__
#define MESH_HPP__

#include "gl.hpp"
#include "buffer.hpp"
#include "shader.hpp"
#include "structure.hpp"
#include "texture.hpp"
#include "utils.hpp"
#include <string>
#include <array>

namespace GL
{
   class Mesh : private GLU::SmartDefs<Mesh>
   {
      public:
         DECL_SHARED(Mesh);
         Mesh(const std::string &obj);
         Mesh(const std::vector<Geo::Triangle> &triangles);
         virtual void render();
         static void set_shader(Program::Ptr shader);

         static void set_projection(const GLMatrix &matrix);
         static void set_camera(const GLMatrix &matrix);

         static void set_light_transform(const GLMatrix &matrix);
         void set_transform(const GLMatrix &matrix);
         void set_normal(const GLMatrix &matrix);
         void set_texture(Texture::Ptr tex);

         static void set_light(unsigned index,
               const vec3 &pos, const vec3 &color);
         static void unset_light(unsigned index);
         static void set_ambient(const vec3 &color);

      private:
         unsigned num_vertices;
         Buffer vbo;
         VAO vao;

         static Program::Ptr shader;
         Texture::Ptr tex;

         static UniformBuffer::Ptr trans_unibuf;
         static UniformBuffer::Ptr lights_unibuf;
         struct Transforms
         {
            GLMatrix projection;
            GLMatrix camera;
            GLMatrix light_matrix;
         } static transforms;
         GLMatrix trans_matrix;
         GLMatrix normal_matrix;
         static bool transforms_changed;
         bool local_transforms_changed;
         enum { max_lights = 8 };
         static std::array<bool, max_lights> light_enabled;
         struct Lights
         {
            vec4 light_ambient;
            vec4 light_pos[max_lights];
            vec4 light_color[max_lights];
            GLint lights;
            GLint padding[3];
         } static lights;
         static bool lights_changed;
         void init_uniform_buffers();

         void load_object(const std::string &obj);
         void load_object(const std::vector<Geo::Triangle> &obj);
         void set_uniforms();
         void set_lights();
         void set_transforms();
   };
}

#endif
