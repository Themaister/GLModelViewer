#include "object.hpp"
#include "utils.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <array>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <map>
#include <assert.h>

namespace GLU
{
   enum Attr
   {
      Vertex,
      Normal,
      Texture,
      Elem
   };

   static void parse_indices(char *list, size_t indices[3], size_t offsets[3])
   {
      for (unsigned i = 0; i < 3; i++)
         indices[i] = 0;

      char *end = list;
      for (unsigned i = 0; i < 3; i++, end++)
      {
         char *old = end;

         int indice = std::strtol(old, &end, 0);
         if (indice > 0)
            indices[i] = indice;
         else if (indice < 0) // Relative to end
            indices[i] = offsets[i] + indice + 1;

         if (end[0] == '\0')
            break;
      }
   }

   static bool get_attr(char *line, Attr &attr, float elems[3], size_t indices[3][3], size_t offsets[3])
   {
      char *elem = std::strtok(line, " ");
      if (!elem)
         return false;

      std::string vert_type = elem;
      if (vert_type == "v")
         attr = Vertex;
      else if (vert_type == "vn")
         attr = Normal;
      else if (vert_type == "vt")
         attr = Texture;
      else if (vert_type == "f")
         attr = Elem;
      else
         return false;

      if (attr == Elem)
      {
         for (unsigned i = 0; i < 3 && elem; i++)
         {
            elem = std::strtok(nullptr, " ");
            if (elem)
               parse_indices(elem, indices[i], offsets);
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
               elems[i] = static_cast<float>(strtod(elem, nullptr));
         }
      }

      return true;
   }

   typedef std::vector<std::array<float, 3>> lvec3;
   typedef std::vector<std::array<float, 2>> lvec2;

   static GL::Geo::Triangle triangle_from_indices(
         const lvec3 &vertices, const lvec3 &normals, const lvec2 &tex_coords, size_t indices[3][3])
   {
      GL::Geo::Triangle tri;
      std::memset(&tri, 0, sizeof(tri));

      for (unsigned i = 0; i < 3; i++)
      {
         if (indices[i][0])
         {
            size_t real_indice = indices[i][0] - 1;
            if (real_indice >= vertices.size())
               throw GL::Exception("Object face index exceeds maximum recorded vertices!");

            for (unsigned j = 0; j < 3; j++)
               tri.coord[i].vertex[j] = vertices[real_indice][j];
         }

         if (indices[i][1])
         {
            size_t real_indice = indices[i][1] - 1;
            if (real_indice >= tex_coords.size())
               throw GL::Exception("Object face index exceeds maximum recorded texture coordinates!");

            for (unsigned j = 0; j < 2; j++)
               tri.coord[i].tex[j] = tex_coords[real_indice][j];
         }

         if (indices[i][2])
         {
            size_t real_indice = indices[i][2] - 1;
            if (real_indice >= normals.size())
               throw GL::Exception("Object face index exceeds maximum recorded normal coordinates!");

            for (unsigned j = 0; j < 3; j++)
               tri.coord[i].normal[j] = normals[real_indice][j];
         }
      }

      return tri;
   }

   std::vector<GL::Geo::Triangle> LoadObject(const std::string &path)
   {
      std::vector<GL::Geo::Triangle> triangles;
      lvec3 vertices;
      lvec3 normals;
      lvec2 tex_coords;

      std::fstream file(path, std::ios::in);
      if (!file.is_open())
         throw GL::Exception(join("Failed to open OBJ: ", path));

      while (!file.eof())
      {
         char buf_[256];
         file.getline(buf_, sizeof(buf_));
         if (file.bad())
            throw GL::Exception("Failed to load object!");

         std::string buf(buf_);

         float elems[3];
         size_t indices[3][3];
         Attr attr;
         size_t arr[3] = { vertices.size(), tex_coords.size(), normals.size() };

         std::vector<char> mut_buf(buf.begin(), buf.end());
         mut_buf.push_back('\0');

         if (get_attr(mut_buf.data(), attr, elems, indices, arr))
         {
            switch (attr)
            {
               case Vertex:
               {
                  std::array<float, 3> arr;
                  for (unsigned i = 0; i < 3; i++)
                     arr[i] = elems[i];

                  vertices.push_back(arr);
                  break;
               }

               case Texture:
               {
                  std::array<float, 2> arr;
                  for (unsigned i = 0; i < 2; i++)
                     arr[i] = elems[i];
                  tex_coords.push_back(arr);
                  break;
               }

               case Normal:
               {
                  std::array<float, 3> arr;
                  for (unsigned i = 0; i < 3; i++)
                     arr[i] = elems[i];
                  normals.push_back(arr);
                  break;
               }

               case Elem:
                  triangles.push_back(triangle_from_indices(vertices, normals, tex_coords, indices));
                  break;

               default:
                  break;
            }
         }
      }

      return triangles;
   }

