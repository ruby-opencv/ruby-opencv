/************************************************************

   cvmoments.cpp -

   $Author: lsxi $

   Copyright (C) 2007 Masakazu Yonekura

************************************************************/
#include "cvmoments.h"
#include "cvpoint2d32f.h"
#include "cvmat.h"
#include "cvseq.h"
#include "cvhumoments.h"
#include "opencv.h"

/*
 * Document-class: OpenCV::CvMoments
 *
 * moments
 */

#define DEFINE_CVMOMENTS_ACCESSOR(elem)					\
  rb_define_method(rb_klass, #elem, RUBY_METHOD_FUNC(rb_##elem), 0)

#define CVMOMENTS_ACCESSOR(elem)					\
  VALUE rb_##elem(VALUE self) { return DBL2NUM(CVMOMENTS(self)->elem); }

namespace mOpenCV {
  namespace cCvMoments {
    VALUE rb_klass;

    VALUE
    rb_class()
    {
      return rb_klass;
    }

    VALUE
    rb_allocate(VALUE klass)
    {
      CvMoments *ptr;
      return Data_Make_Struct(klass, CvMoments, 0, -1, ptr);
    }
    /*
     * call-seq:
     *   CvMoments.new(src[,is_binary = nil])
     *
     * Calculates all moments up to third order of a polygon or rasterized shape.
     * <i>src</i> should be CvMat or CvPolygon.
     *
     * If is_binary = true, all the zero pixel values are treated as zeroes, all the others are treated as 1's.
     */
    VALUE
    rb_initialize(int argc, VALUE *argv, VALUE self)
    {
      VALUE src, is_binary;
      rb_scan_args(argc, argv, "02", &src, &is_binary);
      if (!NIL_P(src)) {
	if (rb_obj_is_kind_of(src, cCvMat::rb_class()) || rb_obj_is_kind_of(src, cCvSeq::rb_class())) {
	  try {
	    cvMoments(CVARR(src), CVMOMENTS(self), TRUE_OR_FALSE(is_binary, 0));
	  }
	  catch (cv::Exception& e) {
	    raise_cverror(e);
	  }
	}
	else
	  rb_raise(rb_eTypeError, "argument 1 (src) should be %s or %s.",
		   rb_class2name(cCvMat::rb_class()), rb_class2name(cCvSeq::rb_class()));
      }
      return self;      
    }

    CVMOMENTS_ACCESSOR(m00);
    CVMOMENTS_ACCESSOR(m10);
    CVMOMENTS_ACCESSOR(m01);
    CVMOMENTS_ACCESSOR(m20);
    CVMOMENTS_ACCESSOR(m11);
    CVMOMENTS_ACCESSOR(m02);
    CVMOMENTS_ACCESSOR(m30);
    CVMOMENTS_ACCESSOR(m21);
    CVMOMENTS_ACCESSOR(m12);
    CVMOMENTS_ACCESSOR(m03);

    CVMOMENTS_ACCESSOR(mu20);
    CVMOMENTS_ACCESSOR(mu11);
    CVMOMENTS_ACCESSOR(mu02);
    CVMOMENTS_ACCESSOR(mu30);
    CVMOMENTS_ACCESSOR(mu21);
    CVMOMENTS_ACCESSOR(mu12);
    CVMOMENTS_ACCESSOR(mu03);

    CVMOMENTS_ACCESSOR(inv_sqrt_m00);

    /*
     * call-seq:
     *   spatial -> float
     *
     * Retrieves spatial moment.
     *
     * which in case of image moments is defined as:
     *   Mx_order,y_order=sumx,y(I(x,y)*xx_order*yy_order)
     *   where I(x,y) is the intensity of the pixel (x, y). 
     */
    VALUE
    rb_spatial(VALUE self, VALUE x_order, VALUE y_order)
    {
      double result = 0;
      try {
	result = cvGetSpatialMoment(CVMOMENTS(self), NUM2INT(x_order), NUM2INT(y_order));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return rb_float_new(result);
    }

    /*
     * call-seq:
     *   central -> float
     *
     * Retrieves central moment.
     *
     * which in case of image moments is defined as:
     *   μx_order,y_order=sumx,y(I(x,y)*(x-xc)x_order*(y-yc)y_order),
     *   where xc=M10/M00, yc=M01/M00 - coordinates of the gravity center
     */
    VALUE
    rb_central(VALUE self, VALUE x_order, VALUE y_order)
    {
      double result = 0;
      try {
	result = cvGetCentralMoment(CVMOMENTS(self), NUM2INT(x_order), NUM2INT(y_order));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return rb_float_new(result);
    }

    /*
     * call-seq:
     *   normalized_central -> float
     *
     * Retrieves normalized central moment.
     *
     *  ηx_order,y_order= μx_order,y_order/M00((y_order+x_order)/2+1)
     */
    VALUE
    rb_normalized_central(VALUE self, VALUE x_order, VALUE y_order)
    {
      double result = 0;
      try {
	result = cvGetNormalizedCentralMoment(CVMOMENTS(self), NUM2INT(x_order), NUM2INT(y_order));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return rb_float_new(result);
    }

    /*
     * call-seq:
     *   hu -> cvhumoments
     *
     * Calculates seven Hu invariants.
     *
     * seven Hu invariants that are defined as:
     *   h1=η20+η02
     *   h2=(η20-η02)²+4η11²
     *   h3=(η30-3η12)²+ (3η21-η03)²
     *   h4=(η30+η12)²+ (η21+η03)²
     *   h5=(η30-3η12)(η30+η12)[(η30+η12)²-3(η21+η03)²]+(3η21-η03)(η21+η03)[3(η30+η12)²-(η21+η03)²]
     *   h6=(η20-η02)[(η30+η12)²- (η21+η03)²]+4η11(η30+η12)(η21+η03)
     *   h7=(3η21-η03)(η21+η03)[3(η30+η12)²-(η21+η03)²]-(η30-3η12)(η21+η03)[3(η30+η12)²-(η21+η03)²]
     * where ηi,j are normalized central moments of 2-nd and 3-rd orders. The computed values are proved to be invariant to the image scaling, rotation, and reflection except the seventh one, whose sign is changed by reflection. 
     */
    VALUE
    rb_hu(VALUE self)
    {
      return cCvHuMoments::new_object(CVMOMENTS(self));
    }

    /*
     * call-seq:
     *   gravity_center -> cvpoint2d32f
     *
     * Return gravity center.
     */
    VALUE
    rb_gravity_center(VALUE self)
    {
      CvMoments *moments = CVMOMENTS(self);
      CvPoint2D32f point;
      double m00 = 0, m01 = 0, m10 = 0;
      try {
	m00 = cvGetSpatialMoment(moments, 0, 0);
	m10 = cvGetSpatialMoment(moments, 1, 0);
	m01 = cvGetSpatialMoment(moments, 0, 1);
	point = cvPoint2D32f(m10 / m00, m01 / m00);
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return cCvPoint2D32f::new_object(point);
    }

    /*
     * call-seq:
     *   angle -> float
     *
     * Return angle.
     */
    VALUE
    rb_angle(VALUE self)
    {
      CvMoments *moments = CVMOMENTS(self);
      double m11 = 0, m20 = 0, m02 = 0;
      try {
	m11 = cvGetCentralMoment(moments, 1, 1);
	m20 = cvGetCentralMoment(moments, 2, 0);
	m02 = cvGetCentralMoment(moments, 0, 2);
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      double mangle = 0.5 * atan(2 * m11 / (m20 - m02));
      if (cvIsNaN(mangle) || cvIsInf(mangle))
	return Qnil;
      else
	return rb_float_new(mangle);
    }

    VALUE
    new_object(CvArr *arr, int is_binary = 0)
    {
      VALUE object = rb_allocate(rb_klass);
      try {
	cvMoments(arr, CVMOMENTS(object), is_binary);
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return object;
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
      /* 
       * opencv = rb_define_module("OpenCV");
       * 
       * note: this comment is used by rdoc.
       */
      VALUE opencv = rb_module_opencv();
  
      rb_klass = rb_define_class_under(opencv, "CvMoments", rb_cObject);
      rb_define_alloc_func(rb_klass, rb_allocate);
      rb_define_method(rb_klass, "initialize", RUBY_METHOD_FUNC(rb_initialize), -1);
      rb_define_method(rb_klass, "spatial", RUBY_METHOD_FUNC(rb_spatial), 2);
      rb_define_method(rb_klass, "central", RUBY_METHOD_FUNC(rb_central), 2);
      rb_define_method(rb_klass, "normalized_central", RUBY_METHOD_FUNC(rb_normalized_central), 2);
      rb_define_method(rb_klass, "hu", RUBY_METHOD_FUNC(rb_hu), 0);     
      rb_define_method(rb_klass, "gravity_center", RUBY_METHOD_FUNC(rb_gravity_center), 0);
      rb_define_method(rb_klass, "angle", RUBY_METHOD_FUNC(rb_angle), 0);

      DEFINE_CVMOMENTS_ACCESSOR(m00);
      DEFINE_CVMOMENTS_ACCESSOR(m10);
      DEFINE_CVMOMENTS_ACCESSOR(m01);
      DEFINE_CVMOMENTS_ACCESSOR(m20);
      DEFINE_CVMOMENTS_ACCESSOR(m11);
      DEFINE_CVMOMENTS_ACCESSOR(m02);
      DEFINE_CVMOMENTS_ACCESSOR(m30);
      DEFINE_CVMOMENTS_ACCESSOR(m21);
      DEFINE_CVMOMENTS_ACCESSOR(m12);
      DEFINE_CVMOMENTS_ACCESSOR(m03);

      DEFINE_CVMOMENTS_ACCESSOR(mu20);
      DEFINE_CVMOMENTS_ACCESSOR(mu11);
      DEFINE_CVMOMENTS_ACCESSOR(mu02);
      DEFINE_CVMOMENTS_ACCESSOR(mu30);
      DEFINE_CVMOMENTS_ACCESSOR(mu21);
      DEFINE_CVMOMENTS_ACCESSOR(mu12);
      DEFINE_CVMOMENTS_ACCESSOR(mu03);

      DEFINE_CVMOMENTS_ACCESSOR(inv_sqrt_m00);
    }
  }
}
