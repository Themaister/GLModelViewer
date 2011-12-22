#include "texture.hpp"
#include <string>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <iterator>

namespace GL
{
   std::list<Texture *> Texture::bound_textures;

   Texture::Texture(const std::string &path) : obj(0), bound_index(-1)
   {
      auto image = load_tga(path);

      GLSYM(glGenTextures)(1, &obj);

      auto itr = std::find_if(std::begin(bound_textures), std::end(bound_textures),
            [](const Texture *tex) { return tex->bound_index == 0; });

      Texture *rebind_tex = nullptr;
      if (itr != std::end(bound_textures))
         rebind_tex = *itr;

      GLSYM(glActiveTexture)(GL_TEXTURE0);
      GLSYM(glBindTexture)(GL_TEXTURE_2D, obj);
      GLSYM(glTexImage2D)(GL_TEXTURE_2D, 0, GL_RGBA,
            image.width, image.height, 0,
            GL_BGRA, GL_UNSIGNED_INT_8_8_8_8, &image.pixels[0]);
      GLSYM(glGenerateMipmap)(GL_TEXTURE_2D);

      GLSYM(glBindTexture)(GL_TEXTURE_2D, rebind_tex ? rebind_tex->obj : 0);
   }

   Texture::~Texture()
   {
      if (obj)
      {
         unbind();
         GLSYM(glDeleteTextures)(1, &obj);
      }
   }

   static inline GLint gl_edge(Texture::Edge edge)
   {
      switch (edge)
      {
         case Texture::Edge::Clamp:
            return GL_CLAMP;
         case Texture::Edge::ClampToBorder:
            return GL_CLAMP_TO_BORDER;
         case Texture::Edge::Repeat:
            return GL_REPEAT;
         default:
            throw Exception("Invalid Edge option!");
      }
   }

   static inline GLint gl_filter(Texture::Filter filter)
   {
      switch (filter)
      {
         case Texture::Filter::Nearest:
            return GL_NEAREST;
         case Texture::Filter::Linear:
            return GL_LINEAR;
         default:
            throw Exception("Invalid Filter option!");
      }
   }

