// -*- mode: c++; coding: utf-8 -*-
#ifndef RUBY_OPENCV_SCALAR_H
#define RUBY_OPENCV_SCALAR_H

namespace rubyopencv {
  namespace Scalar {
    void init();
    VALUE klass();
    cv::Scalar* obj2scalar(VALUE obj);
    VALUE scalar2obj(cv::Scalar* ptr);
  }
}

#endif // RUBY_OPENCV_SCALAR_H
