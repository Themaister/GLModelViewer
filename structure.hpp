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
         float normal[3];
         float tex[2];
         float color[4];
      };

      enum 
      { 
         VertexOffset = offsetof(Coord, vertex),
         NormalOffset = offsetof(Coord, normal),
         TextureOffset = offsetof(Coord, tex),
         ColorOffset = offsetof(Coord, color)
      };

      struct Triangle
      {
         Coord coord[3];
      };
   }
}

#endif
