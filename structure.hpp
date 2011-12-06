#ifndef STRUCTURE_H__
#define STRUCTURE_H__

#include <stddef.h>
#include "linear.hpp"

namespace GL
{
   namespace Geo
   {
      struct Coord
      {
         float vertex[3];
         float tex[2];
         float normal[3];
      };

      enum 
      { 
         VertexOffset = offsetof(Coord, vertex),
         TextureOffset = offsetof(Coord, tex),
         NormalOffset = offsetof(Coord, normal)
      };

      struct Triangle
      {
         Coord coord[3];
      };
   }
}

#endif
