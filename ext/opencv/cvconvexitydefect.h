/************************************************************

   cvconvexitydefect.h -

   $Author: lsxi $

   Copyright (C) 2007 Masakazu Yonekura

************************************************************/
#ifndef RUBY_OPENCV_CVCONVEXITYDEFECT_H
#define RUBY_OPENCV_CVCONVEXITYDEFECT_H

#include <ruby.h>
#include "opencv2/imgproc/types_c.h"

namespace mOpenCV {
  namespace cCvConvexityDefect {
    VALUE rb_class();
    void init_ruby_class();
    VALUE rb_start(VALUE self);
    VALUE rb_end(VALUE self);
    VALUE rb_depth_point(VALUE self);
    VALUE rb_depth(VALUE self);
  }

  inline CvConvexityDefect* CVCONVEXITYDEFECT(VALUE object) {
    CvConvexityDefect *ptr;
    Data_Get_Struct(object, CvConvexityDefect, ptr);
    return ptr;
  }
}

#endif // RUBY_OPENCV_CVCONVEXITYDEFECT_H
