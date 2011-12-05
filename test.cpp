#include "window.hpp"
#include <iostream>
#include "shader.hpp"
#include "structure.hpp"
#include "mesh.hpp"
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

   GLMatrix rotation;

   bool mouse;
   Vector<int, 2> delta;
   Vector<int, 2> old_mouse;
};

static void update_camera(Mesh &mesh, Camera &cam, float speed)
{
   vec3 movement {0, 0, 0};
   if (cam.forward)
      movement = movement + vec3({0, 0, -1});
   if (cam.backward)
      movement = movement + vec3({0, 0, 1});
   if (cam.left)
      movement = movement + vec3({-1, 0, 0});
   if (cam.right)
      movement = movement + vec3({1, 0, 0});
   if (cam.up)
      movement = movement + vec3({0, 1, 0});
   if (cam.down)
      movement = movement + vec3({0, -1, 0});

   auto direction = cam.rotation * vec_conv<3, 4>(speed * movement);
   cam.pos = cam.pos + direction;

   float rot_x = 0.0;
   float rot_y = 0.0;

   if (cam.rot_up)
      rot_x += 3 * speed;
   if (cam.rot_down)
      rot_x -= 3 * speed;
   if (cam.rot_left)
      rot_y += 3 * speed;
   if (cam.rot_right)
      rot_y -= 3 * speed;

   if (cam.mouse)
   {
      rot_y -= 0.2 * cam.delta(0);
      rot_x -= 0.3 * cam.delta(1);
   }
   cam.delta = Vector<int, 2>();

   cam.rotation = cam.rotation * Rotate(rot_x, -rot_y, 0.0);

   auto translation = Translate(-cam.pos(0), -cam.pos(1), -cam.pos(2));
   mesh.set_camera(Transpose(cam.rotation) * translation);

}

static void gl_prog(const std::string &object_path, const std::string &texture_path)
{
   auto win = Window::get(200, 200, {3, 3});
   win->vsync();

   win->set_resize_cb([](unsigned width, unsigned height) {
         glViewport(0, 0, width, height);
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
   camera.rotation = Identity();

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
            camera.rotation = Identity();
         }
      });

   glEnable(GL_DEPTH_TEST);
   glEnable(GL_CULL_FACE);
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   auto prog = Program::shared();
   prog->add(FileToString("shader.vp"), Shader::Type::Vertex);
   prog->add(FileToString("shader.fp"), Shader::Type::Fragment);
   prog->link();

   GLMatrix proj_matrix = Projection(2.0, 200.0);

   Mesh mesh(object_path);
   mesh.set_shader(prog);
   mesh.set_texture(Texture::shared(texture_path));
   mesh.set_mvp(proj_matrix);
   mesh.set_ambient({0.15, 0.15, 0.15});
   mesh.set_light(1, {-20.0, -20.0, -5.0}, {1.0, 1.0, 1.0});
   mesh.set_light(2, {20.0, -20.0, -5.0}, {1.0, 1.0, 1.0});

   glClearColor(0, 0, 0, 1);
   float frame_count = 0.0;
   while (!quit)
   {
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      //auto rotate_mat = Rotate(Rotation::Z, frame_count * 0.05) * Rotate(Rotation::Y, 180) * Rotate(Rotation::X, frame_count * 0.2);
      auto rotate_mat = Identity();
      mesh.set_normal(rotate_mat, true);

      update_camera(mesh, camera, 0.2);

      auto trans_matrix1 = Translate(0.0, 0.0, -25.0) * Scale(6.0) * rotate_mat;
      mesh.set_transform(trans_matrix1);
      mesh.render();

      trans_matrix1 = Translate(-20.0, 20.0, -25.0) * Scale(6.0) * rotate_mat;
      mesh.set_transform(trans_matrix1);
      mesh.render();

      trans_matrix1 = Translate(20.0, -20.0, -70.0) * Scale(6.0) * rotate_mat;
      mesh.set_transform(trans_matrix1);
      mesh.render();

      auto light_pos = Translate(0.0, 0.0, -30.0) * Rotate(Rotation::Y, frame_count) * vec4({30.0, 20.0, 0.0, 1.0});
      mesh.set_light(0, vec_conv<4, 3>(light_pos), {4.0, 4.0, 4.0});

      frame_count += 1.0;

      win->flip();
   }
}

int main(int argc, char *argv[])
{
   if (argc != 3)
      std::cerr << "Usage: " << argv[0] << " <Object> <Texture>" << std::endl;

   try
   {
      gl_prog(argv[1], argv[2]);
   }
   catch (const Exception& e)
   {
      std::cerr << e.what() << std::endl;
   }
}
