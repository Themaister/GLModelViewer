#ifndef WINDOW_HPP__
#define WINDOW_HPP__

#include "gl.hpp"
#include "utils.hpp"
#include <functional>
#include <utility>

namespace GL
{
   extern "C"
   {
      void GLFWCALL glfw_resize_cb(int, int);
      int GLFWCALL glfw_close_cb();
      void GLFWCALL glfw_key_cb(int, int);
      void GLFWCALL glfw_mouse_pos_cb(int, int);
   }

   class GLFWWindow : public GLU::RefCounted<GLFWWindow>
   {
      public:
         GLFWWindow(const std::pair<unsigned, unsigned> &version)
         {
            if (ref() == 0)
               init_glfw(version);

            ref()++;
         }

         ~GLFWWindow()
         {
            ref()--;
            if (ref() == 0)
            {
               glfwCloseWindow();
               glfwTerminate();
            }
         }

         static void set_callbacks()
         {
            glfwSetWindowSizeCallback(glfw_resize_cb);
            glfwSetWindowCloseCallback(glfw_close_cb);
            glfwSetKeyCallback(glfw_key_cb);
            glfwSetMousePosCallback(glfw_mouse_pos_cb);
         }

      private:
         void init_glfw(const std::pair<unsigned, unsigned> &version)
         {
            if (glfwInit() != GL_TRUE)
               throw Exception("Failed to init GLFW.\n");

            // Modern GL ... :)
            if (version.first >= 3)
            {
               glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
               glfwOpenWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
            }
            glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, version.first);
            glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, version.second);
         }
   };
   
   class Window : public GLFWWindow, private GLU::SmartDefs<Window>
   {
      public:
         DECL_SHARED(Window);
         static Ptr get(unsigned width, unsigned height, const std::pair<unsigned, unsigned> &gl_version, bool fullscreen = false);
         static Ptr get();
         void operator=(const Window&) = delete;

         void flip();
         void vsync(bool activate = true);
         void poll();

         void set_resize_cb(const std::function<void (unsigned, unsigned)>& cb);
         void set_close_cb(const std::function<bool ()>& cb);
         void set_key_cb(const std::function<void (unsigned, bool)>& cb);
         void set_mouse_pos_cb(const std::function<void (int, int)>& cb);

      private:
         Window(unsigned width, unsigned height, const std::pair<unsigned, unsigned> &gl_version, bool fullscreen = false);
         static Ptr m_ptr;

         std::function<void (unsigned, unsigned)> resize_cb;
         std::function<bool ()> close_cb;
         std::function<void (unsigned, bool)> key_cb;
         std::function<void (int, int)> mouse_pos_cb;
         friend void GLFWCALL glfw_resize_cb(int, int);
         friend int GLFWCALL glfw_close_cb();
         friend void GLFWCALL glfw_key_cb(int, int);
         friend void GLFWCALL glfw_mouse_pos_cb(int, int);
   };
}

#endif
