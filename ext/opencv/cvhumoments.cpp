/************************************************************

   cvhumoments.cpp -

   $Author: ser1zw $

   Copyright (C) 2011 ser1zw

************************************************************/
#include "cvhumoments.h"
#include "cvmoments.h"
#include "opencv.h"

/*
 * Document-class: OpenCV::CvHuMoments
 *
 * Hu invariants
 */
namespace mOpenCV {
  namespace cCvHuMoments {

    VALUE rb_klass;

    VALUE
    rb_class()
    {
      return rb_klass;
    }

    VALUE
    rb_allocate(VALUE klass)
    {
      CvHuMoments *ptr;
      return Data_Make_Struct(klass, CvHuMoments, 0, -1, ptr);
    }

    /*
     * Calculates the seven Hu invariants
     * @overload new(src_moments)
     * @param src_moments [CvMoment] Input moments
     * @return [CvHuMoments] Output Hu invariants
     * @opencv_func cvGetHuMoments
     * @see http://en.wikipedia.org/wiki/Image_moment#Rotation_invariant_moments
     */
    VALUE
    rb_initialize(VALUE self, VALUE src_moments)
    {
      try {
	cvGetHuMoments(CVMOMENTS(src_moments), CVHUMOMENTS(self));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return self;      
    }

    /*
     * Returns hu1 invariant
     * @overload hu1
     * @return [Number] hu1 invariant
     */
    VALUE rb_hu1(VALUE self) {
      return DBL2NUM(CVHUMOMENTS(self)->hu1);
    }

    /*
     * Returns hu2 invariant
     * @overload hu2
     * @return [Number] hu2 invariant
     */
    VALUE rb_hu2(VALUE self) {
      return DBL2NUM(CVHUMOMENTS(self)->hu2);
    }

    /*
     * Returns hu3 invariant
     * @overload hu3
     * @return [Number] hu3 invariant
     */
    VALUE rb_hu3(VALUE self) {
      return DBL2NUM(CVHUMOMENTS(self)->hu3);
    }

    /*
     * Returns hu4 invariant
     * @overload hu4
     * @return [Number] hu4 invariant
     */
    VALUE rb_hu4(VALUE self) {
      return DBL2NUM(CVHUMOMENTS(self)->hu4);
    }

    /*
     * Returns hu5 invariant
     * @overload hu5
     * @return [Number] hu5 invariant
     */
    VALUE rb_hu5(VALUE self) {
      return DBL2NUM(CVHUMOMENTS(self)->hu5);
    }

    /*
     * Returns hu6 invariant
     * @overload hu6
     * @return [Number] hu6 invariant
     */
    VALUE rb_hu6(VALUE self) {
      return DBL2NUM(CVHUMOMENTS(self)->hu6);
    }

    /*
     * Returns hu7 invariant
     * @overload hu7
     * @return [Number] hu7 invariant
     */
    VALUE rb_hu7(VALUE self) {
      return DBL2NUM(CVHUMOMENTS(self)->hu7);
    }

    /*
     * Returns hu invaliants as an Array
     * @overload to_ary
     * @return [Array<Number>] Hu invaliants
     */
    VALUE
    rb_to_ary(VALUE self)
    {
      CvHuMoments *hu_moments = CVHUMOMENTS(self);
      return rb_ary_new3(7,
			 rb_float_new(hu_moments->hu1),
			 rb_float_new(hu_moments->hu2),
			 rb_float_new(hu_moments->hu3),
			 rb_float_new(hu_moments->hu4),
			 rb_float_new(hu_moments->hu5),
			 rb_float_new(hu_moments->hu6),
			 rb_float_new(hu_moments->hu7));
    }

    VALUE
    new_object(CvMoments *src_moments)
    {
      VALUE object = rb_allocate(rb_klass);
      try {
	cvGetHuMoments(src_moments, CVHUMOMENTS(object));
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
      VALUE opencv = rb_module_opencv();

      rb_klass = rb_define_class_under(opencv, "CvHuMoments", rb_cObject);
      rb_define_alloc_func(rb_klass, rb_allocate);
      rb_define_method(rb_klass, "initialize", RUBY_METHOD_FUNC(rb_initialize), 1);
      rb_define_method(rb_klass, "to_ary", RUBY_METHOD_FUNC(rb_to_ary), 0);
      rb_define_alias(rb_klass, "to_a", "to_ary");

      rb_define_method(rb_klass, "hu1", RUBY_METHOD_FUNC(rb_hu1), 0);
      rb_define_method(rb_klass, "hu2", RUBY_METHOD_FUNC(rb_hu2), 0);
      rb_define_method(rb_klass, "hu3", RUBY_METHOD_FUNC(rb_hu3), 0);
      rb_define_method(rb_klass, "hu4", RUBY_METHOD_FUNC(rb_hu4), 0);
      rb_define_method(rb_klass, "hu5", RUBY_METHOD_FUNC(rb_hu5), 0);
      rb_define_method(rb_klass, "hu6", RUBY_METHOD_FUNC(rb_hu6), 0);
      rb_define_method(rb_klass, "hu7", RUBY_METHOD_FUNC(rb_hu7), 0);
    }
  }
}
