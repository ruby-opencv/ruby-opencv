// -*- mode: c++; coding: utf-8 -*-
#ifndef RUBY_OPENCV_SIZE_H
#define RUBY_OPENCV_SIZE_H

namespace rubyopencv {
  namespace Size {
    void init();
    cv::Size* obj2size(VALUE obj);
  }
}
#endif // RUBY_OPENCV_SIZE_H
