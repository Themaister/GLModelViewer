#ifndef UTILS_H__
#define UTILS_H__

#define DECL_SHARED(type) using ::GLU::SmartDefs< type >::Ptr; \
   using ::GLU::SmartDefs< type >::shared

#define DECL_UNIQUE(type) using ::GLU::SmartDefs< type >::UPtr; \
   using ::GLU::SmartDefs< type >::unique

#define DECL_SMART(type) DECL_SHARED(type); \
   DECL_UNIQUE(type)

#include <string>
#include <sstream>

namespace GLU
{
   template <class T>
   std::string join(const T& t)
   {
      std::ostringstream stream;
      stream << t;
      return stream.str();
   }

   template <class T, class R, class... P>
   std::string join(const T& t, const R& r, const P&... p)
   {
      std::ostringstream stream;
      stream << t << join(r, p...);
      return stream.str();
   }

   template <class T>
   class RefCounted
   {
      public:
         unsigned& ref()
         {
            return cnt;
         }

      private:
         static unsigned cnt;
   };
   template <class T>
   unsigned RefCounted<T>::cnt = 0;
}

#include <memory>
#include <iomanip>

namespace GLU
{
   // Template magic incoming!
   namespace Internal
   {
      template <class T>
      struct DeclareShared
      {
         typedef std::shared_ptr<T> type;
      };

      template <class T>
      struct DeclareUnique
      {
         typedef std::unique_ptr<T> type;
      };
   }

   // Inherit this privately to use a generic smart pointer interface.
   template <class T>
   struct SmartDefs
   {
      typedef typename Internal::DeclareShared<T>::type Ptr;
      typedef typename Internal::DeclareUnique<T>::type UPtr;

      template <class... P>
      static Ptr shared(P&&... p)
      {
         return std::make_shared<T>(std::forward<P>(p)...);
      }

      template <class... P>
      static UPtr unique(P&&... p)
      {
         return std::unique_ptr<T>(new T(std::forward<P>(p)...));
      }
   };
   
   std::string FileToString(const std::string &path);
}

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

      enum class Rotation
      {
         X,
         Y,
         Z
      };

      GL::GLMatrix Rotate(Rotation dir, GLfloat degrees);
      GL::GLMatrix Rotate(GLfloat x_deg, GLfloat y_deg, GLfloat z_deg);

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
   }
}

#endif
