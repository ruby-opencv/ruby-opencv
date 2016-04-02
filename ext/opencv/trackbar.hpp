#ifndef RUBY_OPENCV_TRACKBAR_H
#define RUBY_OPENCV_TRACKBAR_H

#include "ruby.h"

namespace rubyopencv {
  namespace Trackbar {
    typedef struct _trackbar {
      char *name;
      int maxval;
      int value;
      VALUE block;
    } trackbar_t;

    void init();
    trackbar_t* obj2trackbar(VALUE obj);
    VALUE newobj(int argc, VALUE* argv);
  }
}
#endif // RUBY_OPENCV_TRACKBAR_H
