#ifndef TEXTURE_HPP__
#define TEXTURE_HPP__

#include "gl.hpp"
#include "utils.hpp"
#include <list>
#include <utility>

namespace GL
{
   class Texture : private GLU::SmartDefs<Texture>, public GLResource
   {
      public:
         DECL_SHARED(Texture);

         Texture(const std::string &path);
         ~Texture();

         enum class Edge : unsigned { Clamp, ClampToBorder, Repeat };
         enum class Filter : unsigned { Nearest, Linear };

         void bind(unsigned index = 0,
               Filter filt = Filter::Linear,
               Edge edge = Edge::Repeat);
         void unbind();

         void operator=(const Texture&) = delete;

      private:
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
}

#endif

