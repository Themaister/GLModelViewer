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
      set_symbols();
   }

   Window::~Window()
   {
      std::cerr << "[SGL]: Tearing down SGL!" << std::endl;
      sgl_deinit();
   }

   Window::Ptr Window::m_ptr;

   Window::Ptr Window::get(unsigned width, unsigned height,
         const std::pair<unsigned, unsigned> &gl_version, bool fullscreen)
   {
      if (!m_ptr)
      {
         m_ptr = Window::Ptr(new Window(width, height,
                  gl_version, fullscreen));
      }

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
      sgl_set_mouse_mode(false, true, true);
   }

   void Window::set_symbols()
   {
      // GL 1.1 stuff is not found dynamically in Windows. :(
      // Statically initialize them.
#define _D(sym) { #sym, reinterpret_cast<sgl_function_t>(sym) }
      static const std::vector<std::pair<std::string, sgl_function_t>>
         bind_map = {
            _D(glEnable),
            _D(glBlendFunc),
            _D(glClearColor),
            _D(glTexImage2D),
            _D(glViewport),
            _D(glClear),
            _D(glTexParameteri),
            _D(glDeleteTextures),
#if 0
            _D(glGetError),
#endif
         };
#undef _D

      for (auto &bind : bind_map)
         symbol_map[bind.first] = bind.second;
      symbol_map.grant(get());
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
#if 1
      GLenum err = GLSYM(glGetError)();
      if (err != GL_NO_ERROR)
         throw Exception(GLU::join("Caught GL error: ", static_cast<unsigned>(err)));
#endif

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

   SymbolTable symbol_map;
}

