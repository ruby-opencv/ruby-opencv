/************************************************************

   cvbox2d.h -

   $Author: lsxi $

   Copyright (C) 2005-2006 Masakazu Yonekura

************************************************************/
#ifndef RUBY_OPENCV_CVBOX2D_H
#define RUBY_OPENCV_CVBOX2D_H

#include <ruby.h>
#include "opencv2/core/types_c.h"
#include "cvutils.h"

namespace mOpenCV {
  namespace cCvBox2D {

    VALUE rb_class();

    void init_ruby_class();

    VALUE rb_allocate(VALUE klass);

    VALUE rb_initialize(int argc, VALUE *argv, VALUE self);
    VALUE rb_center(VALUE self);
    VALUE rb_set_center(VALUE self, VALUE value);
    VALUE rb_size(VALUE self);
    VALUE rb_set_size(VALUE self, VALUE value);
    VALUE rb_angle(VALUE self);
    VALUE rb_set_angle(VALUE self, VALUE value);
    VALUE rb_points(VALUE self);

    VALUE new_object();
    VALUE new_object(CvBox2D box);

  }

  inline CvBox2D*
    CVBOX2D(VALUE object){
    CvBox2D *ptr;
    Data_Get_Struct(object, CvBox2D, ptr);
    return ptr;
  }

  inline CvBox2D
    VALUE_TO_CVBOX2D(VALUE object){
    if (rb_obj_is_kind_of(object, cCvBox2D::rb_class())) {
      return *CVBOX2D(object);
    }
    else {
      raise_typeerror(object, cCvBox2D::rb_class());
    }
    throw "Should never reach here";
  }

}
#endif // RUBY_OPENCV_CVBOX2D_H
