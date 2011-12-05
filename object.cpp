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

   static bool get_attr(char *line, Attr &attr, float elems[4], GLuint index[3])
   {
      const char *elem = std::strtok(line, " ");
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
               index[i] = strtoul(elem, nullptr, 0);
            else
               return false;
         }
      }
      else
      {
         for (unsigned i = 0; i < 4 && elem; i++)
         {
            elem = std::strtok(nullptr, " ");
            if (elem)
               elems[i] = strtof(elem, nullptr);
         }
      }

      return true;
   }

#if 0
   static void log_mesh(const std::vector<GL::Geo::Coord> &coords, const std::vector<GLuint> &elem_index)
   {
      std::cout << "Loaded mesh:" << std::endl;
      std::cout << "\tCoords:" << coords.size() << std::endl;
      std::cout << "\tElems:" << elem_index.size() << std::endl;

      std::cout << "Vertices:" << std::endl;
      for (unsigned i = 0; i < 3; i++)
      {
         for (unsigned j = 0; j < 3; j++)
         {
            std::cout << coords[i].vertex[j] << " ";
         }
         std::cout << std::endl;
      }

      std::cout << "Normals:" << std::endl;
      for (unsigned i = 0; i < 3; i++)
      {
         for (unsigned j = 0; j < 3; j++)
         {
            std::cout << coords[i].normal[j] << " ";
         }
         std::cout << std::endl;
      }

      std::cout << "Elems:" << std::endl;
      for (unsigned i = 0; i < 3; i++)
         std::cout << elem_index[i] << ", ";
      std::cout << std::endl;
   }
#endif

   void LoadObject(const std::string &path,
         std::vector<GL::Geo::Coord> &coords, std::vector<GLuint> &elem_index)
   {
      std::vector<std::array<float, 3>> vertices;
      std::vector<std::array<float, 3>> normals;
      std::vector<std::array<float, 2>> tex_coords;

      std::fstream file(path, std::ios::in);
      if (!file.is_open())
         throw GL::Exception(join("Failed to open OBJ: ", path));

      while (!file.eof())
      {
         char buf[128];
         file.getline(buf, sizeof(buf));

         float elems[4];
         GLuint index[3];
         Attr attr;
         if (get_attr(buf, attr, elems, index))
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
                  if (index[0] == 0 || index[1] == 0 || index[2] == 0)
                     throw GL::Exception("Invalid object. Element indice of 0!");
                  elem_index.push_back(index[0] - 1);
                  elem_index.push_back(index[1] - 1);
                  elem_index.push_back(index[2] - 1);
                  break;

               default:
                  break;
            }
         }
      }

      size_t indices = std::min(std::min(vertices.size(), normals.size()), tex_coords.size());
      for (size_t i = 0; i < indices; i++)
      {
         GL::Geo::Coord coord;
         coord.vertex[0] = vertices[i][0];
         coord.vertex[1] = vertices[i][1];
         coord.vertex[2] = vertices[i][2];
         coord.normal[0] = normals[i][0];
         coord.normal[1] = normals[i][1];
         coord.normal[2] = normals[i][2];
         coord.tex[0]    = tex_coords[i][0];
         coord.tex[1]    = tex_coords[i][1];

         coords.push_back(coord);
      }

#if 0
      log_mesh(coords, elem_index);
#endif
   }
}
