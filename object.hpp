#ifndef OBJECT_HPP__
#define OBJECT_HPP__

#include "gl.hpp"
#include "structure.hpp"
#include "mesh.hpp"
#include <vector>

namespace GLU
{
   std::vector<GL::Geo::Triangle> LoadObject(const std::string &path);
   std::vector<GL::Mesh::Ptr> LoadTexturedMeshes(const std::string &path);
}

#endif
