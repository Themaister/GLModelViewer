#include "window.hpp"
#include <iostream>

namespace GL
{
   Window::Window(unsigned width, unsigned height, 
         const std::pair<unsigned, unsigned> &gl_version, bool fullscreen)
      : GLFWWindow(gl_version)
   {
      if (glfwOpenWindow(width, height, 8, 8, 8, 8, 24, 8, fullscreen ? GLFW_FULLSCREEN : GLFW_WINDOW) != GL_TRUE)
         throw Exception("Failed to open GL window ...\n");

      glfwSetWindowTitle("GLModelViewer");

      set_callbacks();
   }

   Window::Ptr Window::m_ptr;

   Window::Ptr Window::get(unsigned width, unsigned height, const std::pair<unsigned, unsigned> &gl_version, bool fullscreen)
   {
      if (m_ptr.get() == nullptr)
         m_ptr = std::shared_ptr<Window>(new Window(width, height, gl_version, fullscreen));

      return m_ptr;
   }

   Window::Ptr Window::get()
   {
      return m_ptr;
   }

   void Window::flip()
   {
      glfwSwapBuffers();
   }

   void Window::vsync(bool activate)
   {
      glfwSwapInterval(activate ? 1 : 0);
   }

   void Window::poll()
   {
      glfwPollEvents();
   }

   void Window::set_resize_cb(const std::function<void (unsigned, unsigned)>& cb)
   {
      resize_cb = cb;
   }

   void Window::set_close_cb(const std::function<bool ()>& cb)
   {
      close_cb = cb;
   }

   void Window::set_key_cb(const std::function<void (unsigned, bool)>& cb)
   {
      key_cb = cb;
   }

   void Window::set_mouse_pos_cb(const std::function<void (int, int)>& cb)
   {
      mouse_pos_cb = cb;
   }

   void GLFWCALL glfw_resize_cb(int width, int height)
   {
      auto win = Window::get();
      if (win && win->resize_cb)
         win->resize_cb(width, height);
      else
         GLSYM(glViewport)(0, 0, width, height);
   }

   int GLFWCALL glfw_close_cb()
   {
      auto win = Window::get();
      if (win && win->close_cb)
         return win->close_cb();
      return GL_TRUE;
   }

   void GLFWCALL glfw_key_cb(int key, int action)
   {
      auto win = Window::get();
      if (win && win->key_cb)
         win->key_cb(key, action == GLFW_PRESS);
   }

   void GLFWCALL glfw_mouse_pos_cb(int x, int y)
   {
      auto win = Window::get();
      if (win && win->mouse_pos_cb)
         win->mouse_pos_cb(x, y);
   }
}
