/************************************************************

   cvrect.cpp -

   $Author: lsxi $

   Copyright (C) 2005-2006 Masakazu Yonekura

************************************************************/
#include "cvrect.h"
#include "cvpoint.h"
#include "cvpoint2d32f.h"
#include "opencv.h"
/*
 * Document-class: OpenCV::CvRect
 *
 * This class have coordinate of top-left point(x, y) and size, width and height.
 * 
 * 
 * C stracture is here, very simple.
 *   typdef struct CvRect {
 *     int x;
 *     int y;
 *     int width;
 *     int height;
 *   }
 * 
 */
namespace mOpenCV {
  namespace cCvRect {
    VALUE rb_klass;

    VALUE
    rb_class()
    {
      return rb_klass;
    }

    /*
     * call-seq:
     *   combatible?(obj)
     *
     * Return compatibility to CvRect. Return true if object have method #x and #y and #width and #height.
     *
     * For example.
     *   class MyRect
     *     def x
     *       1
     *     end
     *     def y
     *       2
     *     end
     *     def width
     *       10
     *     end
     *     def height
     *       20
     *     end
     *   end
     *   mr = MyRect.new
     *   CvRect.compatible?(mp)  #=> true
     *   CvRect.new(mp)          #=> same as CvRect(1, 2, 10, 20)
     */
    VALUE
    rb_compatible_q(VALUE klass, VALUE object)
    {
      return (rb_respond_to(object, rb_intern("x")) &&
	      rb_respond_to(object, rb_intern("y")) &&
	      rb_respond_to(object, rb_intern("width")) &&
	      rb_respond_to(object, rb_intern("height"))) ? Qtrue : Qfalse;
    }

    /*
     * call-seq:
     *   max_rect(rect1, rect2) -> cvrect
     *
     * Finds bounding rectangle for given rectangles.
     */
    VALUE
    rb_max_rect(VALUE klass, VALUE rect1, VALUE rect2)
    {
      return cCvRect::new_object(cvMaxRect(CVRECT(rect1), CVRECT(rect2)));
    }

    VALUE
    rb_allocate(VALUE klass)
    {
      CvRect *ptr;
      return Data_Make_Struct(klass, CvRect, 0, -1, ptr);
    }

    /*
     * call-seq:
     *   new -> CvRect.new(0, 0, 0, 0)
     *   new(obj) -> CvRect.new(obj.x.to_i, obj.y.to_i, obj.width.to_i, obj.height.to_i)
     *   new(x, y, width, height)
     *
     * Create new rectangle area. (x, y) is top-left point, and width, height is size of area.
     * It is dropped below the decimal point.
     *
     * new() is same as new(0, 0, 0, 0)
     *
     * new(obj) is same as new(obj.x.to_i, obj.y.to_i, obj.width.to_i, obj.height.to_i)
     */
    VALUE
    rb_initialize(int argc, VALUE *argv, VALUE self)
    {
      CvRect *self_ptr = CVRECT(self);
      switch (argc) {
      case 0:
	break;
      case 1: {
	CvRect rect = VALUE_TO_CVRECT(argv[0]);
	self_ptr->x = rect.x;
	self_ptr->y = rect.y;
	self_ptr->width = rect.width;
	self_ptr->height = rect.height;
	break;
      }
      case 4:
	self_ptr->x = NUM2INT(argv[0]);
	self_ptr->y = NUM2INT(argv[1]);
	self_ptr->width = NUM2INT(argv[2]);
	self_ptr->height = NUM2INT(argv[3]);
	break;
      default:
	rb_raise(rb_eArgError, "wrong number of arguments (%d for 0..2)", argc);
	break;
      }
      return self;
    }

    /*
     * Return parameter on x-axis of top-left point.
     */
    VALUE
    rb_x(VALUE self)
    {
      return INT2NUM(CVRECT(self)->x);
    }

    /*
     * call-seq:
     *   x = val
     *
     * Set x-axis parameter of top-left point, return self.
     * It is dropped below the decimal point.
     */
    VALUE
    rb_set_x(VALUE self, VALUE x)
    {
      CVRECT(self)->x = NUM2INT(x);
      return self;
    }

    /*
     * Return parameter on y-axis of top-left point.
     */
    VALUE
    rb_y(VALUE self)
    {
      return INT2NUM(CVRECT(self)->y);
    }

    /*
     * call-seq:
     *   y = val
     *
     * Set y-axis parameter of top-left point, return self.
     * It is dropped below the decimal point.
     */
    VALUE
    rb_set_y(VALUE self, VALUE y)
    {
      CVRECT(self)->y = NUM2INT(y);
      return self;
    }

    /*
     * Return size of x-axis.
     */
    VALUE
    rb_width(VALUE self)
    {
      return INT2NUM(CVRECT(self)->width);
    }

