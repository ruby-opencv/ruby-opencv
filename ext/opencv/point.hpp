#ifndef RUBY_OPENCV_POINT_H
#define RUBY_OPENCV_POINT_H

namespace rubyopencv {
  namespace Point {
    void init();
    cv::Point conpatible_obj2point(VALUE obj);
  }
}

#endif // RUBY_OPENCV_POINT_H
