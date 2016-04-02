// -*- mode: c++; coding: utf-8 -*-
#ifndef RUBY_OPENCV_MAT_IMGPROC_H
#define RUBY_OPENCV_MAT_IMGPROC_H

#include "ruby.h"

namespace rubyopencv {
  namespace Mat {
    VALUE rb_sobel(int argc, VALUE *argv, VALUE self);
    VALUE rb_canny(int argc, VALUE *argv, VALUE self);
    VALUE rb_laplacian(int argc, VALUE *argv, VALUE self);
    VALUE rb_cvt_color(int argc, VALUE *argv, VALUE self);
    VALUE rb_resize(int argc, VALUE *argv, VALUE self);
  }
}

#endif // RUBY_OPENCV_MAT_IMGPROC_H
