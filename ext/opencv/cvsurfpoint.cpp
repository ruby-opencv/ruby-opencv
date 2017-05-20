/************************************************************

   cvsurfpoint.cpp -

   $Author: ser1zw $

   Copyright (C) 2011 ser1zw

************************************************************/
#include "cvsurfpoint.h"
#include "cvpoint2d32f.h"
#include "opencv.h"
/*
 * Document-class: OpenCV::CvSURFPoint
 *
 * C structure is here.
 *   typedef struct CvSURFPoint {
 *     CvPoint2D32f pt; // position of the feature within the image
 *     int laplacian;   // -1, 0 or +1. sign of the laplacian at the point.
 *                      // can be used to speedup feature comparison
 *                      // (normally features with laplacians of different
 *                      // signs can not match)
 *     int size;        // size of the feature
 *     float dir;       // orientation of the feature: 0..360 degrees
 *     float hessian;   // value of the hessian (can be used to
 *                      // approximately estimate the feature strengths)
 *   } CvSURFPoint;
 */
namespace mOpenCV {
  namespace cCvSURFPoint {

    VALUE rb_klass;

    VALUE
    rb_class()
    {
      return rb_klass;
    }

#ifdef IS_OPENCV2
    VALUE
    rb_allocate(VALUE klass)
    {
      CvSURFPoint *ptr;
      return Data_Make_Struct(klass, CvSURFPoint, 0, -1, ptr);
    }

    /*
     * Create a CvSURFPoint
     *
     * @overload new(pt, laplacian, size, dir, hessian)
     *   @param pt [CvPoint2D32f] Position of the feature within the image
     *   @param laplacian [Integer] -1, 0 or +1. sign of the laplacian at the point.
     *     Can be used to speedup feature comparison
     *     (normally features with laplacians of different signs can not match)
     *   @param size [Integer] Size of the feature
     *   @param dir [Number] Orientation of the feature: 0..360 degrees
     *   @param hessian [Number] Value of the hessian (can be used to
     *     approximately estimate the feature strengths)
     * @return [CvSURFPoint] self
     */
    VALUE
    rb_initialize(VALUE self, VALUE pt, VALUE laplacian, VALUE size, VALUE dir, VALUE hessian)
    {
      CvSURFPoint *self_ptr = CVSURFPOINT(self);
      self_ptr->pt = VALUE_TO_CVPOINT2D32F(pt);
      self_ptr->laplacian = NUM2INT(laplacian);
      self_ptr->size = NUM2INT(size);
      self_ptr->dir = (float)NUM2DBL(dir);
      self_ptr->hessian = (float)NUM2DBL(hessian);

      return self;
    }

    /*
     * Return position of the feature as CvPoint2D32f.
     *
     * @overload pt
     * @return [CvPoint2D32f] Position of the feature.
     */
    VALUE
    rb_get_pt(VALUE self)
    {
      return REFER_OBJECT(cCvPoint2D32f::rb_class(), &CVSURFPOINT(self)->pt, self);
    }

    /*
     * Set position of the feature.
     *
     * @overload pt=(value)
     *   @param value [CvPoint2D32f] Valuet to set.
     */
    VALUE
    rb_set_pt(VALUE self, VALUE value)
    {
      CVSURFPOINT(self)->pt = VALUE_TO_CVPOINT2D32F(value);
      return self;
    }

    /*
     * Return sign of the laplacian at the point (-1, 0 or +1)
     *
     * @overload laplacian
     * @return [Integer] Sign of the laplacian at the point.
     */
    VALUE
    rb_get_laplacian(VALUE self)
    {
      return INT2NUM(CVSURFPOINT(self)->laplacian);
    }

    /*
     * Set sign of the laplacian at the point
     *
     * @overload laplacian=(value)
     *   @param value [Integer] Value to set.
     */
    VALUE
    rb_set_laplacian(VALUE self, VALUE value)
    {
      int val = NUM2INT(value);
      CVSURFPOINT(self)->laplacian = (val > 0) ? 1 : (val < 0) ? -1 : 0;
      return self;
    }

    /*
     * Returns size of feature.
     *
     * @overload size
     * @return [Integer] Size of feature.
     */
    VALUE
    rb_get_size(VALUE self)
    {
      return INT2NUM(CVSURFPOINT(self)->size);
    }

