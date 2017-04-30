/************************************************************

   cvtwopoints.h -

   $Author: lsxi $

   Copyright (C) 2005-2006 Masakazu Yonekura

************************************************************/
#ifndef RUBY_OPENCV_CVTWOPOINTS_H
#define RUBY_OPENCV_CVTWOPOINTS_H

#include <ruby.h>
#include "opencv2/imgproc/types_c.h"

namespace mOpenCV {
  typedef struct CvTwoPoints {
    CvPoint p1;
    CvPoint p2;
  } CvTwoPoints;

  namespace cCvTwoPoints {
    VALUE rb_class();
    void init_ruby_class();
    VALUE rb_allocate(VALUE klass);
    VALUE rb_point1(VALUE self);
    VALUE rb_point2(VALUE self);
    VALUE rb_aref(VALUE self, VALUE index);
    VALUE rb_to_ary(VALUE self);
    VALUE new_object(CvTwoPoints twopoints);
  }

  inline CvTwoPoints*
    CVTWOPOINTS(VALUE object) {
    CvTwoPoints *ptr;
    Data_Get_Struct(object, CvTwoPoints, ptr);
    return ptr;
  }
}
#endif // RUBY_OPENCV_CVTWOPOINTS_H
