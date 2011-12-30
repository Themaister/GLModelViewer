#include "window.hpp"
#include <iostream>
#include <vector>

namespace GL
{
#ifdef DEBUG
   extern "C"
   {
      static void APIENTRY debug_cb(GLenum, GLenum, GLuint, GLenum, GLsizei, const GLchar *message, GLvoid*)
      {
         std::cerr << "GL DEBUG: " << message << std::endl;
      }
   }
#endif

   Window::Window(unsigned width, unsigned height, 
         const std::pair<unsigned, unsigned> &gl_version, bool fullscreen)
   {
      sgl_context_options opts;
      std::memset(&opts, 0, sizeof(opts));

      opts.context.style = SGL_CONTEXT_MODERN;
      opts.context.major = gl_version.first;
      opts.context.minor = gl_version.second;
      opts.samples = 4;
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
      sgl_deinit();
   }

   std::shared_ptr<Window> Window::m_ptr;

   std::shared_ptr<Window> Window::get(unsigned width, unsigned height,
         const std::pair<unsigned, unsigned> &gl_version, bool fullscreen)
   {
      if (!m_ptr)
      {
         m_ptr = std::shared_ptr<Window>(new Window(width, height,
                  gl_version, fullscreen));
      }

#ifdef DEBUG
      GLSYM(glDebugMessageControlARB)(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
      GLSYM(glDebugMessageCallbackARB)(debug_cb, nullptr);
#endif

      return m_ptr;
   }

   std::shared_ptr<Window> Window::get()
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

      struct mapper { const char *sym; sgl_function_t func; };
      
#define _D(sym) { #sym, reinterpret_cast<sgl_function_t>(sym) }
      static const mapper bind_map[] = {
            _D(glEnable),
            _D(glBlendFunc),
            _D(glClearColor),
            _D(glTexImage2D),
            _D(glViewport),
            _D(glClear),
            _D(glTexParameteri),
            _D(glDeleteTextures),
            _D(glGenTextures),
            _D(glBindTexture),
            _D(glDrawArrays),
#ifdef DEBUG
            _D(glGetError),
#endif
         };
#undef _D

      for (auto itr = std::begin(bind_map); itr != std::end(bind_map); ++itr)
         sym_map[itr->sym] = itr->func;
   }

   void Window::vsync(bool activate)
   {
      sgl_set_swap_interval(activate ? 1 : 0);
   }

   bool Window::check_resize(int &w, int &h)
   {
      unsigned w_ = w, h_ = h;
      bool ret = sgl_check_resize(&w_, &h_) == SGL_TRUE;
      w = w_;
      h = h_;
      return ret;
   }

   bool Window::alive()
   {
#ifdef DEBUG
      GLenum err = GLSYM(glGetError)();
      if (err != GL_NO_ERROR)
         throw Exception(GLU::join("Caught GL error: ", static_cast<unsigned>(err)));
#endif

      return sgl_is_alive() == SGL_TRUE;
   }

   sgl_function_t& Window::symbol(const std::string &str)
   {
      return sym_map[str];
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
         win->key_cb(key, pressed == SGL_TRUE);
   }

   void sgl_mouse_move_cb(int x, int y)
   {
      auto win = Window::get();
      if (win && win->mouse_move_cb)
         win->mouse_move_cb(x, y);
   }
}

