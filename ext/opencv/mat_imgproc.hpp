// -*- mode: c++; coding: utf-8 -*-
#ifndef RUBY_OPENCV_MAT_IMGPROC_H
#define RUBY_OPENCV_MAT_IMGPROC_H

#include "ruby.h"

namespace rubyopencv {
  namespace Mat {
    VALUE rb_sobel(int argc, VALUE *argv, VALUE self);
    VALUE rb_sobel_bang(int argc, VALUE *argv, VALUE self);
    VALUE rb_canny(int argc, VALUE *argv, VALUE self);
    VALUE rb_canny_bang(int argc, VALUE *argv, VALUE self);
    VALUE rb_laplacian(int argc, VALUE *argv, VALUE self);
    VALUE rb_laplacian_bang(int argc, VALUE *argv, VALUE self);
    VALUE rb_cvt_color(int argc, VALUE *argv, VALUE self);
    VALUE rb_cvt_color_bang(int argc, VALUE *argv, VALUE self);
    VALUE rb_resize(int argc, VALUE *argv, VALUE self);
    VALUE rb_resize_bang(int argc, VALUE *argv, VALUE self);
    VALUE rb_blur(int argc, VALUE *argv, VALUE self);
    VALUE rb_blur_bang(int argc, VALUE *argv, VALUE self);
    VALUE rb_gaussian_blur(int argc, VALUE *argv, VALUE self);
    VALUE rb_gaussian_blur_bang(int argc, VALUE *argv, VALUE self);
    VALUE rb_median_blur(VALUE self, VALUE ksize);
    VALUE rb_threshold(VALUE self, VALUE threshold, VALUE max_value, VALUE threshold_type);
    VALUE rb_adaptive_threshold(VALUE self, VALUE max_value, VALUE adaptive_method,
				VALUE threshold_type, VALUE block_size, VALUE delta);
  }
}

#endif // RUBY_OPENCV_MAT_IMGPROC_H
