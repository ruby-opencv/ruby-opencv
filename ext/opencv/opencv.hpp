/**
 * opencv.hpp
 */
#ifndef RUBY_OPENCV_H
#define RUBY_OPENCV_H

#include "ruby.h"

namespace rubyopencv {
  inline int NUM2INT_DEFAULT(VALUE value, int default_value) {
    return NIL_P(value) ? (default_value) : NUM2INT(value);
  }

  inline double NUM2DBL_DEFAULT(VALUE value, double default_value) {
    return NIL_P(value) ? (default_value) : NUM2DBL(value);
  }

  inline VALUE HASH_LOOKUP(VALUE hash, std::string key) {
    return rb_hash_lookup(hash, ID2SYM(rb_intern(key.c_str())));
  }

  VALUE rb_module_opencv();
}

#endif // RUBY_OPENCV_H
