#ifndef LINEAR_HPP__
#define LINEAR_HPP__

#include <algorithm>
#include <utility>
#include <array>
#include <type_traits>

namespace GL
{
   class Exception;

   template <class T, unsigned N>
   class Vector;
   template <unsigned M, unsigned N, class T>
   Vector<T, N> vec_conv(const Vector<T, M> &in);

   template <class T, unsigned N = 4>
   class Vector
   {
      public:
         Vector()
         {
            std::fill(vec.begin(), vec.end(), static_cast<T>(0));
         }

         Vector(const T* list)
         {
            std::copy(list, list + N, vec.begin());
         }

         template <class U>
         Vector(typename std::enable_if<N == 1, U>::type t1)
         {
            vec[0] = static_cast<T>(t1);
         }

         template <class U>
         Vector(typename std::enable_if<N == 2, U>::type t1,
            U t2 = static_cast<U>(0))
         {
            vec[0] = static_cast<T>(t1);
            vec[1] = static_cast<T>(t2);
         }

         template <class U>
         Vector(typename std::enable_if<N == 3, U>::type t1,
               U t2 = static_cast<U>(0),
               U t3 = static_cast<U>(0))
         {
            vec[0] = static_cast<T>(t1);
            vec[1] = static_cast<T>(t2);
            vec[2] = static_cast<T>(t3);
         }

         template <class U>
         Vector(typename std::enable_if<N == 4, U>::type t1,
               U t2 = static_cast<U>(0),
               U t3 = static_cast<U>(0),
               U t4 = static_cast<U>(0))
         {
            vec[0] = static_cast<T>(t1);
            vec[1] = static_cast<T>(t2);
            vec[2] = static_cast<T>(t3);
            vec[3] = static_cast<T>(t4);
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

         template <unsigned M>
         Vector(const Vector<T, M> &in)
         {
            *this = vec_conv<M, N>(in);
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

         Vector<T, N>& operator+=(const Vector<T, N> &in)
         {
            for (unsigned i = 0; i < N; i++)
               operator()(i) += in(i);
            return *this;
         }

         Vector<T, N>& operator-=(const Vector<T, N> &in)
         {
            for (unsigned i = 0; i < N; i++)
               operator()(i) -= in(i);
            return *this;
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
            std::fill(matrix, matrix + 16, static_cast<T>(0));
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

         Matrix<T>& operator+=(const Matrix<T> &in)
         {
            for (unsigned i = 0; i < 4; i++)
               for (unsigned j = 0; j < 4; j++)
                  operator()(i, j) += in(i, j);

            return *this;
         }

         Matrix<T>& operator-=(const Matrix<T> &in)
         {
            for (unsigned i = 0; i < 4; i++)
               for (unsigned j = 0; j < 4; j++)
                  operator()(i, j) -= in(i, j);

            return *this;
         }

         Matrix<T>& operator*=(const Matrix<T> &in)
         {
            *this = in * (*this); 
            return *this;
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
   Vector<T, N> operator-(const Vector<T, N> &in)
   {
      Vector<T, N> ret;
      for (unsigned i = 0; i < N; i++)
         ret(i) = -in(i);
      return ret;
   }

   template <class T>
   Matrix<T> operator-(const Matrix<T> &in)
   {
      Matrix<T> out;
      for (unsigned i = 0; i < 4; i++)
         for (unsigned j = 0; j < 4; j++)
            out(i, j) = -in(i, j);
      return out;
   }

   template <class T, unsigned N>
   Matrix<T> operator*(T scale, const Matrix<T> &in)
   {
      Matrix<T> out;
      for (unsigned i = 0; i < 4; i++)
         for (unsigned j = 0; j < 4; j++)
            out(i, j) = scale * in(i, j);
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
