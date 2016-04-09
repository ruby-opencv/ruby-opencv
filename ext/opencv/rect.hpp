#ifndef RUBY_OPENCV_RECT_H
#define RUBY_OPENCV_RECT_H
/*
 * Document-class: Cv::Rect
 */
namespace rubyopencv {
  namespace Rect {
    void init();
    VALUE rect2obj (cv::Rect rect);
    cv::Rect* obj2rect(VALUE obj);
  }
}

#endif // RUBY_OPENCV_RECT_H
