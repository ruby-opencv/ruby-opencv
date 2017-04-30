/************************************************************

   cvconvexitydefect.cpp -

   $Author: lsxi $

   Copyright (C) 2007 Masakazu Yonekura

************************************************************/
#include "cvconvexitydefect.h"
#include "cvpoint.h"
#include "opencv.h"
/*
 * Document-class: OpenCV::CvConvexityDefect
 *
 * Convexity defect
 */
namespace mOpenCV {
  namespace cCvConvexityDefect {
    VALUE rb_klass;

    VALUE
    rb_class()
    {
      return rb_klass;
    }

    /*
     * Returns the point of the contour where the defect begins
     * @overload start
     * @return [CvPoint] Start point of the contour
     */
    VALUE
    rb_start(VALUE self)
    {
      return cCvPoint::new_object(*CVCONVEXITYDEFECT(self)->start);
    }

    /*
     * Returns the point of the contour where the defect ends
     * @overload end
     * @return [CvPoint] End point of the contour
     */
    VALUE
    rb_end(VALUE self)
    {
      return cCvPoint::new_object(*CVCONVEXITYDEFECT(self)->end);
    }

    /*
     * Returns the farthest from the convex hull point within the defect
     * @overload depth_point
     * @return [CvPoint] The farthest from the convex hull point within the defect
     */
    VALUE
    rb_depth_point(VALUE self)
    {
      return cCvPoint::new_object(*CVCONVEXITYDEFECT(self)->depth_point);
    }

    /*
     * Returns distance between the farthest point and the convex hull
     * @overload depth
     * @return [Number] Distance between the farthest point and the convex hull
     */
    VALUE
    rb_depth(VALUE self)
    {
      return rb_float_new(CVCONVEXITYDEFECT(self)->depth);
    }

    void
    init_ruby_class()
    {
#if 0
      // For documentation using YARD
      VALUE opencv = rb_define_module("OpenCV");
#endif

      if (rb_klass)
	return;

      VALUE opencv = rb_module_opencv();

      rb_klass = rb_define_class_under(opencv, "CvConvexityDefect", rb_cObject);
      rb_define_method(rb_klass, "start", RUBY_METHOD_FUNC(rb_start), 0);
      rb_define_method(rb_klass, "end", RUBY_METHOD_FUNC(rb_end), 0);
      rb_define_method(rb_klass, "depth_point", RUBY_METHOD_FUNC(rb_depth_point), 0);
      rb_define_method(rb_klass, "depth", RUBY_METHOD_FUNC(rb_depth), 0);
    }
  }
}
