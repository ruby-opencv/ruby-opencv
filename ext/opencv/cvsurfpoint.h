/************************************************************

   cvsurfpoint.h -

   $Author: ser1zw $

   Copyright (C) 2011 ser1zw

************************************************************/
#ifndef RUBY_OPENCV_CVSURFPOINT_H
#define RUBY_OPENCV_CVSURFPOINT_H

#include "ruby.h"
#ifdef IS_OPENCV2
#  include "opencv2/legacy/compat.hpp"
#endif

namespace mOpenCV {
  namespace cCvSURFPoint {
    VALUE rb_class();
    void init_ruby_class();
  }

#ifdef IS_OPENCV2
  inline CvSURFPoint*
  CVSURFPOINT(VALUE object)
  {
    CvSURFPoint* ptr;
    Data_Get_Struct(object, CvSURFPoint, ptr);
    return ptr;
  }
#endif
}
#endif // RUBY_OPENCV_CVSURFPOINT_H