    /*
     * Return size of feature
     *
     * @overload size=(value)
     *   @param [Integer] Value to set.
     */
    VALUE
    rb_set_size(VALUE self, VALUE value)
    {
      CVSURFPOINT(self)->size = NUM2INT(value);
      return self;
    }

    /*
     * Return orientation of the feature: 0..360 degrees
     *
     * @overload dir
     * @return [Number] Orientation of the feature.
     */
    VALUE
    rb_get_dir(VALUE self)
    {
      return DBL2NUM((double)(CVSURFPOINT(self)->dir));
    }

    /*
     * Set orientation of the feature: 0..360 degrees.
     *
     * @overload dir=(value)
     *   @param [Number] Value to set.
     */
    VALUE
    rb_set_dir(VALUE self, VALUE value)
    {
      CVSURFPOINT(self)->dir = (float)NUM2DBL(value);
      return self;
    }

    /*
     * Return value of the hessian
     *
     * @overload hessian
     * @return [Number] Hessian
     */
    VALUE
    rb_get_hessian(VALUE self)
    {
      return DBL2NUM((double)(CVSURFPOINT(self)->hessian));
    }

    /*
     * Set value of the hessian
     *
     * @overload hessian=(value)
     *   @param [Number] Value to set.
     */
    VALUE
    rb_set_hessian(VALUE self, VALUE value)
    {
      CVSURFPOINT(self)->hessian = (float)NUM2DBL(value);
      return self;
    }

    VALUE
    new_object()
    {
      return rb_allocate(rb_klass);
    }

    VALUE
    new_object(CvSURFPoint* cvsurfpoint)
    {
      VALUE object = rb_allocate(rb_klass);
      CvSURFPoint *ptr = CVSURFPOINT(object);
      *ptr = *cvsurfpoint;
      return object;
    }
#else
    VALUE
    rb_allocate(VALUE klass)
    {
      return Qnil;
    }
#  define rb_initialize raise_opencv3_unsupported_1
#  define rb_get_pt raise_opencv3_unsupported_n
#  define rb_set_pt raise_opencv3_unsupported_n
#  define rb_get_laplacian raise_opencv3_unsupported_n
#  define rb_set_laplacian raise_opencv3_unsupported_n
#  define rb_get_size raise_opencv3_unsupported_n
#  define rb_set_size raise_opencv3_unsupported_n
#  define rb_get_dir raise_opencv3_unsupported_n
#  define rb_set_dir raise_opencv3_unsupported_n
#  define rb_get_hessian raise_opencv3_unsupported_n
#  define rb_set_hessian raise_opencv3_unsupported_n
#endif

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
      rb_klass = rb_define_class_under(opencv, "CvSURFPoint", rb_cObject);
      rb_define_alloc_func(rb_klass, rb_allocate);
      rb_define_method(rb_klass, "initialize", RUBY_METHOD_FUNC(rb_initialize), 5);
      rb_define_method(rb_klass, "pt", RUBY_METHOD_FUNC(rb_get_pt), 0);
      rb_define_method(rb_klass, "pt=", RUBY_METHOD_FUNC(rb_set_pt), 1);
      rb_define_method(rb_klass, "laplacian", RUBY_METHOD_FUNC(rb_get_laplacian), 0);
      rb_define_method(rb_klass, "laplacian=", RUBY_METHOD_FUNC(rb_set_laplacian), 1);
      rb_define_method(rb_klass, "size", RUBY_METHOD_FUNC(rb_get_size), 0);
      rb_define_method(rb_klass, "size=", RUBY_METHOD_FUNC(rb_set_size), 1);
      rb_define_method(rb_klass, "dir", RUBY_METHOD_FUNC(rb_get_dir), 0);
      rb_define_method(rb_klass, "dir=", RUBY_METHOD_FUNC(rb_set_dir), 1);
      rb_define_method(rb_klass, "hessian", RUBY_METHOD_FUNC(rb_get_hessian), 0);
      rb_define_method(rb_klass, "hessian=", RUBY_METHOD_FUNC(rb_set_hessian), 1);
    }
  }
}
