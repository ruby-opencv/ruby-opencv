// -*- mode: c++; coding: utf-8 -*-
#include "ruby.h"
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"

#include "opencv.hpp"
#include "opencv_const.hpp"
#include "mat.hpp"
#include "point.hpp"
#include "rect.hpp"
#include "size.hpp"
#include "scalar.hpp"

#include "cascadeclassifier.hpp"
#include "videocapture.hpp"

#include "error.hpp"
#include "window.hpp"
#include "trackbar.hpp"

namespace rubyopencv {
  VALUE rb_module = Qnil;
  Mat::RubyMatAllocator allocator;

  VALUE rb_module_opencv() {
    return rb_module;
  }

  int error_callback(int status, const char *function_name, const char *error_message,
		     const char *file_name, int line, void *user_data) {
    return 0;
  }

  /*
   * Returns full configuration time cmake output.
   * Returned value is raw cmake output including version control system revision,
   * compiler version, compiler flags, enabled modules and third party libraries, etc.
   * Output format depends on target architecture.
   *
   * @overload rb_build_information()
   * @return [String] Full configuration time cmake output.
   * @opencv_func cv::getBuildInformation
   */
  VALUE rb_build_information(VALUE klass) {
    const char* ptr = cv::getBuildInformation().c_str();
    return rb_str_new(ptr, strlen(ptr));
  }

  /*
   * Saves an image to a specified file.
   *
   * @overload imwrite(filename, img, params = nil)
   *   @param filename [String] Name of the file.
   *   @param img [Mat] Image to be saved.
   *   @param params [Array<int>]
   *	 Format-specific parameters encoded as pairs (paramId_1, paramValue_1, paramId_2, paramValue_2, ...)
   *   @return [Bool] Result
   * @opencv_func cv::imwrite
   */
  VALUE rb_imwrite(int argc, VALUE* argv, VALUE self) {
    VALUE filename, img, params;
    rb_scan_args(argc, argv, "21", &filename, &img, &params);
    return Mat::rb_imwrite_internal(filename, img, params);
  }

  /*
   * Makes a type from depth and channels
   *
   * @overload CV_MAKETYPE(depth, cn)
   *   @param depth [String] Depth
   *   @param cn [Mat] Number of channels
   *   @return [Integer] Type
   * @opencv_func CV_MAKETYPE
   */
  VALUE rb_maketype(VALUE self, VALUE depth, VALUE channels) {
    int type = CV_MAKETYPE(NUM2INT(depth), NUM2INT(channels));
    return INT2NUM(type);
  }

  extern "C"
  void Init_opencv() {
    cv::Mat::setDefaultAllocator(&allocator);
    cv::redirectError(error_callback, NULL, NULL);

    rb_module = rb_define_module("Cv");

    define_const(rb_module);

    Mat::init();
    Point::init();
    Rect::init();
    Size::init();
    Scalar::init();
    CascadeClassifier::init();
    VideoCapture::init();
    Window::init();
    Trackbar::init();
    Error::init();

    rb_define_module_function(rb_module, "build_information", RUBY_METHOD_FUNC(rb_build_information), 0);

    rb_define_singleton_method(rb_module, "imread", RUBY_METHOD_FUNC(Mat::rb_imread), 2); // in ext/opencv/mat.cpp
    rb_define_singleton_method(rb_module, "imwrite", RUBY_METHOD_FUNC(rb_imwrite), -1);
    rb_define_singleton_method(rb_module, "imdecode", RUBY_METHOD_FUNC(Mat::rb_imdecode), 2); // in ext/opencv/mat.cpp
    rb_define_singleton_method(rb_module, "wait_key", RUBY_METHOD_FUNC(Window::rb_wait_key), -1); // in ext/opencv/window.cpp

    rb_define_singleton_method(rb_module, "add_weighted", RUBY_METHOD_FUNC(Mat::rb_add_weighted), -1); // in ext/opencv/mat.cpp
    rb_define_singleton_method(rb_module, "merge", RUBY_METHOD_FUNC(Mat::rb_merge), 1); // in ext/opencv/mat.cpp
    rb_define_singleton_method(rb_module, "hconcat", RUBY_METHOD_FUNC(Mat::rb_hconcat), 1); // in ext/opencv/mat.cpp
    rb_define_singleton_method(rb_module, "vconcat", RUBY_METHOD_FUNC(Mat::rb_vconcat), 1); // in ext/opencv/mat.cpp

    rb_define_singleton_method(rb_module, "CV_MAKETYPE", RUBY_METHOD_FUNC(rb_maketype), 2);

    int state = 0;
    VALUE ret = rb_eval_string_protect("OpenCV = Cv", &state);
    if (NIL_P(ret)) {
      rb_warn("An exception occured: %d", state);
    }
  }
}
