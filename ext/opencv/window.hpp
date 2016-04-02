#ifndef RUBY_OPENCV_WINDOW_H
#define RUBY_OPENCV_WINDOW_H

#include "ruby.h"
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"

namespace rubyopencv {
  namespace Window {

    typedef struct _window {
      VALUE name;
      VALUE trackbars;
    } window_t;

    void init();
    VALUE rb_wait_key(int argc, VALUE* argv, VALUE self);
  }
}
#endif // RUBY_OPENCV_WINDOW_H

