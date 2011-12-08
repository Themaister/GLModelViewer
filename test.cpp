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
   Vector<int, 2> delta;
};

static GLMatrix update_camera(Camera &cam, float speed)
{
   vec3 movement {0, 0, 0};
   if (cam.forward)
      movement += vec3({0, 0, -1});
   if (cam.backward)
      movement += vec3({0, 0, 1});
   if (cam.left)
      movement += vec3({-1, 0, 0});
   if (cam.right)
      movement += vec3({1, 0, 0});

   float rot_x = 0.0;
   float rot_y = 0.0;

   if (cam.mouse)
   {
      rot_y -= 0.08 * cam.delta(0);
      rot_x -= 0.12 * cam.delta(1);
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

   auto direction = rotation * Rotate(Rotation::X, cam.rot_x) * vec_conv<3, 4>(speed * movement);
   if (cam.up)
      direction += vec_conv<3, 4>(speed * vec3({0, 1, 0}));
   if (cam.down)
      direction += vec_conv<3, 4>(speed * vec3({0, -1, 0}));

   cam.pos = cam.pos + direction;

   auto translation = Translate(-cam.pos(0), -cam.pos(1), -cam.pos(2));
   return Rotate(Rotation::X, -cam.rot_x) * Transpose(rotation) * translation;
}

static void gl_prog(const std::string &object_path)
{
   auto win = Window::get(200, 200, {3, 3});
   win->vsync();

   Camera camera;
   std::memset(&camera, 0, sizeof(camera)); 
   camera.pos = {0, 0, 0, 1};
   camera.rot_x = camera.rot_y = 0.0;

   win->set_mouse_move_cb([&camera](int x, int y) {
         camera.delta = { x, y };
      });

   float scale = 1.0;
   float scale_factor = 1.0;
   bool quit = false;

   win->set_key_cb([&quit, &camera, &scale_factor](unsigned key, bool pressed) {
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
               camera.mouse = true;
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
               scale_factor = pressed ? 0.98 : 1.0;
               break;

            case SGLK_x:
               scale_factor = pressed ? 1.02 : 1.0;
               break;

            default:
               break;
         }

         if (key == SGLK_r && pressed)
         {
            camera.pos = {0, 0, 0, 1};
            camera.rot_x = camera.rot_y = 0.0;
         }
      });

   GLSYM(glEnable)(GL_DEPTH_TEST);
   GLSYM(glEnable)(GL_CULL_FACE);
   GLSYM(glEnable)(GL_BLEND);
   GLSYM(glBlendFunc)(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   auto prog = Program::shared();
   prog->add(FileToString("shader.vp"), Shader::Type::Vertex);
   prog->add(FileToString("shader.fp"), Shader::Type::Fragment);
   prog->link();

   GLMatrix proj_matrix = Projection(2.0, 200.0);

   auto meshes = LoadTexturedMeshes(object_path);
   for (auto mesh : meshes)
   {
      mesh->set_shader(prog);
      mesh->set_mvp(proj_matrix);
      mesh->set_ambient({0.15, 0.15, 0.15});
      mesh->set_light(1, {-20.0, -20.0, -5.0}, {1.0, 1.0, 1.0});
      mesh->set_light(2, {20.0, -20.0, -5.0}, {1.0, 1.0, 1.0});
   }

   GLSYM(glClearColor)(0, 0, 0, 1);
   float frame_count = 0.0;
   while (win->alive() && !quit)
   {
      unsigned w, h;
      if (win->check_resize(w, h))
         GLSYM(glViewport)(0, 0, w, h);

      GLSYM(glClear)(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      GLMatrix camera_matrix = update_camera(camera, 0.2);

      scale *= scale_factor;
      for (auto mesh : meshes)
      {
         auto rotate_mat = Rotate(Rotation::Y, frame_count * 0.2);
         mesh->set_normal(rotate_mat, true);
         mesh->set_camera(camera_matrix);

         auto base_transform = Scale(scale) * rotate_mat;
         auto trans_matrix = Translate(0.0, 0.0, -25.0) * base_transform;
         mesh->set_transform(trans_matrix);
         mesh->render();

         trans_matrix = Translate(-20.0, 20.0, -25.0) * base_transform;
         mesh->set_transform(trans_matrix);
         mesh->render();

         trans_matrix = Translate(-20.0, 70.0, -80.0) * base_transform;
         mesh->set_transform(trans_matrix);
         mesh->render();

         trans_matrix = Translate(50.0, 0.0, -40.0) * base_transform;
         mesh->set_transform(trans_matrix);
         mesh->render();

         trans_matrix = Translate(20.0, -20.0, -70.0) * base_transform;
         mesh->set_transform(trans_matrix);
         mesh->render();

         auto light_pos = Translate(0.0, 0.0, -30.0) * Rotate(Rotation::Y, frame_count) * vec4({30.0, 20.0, 0.0, 1.0});
         mesh->set_light(0, vec_conv<4, 3>(light_pos), {4.0, 4.0, 4.0});
      }

      frame_count += 1.0;

      win->flip();
   }
}

int main(int argc, char *argv[])
{
   if (argc != 2)
   {
      std::cerr << "Usage: " << argv[0] << " <Object>" << std::endl;
      return 1;
   }

   try
   {
      gl_prog(argv[1]);
   }
   catch (const Exception& e)
   {
      std::cerr << e.what() << std::endl;
   }
}
