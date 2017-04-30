/************************************************************

   pointset.cpp -

   $Author: lsxi $

   Copyright (C) 2005-2006 Masakazu Yonekura

************************************************************/
#include "opencv2/imgproc/imgproc_c.h"

#include "pointset.h"
#include "cvbox2d.h"
#include "cvcircle32f.h"
#include "cvmemstorage.h"
#include "cvpoint.h"
#include "cvpoint2d32f.h"
#include "cvmat.h"
#include "cvseq.h"
#include "cvslice.h"
#include "cvcontour.h"
#include "cvcircle32f.h"
#include "cvconvexitydefect.h"
/*
 * Document-class: OpenCV::PointSet
 */

namespace mOpenCV {
  namespace mPointSet {
    VALUE module;

    VALUE
    rb_module()
    {
      return module;
    }

    /*
     * call-seq:
     *   contour_area -> float
     *
     * Calculates area of the whole contour or contour section.
     * 
     * note: Orientation of the contour affects the area sign, thus the method may return negative result.
     */
    VALUE
    rb_contour_area(int argc, VALUE *argv, VALUE self)
    {
      VALUE slice;
      rb_scan_args(argc, argv, "01", &slice);
      double area = 0;
      try {
	area = cvContourArea(CVARR(self), NIL_P(slice) ? CV_WHOLE_SEQ : VALUE_TO_CVSLICE(slice));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return rb_float_new(area);
    }

    /*
     * call-seq:
     *   fit_ellipse2 -> cvbox2d
     *
     * Return fits ellipse to set of 2D points.
     */
    VALUE
    rb_fit_ellipse2(VALUE self)
    {
      CvBox2D box;
      try {
	box = cvFitEllipse2(CVARR(self));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return cCvBox2D::new_object(box);
    }

    /*
     * call-seq:
     *   convex_hull2([orientation_clockwise = true]) -> cvcontour
     *
     * Finds convex hull of 2D point set using Sklansky's algorithm.
     * 
     * <i>orientation_clockwise</i>: Desired orientation of convex hull (true: clockwise, false: counter clockwise).
     */
    VALUE
    rb_convex_hull2(int argc, VALUE *argv, VALUE self)
    {
      VALUE clockwise, return_points;
      rb_scan_args(argc, argv, "02", &clockwise, &return_points);
      VALUE storage = cCvMemStorage::new_object();
      CvSeq *hull = NULL;
      int return_pts = TRUE_OR_FALSE(return_points, 1);
      try {
	hull = cvConvexHull2(CVSEQ(self), CVMEMSTORAGE(storage),
			     TRUE_OR_FALSE(clockwise, 1) ? CV_CLOCKWISE : CV_COUNTER_CLOCKWISE,
			     return_pts);
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return cCvSeq::new_sequence(cCvContour::rb_class(), hull, cCvPoint::rb_class(), storage);
    }

    /*
     * call-seq:
     *   check_contour_convexity -> true or false
     *
     * Tests whether the input contour is convex or not. The contour must be simple, i.e. without self-intersections.
     */
    VALUE
    rb_check_contour_convexity(VALUE self)
    {
      int convexity = 0;
      try {
	convexity = cvCheckContourConvexity(CVARR(self));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return convexity ? Qtrue : Qfalse;
    }

    /*
     * call-seq:
     *   convexity_defects(hull) -> cvseq(include CvConvexityDefect)
     *
     * Finds convexity defects of contour.
     */
    VALUE
    rb_convexity_defects(VALUE self, VALUE hull)
    {
      CvSeq *defects = NULL;
      CvSeq *hull_seq = CVSEQ_WITH_CHECK(hull);
      VALUE storage = cCvMemStorage::new_object();
      CvMemStorage *storage_ptr = CVMEMSTORAGE(storage);
      try {
	defects = cvConvexityDefects(CVSEQ(self), hull_seq, storage_ptr);
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return cCvSeq::new_sequence(cCvSeq::rb_class(), defects, cCvConvexityDefect::rb_class(), storage);
    }

    /*
     * call-seq:
     *   min_area_rect2 -> cvbox2d
     *
     * Finds circumscribed rectangle of minimal area for given 2D point set.
     */
    VALUE
    rb_min_area_rect2(VALUE self)
    {
      VALUE storage = cCvMemStorage::new_object();
      CvBox2D rect;
      try {
	rect = cvMinAreaRect2(CVARR(self), CVMEMSTORAGE(storage));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return cCvBox2D::new_object(rect);
    }

    /*
     * call-seq:
     *   min_enclosing_circle -> cvcircle32f
     *
     * Finds circumscribed circle of minimal area for given 2D point set.
     */
    VALUE
    rb_min_enclosing_circle(VALUE self)
    {
      VALUE circle = cCvCircle32f::rb_allocate(cCvCircle32f::rb_class());
      int success = 0;
      try {
	success = cvMinEnclosingCircle(CVARR(self), &CVCIRCLE32F(circle)->center,
				       &CVCIRCLE32F(circle)->radius);
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return success ? circle : Qnil;
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
      /* 
       * opencv = rb_define_module("OpenCV");
       *
       * note: this comment is used by rdoc.
       */
      VALUE opencv = rb_module_opencv();
      module = rb_define_module_under(opencv, "PointSet");
      rb_define_method(module, "contour_area", RUBY_METHOD_FUNC(rb_contour_area), -1);
      rb_define_method(module, "fit_ellipse2", RUBY_METHOD_FUNC(rb_fit_ellipse2), 0);
  
      rb_define_method(module, "convex_hull2", RUBY_METHOD_FUNC(rb_convex_hull2), -1);
      rb_define_method(module, "check_contour_convexity", RUBY_METHOD_FUNC(rb_check_contour_convexity), 0);
      rb_define_alias(module, "convexity?", "check_contour_convexity");
      rb_define_method(module, "convexity_defects", RUBY_METHOD_FUNC(rb_convexity_defects), 1);
      rb_define_method(module, "min_area_rect2", RUBY_METHOD_FUNC(rb_min_area_rect2), 0);
      rb_define_method(module, "min_enclosing_circle", RUBY_METHOD_FUNC(rb_min_enclosing_circle), 0);
    }
  }

  int
  CVPOINTS_FROM_POINT_SET(VALUE object, CvPoint **pointset)
  {
    if (rb_obj_is_kind_of(object, cCvSeq::rb_class())) {
      if (CV_IS_SEQ_POINT_SET(CVSEQ(object))) {
	*pointset = (CvPoint*)cvCvtSeqToArray(CVSEQ(object),
					      rb_cvAlloc(CVSEQ(object)->total * CVSEQ(object)->elem_size));
	return CVSEQ(object)->total;
      }
      else {
	rb_raise(rb_eTypeError, "sequence does not contain %s or %s.",
		 rb_class2name(cCvPoint::rb_class()), rb_class2name(cCvPoint2D32f::rb_class()));
      }
    }
    else if (rb_obj_is_kind_of(object, cCvMat::rb_class())) {
      /* to do */
      rb_raise(rb_eNotImpError, "CvMat to CvSeq conversion not implemented.");
    }
    else if (rb_obj_is_kind_of(object, rb_cArray)) {
      int len = RARRAY_LEN(object);
      *pointset = (CvPoint*)rb_cvAlloc(len * sizeof(CvPoint));
      ID id_x = rb_intern("x");
      ID id_y = rb_intern("y");
      for (int i = 0; i < len; ++i) {
	(*pointset)[i].x = NUM2INT(rb_funcall(rb_ary_entry(object, i), id_x, 0));
	(*pointset)[i].y = NUM2INT(rb_funcall(rb_ary_entry(object, i), id_y, 0));
      }
      return len;
    }
    else {
      rb_raise(rb_eTypeError, "Can't convert CvSeq(PointSet).");
    }  
  }

  CvSeq*
  VALUE_TO_POINT_SET(VALUE object)
  {
    CvSeq *seq = 0;
    VALUE tmp, storage;
    int length;
    CvPoint2D32f p32;
    if (rb_obj_is_kind_of(object, cCvSeq::rb_class())) {
      seq = CVSEQ(object);
      if (CV_IS_SEQ_POINT_SET(seq)) {
	return seq;
      }
      else {
	rb_raise(rb_eTypeError, "sequence is not contain %s or %s.", rb_class2name(cCvPoint::rb_class()), rb_class2name(cCvPoint2D32f::rb_class()));
      }
    }
    else if (rb_obj_is_kind_of(object, cCvMat::rb_class())) {
      /* to do */
      rb_raise(rb_eNotImpError, "CvMat to CvSeq conversion not implemented.");
    }
    else if (rb_obj_is_kind_of(object, rb_cArray)) {
      //pointset = cCvSeq::new_sequence(cCvSeq::rb_class(), )
      length = RARRAY_LEN(object);
      storage = cCvMemStorage::new_object();
      seq = cvCreateSeq(CV_SEQ_POINT_SET, sizeof(CvSeq), sizeof(CvPoint), CVMEMSTORAGE(storage));    
      for (int i = 0; i < RARRAY_LEN(object); i++) {
	p32.x = NUM2DBL(rb_funcall(rb_ary_entry(object, i), rb_intern("x"), 0));
	p32.y = NUM2DBL(rb_funcall(rb_ary_entry(object, i), rb_intern("y"), 0));
	cvSeqPush(seq, &p32);
      }
      tmp = cCvSeq::new_sequence(cCvSeq::rb_class(), seq, cCvPoint2D32f::rb_class(), storage);
      return seq;
    }
    else {
      rb_raise(rb_eTypeError, "Can't convert CvSeq(PointSet).");
    }  
  }
}
