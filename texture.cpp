#include "texture.hpp"
#include <string>
#include <algorithm>
#include <iostream>

namespace GL
{
   std::list<Texture *> Texture::bound_textures;

   Texture::Texture(const std::string &path) : obj(0), bound_index(-1)
   {
      GLFWimage img;
      if (glfwReadImage(path.c_str(), &img,
            GLFW_NO_RESCALE_BIT | GLFW_ALPHA_MAP_BIT) != GL_TRUE)
      {
         throw Exception(GLU::join("Failed to load texture: ", path));
      }

      glGenTextures(1, &obj);

      auto itr = std::find_if(std::begin(bound_textures), std::end(bound_textures),
            [](const Texture *tex) { return tex->bound_index == 0; });

      Texture *rebind_tex = nullptr;
      if (itr != std::end(bound_textures))
         rebind_tex = *itr;

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, obj);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
            img.Width, img.Height, 0, img.Format, GL_UNSIGNED_BYTE, img.Data);
      glBindTexture(GL_TEXTURE_2D, rebind_tex ? rebind_tex->obj : 0);

      glfwFreeImage(&img);
   }

   Texture::~Texture()
   {
      if (obj)
      {
         unbind();
         glDeleteTextures(1, &obj);
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

      glActiveTexture(GL_TEXTURE0 + bound_index);
      glBindTexture(GL_TEXTURE_2D, obj);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, gl_edge(edge));
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, gl_edge(edge));
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gl_filter(filter));
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, gl_filter(filter));
   }

   void Texture::unbind()
   {
      if (bound_index >= 0)
      {
         bound_textures.erase(std::find_if(std::begin(bound_textures), std::end(bound_textures),
                  [this](const Texture *tex) { return this == tex; }));

         glActiveTexture(GL_TEXTURE0 + bound_index);
         glBindTexture(GL_TEXTURE_2D, 0);
      }

      bound_index = -1;
   }
}

