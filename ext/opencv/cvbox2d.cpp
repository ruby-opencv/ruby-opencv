/************************************************************

   cvbox2d.cpp -

   $Author: lsxi $

   Copyright (C) 2005-2006 Masakazu Yonekura

************************************************************/
#include "opencv2/core/core_c.h"
#include "cvbox2d.h"
#include "cvpoint2d32f.h"
#include "cvsize2d32f.h"
/*
 * Document-class: OpenCV::CvBox2D
 *
 * Stores coordinates of a rotated rectangle.
 */
namespace mOpenCV {
  namespace cCvBox2D {
    VALUE rb_klass;

    VALUE
    rb_class()
    {
      return rb_klass;
    }

    VALUE
    rb_allocate(VALUE klass)
    {
      CvBox2D *ptr;
      return Data_Make_Struct(klass, CvBox2D, 0, -1, ptr);
    }

    /*
     * Create a box
     * @overload new(center=nil, size=nil, angle=nil)
     *   @param center [CvPoint2D32f,nil] Center of the box
     *   @param size [CvSize,nil] Size of the box
     *   @param angle [Number,nil] Angle between the horizontal axis and the first side in degrees
     * @return [CvBox2D] New box
     */
    VALUE
    rb_initialize(int argc, VALUE *argv, VALUE self)
    {
      VALUE center, size, angle;
      CvBox2D* self_ptr = CVBOX2D(self);
      rb_scan_args(argc, argv, "03", &center, &size, &angle);
  
      if (!NIL_P(center)) {
	self_ptr->center = VALUE_TO_CVPOINT2D32F(center);
      }
      if (!NIL_P(size)) {
	self_ptr->size = VALUE_TO_CVSIZE2D32F(size);
	self_ptr->angle = NUM2DBL(angle);
      }
  
      return self;
    }

    /*
     * Returns center point of the box
     * @overload center
     * @return [CvPoint2D32f] Center of the box
     */
    VALUE
    rb_center(VALUE self)
    {
      return REFER_OBJECT(cCvPoint2D32f::rb_class(), &CVBOX2D(self)->center, self);
    }

    /*
     * Set center point of the box
     * @overload center=value
     *   @param value [CvPoint2D32f] Center of the box
     * @return [CvBox2D] self
     */
    VALUE
    rb_set_center(VALUE self, VALUE value)
    {
      CVBOX2D(self)->center = VALUE_TO_CVPOINT2D32F(value);
      return self;
    }

    /*
     * Returns size of the box
     * @overload size
     * @return [CvSize2D32f] Size of the box
     */
    VALUE
    rb_size(VALUE self)
    {
      return REFER_OBJECT(cCvSize2D32f::rb_class(), &CVBOX2D(self)->size, self);
    }

    /*
     * Set size of the box
     * @overload size=value
     *   @param value [CvSize2D32f] Size of the box
     * @return [CvBox2D] self
     */
    VALUE
    rb_set_size(VALUE self, VALUE value)
    {
      CVBOX2D(self)->size = VALUE_TO_CVSIZE2D32F(value);
      return self;
    }

    /*
     * Returns angle of the box
     * @overload angle
     * @return [Float] Angle of the box
     */
    VALUE
    rb_angle(VALUE self)
    {
      return rb_float_new(CVBOX2D(self)->angle);
    }

    /*
     * Set angle of the box
     * @overload angle=value
     *   @param value [Number] Angle of the box
     * @return [CvBox2D] self
     */
    VALUE
    rb_set_angle(VALUE self, VALUE value)
    {
      CVBOX2D(self)->angle = NUM2DBL(value);
      return self;
    }

    /*
     * Find box vertices
     * @overload points
     * @return [Array<CvPoint2D32f>] Vertices of the box
     * @opencv_func cvBoxPoints
     */
    VALUE
    rb_points(VALUE self)
    {
      const int n = 4;
      CvPoint2D32f p[n];
      try {
	cvBoxPoints(*CVBOX2D(self), p);
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      VALUE points = rb_ary_new2(n);
      for (int i = 0; i < n; ++i) {
	rb_ary_store(points, i, cCvPoint2D32f::new_object(p[i]));
      }
      return points;
    }

    VALUE
    new_object()
    {
      return rb_allocate(cCvBox2D::rb_class());
    }

    VALUE
    new_object(CvBox2D box)
    {
      VALUE object = rb_allocate(rb_klass);
      *CVBOX2D(object) = box;
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
      rb_klass = rb_define_class_under(opencv, "CvBox2D", rb_cObject);
      rb_define_alloc_func(rb_klass, rb_allocate);
      rb_define_method(rb_klass, "initialize", RUBY_METHOD_FUNC(rb_initialize), -1);
      rb_define_method(rb_klass, "center", RUBY_METHOD_FUNC(rb_center), 0);
      rb_define_method(rb_klass, "center=", RUBY_METHOD_FUNC(rb_set_center), 1);
      rb_define_method(rb_klass, "size", RUBY_METHOD_FUNC(rb_size), 0);
      rb_define_method(rb_klass, "size=", RUBY_METHOD_FUNC(rb_set_size), 1);
      rb_define_method(rb_klass, "angle", RUBY_METHOD_FUNC(rb_angle), 0);
      rb_define_method(rb_klass, "angle=", RUBY_METHOD_FUNC(rb_set_angle), 1);
      rb_define_method(rb_klass, "points", RUBY_METHOD_FUNC(rb_points), 0);
    }
  }
}