    /*
     * call-seq:
     *   width = val
     *
     * Set x-axis size, return self.
     * It is dropped below the decimal point.
     */
    VALUE
    rb_set_width(VALUE self, VALUE x)
    {
      CVRECT(self)->width = NUM2INT(x);
      return self;
    }

    /*
     * Return size of y-axis.
     */
    VALUE
    rb_height(VALUE self)
    {
      return INT2NUM(CVRECT(self)->height);
    }

    /*
     * call-seq:
     *   height = val
     *
     * Set y-axis size, return self.
     * It is dropped below the decimal point.
     */
    VALUE
    rb_set_height(VALUE self, VALUE y)
    {
      CVRECT(self)->height = NUM2INT(y);
      return self;
    }

    /*
     * Return center point of rectangle.
     */
    VALUE
    rb_center(VALUE self)
    {
      CvRect *rect = CVRECT(self);  
      return cCvPoint2D32f::new_object(cvPoint2D32f((float)rect->x + (float)rect->width / 2.0,
						    (float)rect->y + (float)rect->height / 2.0));
    }

    /*
     * Return 4 points (top-left, bottom-left, bottom-right, top-right)
     */
    VALUE
    rb_points(VALUE self)
    {
      CvRect *rect = CVRECT(self);
      return rb_ary_new3(4,
			 cCvPoint::new_object(cvPoint(rect->x, rect->y)),
			 cCvPoint::new_object(cvPoint(rect->x, rect->y + rect->height)),
			 cCvPoint::new_object(cvPoint(rect->x + rect->width, rect->y + rect->height)),
			 cCvPoint::new_object(cvPoint(rect->x + rect->width, rect->y))
			 );
    }

    /*
     * Return top-left point of rectangle.
     */
    VALUE
    rb_top_left(VALUE self)
    {
      CvRect* rect = CVRECT(self);
      return cCvPoint::new_object(cvPoint(rect->x, rect->y));
    }

    /*
     * Return top-right point of rectangle.
     */
    VALUE
    rb_top_right(VALUE self)
    {
      CvRect* rect = CVRECT(self);
      return cCvPoint::new_object(cvPoint(rect->x + rect->width, rect->y));
    }

    /*
     * Return bottom-left point of rectangle.
     */
    VALUE
    rb_bottom_left(VALUE self)
    {
      CvRect* rect = CVRECT(self);
      return cCvPoint::new_object(cvPoint(rect->x,
					  rect->y + rect->height));
    }

    /*
     * Return bottom-right point of rectangle.
     */
    VALUE
    rb_bottom_right(VALUE self)
    {
      CvRect* rect = CVRECT(self);
      return cCvPoint::new_object(cvPoint(rect->x + rect->width,
					  rect->y + rect->height));
    }

    VALUE
    new_object(CvRect rect)
    {
      VALUE object = rb_allocate(rb_klass);
      *CVRECT(object) = rect;
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
      rb_klass = rb_define_class_under(opencv, "CvRect", rb_cObject);            
      rb_define_alloc_func(rb_klass, rb_allocate);
      rb_define_singleton_method(rb_klass, "compatible?", RUBY_METHOD_FUNC(rb_compatible_q), 1);
      rb_define_singleton_method(rb_klass, "max_rect", RUBY_METHOD_FUNC(rb_max_rect), 2);
      rb_define_method(rb_klass, "initialize", RUBY_METHOD_FUNC(rb_initialize), -1);

      rb_define_method(rb_klass, "x", RUBY_METHOD_FUNC(rb_x), 0);
      rb_define_method(rb_klass, "x=", RUBY_METHOD_FUNC(rb_set_x), 1);
      rb_define_method(rb_klass, "y", RUBY_METHOD_FUNC(rb_y), 0);
      rb_define_method(rb_klass, "y=", RUBY_METHOD_FUNC(rb_set_y), 1);
      rb_define_method(rb_klass, "width", RUBY_METHOD_FUNC(rb_width), 0);
      rb_define_method(rb_klass, "width=", RUBY_METHOD_FUNC(rb_set_width), 1);
      rb_define_method(rb_klass, "height", RUBY_METHOD_FUNC(rb_height), 0);
      rb_define_method(rb_klass, "height=", RUBY_METHOD_FUNC(rb_set_height), 1);      
      rb_define_method(rb_klass, "center", RUBY_METHOD_FUNC(rb_center), 0);
      rb_define_method(rb_klass, "points", RUBY_METHOD_FUNC(rb_points), 0);
      rb_define_method(rb_klass, "top_left", RUBY_METHOD_FUNC(rb_top_left), 0);
      rb_define_method(rb_klass, "top_right", RUBY_METHOD_FUNC(rb_top_right), 0);
      rb_define_method(rb_klass, "bottom_left", RUBY_METHOD_FUNC(rb_bottom_left), 0);
      rb_define_method(rb_klass, "bottom_right", RUBY_METHOD_FUNC(rb_bottom_right), 0);
    }
  }
}
