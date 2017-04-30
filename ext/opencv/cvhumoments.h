/************************************************************

   cvhumoments.h -

   $Author: ser1zw $

   Copyright (C) 2011 ser1zw

************************************************************/
#ifndef RUBY_OPENCV_CVHUMOMENTS_H
#define RUBY_OPENCV_CVHUMOMENTS_H

#include <ruby.h>
#include "opencv2/imgproc/types_c.h"

namespace mOpenCV {
  namespace cCvHuMoments {
    VALUE rb_class();
    void init_ruby_class();
    VALUE rb_allocate(VALUE klass);
    VALUE rb_initialize(VALUE src_moments, VALUE self);
    VALUE rb_hu1(VALUE self);
    VALUE rb_hu2(VALUE self);
    VALUE rb_hu3(VALUE self);
    VALUE rb_hu4(VALUE self);
    VALUE rb_hu5(VALUE self);
    VALUE rb_hu6(VALUE self);
    VALUE rb_hu7(VALUE self);
    VALUE rb_to_ary(VALUE self);
    VALUE new_object(CvMoments *cvmoments);
  }

  inline CvHuMoments*
    CVHUMOMENTS(VALUE object)
  {
    CvHuMoments *ptr;
    Data_Get_Struct(object, CvHuMoments, ptr);
    return ptr;
  }
}
#endif // RUBY_OPENCV_CVHUMOMENTS_H
