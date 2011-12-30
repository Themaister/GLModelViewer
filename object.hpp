#ifndef OBJECT_HPP__
#define OBJECT_HPP__

#include "gl.hpp"
#include "structure.hpp"
#include "mesh.hpp"
#include <vector>

namespace GLU
{
   std::vector<GL::Geo::Triangle> LoadObject(const std::string &path);
   std::vector<std::shared_ptr<GL::Mesh>> LoadTexturedMeshes(const std::string &path);
}

#endif
