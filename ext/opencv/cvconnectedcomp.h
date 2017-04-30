/************************************************************

   cvconnectedcomp.h -

   $Author: lsxi $

   Copyright (C) 2005-2007 Masakazu Yonekura

************************************************************/
#ifndef RUBY_OPENCV_CVCONNECTEDCOMP_H
#define RUBY_OPENCV_CVCONNECTEDCOMP_H

#include <ruby.h>
#include "opencv2/imgproc/types_c.h"

namespace mOpenCV {
  namespace cCvConnectedComp {
    VALUE rb_class();
    void init_ruby_class();
    VALUE rb_allocate(VALUE klass);
    VALUE rb_initialize(int argc, VALUE *argv, VALUE self);
    VALUE rb_area(VALUE self);
    VALUE rb_value(VALUE self);
    VALUE rb_rect(VALUE self);
    VALUE rb_set_rect(VALUE self, VALUE rect);
    VALUE rb_contour(VALUE self);
    VALUE new_object();
    VALUE new_object(CvConnectedComp comp);
  }

  inline CvConnectedComp* CVCONNECTEDCOMP(VALUE object) {
    CvConnectedComp *ptr;
    Data_Get_Struct(object, CvConnectedComp, ptr);
    return ptr;
  }
}
#endif // RUBY_OPENCV_CVCONNECTEDCOMP_H
