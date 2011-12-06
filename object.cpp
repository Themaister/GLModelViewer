#include "object.hpp"
#include "utils.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <array>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <assert.h>

namespace GLU
{
   enum class Attr
   {
      Vertex,
      Normal,
      Texture,
      Elem
   };

   static void parse_indices(char *list, unsigned indices[3])
   {
      for (unsigned i = 0; i < 3; i++)
         indices[i] = 0;

      char *end = list;
      for (unsigned i = 0; i < 3 && *end; i++, end++)
      {
         char *old = end;
         indices[i] = std::strtoul(old, &end, 0);
      }
   }

   static bool get_attr(char *line, Attr &attr, float elems[3], unsigned indices[3][3])
   {
      char *elem = std::strtok(line, " ");
      if (!elem)
         return false;

      std::string vert_type = elem;
      if (vert_type == "v")
         attr = Attr::Vertex;
      else if (vert_type == "vn")
         attr = Attr::Normal;
      else if (vert_type == "vt")
         attr = Attr::Texture;
      else if (vert_type == "f")
         attr = Attr::Elem;
      else
         return false;

      if (attr == Attr::Elem)
      {
         for (unsigned i = 0; i < 3 && elem; i++)
         {
            elem = std::strtok(nullptr, " ");
            if (elem)
               parse_indices(elem, indices[i]);
            else
               return false;
         }
      }
      else
      {
         for (unsigned i = 0; i < 3 && elem; i++)
         {
            elem = std::strtok(nullptr, " ");
            if (elem)
               elems[i] = strtof(elem, nullptr);
         }
      }

      return true;
   }

   typedef std::vector<std::array<float, 3>> vec3;
   typedef std::vector<std::array<float, 2>> vec2;

   static GL::Geo::Triangle triangle_from_indices(
         const vec3 &vertices, const vec3 &normals, const vec2 &tex_coords, unsigned indices[3][3])
   {
      GL::Geo::Triangle tri;
      std::memset(&tri, 0, sizeof(tri));

      for (unsigned i = 0; i < 3; i++)
      {
         if (indices[i][0])
         {
            unsigned real_indice = indices[i][0] - 1;
            if (real_indice >= vertices.size())
               throw GL::Exception("Object face index exceeds maximum recorded vertices!");

            for (unsigned j = 0; j < 3; j++)
               tri.coord[i].vertex[j] = vertices[real_indice][j];
         }

         if (indices[i][1])
         {
            unsigned real_indice = indices[i][1] - 1;
            if (real_indice >= tex_coords.size())
               throw GL::Exception("Object face index exceeds maximum recorded texture coordinates!");

            for (unsigned j = 0; j < 2; j++)
               tri.coord[i].tex[j] = tex_coords[real_indice][j];
         }

         if (indices[i][2])
         {
            unsigned real_indice = indices[i][2] - 1;
            if (real_indice >= normals.size())
               throw GL::Exception("Object face index exceeds maximum recorded normal coordinates!");

            for (unsigned j = 0; j < 3; j++)
               tri.coord[i].normal[j] = normals[real_indice][j];
         }
      }

      return tri;
   }

   void LoadObject(const std::string &path,
         std::vector<GL::Geo::Triangle> &triangles)
   {
      vec3 vertices;
      vec3 normals;
      vec2 tex_coords;

      std::fstream file(path, std::ios::in);
      if (!file.is_open())
         throw GL::Exception(join("Failed to open OBJ: ", path));

      while (!file.eof())
      {
         char buf[128];
         file.getline(buf, sizeof(buf));

         float elems[3];
         unsigned indices[3][3];
         Attr attr;
         if (get_attr(buf, attr, elems, indices))
         {
            switch (attr)
            {
               case Attr::Vertex:
                  vertices.push_back({{elems[0], elems[1], elems[2]}});
                  break;

               case Attr::Normal:
                  normals.push_back({{elems[0], elems[1], elems[2]}});
                  break;
                   
               case Attr::Texture:
                  tex_coords.push_back({{elems[0], elems[1]}});
                  break;

               case Attr::Elem:
                  triangles.push_back(triangle_from_indices(vertices, normals, tex_coords, indices));
                  break;

               default:
                  break;
            }
         }
      }
   }
}
