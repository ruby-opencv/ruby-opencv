/************************************************************

   cvpoint2d32f.cpp -

   $Author: lsxi $

   Copyright (C) 2005 Masakazu Yonekura

************************************************************/
#include "cvpoint2d32f.h"

#include "opencv.h"

/*
 * Document-class: OpenCV::CvPoint2D32f
 *
 * This class means one point on X axis Y axis. 
 * X and Y takes the value of the Float. see also CvPoint
 * 
 * C structure is here, very simple.
 *   typdef struct CvPoint2D32f {
 *     float x;
 *     float y;
 *   }
 */
namespace mOpenCV {
  namespace cCvPoint2D32f {
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
     * Return compatibility to CvPoint2D32f. Return true if object have method #x and #y.
     *
     * For example.
     *   class MyPoint2D32f
     *     def x
     *       95.7
     *     end
     *     def y
     *       70.2
     *     end
     *   end
     *   mp = MyPoint2D32f.new
     *   CvPoint2D32f.compatible?(mp)  #=> true
     *   CvPoint2D32f.new(mp)          #=> same as CvPoint2D32f(95.7, 70.2)
     */
    VALUE
    rb_compatible_q(VALUE klass, VALUE object)
    {
      return (rb_respond_to(object, rb_intern("x")) && rb_respond_to(object, rb_intern("y"))) ? Qtrue : Qfalse;
    }

    VALUE
    rb_allocate(VALUE klass)
    {
      CvPoint2D32f *ptr;
      return Data_Make_Struct(klass, CvPoint2D32f, 0, -1, ptr);
    }

    /*
     * call-seq:
     *   new
     *   new(obj)
     *   new(x, y)
     *
     * Create new 2D-coordinate, (x, y).
     *
     * new() is same as new(0.0, 0.0)
     *
     * new(obj) is same as new(obj.x.to_f, obj.y.to_f)
     */
    VALUE
    rb_initialize(int argc, VALUE *argv, VALUE self)
    {
      CvPoint2D32f *self_ptr = CVPOINT2D32F(self);
      switch (argc) {
      case 0:
	break;
      case 1: {
	CvPoint2D32f point = VALUE_TO_CVPOINT2D32F(argv[0]);
	self_ptr->x = point.x;
	self_ptr->y = point.y;
	break;
      }
      case 2:
	self_ptr->x = NUM2DBL(argv[0]);
	self_ptr->y = NUM2DBL(argv[1]);
	break;
      default:
	rb_raise(rb_eArgError, "wrong number of arguments (%d for 0..2)", argc);
	break;
      }
      return self;
    }

    /*
     * Return parameter on x-axis.
     */
    VALUE
    rb_x(VALUE self)
    {
      return rb_float_new(CVPOINT2D32F(self)->x);
    }

    /*
     * call-seq:
     *   x = val
     *
     * Set x-axis parameter, return self.
     */
    VALUE
    rb_set_x(VALUE self, VALUE x)
    {
      CVPOINT2D32F(self)->x = NUM2DBL(x);
      return self;
    }

    /*
     * Return parameter on y-axis.
     */
    VALUE
    rb_y(VALUE self)
    {
      return rb_float_new(CVPOINT2D32F(self)->y);
    }

    /*
     * call-seq:
     *   y = val
     *
     * Set y-axis parameter, return self.
     */
    VALUE
    rb_set_y(VALUE self, VALUE y)
    {
      CVPOINT2D32F(self)->y = NUM2DBL(y);
      return self;
    }

    /*
     * call-seq:
     *   to_s -> "<OpenCV::CvSize2D32f:(self.x,self.y)>"
     *
     * Return x and y by String.
     */
    VALUE
    rb_to_s(VALUE self)
    {
      const int i = 4;
      VALUE str[i];
      str[0] = rb_str_new2("<%s:(%g,%g)>");
      str[1] = rb_str_new2(rb_class2name(CLASS_OF(self)));
      str[2] = rb_x(self);
      str[3] = rb_y(self);
      return rb_f_sprintf(i, str);
    }

    /*
     * call-seq:
     *   to_ary -> [x, y]
     *
     * Return x and y by Array.
     */
    VALUE
    rb_to_ary(VALUE self)
    {
      return rb_ary_new3(2, rb_x(self), rb_y(self));
    }

    VALUE
    new_object(CvPoint2D32f point)
    {
      VALUE object = rb_allocate(rb_klass);
      *CVPOINT2D32F(object) = point;
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
  
      rb_klass = rb_define_class_under(opencv, "CvPoint2D32f", rb_cObject);
      rb_define_alloc_func(rb_klass, rb_allocate);
      rb_define_singleton_method(rb_klass, "compatible?", RUBY_METHOD_FUNC(rb_compatible_q), 1);
      rb_define_method(rb_klass, "initialize", RUBY_METHOD_FUNC(rb_initialize), -1);
      rb_define_method(rb_klass, "x", RUBY_METHOD_FUNC(rb_x), 0);
      rb_define_method(rb_klass, "x=", RUBY_METHOD_FUNC(rb_set_x), 1);
      rb_define_method(rb_klass, "y", RUBY_METHOD_FUNC(rb_y), 0);
      rb_define_method(rb_klass, "y=", RUBY_METHOD_FUNC(rb_set_y), 1);

      rb_define_method(rb_klass, "to_s", RUBY_METHOD_FUNC(rb_to_s), 0);
      rb_define_method(rb_klass, "to_ary", RUBY_METHOD_FUNC(rb_to_ary), 0);
      rb_define_alias(rb_klass, "to_a", "to_ary");
    }
  }
}
