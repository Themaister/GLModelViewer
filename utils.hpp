#ifndef UTILS_H__
#define UTILS_H__

#include <string>
#include <sstream>

namespace GLU
{
   // MSVC 2010 doesn't support variadic templates.
   template <class T1> std::string join(const T1& t1)
      { std::ostringstream stream; stream << t1; return stream.str(); }
   template <class T1, class T2> std::string join(const T1& t1, const T2& t2)
      { std::ostringstream stream; stream << t1 << join(t2); return stream.str(); }
   template <class T1, class T2, class T3> std::string join(const T1& t1, const T2& t2, const T3& t3)
      { std::ostringstream stream; stream << t1 << join(t2, t3); return stream.str(); }
   template <class T1, class T2, class T3, class T4> std::string join(const T1& t1, const T2& t2, const T3 &t3, const T4& t4)
      { std::ostringstream stream; stream << t1 << join(t2, t3, t4); return stream.str(); }
   template <class T1, class T2, class T3, class T4, class T5> std::string join(const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5)
      { std::ostringstream stream; stream << t1 << join(t2, t3, t4, t5); return stream.str(); }

   std::string FileToString(const std::string &path);
}

#include <memory>
#include <iomanip>
#include "gl.hpp"
#include "structure.hpp"

namespace GLU
{
   namespace Matrices
   {
      GL::GLMatrix Projection(GLfloat zNear, GLfloat zFar);
      GL::GLMatrix Identity();
      GL::GLMatrix Scale(GLfloat x, GLfloat y, GLfloat z);
      GL::GLMatrix Scale(GLfloat scale);
      GL::GLMatrix Translate(GLfloat x, GLfloat y, GLfloat z);
      GL::GLMatrix Translate(const GL::vec3 &vec);

      enum Rotation
      {
         RotX,
         RotY,
         RotZ
      };

      GL::vec3 Normalize(const GL::vec3 &dir);

      GL::GLMatrix Rotate(Rotation dir, GLfloat degrees);
      GL::GLMatrix Rotate(GLfloat x_deg, GLfloat y_deg, GLfloat z_deg);
      GL::GLMatrix Derotate(const GL::vec3 &dir);

      GL::GLMatrix Transpose(const GL::GLMatrix &mat);

      // For debugging :D
      template <class T>
      std::ostream& operator<<(std::ostream &stream, const GL::Matrix<T> &matrix)
      {
         stream << "================================" << std::endl;
         for (unsigned i = 0; i < 4; i++)
         {
            for (unsigned j = 0; j < 4; j++)
               stream << std::setw(6) << std::fixed << std::setprecision(2) << matrix(i, j) << " ";
            stream << std::endl;
         }
         stream << "================================" << std::endl;

         return stream;
      }

      // For debugging :D
      template <class T, unsigned N>
      std::ostream& operator<<(std::ostream &stream, const GL::Vector<T, N> &vec)
      {
         stream << "================================" << std::endl;
         for (unsigned i = 0; i < N; i++)
         {
            stream << std::setw(6) <<
               std::fixed << std::setprecision(2) <<
               vec(i) << " ";
         }

         stream << std::endl;
         stream << "================================" << std::endl;

         return stream;
      }
   }
}

#endif