   void Texture::bind(unsigned index, Texture::Filter filter, Texture::Edge edge)
   {
      if (bound_index >= 0)
         throw Exception("Binding one texture to several units currently not supported!");

      auto itr = std::find_if(std::begin(bound_textures), std::end(bound_textures),
            [index](const Texture *tex) { return tex->bound_index == static_cast<int>(index); });
      if (itr != std::end(bound_textures))
         throw Exception("A texture is already bound to this texture unit!");

      bound_index = index;

      bound_textures.push_back(this);

      GLSYM(glActiveTexture)(GL_TEXTURE0 + bound_index);
      GLSYM(glBindTexture)(GL_TEXTURE_2D, obj);
      GLSYM(glTexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, gl_edge(edge));
      GLSYM(glTexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, gl_edge(edge));
      GLSYM(glTexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      GLSYM(glTexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
   }

   void Texture::unbind()
   {
      if (bound_index >= 0)
      {
         bound_textures.erase(std::find_if(std::begin(bound_textures), std::end(bound_textures),
                  [this](const Texture *tex) { return this == tex; }));

         GLSYM(glActiveTexture)(GL_TEXTURE0 + bound_index);
         GLSYM(glBindTexture)(GL_TEXTURE_2D, 0);
      }

      bound_index = -1;
   }

   Texture::Image Texture::load_tga(const std::string &path)
   {
      Image img;

      std::vector<uint8_t> in_file;
      std::ifstream file(path, std::ios::in | std::ios::binary);
      if (!file.is_open())
         throw Exception(GLU::join("Cannot open file: ", path));

      file >> std::noskipws;
      std::copy(std::istream_iterator<uint8_t>(file),
            std::istream_iterator<uint8_t>(),
            std::back_inserter(in_file));

      if (in_file[2] != 2)
         throw Exception("Uncompressed RGB Targa not supported.");

      img.width = in_file[12] + in_file[13] * 256;
      img.height = in_file[14] + in_file[15] * 256;

      img.pixels.resize(img.width * img.height);

      const uint8_t *tmp = &in_file[18];
      unsigned bits = in_file[16];

      if (bits == 32)
      {
         for (unsigned i = 0; i < img.width * img.height; i++)
         {
            uint32_t b = tmp[i * 4 + 0];
            uint32_t g = tmp[i * 4 + 1];
            uint32_t r = tmp[i * 4 + 2];
            uint32_t a = tmp[i * 4 + 3];

            img.pixels[i] = (b << 24) | (g << 16) | (r << 8) | a;
         }
      }
      else if (bits == 24)
      {
         for (unsigned i = 0; i < img.width * img.height; i++)
         {
            uint32_t b = tmp[i * 3 + 0];
            uint32_t g = tmp[i * 3 + 1];
            uint32_t r = tmp[i * 3 + 2];
            uint32_t a = 0xff;

            img.pixels[i] = (b << 24) | (g << 16) | (r << 8) | a;
         }
      }
      else
         throw Exception(GLU::join("Targa with bit depth ", bits, " not supported!"));

      return img;
   }

   RenderBuffer::RenderBuffer()
   {
      GLSYM(glGenFramebuffers)(1, &fb_obj);
      GLSYM(glGenTextures)(1, &tex);
      GLSYM(glGenRenderbuffers)(1, &render_buffer);
   }

   RenderBuffer::RenderBuffer(unsigned width, unsigned height)
      : width(width), height(height)
   {
      GLSYM(glGenFramebuffers)(1, &fb_obj);
      GLSYM(glGenTextures)(1, &tex);
      GLSYM(glGenRenderbuffers)(1, &render_buffer);
      GLSYM(glBindTexture)(GL_TEXTURE_2D, tex);

      GLSYM(glTexImage2D)(GL_TEXTURE_2D,
            0, GL_RGBA,
            width, height,
            0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8, nullptr);

      GLSYM(glTexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
      GLSYM(glTexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
      GLSYM(glTexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      GLSYM(glTexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      GLSYM(glBindTexture)(GL_TEXTURE_2D, 0);

      GLSYM(glBindRenderbuffer)(GL_RENDERBUFFER, render_buffer);
      GLSYM(glRenderbufferStorage)(GL_RENDERBUFFER,
            GL_DEPTH_COMPONENT32,
            width, height);
      GLSYM(glBindRenderbuffer)(GL_RENDERBUFFER, 0);

      GLSYM(glBindFramebuffer)(GL_FRAMEBUFFER, fb_obj);
      GLSYM(glFramebufferTexture2D)(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_2D, tex, 0);
      GLSYM(glFramebufferRenderbuffer)(GL_FRAMEBUFFER,
            GL_DEPTH_ATTACHMENT,
            GL_RENDERBUFFER, render_buffer);

      if (GLSYM(glCheckFramebufferStatus)(GL_FRAMEBUFFER) !=
            GL_FRAMEBUFFER_COMPLETE)
      {
         throw Exception("Framebuffer is not complete!");
      }

      GLSYM(glBindFramebuffer)(GL_FRAMEBUFFER, 0);
   }

   void RenderBuffer::bind()
   {
      GLSYM(glBindFramebuffer)(GL_FRAMEBUFFER, fb_obj);
   }

   void RenderBuffer::bind_texture(unsigned index)
   {
      GLSYM(glActiveTexture)(GL_TEXTURE0 + index);
      GLSYM(glBindTexture)(GL_TEXTURE_2D, tex);
      bound_index = index;
   }

   void RenderBuffer::unbind_texture()
   {
      GLSYM(glActiveTexture)(GL_TEXTURE0 + bound_index);
      GLSYM(glBindTexture)(GL_TEXTURE_2D, 0);
      bound_index = 0;
   }

   void RenderBuffer::unbind()
   {
      GLSYM(glBindFramebuffer)(GL_FRAMEBUFFER, 0);
   }

   void RenderBuffer::size(unsigned &width, unsigned &height) const
   {
      width = this->width;
      height = this->height;
   }

   RenderBuffer::~RenderBuffer()
   {
      GLSYM(glDeleteFramebuffers)(1, &fb_obj);
      GLSYM(glDeleteTextures)(1, &tex);
      GLSYM(glDeleteRenderbuffers)(1, &render_buffer);
   }

   ShadowBuffer::ShadowBuffer(unsigned width, unsigned height)
      : RenderBuffer()
   {
      this->width = width;
      this->height = height;
      GLSYM(glBindTexture)(GL_TEXTURE_2D, tex);
      GLSYM(glTexImage2D)(GL_TEXTURE_2D,
            0, GL_DEPTH_COMPONENT32F,
            width, height,
            0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr);
      GLSYM(glTexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
      GLSYM(glTexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
      GLSYM(glTexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      GLSYM(glTexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      GLSYM(glTexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
      GLSYM(glTexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
      GLSYM(glBindTexture)(GL_TEXTURE_2D, 0);

      GLSYM(glBindFramebuffer)(GL_FRAMEBUFFER, fb_obj);

      GLSYM(glFramebufferTexture2D)(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
            GL_TEXTURE_2D, tex, 0);
      glDrawBuffer(GL_NONE);

      if (GLSYM(glCheckFramebufferStatus)(GL_FRAMEBUFFER) !=
            GL_FRAMEBUFFER_COMPLETE)
      {
         throw Exception("Framebuffer is not complete!");
      }

      GLSYM(glBindFramebuffer)(GL_FRAMEBUFFER, 0);
   }
}

