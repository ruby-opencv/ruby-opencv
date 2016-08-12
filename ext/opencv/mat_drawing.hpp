#include "ruby.h"

namespace rubyopencv {
  namespace Mat {
    VALUE rb_line(int argc, VALUE *argv, VALUE self);
    VALUE rb_line_bang(int argc, VALUE *argv, VALUE self);

    VALUE rb_rectangle(int argc, VALUE *argv, VALUE self);
    VALUE rb_rectangle_bang(int argc, VALUE *argv, VALUE self);    

    VALUE rb_circle(int argc, VALUE *argv, VALUE self);
    VALUE rb_circle_bang(int argc, VALUE *argv, VALUE self);
  }
}
