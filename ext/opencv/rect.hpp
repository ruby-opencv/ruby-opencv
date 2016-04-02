#ifndef RUBY_OPENCV_RECT_H
#define RUBY_OPENCV_RECT_H
/*
 * Document-class: OpenCV::Rect
 */
namespace rubyopencv {
  namespace Rect {
    void init();
    VALUE rect2obj (cv::Rect rect);
  }
}

#endif // RUBY_OPENCV_RECT_H
