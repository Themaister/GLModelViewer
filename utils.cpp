#include "utils.hpp"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <cmath>

namespace GLU
{
   std::string FileToString(const std::string &path)
   {
      std::ifstream file(path);
      if (!file.is_open())
         throw GL::Exception(join("Failed to open file: ", path));

      std::stringstream str;
      str << file.rdbuf();
      return str.str();
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

      GL::vec3 Normalize(const GL::vec3 &dir)
      {
         float factor = 1.0 / std::sqrt(dir(0) * dir(0) + dir(1) * dir(1) + dir(2) * dir(2));
         return factor * dir;
      }

      GL::GLMatrix Derotate(const GL::vec3 &dir)
      {
         auto norm_dir = Normalize(dir);

         float x = norm_dir(0);
         float y = norm_dir(1);
         float z = norm_dir(2);

         float y_rot;
	 // Need to rotate y in reverse due to Z flipping sign in projection.
         if (x > 0.0)
            y_rot = -std::acos(-z) * 180 / M_PI;
         else
            y_rot = std::acos(-z) * 180 / M_PI;

         float x_rot = std::asin(-y) * 180 / M_PI;

         return Rotate(x_rot, y_rot, 0.0);
      }
   }
}
