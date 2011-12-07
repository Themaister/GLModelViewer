#include "utils.hpp"
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iterator>
#include <cmath>

namespace GLU
{
   std::string FileToString(const std::string &path)
   {
      std::fstream file(path, std::ios::in);
      if (!file.is_open())
         throw GL::Exception(join("Failed to open file: ", path));

      file.seekg(0, std::ios_base::end);
      long len = file.tellg();
      file.seekg(0, std::ios_base::beg);

      std::vector<char> vec;
      vec.reserve(len + 1);
      vec[len] = '\0';

      file.read(&vec[0], len);
      return &vec[0];
   }

   namespace Matrices
   {
      GL::GLMatrix Projection(GLfloat zNear, GLfloat zFar)
      {
         GL::GLMatrix mat;

         mat(0, 0) = zNear;
         mat(1, 1) = zNear;
         mat(2, 2) = -(zFar + zNear) / (zFar - zNear);
         mat(2, 3) = -2.0 * zFar * zNear / (zFar - zNear);
         mat(3, 2) = -1.0;

         return mat;
      }

      GL::GLMatrix Identity()
      {
         GL::GLMatrix mat;
         mat(0, 0) = 1.0;
         mat(1, 1) = 1.0;
         mat(2, 2) = 1.0;
         mat(3, 3) = 1.0;
         return mat;
      }

      GL::GLMatrix Scale(GLfloat x, GLfloat y, GLfloat z)
      {
         GL::GLMatrix mat;
         mat(0, 0) = x;
         mat(1, 1) = y;
         mat(2, 2) = z;
         mat(3, 3) = 1.0;
         return mat;
      }

      GL::GLMatrix Scale(GLfloat scale)
      {
         return Scale(scale, scale, scale);
      }

      GL::GLMatrix Translate(GLfloat x, GLfloat y, GLfloat z)
      {
         GL::GLMatrix mat;
         mat(0, 0) = 1.0;
         mat(1, 1) = 1.0;
         mat(2, 2) = 1.0;
         mat(3, 3) = 1.0;
         mat(0, 3) = x;
         mat(1, 3) = y;
         mat(2, 3) = z;
         return mat;
      }

      GL::GLMatrix Rotate(Rotation dir, GLfloat degrees)
      {
         GL::GLMatrix matrix;
         GLfloat cosine = std::cos(M_PI * degrees / 180.0);
         GLfloat sine = std::sin(M_PI * degrees / 180.0); 

         switch (dir)
         {
            case Rotation::Z:
               matrix(0, 0) = cosine;
               matrix(1, 1) = cosine;
               matrix(0, 1) = -sine;
               matrix(1, 0) = sine;
               matrix(2, 2) = 1.0;
               matrix(3, 3) = 1.0;
               break;
            case Rotation::Y:
               matrix(0, 0) = cosine;
               matrix(2, 2) = cosine;
               matrix(0, 2) = -sine;
               matrix(2, 0) = sine;
               matrix(1, 1) = 1.0;
               matrix(3, 3) = 1.0;
               break;
            case Rotation::X:
               matrix(1, 1) = cosine;
               matrix(2, 2) = cosine;
               matrix(1, 2) = -sine;
               matrix(2, 1) = sine;
               matrix(0, 0) = 1.0;
               matrix(3, 3) = 1.0;
               break;
         }

         return matrix;
      }

      GL::GLMatrix Rotate(GLfloat x_deg, GLfloat y_deg, GLfloat z_deg)
      {
         return Rotate(Rotation::X, x_deg) * 
            Rotate(Rotation::Y, y_deg) * 
            Rotate(Rotation::Z, z_deg);
      }

      GL::GLMatrix Transpose(const GL::GLMatrix &mat)
      {
         GL::GLMatrix ret;
         for (unsigned i = 0; i < 4; i++)
            for (unsigned j = 0; j < 4; j++)
               ret(j, i) = mat(i, j);
         return ret;
      }
   }
}
