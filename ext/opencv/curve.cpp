/************************************************************

   curve.cpp -

   $Author: lsxi $

   Copyright (C) 2005 Masakazu Yonekura

************************************************************/
#include "opencv2/core/core.hpp"

#include "cvseq.h"
#include "cvslice.h"
#include "cverror.h"
#include "curve.h"

/*
 * Document-class: OpenCV::Curve
 *
 * Curve sequence
 */
namespace mOpenCV {
  namespace mCurve {
    VALUE module;
    
    VALUE
    rb_module()
    {
      return module;
    }

    /*
     * If the curve is closed, return true. Otherwise return false.
     * @overload closed?
     * @return [Boolean] Closed or not
     * @opencv_func CV_IS_SEQ_CLOSED
     */
    VALUE
    rb_closed_q(VALUE self)
    {
      return CV_IS_SEQ_CLOSED(CVSEQ(self)) ? Qtrue : Qfalse;
    }

    /*
     * If the curve is convex, return true. Otherwise return false.
     * @overload convex?
     * @return [Boolean] Convex or not
     * @opencv_func CV_IS_SEQ_CONVEX
     */
    VALUE
    rb_convex_q(VALUE self)
    {
      return CV_IS_SEQ_CONVEX(CVSEQ(self)) ? Qtrue : Qfalse;
    }

    /*
     * If the curve is hole(inner contour), return true. Otherwise return false.
     * @overload hole?
     * @return [Boolean] Hole or not
     * @opencv_func CV_IS_SEQ_HOLE
     */
    VALUE
    rb_hole_q(VALUE self)
    {  
      return CV_IS_SEQ_HOLE(CVSEQ(self)) ? Qtrue : Qfalse;
    }

    /*
     * If the curve is simple, return true. Otherwise return false.
     * @overload simple?
     * @return [Boolean] Simple or not
     * @opencv_func CV_IS_SEQ_SIMPLE
     */
    VALUE
    rb_simple_q(VALUE self)
    {
      return CV_IS_SEQ_SIMPLE(CVSEQ(self)) ? Qtrue : Qfalse;
    }

    /*
     * Calculates length of a curve
     * @overload arc_length(slice = nil, is_closed = nil)
     *   @param slice [Range,CvSlice,nil] Starting and ending points of the curve.
     *     By default, the whole curve length is calculated.
     *   @param is_closed [Boolean,nil] Indicates whether the curve is closed or not.
     *     There are 3 cases:
     *     * is_closed = true  - the curve is assumed to be unclosed.
     *     * is_closed = false - the curve is assumed to be closed.
     *     * is_closed = nil (default) use self#closed?
     * @return [Number] Length of the curve
     * @opencv_func cvArcLength
     */
    VALUE
    rb_arc_length(int argc, VALUE *argv, VALUE self)
    {
      VALUE slice, is_closed;
      rb_scan_args(argc, argv, "02", &slice, &is_closed);
      double length = 0;
      try {
	length = cvArcLength(CVARR(self),
			     NIL_P(slice) ? CV_WHOLE_SEQ : VALUE_TO_CVSLICE(slice),
			     TRUE_OR_FALSE(is_closed, -1));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return rb_float_new(length);
    }

    void
    init_ruby_module()
    {
#if 0
      // For documentation using YARD
      VALUE opencv = rb_define_module("OpenCV");
#endif

      if (module)
	return;

      VALUE opencv = rb_module_opencv();
      module = rb_define_module_under(opencv, "Curve");
      rb_define_method(module, "closed?", RUBY_METHOD_FUNC(rb_closed_q), 0);
      rb_define_method(module, "convex?", RUBY_METHOD_FUNC(rb_convex_q), 0);
      rb_define_method(module, "hole?", RUBY_METHOD_FUNC(rb_hole_q), 0);
      rb_define_method(module, "simple?", RUBY_METHOD_FUNC(rb_simple_q), 0);
      rb_define_method(module, "arc_length", RUBY_METHOD_FUNC(rb_arc_length), -1);
    }
  }
}