   std::vector<std::shared_ptr<GL::Mesh>> LoadTexturedMeshes(const std::string &path)
   {
      std::vector<std::shared_ptr<GL::Mesh>> meshes;
      std::vector<GL::Geo::Triangle> triangles;

      lvec3 vertices;
      lvec3 normals;
      lvec2 tex_coords;

      std::fstream file(path, std::ios::in);
      if (!file.is_open())
         throw GL::Exception(join("Failed to open OBJ: ", path));

      std::string directory = path;
      auto itr = directory.find_last_of("/\\");
      if (itr != std::string::npos)
         directory = directory.substr(0, itr + 1);
      else
         directory = "";

      std::string current_material;
      std::map<std::string, std::shared_ptr<GL::Texture>> tex_map;

      while (!file.eof())
      {
         char buf_[256];
         file.getline(buf_, sizeof(buf_));
         if (file.bad())
            throw GL::Exception("Failed to load object!");

         std::string buf(buf_);

         if (std::strstr(buf.c_str(), "texture") == buf.c_str())
         {
            if (triangles.size() > 0)
            {
               meshes.push_back(std::make_shared<GL::Mesh>(triangles));
               if (current_material.size() > 0)
               {
                  auto ptr = tex_map[current_material];
                  if (ptr)
                     meshes.back()->set_texture(ptr);
                  else
                  {
                     auto tex = std::make_shared<GL::Texture>(current_material);
                     meshes.back()->set_texture(tex);
                     tex_map[current_material] = tex;
                  }
               }

               triangles.clear();
            }

            current_material = directory;
            auto path = buf.substr(buf.find_last_of(' ') + 1);
            path = path.substr(0, path.find_last_of('\r'));
            current_material += path;
            current_material += ".tga";
         }

         float elems[3];
         size_t indices[3][3];
         Attr attr;
         size_t arr[3] = { vertices.size(), tex_coords.size(), normals.size() };

         std::vector<char> mut_buf(buf.begin(), buf.end());
         mut_buf.push_back('\0');

         if (get_attr(mut_buf.data(), attr, elems, indices, arr))
         {
            switch (attr)
            {
               case Vertex:
               {
                  std::array<float, 3> arr;
                  for (unsigned i = 0; i < 3; i++)
                     arr[i] = elems[i];
                  vertices.push_back(arr);
                  break;
               }

               case Texture:
               {
                  std::array<float, 2> arr;
                  for (unsigned i = 0; i < 2; i++)
                     arr[i] = elems[i];
                  tex_coords.push_back(arr);
                  break;
               }

               case Normal:
               {
                  std::array<float, 3> arr;
                  for (unsigned i = 0; i < 3; i++)
                     arr[i] = elems[i];
                  normals.push_back(arr);
                  break;
               }

               case Elem:
                  triangles.push_back(triangle_from_indices(vertices, normals, tex_coords, indices));
                  break;

               default:
                  break;
            }
         }
      }

      if (triangles.size() > 0)
      {
         meshes.push_back(std::make_shared<GL::Mesh>(triangles));
         if (current_material.size() > 0)
         {
            auto ptr = tex_map[current_material];
            if (ptr)
               meshes.back()->set_texture(ptr);
            else
            {
               auto tex = std::make_shared<GL::Texture>(current_material);
               meshes.back()->set_texture(tex);
               tex_map[current_material] = tex;
            }
         }
      }

      return meshes;
   }
}
