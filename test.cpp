#include "window.hpp"
#include <iostream>
#include "shader.hpp"
#include "structure.hpp"
#include "mesh.hpp"
#include "object.hpp"
#include <assert.h>
#include <cstring>

using namespace GL;
using namespace GLU;
using namespace GLU::Matrices;

struct Camera
{
   vec4 pos;

   bool forward;
   bool backward;
   bool left;
   bool right;
   bool up;
   bool down;
   bool rot_left;
   bool rot_right;
   bool rot_up;
   bool rot_down;

   float rot_x, rot_y;

   bool mouse;
   ivec2 delta;
};

static GLMatrix update_camera(Camera &cam, float speed)
{
   vec3 movement(0, 0, 0);
   if (cam.forward)
      movement += vec3(0, 0, -1);
   if (cam.backward)
      movement += vec3(0, 0, 1);
   if (cam.left)
      movement += vec3(-1, 0, 0);
   if (cam.right)
      movement += vec3(1, 0, 0);

   float rot_x = 0.0f;
   float rot_y = 0.0f;

   if (cam.mouse)
   {
      rot_y -= 0.08f * cam.delta(0);
      rot_x -= 0.12f * cam.delta(1);
      cam.delta = Vector<int, 2>();
   }
   else
   {
      if (cam.rot_up)
         rot_x += 3 * speed;
      if (cam.rot_down)
         rot_x -= 3 * speed;
      if (cam.rot_left)
         rot_y += 3 * speed;
      if (cam.rot_right)
         rot_y -= 3 * speed;
   }

   cam.rot_x += rot_x;
   cam.rot_y += rot_y;

   if (cam.rot_x > 85.0)
      cam.rot_x = 85.0;
   else if (cam.rot_x < -85.0)
      cam.rot_x = -85.0;

   auto rotation = Rotate(0.0, -cam.rot_y, 0.0);

   vec3 direction = rotation * Rotate(RotX, cam.rot_x) * vec_conv<3, 4>(speed * movement);
   if (cam.up)
      direction += speed * vec3(0, 1, 0);
   if (cam.down)
      direction += speed * vec3(0, -1, 0);

   cam.pos = cam.pos + direction;

   auto translation = Translate(-cam.pos);
   return Rotate(RotX, -cam.rot_x) * Transpose(rotation) * translation;
}

static void key_callback(unsigned key, bool pressed,
      bool &quit, Camera &camera, float &scale_factor,
      float &light_rot_y)
{
   quit |= key == SGLK_ESCAPE && pressed;

   switch (key)
   {
      case SGLK_UP:
         camera.forward = pressed;
         break;

      case SGLK_DOWN:
         camera.backward = pressed;
         break;

      case SGLK_LEFT:
         camera.left = pressed;
         break;

      case SGLK_RIGHT:
         camera.right = pressed;
         break;

      case SGLK_SPACE:
         camera.up = pressed;
         break;

      case SGLK_c:
         sgl_set_mouse_mode(true, true, false);
         camera.mouse = true;
         break;

      case SGLK_v:
         sgl_set_mouse_mode(false, true, true);
         camera.mouse = false;
         break;

      case SGLK_m:
         camera.down = pressed;
         break;

      case SGLK_w:
         camera.rot_up = pressed;
         break;

      case SGLK_s:
         camera.rot_down = pressed;
         break;

      case SGLK_a:
         camera.rot_left = pressed;
         break;

      case SGLK_d:
         camera.rot_right = pressed;
         break;

      case SGLK_z:
         scale_factor = pressed ? 0.98f : 1.0f;
         break;

      case SGLK_x:
         scale_factor = pressed ? 1.02f : 1.0f;
         break;

      case SGLK_j:
         light_rot_y = pressed ? 2.0f : 0.0f;
         break;

      case SGLK_l:
         light_rot_y = pressed ? -2.0f : 0.0f;
         break;

      default:
         break;
   }

   if (key == SGLK_r && pressed)
   {
      camera.pos = vec4(0, 0, 0, 1);
      camera.rot_x = camera.rot_y = 0.0;
   }
}

