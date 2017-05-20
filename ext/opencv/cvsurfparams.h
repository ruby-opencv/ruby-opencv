/************************************************************

   cvsurfparams.h -

   $Author: ser1zw $

   Copyright (C) 2011 ser1zw

************************************************************/
#ifndef RUBY_OPENCV_CVSURFPARAMS_H
#define RUBY_OPENCV_CVSURFPARAMS_H

#include "ruby.h"
#ifdef IS_OPENCV2
#  include "opencv2/legacy/compat.hpp"
#endif
#include "cvutils.h"

namespace mOpenCV {
  namespace cCvSURFParams {
    VALUE rb_class();
    void init_ruby_class();
  }

#ifdef IS_OPENCV2
  inline CvSURFParams*
  CVSURFPARAMS(VALUE object)
  {
    CvSURFParams* ptr;
    Data_Get_Struct(object, CvSURFParams, ptr);
    return ptr;
  }

  inline CvSURFParams*
  CVSURFPARAMS_WITH_CHECK(VALUE object)
  {
    if (!rb_obj_is_kind_of(object, cCvSURFParams::rb_class()))
      raise_typeerror(object, cCvSURFParams::rb_class());
    return CVSURFPARAMS(object);
  }
#endif
}

#endif // RUBY_OPENCV_CVSURFPARAMS_H
