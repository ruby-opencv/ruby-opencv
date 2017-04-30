/************************************************************

   iplconvkernel.h -

   $Author: lsxi $

   Copyright (C) 2005-2006 Masakazu Yonekura

************************************************************/
#ifndef RUBY_OPENCV_IPLCONVKERNEL_H
#define RUBY_OPENCV_IPLCONVKERNEL_H

#include <ruby.h>
#include "opencv2/core/core_c.h"
#include "opencv2/core/types_c.h"

#include "cvutils.h"

namespace mOpenCV {
  namespace cIplConvKernel {
    VALUE rb_class();
    VALUE rb_allocate(VALUE klass);
    void init_ruby_class();
    VALUE rb_initialize(int argc, VALUE *argv, VALUE self);
    VALUE rb_size(VALUE self);
    VALUE rb_cols(VALUE self);
    VALUE rb_rows(VALUE self);
    VALUE rb_anchor(VALUE self);
    VALUE rb_anchor_x(VALUE self);
    VALUE rb_anchor_y(VALUE self);
  }

  inline IplConvKernel*
    IPLCONVKERNEL(VALUE object)
  {
    IplConvKernel *ptr;
    Data_Get_Struct(object, IplConvKernel, ptr);
    return ptr;
  }

  inline IplConvKernel*
    IPLCONVKERNEL_WITH_CHECK(VALUE object)
  {
    if (!rb_obj_is_kind_of(object, cIplConvKernel::rb_class()))
      raise_typeerror(object, cIplConvKernel::rb_class());
    return IPLCONVKERNEL(object);
  }
}
#endif // RUBY_OPENCV_IPLCONVKERNEL_H
