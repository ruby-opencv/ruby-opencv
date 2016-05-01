// -*- mode: c++; coding: utf-8 -*-
#include <sstream>
#include "ruby.h"
#include "opencv2/core.hpp"

#include "size.hpp"

namespace rubyopencv {
  namespace Size {
    void free_size(void* ptr);
    size_t memsize_size(const void* ptr);

    VALUE rb_klass = Qnil;
    rb_data_type_t opencv_size_type = {
      "Size",
      { 0, free_size, memsize_size, 0 },
      0,
      0,
      0
    };

    void free_size(void* ptr) {
      delete (cv::Size*)ptr;
    }

    size_t memsize_size(const void* ptr) {
      return sizeof(cv::Size);
    }

    cv::Size* obj2size(VALUE obj) {
      cv::Size* ptr = NULL;
      TypedData_Get_Struct(obj, cv::Size, &opencv_size_type, ptr);
      return ptr;
    }

    VALUE size2obj(cv::Size* ptr) {
      return TypedData_Wrap_Struct(rb_klass, &opencv_size_type, (void*)ptr);
    }

    VALUE rb_allocate(VALUE klass) {
      cv::Size* ptr = new cv::Size();
      return TypedData_Wrap_Struct(klass, &opencv_size_type, ptr);
    }

    /*
     * Return width
     *
     * @overload width
     * @return [Integer] Width
     */
    VALUE rb_width(VALUE self) {
      return INT2NUM(obj2size(self)->width);
    }

    /*
     * Set width
     *
     * @overload width=(value)
     *   @param value [Integer] Width
     *   @return [Mat] +self+
     */
    VALUE rb_set_width(VALUE self, VALUE width) {
      obj2size(self)->width = NUM2INT(width);
      return self;
    }

    /*
     * Return height
     *
     * @overload height
     * @return [Integer] Height
     */
    VALUE rb_height(VALUE self) {
      return INT2NUM(obj2size(self)->height);
    }

    /*
     * Set height
     *
     * @overload height=(value)
     *   @param value [Integer] Height
     *   @return [Mat] +self+
     */
    VALUE rb_set_height(VALUE self, VALUE height) {
      obj2size(self)->height = NUM2INT(height);
      return self;
    }

    VALUE rb_initialize(int argc, VALUE *argv, VALUE self) {
      VALUE v1, v2;
      rb_scan_args(argc, argv, "02", &v1, &v2);

      cv::Size* selfptr = obj2size(self);
      switch (argc) {
      case 0:
	selfptr->width = 0;
	selfptr->height = 0;
	break;
      case 2:
	selfptr->width = NUM2INT(v1);
	selfptr->height = NUM2INT(v2);
	break;
      default:
	rb_raise(rb_eArgError, "wrong number of arguments (%d for 0 or 2)", argc);
	break;
      }

      return self;
    }

    /*
     * Return size as an +String+.
     *
     * @overload to_s
     * @return [String] String representation of the size
     */
    VALUE rb_to_s(VALUE self) {
      std::stringstream s;
      cv::Size* selfptr = obj2size(self);
      s << *selfptr;

      VALUE param[3];
      param[0] = rb_str_new2("#<%s:%s>");
      param[1] = rb_str_new2(rb_class2name(CLASS_OF(self)));
      param[2] = rb_str_new2(s.str().c_str());

      int n = sizeof(param) / sizeof(param[0]);
      return rb_f_sprintf(n, param);
    }

    void init() {
      VALUE opencv = rb_define_module("Cv");

      rb_klass = rb_define_class_under(opencv, "Size", rb_cData);
      rb_define_alloc_func(rb_klass, rb_allocate);
      rb_define_method(rb_klass, "initialize", RUBY_METHOD_FUNC(rb_initialize), -1);

      rb_define_method(rb_klass, "width", RUBY_METHOD_FUNC(rb_width), 0);
      rb_define_method(rb_klass, "width=", RUBY_METHOD_FUNC(rb_set_width), 1);
      rb_define_method(rb_klass, "height", RUBY_METHOD_FUNC(rb_height), 0);
      rb_define_method(rb_klass, "height=", RUBY_METHOD_FUNC(rb_set_height), 1);

      rb_define_method(rb_klass, "to_s", RUBY_METHOD_FUNC(rb_to_s), 0);
    }
  }
}
