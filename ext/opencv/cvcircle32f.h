/************************************************************

   cvcircle32f.h -

   $Author: lsxi $

   Copyright (C) 2007 Masakazu Yonekura

************************************************************/
#ifndef RUBY_OPENCV_CVCIRCLE32F_H
#define RUBY_OPENCV_CVCIRCLE32F_H

#include <ruby.h>
#include "opencv2/core/core_c.h"
#include "opencv2/core/types_c.h"

namespace mOpenCV {
  typedef struct CvCircle32f {
    CvPoint2D32f center;
    float radius;
  } CvCircle32f;

  namespace cCvCircle32f {
    VALUE rb_class();
    void init_ruby_class();
    VALUE rb_allocate(VALUE klass);
    VALUE rb_initialize(int argc, VALUE *argv, VALUE self);
    VALUE rb_center(VALUE self);
    VALUE rb_radius(VALUE self);
    VALUE rb_aref(VALUE self, VALUE index);
    VALUE rb_to_ary(VALUE self);
    VALUE new_object(CvCircle32f circle32f);
  }

  inline CvCircle32f* CVCIRCLE32F(VALUE object) {
    CvCircle32f *ptr;
    Data_Get_Struct(object, CvCircle32f, ptr);
    return ptr;
  }
}
#endif // RUBY_OPENCV_CVCIRCLE32F_H
