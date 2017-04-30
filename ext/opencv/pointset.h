/************************************************************

   pointset.h -

   $Author: lsxi $

   Copyright (C) 2005-2006 Masakazu Yonekura

************************************************************/
#ifndef RUBY_OPENCV_POINTSET_H
#define RUBY_OPENCV_POINTSET_H

#include <ruby.h>
#include "opencv2/imgproc/types_c.h"

#define POINT_SET_P(object) rb_obj_is_kind_of(object, cCvSeq::rb_class()) && CV_IS_SEQ_POINT_SET(CVSEQ(object))

namespace mOpenCV {
  namespace mPointSet {
    VALUE rb_module();

    void init_ruby_module();
    VALUE rb_contour_area(int argc, VALUE *argv, VALUE self);
    VALUE rb_fit_ellipse2(VALUE self);
    VALUE rb_convex_hull2(int argc, VALUE *argv, VALUE self);
    VALUE rb_fit_line(int argc, VALUE *argv, VALUE self);
    VALUE rb_check_contour_convexity(VALUE self);
    VALUE rb_convexity_defects(VALUE self, VALUE hull);
    VALUE rb_min_area_rect2(VALUE self);
    VALUE rb_min_enclosing_circle(VALUE self);
  }

  int CVPOINTS_FROM_POINT_SET(VALUE object, CvPoint **pointset);
  CvSeq* VALUE_TO_POINT_SET(VALUE object);
}

#endif // RUBY_OPENCV_POINTSET_H
