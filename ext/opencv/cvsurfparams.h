/************************************************************

   cvsurfparams.h -

   $Author: ser1zw $

   Copyright (C) 2011 ser1zw

************************************************************/
#ifndef RUBY_OPENCV_CVSURFPARAMS_H
#define RUBY_OPENCV_CVSURFPARAMS_H

#include "ruby.h"
#ifdef IS_OPENCV2
#  include "opencv2/legacy/compat.hpp"
#endif
#include "cvutils.h"

namespace mOpenCV {
  namespace cCvSURFParams {
    VALUE rb_class();
    void init_ruby_class();

#ifdef IS_OPENCV2
    VALUE rb_allocate(VALUE klass);
    VALUE rb_initialize(int argc, VALUE *argv, VALUE self);
    VALUE rb_get_hessian_threshold(VALUE self);
    VALUE rb_set_hessian_threshold(VALUE self, VALUE value);
    VALUE rb_get_extended(VALUE self);
    VALUE rb_set_extended(VALUE self, VALUE value);
    VALUE rb_get_n_octaves(VALUE self);
    VALUE rb_set_n_octaves(VALUE self, VALUE value);
    VALUE rb_get_n_octave_layers(VALUE self);
    VALUE rb_set_n_octave_layers(VALUE self, VALUE value);
    VALUE new_object(CvSURFPoint *cvsurfparams);
#endif
  }

#ifdef IS_OPENCV2
  inline CvSURFParams*
  CVSURFPARAMS(VALUE object)
  {
    CvSURFParams* ptr;
    Data_Get_Struct(object, CvSURFParams, ptr);
    return ptr;
  }

  inline CvSURFParams*
  CVSURFPARAMS_WITH_CHECK(VALUE object)
  {
    if (!rb_obj_is_kind_of(object, cCvSURFParams::rb_class()))
      raise_typeerror(object, cCvSURFParams::rb_class());
    return CVSURFPARAMS(object);
  }
#endif
}

#endif // RUBY_OPENCV_CVSURFPARAMS_H
