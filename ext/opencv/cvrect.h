/************************************************************

   cvrect.h -

   $Author: lsxi $

   Copyright (C) 2005-2006 Masakazu Yonekura

************************************************************/
#ifndef RUBY_OPENCV_CVRECT_H
#define RUBY_OPENCV_CVRECT_H

#include <ruby.h>
#include "opencv2/core/core_c.h"
#include "opencv2/core/types_c.h"

#include "cvutils.h"

namespace mOpenCV {
  namespace cCvRect {
    VALUE rb_class();
    void init_ruby_class();
    VALUE rb_compatible_q(VALUE klass, VALUE object);
    VALUE rb_max_rect(VALUE klass, VALUE rect1, VALUE rect2);
    VALUE rb_allocate(VALUE klass);
    VALUE rb_initialize(int argc, VALUE *argv, VALUE self);
    VALUE rb_x(VALUE self);
    VALUE rb_set_x(VALUE self, VALUE x);
    VALUE rb_y(VALUE self);
    VALUE rb_set_y(VALUE self, VALUE y);
    VALUE rb_width(VALUE self);
    VALUE rb_set_width(VALUE self, VALUE width);
    VALUE rb_height(VALUE self);
    VALUE rb_set_height(VALUE self, VALUE height);
    VALUE rb_center(VALUE self);
    VALUE rb_points(VALUE self);
    VALUE rb_top_left(VALUE self);
    VALUE rb_set_top_left(VALUE self, VALUE point);
    VALUE rb_top_right(VALUE self);
    VALUE rb_set_top_right(VALUE self, VALUE point);
    VALUE rb_bottom_left(VALUE self);
    VALUE rb_set_bottom_left(VALUE self, VALUE point);
    VALUE rb_bottom_right(VALUE self);
    VALUE rb_set_bottom_right(VALUE self, VALUE point);
    VALUE new_object(CvRect rect);
  }

  inline CvRect*
    CVRECT(VALUE object)
  {
    CvRect *ptr;
    Data_Get_Struct(object, CvRect, ptr);
    return ptr;
  }

  inline CvRect
    VALUE_TO_CVRECT(VALUE object)
  {
    if (cCvRect::rb_compatible_q(cCvRect::rb_class(), object)) {
      return cvRect(NUM2INT(rb_funcall(object, rb_intern("x"), 0)),
		    NUM2INT(rb_funcall(object, rb_intern("y"), 0)),
		    NUM2INT(rb_funcall(object, rb_intern("width"), 0)),
		    NUM2INT(rb_funcall(object, rb_intern("height"), 0)));
    }
    else {
      raise_compatible_typeerror(object, cCvRect::rb_class());
    }
    throw "Should never reach here";
  }
}
#endif // RUBY_OPENCV_CVRECT_H
