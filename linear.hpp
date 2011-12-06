#ifndef LINEAR_HPP__
#define LINEAR_HPP__

#include <algorithm>
#include <utility>
#include <array>

namespace GL
{
   template <class T, unsigned N = 4>
   class Vector
   {
      public:
         Vector()
         {
            std::fill(vec.begin(), vec.end(), 0);
         }

         Vector(const std::initializer_list<T> &list)
         {
            if (list.size() != N)
               throw Exception("Invalid size of vector list...\n");
            std::copy(list.begin(), list.end(), vec.begin());
         }

         T* operator()()
         {
            return &vec[0];
         }

         const T* operator()() const
         {
            return &vec[0];
         }

         T& operator()(unsigned index)
         {
            return vec[index];
         }

         const T& operator()(unsigned index) const
         {
            return vec[index];
         }

         Vector<T, N>& operator=(const Vector<T, N> &in)
         {
            vec = in.vec;
            return *this;
         }

         Vector(const Vector<T, N> &in)
         {
            *this = in;
         }

         Vector<T, N> operator+(const Vector<T, N> &in) const
         {
            Vector<T, N> res;
            for (unsigned i = 0; i < N; i++)
               res(i) = operator()(i) + in(i);
            return res;
         }

         Vector<T, N> operator-(const Vector<T, N> &in) const
         {
            Vector<T, N> res;
            for (unsigned i = 0; i < N; i++)
               res(i) = operator()(i) - in(i);
            return res;
         }

         Vector<T, N> operator*(const Vector<T, N> &in) const
         {
            Vector<T, N> res;
            for (unsigned i = 0; i < N; i++)
               res(i) = operator()(i) * in(i);
            return res;
         }

      private:
         std::array<T, N> vec;
   };

   template <class T>
   class Matrix
   {
      public:
         Matrix()
         {
            std::fill(matrix, matrix + 16, 0);
         }

         Matrix(const float *matrix_, unsigned size = 16)
         {
            std::copy(matrix_, matrix_ + std::min(16u, size), matrix);
         }

         Matrix<T>& operator=(const Matrix<T> &mat)
         {
            std::copy(mat.matrix, mat.matrix + 16, matrix);
            return *this;
         }

         Matrix(const Matrix<T> &in)
         {
            *this = in;
         }

         T& operator()(unsigned r, unsigned c)
         {
            return matrix[r * 4 + c];
         }

         const T& operator()(unsigned r, unsigned c) const
         {
            return matrix[r * 4 + c];
         }

         T* operator()()
         {
            return matrix;
         }

         const T* operator()() const
         {
            return matrix;
         }

         Matrix<T> operator+(const Matrix<T> &in) const
         {
            Matrix<T> ret;
            for (unsigned i = 0; i < 4; i++)
               for (unsigned j = 0; j < 4; j++)
                  ret(i, j) = matrix(i, j) + in(i, j);

            return ret;
         }

         Matrix<T> operator-(const Matrix<T> &in) const
         {
            Matrix<T> ret;
            for (unsigned i = 0; i < 4; i++)
               for (unsigned j = 0; j < 4; j++)
                  ret(i, j) = matrix(i, j) - in(i, j);

            return ret;
         }

         Matrix<T> operator*(const Matrix<T> &in) const
         {
            Matrix<T> out;
            const Matrix<T> &a = *this;
            const Matrix<T> &b = in;
            for (unsigned i = 0; i < 4; i++)
            {
               for (unsigned j = 0; j < 4; j++)
               {
                  T dot = 0;
                  for (unsigned k = 0; k < 4; k++)
                  {
                     dot += a(i, k) * b(k, j);
                  }
                  out(i, j) = dot;
               }
            }

            return out;
         }

      private:
         T matrix[16];
   };

   typedef Matrix<GLfloat> GLMatrix;

   typedef Vector<GLfloat, 2> vec2;
   typedef Vector<GLfloat, 3> vec3;
   typedef Vector<GLfloat, 4> vec4;
   typedef Vector<GLint, 2> ivec2;
   typedef Vector<GLint, 3> ivec3;
   typedef Vector<GLint, 4> ivec4;

   template <class T>
   Vector<T, 4> operator*(const Matrix<T>& mat, const Vector<T, 4> &vec)
   {
      Vector<T, 4> out;
      for (unsigned i = 0; i < 4; i++)
      {
         T sum = 0;
         for (unsigned j = 0; j < 4; j++)
         {
            sum += mat(i, j) * vec(j);
         }
         out(i) = sum;
      }
      return out;
   }

   template <class T, unsigned N>
   Vector<T, N> operator*(T scale, const Vector<T, N> &in)
   {
      Vector<T, N> out;
      for (unsigned i = 0; i < N; i++)
      {
         out(i) = scale * in(i);
      }
      return out;
   }

   template <class T, unsigned N>
   Matrix<T> operator*(T scale, const Matrix<T> &in)
   {
      Matrix<T> out;
      for (unsigned i = 0; i < 4; i++)
      {
         for (unsigned j = 0; j < 4; j++)
         {
            out(i, j) = scale * in(i, j);
         }
      }
      return out;
   }

   template <unsigned From, unsigned To, class T>
   Vector<T, To> vec_conv(const Vector<T, From> &in)
   {
      Vector<T, To> out;
      unsigned indices = std::min(To, From);
      for (unsigned i = 0; i < indices; i++)
         out(i) = in(i);

      return out;
   }
}


#endif
