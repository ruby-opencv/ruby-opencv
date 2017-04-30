/************************************************************

   cvscalar.h -

   $Author: lsxi $

   Copyright (C) 2005 Masakazu Yonekura

************************************************************/
#ifndef RUBY_OPENCV_CVSCALAR_H
#define RUBY_OPENCV_CVSCALAR_H

#include <ruby.h>
#include "opencv2/core/core_c.h"
#include "opencv2/core/types_c.h"

#include "cvutils.h"

namespace mOpenCV {
  namespace cCvScalar {
    VALUE rb_class();
    void init_ruby_class();
    VALUE rb_compatible_q(VALUE klass, VALUE object);
    VALUE rb_allocate(VALUE klass);
    VALUE rb_initialize(int argc, VALUE *argv, VALUE self);
    VALUE rb_aref(VALUE self, VALUE index);
    VALUE rb_aset(VALUE self, VALUE index, VALUE value);
    VALUE rb_sub(int argc, VALUE *argv, VALUE self);
    VALUE rb_to_s(VALUE self);
    VALUE rb_to_ary(VALUE self);
    VALUE new_object();
    VALUE new_object(CvScalar scalar);
  }

  inline CvScalar*
    CVSCALAR(VALUE object)
  {
    CvScalar *ptr;
    Data_Get_Struct(object, CvScalar, ptr);
    return ptr;
  }

  inline CvScalar
    VALUE_TO_CVSCALAR(VALUE object)
  {
    ID aref_id;
    if (FIXNUM_P(object)) {
      return cvScalarAll(FIX2INT(object));
    }
    else if (rb_respond_to(object, (aref_id = rb_intern("[]")))) {
      return cvScalar(NUM2DBL(rb_funcall(object, aref_id, 1, INT2FIX(0))),
		      NUM2DBL(rb_funcall(object, aref_id, 1, INT2FIX(1))),
		      NUM2DBL(rb_funcall(object, aref_id, 1, INT2FIX(2))),
		      NUM2DBL(rb_funcall(object, aref_id, 1, INT2FIX(3))));
    }
    else {
      raise_compatible_typeerror(object, cCvScalar::rb_class());
    }
    throw "Should never reach here";
  }
}
#endif // RUBY_OPENCV_CVSCALAR_H
