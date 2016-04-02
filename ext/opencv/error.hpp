// -*- mode: c++; coding: utf-8 -*-
#ifndef RUBY_OPENCV_ERROR_H
#define RUBY_OPENCV_ERROR_H

#include "ruby.h"
#include "opencv2/core.hpp"

namespace rubyopencv {
  namespace Error {
    void init();
    VALUE by_code(int error_code);
    void raise(cv::Exception e);
  }
}

#endif // RUBY_OPENCV_ERROR_H
