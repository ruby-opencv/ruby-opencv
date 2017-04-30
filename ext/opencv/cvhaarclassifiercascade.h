/************************************************************

   cvhaarclassifiercascade.h -

   $Author: lsxi $

   Copyright (C) 2005-2006 Masakazu Yonekura

************************************************************/
#ifndef RUBY_OPENCV_CVHAARCLASSIFIERCASCADE_H
#define RUBY_OPENCV_CVHAARCLASSIFIERCASCADE_H

#include <ruby.h>
#include "opencv2/objdetect.hpp"

namespace mOpenCV {
  namespace cCvHaarClassifierCascade {
    VALUE rb_class();
    void init_ruby_class();
    VALUE rb_allocate(VALUE klass);
    VALUE rb_load(VALUE klass, VALUE path);
    VALUE rb_detect_objects(int argc, VALUE *argv, VALUE self);
  }

  inline CvHaarClassifierCascade*
    CVHAARCLASSIFIERCASCADE(VALUE object) {
    CvHaarClassifierCascade *ptr;
    Data_Get_Struct(object, CvHaarClassifierCascade, ptr);
    return ptr;
  }
}

#endif // RUBY_OPENCV_CVHAARCLASSIFIERCASCADE_H