static void gl_prog(const std::vector<std::string> &object_paths)
{
   auto win = Window::get(640, 480, std::pair<unsigned, unsigned>(3, 3));
   win->vsync();

   Camera camera;
   std::memset(&camera, 0, sizeof(camera)); 
   camera.pos = vec4(0, 0, 0, 1);
   camera.rot_x = camera.rot_y = 0.0;

   win->set_mouse_move_cb([&camera](int x, int y) {
         camera.delta = ivec2(x, y);
      });

   float scale = 1.0;
   float scale_factor = 1.0;
   float light_total_rot_y = 0.0;
   float light_rot_y = 0.0;
   bool quit = false;

   win->set_key_cb([&quit, &camera, &scale_factor, &light_rot_y](unsigned key, bool pressed) {
         key_callback(key, pressed, quit, camera, scale_factor, light_rot_y);
      });

   GLSYM(glEnable)(GL_DEPTH_TEST);
   //GLSYM(glEnable)(GL_CULL_FACE);

   auto prog = std::make_shared<Program>();
   prog->add(FileToString("shader.vp"), Shader::Vertex);
   prog->add(FileToString("shader.fp"), Shader::Fragment);
   prog->link();
   auto shadow_prog = std::make_shared<Program>();
   shadow_prog->add(FileToString("shadow_shader.vp"), Shader::Vertex);
   shadow_prog->link();
   auto shadow_map_prog = std::make_shared<Program>();
   shadow_map_prog->add(FileToString("shadow_map.vp"), Shader::Vertex);
   shadow_map_prog->add(FileToString("shadow_map.fp"), Shader::Fragment);
   shadow_map_prog->link();

   std::shared_ptr<ShadowBuffer> shadow_buf[1] = { std::make_shared<ShadowBuffer>(1024, 1024) };
   std::shared_ptr<RenderBuffer> shadow_map_buf[1] = { std::make_shared<RenderBuffer>(1024, 1024) };

   int width = 640, height = 480;
   auto proj_matrix = Scale((float)height / width, 1, 1) * Projection(2, 1000);
   Mesh::set_projection(proj_matrix);
   Mesh::set_ambient(vec3(0.15f, 0.15f, 0.15f));
   Mesh::set_shader(prog);
   Mesh::set_viewport_size(ivec2(width, height));

   std::vector<std::shared_ptr<Mesh>> meshes;
   for (auto path = std::begin(object_paths); path != std::end(object_paths); ++path)
   {
      auto mesh = LoadTexturedMeshes(*path);
      meshes.insert(meshes.end(), mesh.begin(), mesh.end());
   }

   GLSYM(glClearColor)(0, 0, 0, 1);
   float frame_count = 0.0;
   while (win->alive() && !quit)
   {
      if (win->check_resize(width, height))
      {
         GLSYM(glViewport)(0, 0, width, height);
         auto proj_matrix = Scale((float)height / width, 1, 1) * Projection(2, 1000);
         Mesh::set_projection(proj_matrix);
         Mesh::set_viewport_size(ivec2(width, height));
         frame_count = 0.0;
      }

      // Update uniforms.
      scale *= scale_factor;
      for (auto mesh = std::begin(meshes); mesh != std::end(meshes); ++mesh)
      {
         auto rotate_mat = Identity();
         (*mesh)->set_normal(rotate_mat);

         auto trans_matrix = Translate(0.0f, 0.0f, -25.0f) * Scale(scale);
         (*mesh)->set_transform(trans_matrix);
      }

      light_total_rot_y += light_rot_y;

      vec3 light_pos[1] = {
         Translate(0, 100, -25) *
         Rotate(RotY, light_total_rot_y) *
            vec4(200, 0, 0, 1)};
      Mesh::set_light(0, light_pos[0], vec3(10, 10, 10));
      GLMatrix light_camera[1] = {
         Projection(2, 1000) * Derotate(vec3(0, 0, -25) - light_pos[0]) * Translate(-light_pos[0]),
      };

      auto camera_matrix = update_camera(camera, 1.0);
      Mesh::set_player_pos(camera.pos);
      Mesh::set_camera(camera_matrix);

      for (unsigned i = 0; i < 1; i++)
      {
         Mesh::set_light_transform(light_camera[i]);
         // Begin rendering
         // 1st pass. Render depth map.
         Mesh::set_shader(shadow_prog);
         shadow_buf[i]->bind();
         GLSYM(glClear)(GL_DEPTH_BUFFER_BIT);
         unsigned shadow_w, shadow_h;
         shadow_buf[i]->size(shadow_w, shadow_h);
         GLSYM(glViewport)(0, 0, shadow_w, shadow_h);

         for (auto mesh = std::begin(meshes); mesh != std::end(meshes); ++mesh)
            (*mesh)->render();
         shadow_buf[i]->unbind();

         // 2nd pass. Generate a shadow map which we can blur.
         shadow_map_buf[i]->bind();
         Mesh::set_shader(shadow_map_prog);
         GLSYM(glClear)(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
         shadow_map_buf[i]->size(shadow_w, shadow_h);
         GLSYM(glViewport)(0, 0, shadow_w, shadow_h);
         shadow_buf[i]->bind_texture(1);

         for (auto mesh = std::begin(meshes); mesh != std::end(meshes); ++mesh)
            (*mesh)->render();
         shadow_buf[i]->unbind_texture();
         shadow_map_buf[i]->unbind();
      }

      // 3rd pass. Render final scene with blurry shadow map.
      shadow_map_buf[0]->bind_texture(1);
      GLSYM(glClear)(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      GLSYM(glViewport)(0, 0, width, height);
      Mesh::set_shader(prog);
      for (auto mesh = std::begin(meshes); mesh != std::end(meshes); ++mesh)
         (*mesh)->render();
      shadow_map_buf[0]->unbind_texture();

      frame_count += 1.0;
      win->flip();
   }
}

int main(int argc, char *argv[])
{
   if (argc < 2)
   {
      std::cerr << "Usage: " << argv[0] << " <Object> [<Objects>]" << std::endl;
      return 1;
   }

   try
   {
      std::vector<std::string> paths;
      for (int i = 1; i < argc; i++)
         paths.push_back(argv[i]);

      gl_prog(paths);
   }
   catch (const Exception& e)
   {
      std::cerr << e.what() << std::endl;
   }
}
