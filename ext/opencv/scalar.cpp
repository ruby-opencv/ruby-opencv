// -*- mode: c++; coding: utf-8 -*-
#include "ruby.h"
#include "opencv2/core.hpp"

namespace rubyopencv {
  namespace Scalar {
    void free_scalar(void* ptr);
    size_t memsize_scalar(const void* ptr);

    VALUE rb_klass = Qnil;
    rb_data_type_t opencv_scalar_type = {
      "Scalar",
      { 0, free_scalar, memsize_scalar, 0 },
      0,
      0,
      0
    };

    void free_scalar(void* ptr) {
      delete (cv::Scalar*)ptr;
    }

    size_t memsize_scalar(const void* ptr) {
      return sizeof(cv::Scalar);
    }

    VALUE klass() {
      return rb_klass;
    }
    
    cv::Scalar* obj2scalar(VALUE obj) {
      cv::Scalar* ptr = NULL;
      TypedData_Get_Struct(obj, cv::Scalar, &opencv_scalar_type, ptr);
      return ptr;
    }

    VALUE scalar2obj(cv::Scalar* ptr) {
      return TypedData_Wrap_Struct(rb_klass, &opencv_scalar_type, ptr);
    }

    VALUE rb_allocate(VALUE klass) {
      cv::Scalar* ptr = NULL;
      return TypedData_Make_Struct(klass, cv::Scalar, &opencv_scalar_type, ptr);
    }

    VALUE rb_initialize(int argc, VALUE *argv, VALUE self) {
      const int SIZE = 4;
      VALUE values[SIZE];
      rb_scan_args(argc, argv, "04", &values[0], &values[1], &values[2], &values[3]);

      cv::Scalar* selfptr = obj2scalar(self);
      for (int i = 0; i < SIZE; i++) {
	(*selfptr)[i] = NIL_P(values[i]) ? 0.0 : NUM2DBL(values[i]);
      }

      return self;
    }

    VALUE rb_aref(VALUE self, VALUE index) {
      cv::Scalar* selfptr = obj2scalar(self);
      int i = NUM2INT(index);
      if (i < 0 || i >= 4) {
	rb_raise(rb_eIndexError, "index should be 0...4");
      }

      return rb_float_new((*selfptr)[i]);
    }

    VALUE rb_aset(VALUE self, VALUE index, VALUE value) {
      int i = NUM2INT(index);
      if (i < 0 || i >= 4) {
	rb_raise(rb_eIndexError, "index should be 0...4");
      }
      cv::Scalar* selfptr = obj2scalar(self);
      (*selfptr)[i] = NUM2DBL(value);

      return self;
    }

    /*
     * @overload to_s
     * @return [String] String representation of the scalar
     */
    VALUE rb_to_s(VALUE self) {
      const int i = 6;
      VALUE str[i];
      str[0] = rb_str_new2("<%s: [%g,%g,%g,%g]>");
      str[1] = rb_str_new2(rb_class2name(CLASS_OF(self)));
      str[2] = rb_aref(self, INT2FIX(0));
      str[3] = rb_aref(self, INT2FIX(1));
      str[4] = rb_aref(self, INT2FIX(2));
      str[5] = rb_aref(self, INT2FIX(3));

      return rb_f_sprintf(i, str);
    }

    /*
     * @overload to_a
     * @return [Array<Number>] Values in Array
     */
    VALUE rb_to_a(VALUE self) {
      return rb_ary_new3(4, rb_aref(self, INT2FIX(0)), rb_aref(self, INT2FIX(1)),
			 rb_aref(self, INT2FIX(2)), rb_aref(self, INT2FIX(3)));
    }
    
    void init() {
      VALUE opencv = rb_define_module("OpenCV");

      rb_klass = rb_define_class_under(opencv, "Scalar", rb_cData);
      rb_define_alloc_func(rb_klass, rb_allocate);

      rb_define_private_method(rb_klass, "initialize", RUBY_METHOD_FUNC(rb_initialize), -1);

      rb_define_method(rb_klass, "to_s", RUBY_METHOD_FUNC(rb_to_s), 0);
      rb_define_method(rb_klass, "to_a", RUBY_METHOD_FUNC(rb_to_a), 0);
      rb_define_method(rb_klass, "[]", RUBY_METHOD_FUNC(rb_aref), 1);
      rb_define_method(rb_klass, "[]=", RUBY_METHOD_FUNC(rb_aset), 2);
    }
  }
}
