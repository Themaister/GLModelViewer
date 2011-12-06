#ifndef OBJECT_HPP__
#define OBJECT_HPP__

#include "gl.hpp"
#include "structure.hpp"
#include <vector>

namespace GLU
{
   void LoadObject(const std::string &path, 
         std::vector<GL::Geo::Triangle> &coords);
}

#endif
