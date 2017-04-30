/************************************************************

   cvmoments.h -

   $Author: lsxi $

   Copyright (C) 2007 Masakazu Yonekura

************************************************************/
#ifndef RUBY_OPENCV_CVMOMENTS_H
#define RUBY_OPENCV_CVMOMENTS_H

#include <ruby.h>
#include "opencv2/imgproc/imgproc_c.h"

#include "cvutils.h"

namespace mOpenCV {
  namespace cCvMoments {
    VALUE rb_class();
    void init_ruby_class();
    VALUE rb_allocate(VALUE klass);
    VALUE rb_initialize(int argc, VALUE *argv, VALUE self);
    VALUE rb_spatial(VALUE self, VALUE x_order, VALUE y_order);
    VALUE rb_central(VALUE self, VALUE x_order, VALUE y_order);
    VALUE rb_normalized_central(VALUE self, VALUE x_order, VALUE y_order);
    VALUE rb_hu(VALUE self);
    VALUE rb_gravity_center(VALUE self);
    VALUE rb_angle(VALUE self);
    VALUE rb_m00(VALUE self);
    VALUE rb_m10(VALUE self);
    VALUE rb_m01(VALUE self);
    VALUE rb_m20(VALUE self);
    VALUE rb_m11(VALUE self);
    VALUE rb_m02(VALUE self);
    VALUE rb_m30(VALUE self);
    VALUE rb_m21(VALUE self);
    VALUE rb_m12(VALUE self);
    VALUE rb_m03(VALUE self);
    VALUE rb_mu20(VALUE self);
    VALUE rb_mu11(VALUE self);
    VALUE rb_mu02(VALUE self);
    VALUE rb_mu30(VALUE self);
    VALUE rb_mu21(VALUE self);
    VALUE rb_mu12(VALUE self);
    VALUE rb_mu03(VALUE self);
    VALUE rb_inv_sqrt_m00(VALUE self);
    VALUE new_object(CvArr *arr, int is_binary);
  }

  inline CvMoments*
    CVMOMENTS(VALUE object)
  {
    CvMoments *ptr;
    Data_Get_Struct(object, CvMoments, ptr);
    return ptr;
  }

  inline CvMoments*
    CVMOMENTS_WITH_CHECK(VALUE object)
  {
    if (!rb_obj_is_kind_of(object, cCvMoments::rb_class()))
      raise_typeerror(object, cCvMoments::rb_class());
    return CVMOMENTS(object);
  }
}

#endif // RUBY_OPENCV_CVMOMENTS_H
