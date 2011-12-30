#ifndef TEXTURE_HPP__
#define TEXTURE_HPP__

#include "gl.hpp"
#include "utils.hpp"
#include <list>
#include <vector>
#include <utility>
#include <stdint.h>

namespace GL
{
   class Texture : public GLResource
   {
      public:
         Texture(const std::string &path);
         ~Texture();

         enum Edge { Clamp, ClampToBorder, Repeat };
         enum Filter { Nearest, Linear };

         void bind(unsigned index = 0,
               Filter filt = Linear,
               Edge edge = Repeat);
         void unbind();

      private:
         void operator=(const Texture&);
         GLuint obj;
         int bound_index;

         static std::list<Texture *> bound_textures;

         struct Image
         {
            unsigned width;
            unsigned height;
            std::vector<uint32_t> pixels;
         };

         Image load_tga(const std::string &path);
   };

   class RenderBuffer : public GLResource
   {
      public:
         RenderBuffer(unsigned width, unsigned height);
         ~RenderBuffer();

         void size(unsigned &width, unsigned &height) const;
         void bind();
         void unbind();
         void bind_texture(unsigned index);
         void unbind_texture();

      protected:
         void operator=(const RenderBuffer&);
         RenderBuffer();
         GLuint fb_obj;
         GLuint tex;
         GLuint render_buffer;
         unsigned bound_index;
         unsigned width, height;
   };

   class ShadowBuffer : public RenderBuffer
   {
      public:
         ShadowBuffer(unsigned width, unsigned height);
   };
}

#endif

