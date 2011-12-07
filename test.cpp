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
   Vector<int, 2> old_mouse;
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
      rot_y -= 0.2 * cam.delta(0);
      rot_x -= 0.3 * cam.delta(1);
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

   win->set_resize_cb([](unsigned width, unsigned height) {
         GLSYM(glViewport)(0, 0, width, height);
      });

   
   bool quit = false;
   win->set_close_cb([&quit]() -> bool {
         std::cerr << "Closing!" << std::endl;
         quit = true;
         return true;
      });

   Camera camera;
   std::memset(&camera, 0, sizeof(camera)); 
   camera.pos = {0, 0, 0, 1};
   camera.rot_x = camera.rot_y = 0.0;

   win->set_mouse_pos_cb([&camera](int x, int y) {
         Vector<int, 2> new_mouse {x, y};
         camera.delta = new_mouse - camera.old_mouse;
         camera.old_mouse = new_mouse;
      });

   win->set_key_cb([&quit, &camera](unsigned key, bool pressed) {
         quit = (key == GLFW_KEY_ESC) && pressed;

         switch (key)
         {
            case GLFW_KEY_UP:
               camera.forward = pressed;
               break;

            case GLFW_KEY_DOWN:
               camera.backward = pressed;
               break;

            case GLFW_KEY_LEFT:
               camera.left = pressed;
               break;

            case GLFW_KEY_RIGHT:
               camera.right = pressed;
               break;

            case GLFW_KEY_SPACE:
               camera.up = pressed;
               break;

            case 'C':
               glfwDisable(GLFW_MOUSE_CURSOR);
               camera.mouse = true;
               break;

            case 'V':
               glfwEnable(GLFW_MOUSE_CURSOR);
               camera.mouse = false;
               break;

            case 'M':
               camera.down = pressed;
               break;

            case 'W':
               camera.rot_up = pressed;
               break;

            case 'S':
               camera.rot_down = pressed;
               break;

            case 'A':
               camera.rot_left = pressed;
               break;

            case 'D':
               camera.rot_right = pressed;
               break;

            default:
               break;
         }

         if (key == 'R' && pressed)
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
   while (!quit)
   {
      GLSYM(glClear)(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      GLMatrix camera_matrix = update_camera(camera, 0.2);

      for (auto mesh : meshes)
      {
         auto rotate_mat = Rotate(Rotation::Y, frame_count * 0.2);
         mesh->set_normal(rotate_mat, true);

         mesh->set_camera(camera_matrix);

         auto trans_matrix = Translate(0.0, 0.0, -25.0) * Scale(30) * rotate_mat;
         mesh->set_transform(trans_matrix);
         mesh->render();

         trans_matrix = Translate(-20.0, 20.0, -25.0) * Scale(30) * rotate_mat;
         mesh->set_transform(trans_matrix);
         mesh->render();

         trans_matrix = Translate(20.0, -20.0, -70.0) * Scale(30) * rotate_mat;
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
