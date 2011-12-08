#include "utils.hpp"

#ifndef WINDOW_HPP__
#define WINDOW_HPP__
#include "sgl/sgl.h"
#include "sgl/sgl_keysym.h"
#include <functional>
#include <utility>

namespace GL
{
   extern "C"
   {
      void sgl_key_cb(int, int);
      void sgl_mouse_move_cb(int, int);
   }

   class Window : private GLU::SmartDefs<Window>
   {
      public:
         DECL_SHARED(Window);
         static Ptr get(unsigned width, unsigned height,
               const std::pair<unsigned, unsigned> &gl_version, bool fullscreen = false);
         static Ptr get();
         void operator=(const Window&) = delete;

         bool check_resize(unsigned &w, unsigned &h);
         bool alive();
         void flip();
         void vsync(bool activate = true);

         void set_key_cb(const std::function<void (int, bool)>& cb);
         void set_mouse_move_cb(const std::function<void (int, int)>& cb);

         ~Window();

      private:
         Window(unsigned width, unsigned height, const std::pair<unsigned, unsigned> &gl_version, bool fullscreen = false);
         static Ptr m_ptr;

         std::function<void (int, bool)> key_cb;
         std::function<void (int, int)> mouse_move_cb;
         friend void sgl_key_cb(int, int);
         friend void sgl_mouse_move_cb(int, int);

         void set_callbacks();
         void set_symbols();
   };

   // Every global resource that manages GL state must hold a reference
   // to the window in question.
   class GLResource
   {
      public:
         GLResource(bool init = true)
         {
            if (init)
               win_hold = GL::Window::get();
         }

         void grant(Window::Ptr ptr) { win_hold = ptr; }

      private:
         Window::Ptr win_hold;
   };

   class SymbolTable : public GLResource
   {
      public:
         SymbolTable() : GLResource(false)
         {}

         sgl_function_t& operator[](const std::string &str)
         {
            return syms[str];
         }

      private:
         std::map<std::string, sgl_function_t> syms;
   };

   extern SymbolTable symbol_map;
}

#endif

