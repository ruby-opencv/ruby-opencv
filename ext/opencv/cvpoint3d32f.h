/************************************************************

   cvpoint3d32f.h -

   $Author: lsxi $

   Copyright (C) 2005-2008 Masakazu Yonekura

************************************************************/
#ifndef RUBY_OPENCV_CVPOINT3D32F_H
#define RUBY_OPENCV_CVPOINT3D32F_H

#include <ruby.h>
#include "opencv2/core/core_c.h"
#include "opencv2/core/types_c.h"

#include "cvutils.h"

namespace mOpenCV {
  namespace cCvPoint3D32f {
    VALUE rb_class();
    void init_ruby_class();
    VALUE rb_compatible_q(VALUE klass, VALUE object);
    VALUE rb_allocate(VALUE klass);
    VALUE rb_initialize(int argc, VALUE *argv, VALUE self);
    VALUE rb_x(VALUE self);
    VALUE rb_set_x(VALUE self, VALUE x);
    VALUE rb_y(VALUE self);
    VALUE rb_set_y(VALUE self, VALUE y);
    VALUE rb_z(VALUE self);
    VALUE rb_set_z(VALUE self, VALUE z);
    VALUE rb_to_s(VALUE self);
    VALUE rb_to_ary(VALUE self);
    VALUE new_object(CvPoint3D32f point);
  }

  inline CvPoint3D32f*
    CVPOINT3D32F(VALUE object)
  {
    CvPoint3D32f *ptr;
    Data_Get_Struct(object, CvPoint3D32f, ptr);
    return ptr;
  }

  inline CvPoint3D32f
    VALUE_TO_CVPOINT3D32F(VALUE object)
  {
    if (cCvPoint3D32f::rb_compatible_q(cCvPoint3D32f::rb_class(), object)) {
      return cvPoint3D32f(NUM2DBL(rb_funcall(object, rb_intern("x"), 0)),
			  NUM2DBL(rb_funcall(object, rb_intern("y"), 0)),
			  NUM2DBL(rb_funcall(object, rb_intern("z"), 0)));
    }
    else {
      raise_compatible_typeerror(object, cCvPoint3D32f::rb_class());
    }
    throw "Should never reach here";
  }

}
#endif // RUBY_OPENCV_CVPOINT3D32F_H
