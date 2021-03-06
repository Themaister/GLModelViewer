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

   class SymbolTable;

   class Window
   {
      public:
         static std::shared_ptr<Window> get(unsigned width, unsigned height,
               const std::pair<unsigned, unsigned> &gl_version, bool fullscreen = false);
         static std::shared_ptr<Window> get();

         bool check_resize(int &w, int &h);
         bool alive();
         void flip();
         void vsync(bool activate = true);

         void set_key_cb(const std::function<void (int, bool)>& cb);
         void set_mouse_move_cb(const std::function<void (int, int)>& cb);

         sgl_function_t& symbol(const std::string &sym);

         ~Window();

      private:
         Window(unsigned width, unsigned height, const std::pair<unsigned, unsigned> &gl_version, bool fullscreen = false);
         static std::shared_ptr<Window> m_ptr;
         void operator=(const Window&);

         std::function<void (int, bool)> key_cb;
         std::function<void (int, int)> mouse_move_cb;
         friend void sgl_key_cb(int, int);
         friend void sgl_mouse_move_cb(int, int);

         void set_callbacks();
         void set_symbols();

         std::map<std::string, sgl_function_t> sym_map;
   };

   // Every global resource that manages GL state must hold a reference
   // to the window in question.
   class GLResource
   {
      public:
         GLResource()
         {
            win_hold = GL::Window::get();
         }

      private:
         std::shared_ptr<Window> win_hold;
   };
}

#endif

