/************************************************************

   cvcircle32f.cpp -

   $Author: lsxi $

   Copyright (C) 2005-2006 Masakazu Yonekura

************************************************************/
#include "cvutils.h"
#include "cvcircle32f.h"
#include "cvpoint2d32f.h"
#include "opencv.h"
/*
 * Document-class: OpenCV::CvCircle32f
 *
 * Combination of center and radius.
 *
 * see CvMat#hough_circles
 */
namespace mOpenCV {
  namespace cCvCircle32f {  
    VALUE rb_klass;

    VALUE
    rb_class()
    {
      return rb_klass;
    }

    VALUE
    rb_allocate(VALUE klass)
    {
      CvCircle32f *ptr;
      return Data_Make_Struct(klass, CvCircle32f, 0, -1, ptr);
    }

    /*
     * Returns center point of the circle
     * @overload center
     * @return [CvPoint2D32f] Center point of the circle
     */
    VALUE
    rb_center(VALUE self)
    {
      return cCvPoint2D32f::new_object(CVCIRCLE32F(self)->center);
    }

    /*
     * Returns radius of the circle
     * @overload radius
     * @return [Number] Radius of the circle
     */
    VALUE
    rb_radius(VALUE self)
    {
      return rb_float_new(CVCIRCLE32F(self)->radius);
    }

    /*
     * Accesses to parameters of the circle by array-like interface ([X-coordinate, Y-coordinate, radius])
     * @overload []
     *   @param index [Integer] Index
     * @return [Number] X-coordinate, Y-coordinate or radius of the circle
     */
    VALUE
    rb_aref(VALUE self, VALUE index)
    {
      switch (NUM2INT(index)) {
      case 0:
	return DBL2NUM(CVCIRCLE32F(self)->center.x);
	break;
      case 1:
	return DBL2NUM(CVCIRCLE32F(self)->center.y);
	break;
      case 2:
	return DBL2NUM(CVCIRCLE32F(self)->radius);
	break;
      default:
	rb_raise(rb_eIndexError, "index should be 0...3");
	break;
      }
      return Qnil;
    }

    /*
     * Returns parameters of the circle as an array which contains [center<CvPoint2D32f>, radius<Number>]
     * @overload to_ary
     * @return [Array<CvPoint2D32f,Number>] An array which contains [center, radius]
     */
    VALUE
    rb_to_ary(VALUE self)
    {
      return rb_ary_new3(2, rb_center(self), rb_radius(self));
    }

    VALUE
    new_object(CvCircle32f circle32f)
    {
      VALUE object = rb_allocate(rb_klass);
      *CVCIRCLE32F(object) = circle32f;
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
  
      rb_klass = rb_define_class_under(opencv, "CvCircle32f", rb_cObject);
      rb_define_alloc_func(rb_klass, rb_allocate);
      rb_define_method(rb_klass, "center", RUBY_METHOD_FUNC(rb_center), 0);
      rb_define_method(rb_klass, "radius", RUBY_METHOD_FUNC(rb_radius), 0);
      rb_define_method(rb_klass, "[]", RUBY_METHOD_FUNC(rb_aref), 1);
      rb_define_method(rb_klass, "to_ary", RUBY_METHOD_FUNC(rb_to_ary), 0);
      rb_define_alias(rb_klass, "to_a", "to_ary");
    }
  }
}
