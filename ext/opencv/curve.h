/************************************************************

   curve.h -

   $Author: lsxi $

   Copyright (C) 2005-2006 Masakazu Yonekura

************************************************************/
#ifndef RUBY_OPENCV_CVSEQ_CURVE_H
#define RUBY_OPENCV_CVSEQ_CURVE_H

#include <ruby.h>

namespace mOpenCV {
  namespace mCurve {
    VALUE rb_module();

    void init_ruby_module();

    VALUE rb_closed_q(VALUE self);
    VALUE rb_convex_q(VALUE self);
    VALUE rb_hole_q(VALUE self);
    VALUE rb_simple_q(VALUE self);
    VALUE rb_arc_length(int argc, VALUE *argv, VALUE self);

  }
}
#endif // RUBY_OPENCV_CVSEQ_CURVE_H
