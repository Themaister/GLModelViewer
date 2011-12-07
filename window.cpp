#include "window.hpp"
#include <iostream>

namespace GL
{
   Window::Window(unsigned width, unsigned height, 
         const std::pair<unsigned, unsigned> &gl_version, bool fullscreen)
   {
      sgl_context_options opts;
      std::memset(&opts, 0, sizeof(opts));

      opts.context.style = SGL_CONTEXT_MODERN;
      opts.context.major = gl_version.first;
      opts.context.minor = gl_version.second;
      opts.res.width = width;
      opts.res.height = height;
      opts.screen_type = fullscreen ? SGL_SCREEN_WINDOWED_FULLSCREEN :
         SGL_SCREEN_WINDOWED;

      opts.swap_interval = 1;
      opts.title = "GLModelViewer";

      if (!sgl_init(&opts))
         throw Exception("Failed to initialize SGL!");

      set_callbacks();
   }

   Window::~Window()
   {
      sgl_deinit();
   }

   Window::Ptr Window::m_ptr;

   Window::Ptr Window::get(unsigned width, unsigned height,
         const std::pair<unsigned, unsigned> &gl_version, bool fullscreen)
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
      sgl_swap_buffers();
   }

   void Window::set_callbacks()
   {
      sgl_input_callbacks cbs = {0};
      cbs.key_cb = sgl_key_cb;
      cbs.mouse_move_cb = sgl_mouse_move_cb;
      sgl_set_input_callbacks(&cbs);
      sgl_set_mouse_mode(false, true, false);
   }

   void Window::vsync(bool activate)
   {
      sgl_set_swap_interval(activate ? 1 : 0);
   }

   bool Window::check_resize(unsigned &w, unsigned &h)
   {
      return sgl_check_resize(&w, &h);
   }

   bool Window::alive()
   {
      return sgl_is_alive();
   }

   void Window::set_key_cb(const std::function<void (int, bool)>& cb)
   {
      key_cb = cb;
   }

   void Window::set_mouse_move_cb(const std::function<void (int, int)>& cb)
   {
      mouse_move_cb = cb;
   }

   void sgl_key_cb(int key, int pressed)
   {
      auto win = Window::get();
      if (win && win->key_cb)
         win->key_cb(key, pressed);
   }

   void sgl_mouse_move_cb(int x, int y)
   {
      auto win = Window::get();
      if (win && win->mouse_move_cb)
         win->mouse_move_cb(x, y);
   }
}

