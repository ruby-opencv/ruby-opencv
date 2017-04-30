/************************************************************

   cvpoint3d32f.cpp -

   $Author: lsxi $

   Copyright (C) 2005-2008 Masakazu Yonekura

************************************************************/
#include "cvpoint3d32f.h"
#include "opencv.h"
/*
 * Document-class: OpenCV::CvPoint3D32f
 *
 * This class means one point on X axis Y axis. 
 * X and Y takes the value of the Float. see also CvPoint
 * 
 * C structure is here, very simple.
 *   typdef struct CvPoint3D32f {
 *     float x;
 *     float y;
 *     float z;
 *   }
 */
namespace mOpenCV {
  namespace cCvPoint3D32f {
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
     * Return compatibility to CvPoint3D32f. Return true if object have method #x and #y and #z.
     *
     * For example.
     *   class MyPoint3D32f
     *     def x
     *       95.7
     *     end
     *     def y
     *       70.2
     *     end
     *     def z
     *       10.0
     *     end
     *   end
     *   mp = MyPoint3D32f.new
     *   CvPoint3D32f.compatible?(mp)  #=> true
     *   CvPoint3D32f.new(mp)          #=> same as CvPoint3D32f(95.7, 70.2)
     */
    VALUE
    rb_compatible_q(VALUE klass, VALUE object)
    {
      return (rb_respond_to(object, rb_intern("x")) &&
	      rb_respond_to(object, rb_intern("y")) &&
	      rb_respond_to(object, rb_intern("z"))) ? Qtrue : Qfalse;
    }

    VALUE
    rb_allocate(VALUE klass)
    {
      CvPoint3D32f *ptr;
      return Data_Make_Struct(klass, CvPoint3D32f, 0, -1, ptr);
    }

    /*
     * call-seq:
     *   new
     *   new(obj)
     *   new(x, y, z)
     *
     * Create new 3D-coordinate, (x, y, z).
     *
     * new() is same as new(0.0, 0.0, 0.0)
     *
     * new(obj) is same as new(obj.x.to_f, obj.y.to_f, obj.z.to_f)
     */
    VALUE
    rb_initialize(int argc, VALUE *argv, VALUE self)
    {
      CvPoint3D32f *self_ptr = CVPOINT3D32F(self);
      switch (argc) {
      case 0:
	break;
      case 1: {
	CvPoint3D32f point = VALUE_TO_CVPOINT3D32F(argv[0]);
	self_ptr->x = point.x;
	self_ptr->y = point.y;
	self_ptr->z = point.z;
	break;
      }
      case 3:
	self_ptr->x = NUM2DBL(argv[0]);
	self_ptr->y = NUM2DBL(argv[1]);
	self_ptr->z = NUM2DBL(argv[2]);
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
      return rb_float_new(CVPOINT3D32F(self)->x);
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
      CVPOINT3D32F(self)->x = NUM2DBL(x);
      return self;
    }

    /*
     * Return parameter on y-axis.
     */
    VALUE
    rb_y(VALUE self)
    {
      return rb_float_new(CVPOINT3D32F(self)->y);
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
      CVPOINT3D32F(self)->y = NUM2DBL(y);
      return self;
    }

    /*
     * Return parameter on z-axis.
     */
    VALUE
    rb_z(VALUE self)
    {
      return rb_float_new(CVPOINT3D32F(self)->z);
    }

    /*
     * call-seq:
     *   z = val
     *
     * Set z-axis parameter, return self.
     */
    VALUE
    rb_set_z(VALUE self, VALUE z)
    {
      CVPOINT3D32F(self)->z = NUM2DBL(z);
      return self;
    }

    /*
     * call-seq:
     *   to_s -> "<OpenCV::CvSize3D32f:(self.x,self.y,self.z)>"
     *
     * Return x and y by String.
     */
    VALUE
    rb_to_s(VALUE self)
    {
      const int i = 5;
      VALUE str[i];
      str[0] = rb_str_new2("<%s:(%g,%g,%g)>");
      str[1] = rb_str_new2(rb_class2name(CLASS_OF(self)));
      str[2] = rb_x(self);
      str[3] = rb_y(self);
      str[4] = rb_z(self);
      return rb_f_sprintf(i, str);
    }

    /*
     * call-seq:
     *   to_ary -> [x, y, z]
     *
     * Return x and y by Array.
     */
    VALUE
    rb_to_ary(VALUE self)
    {
      return rb_ary_new3(3, rb_x(self), rb_y(self), rb_z(self));
    }

    VALUE
    new_object(CvPoint3D32f point)
    {
      VALUE object = rb_allocate(rb_klass);
      *CVPOINT3D32F(object) = point;
      return object;
    }

    void
    init_ruby_class()
    {
#if 0
      // For documentation using YARD
      VALUE opencv = rb_define_module("OpenCV");
      VALUE cvpoint3d32f = rb_define_class_under(opencv, "CvPoint3D32f", rb_cObject);
#endif

      if (rb_klass)
	return;
      /* 
       * opencv = rb_define_module("OpenCV");
       * cvpoint3d32f = rb_define_class_under(opencv, "CvPoint3D32f", rb_cObject);
       *
       * note: this comment is used by rdoc.
       */
      VALUE opencv = rb_module_opencv();
      rb_klass = rb_define_class_under(opencv, "CvPoint3D32f", rb_cObject);
      rb_define_alloc_func(rb_klass, rb_allocate);
      rb_define_singleton_method(rb_klass, "compatible?", RUBY_METHOD_FUNC(rb_compatible_q), 1);
      rb_define_method(rb_klass, "initialize", RUBY_METHOD_FUNC(rb_initialize), -1);
      rb_define_method(rb_klass, "x", RUBY_METHOD_FUNC(rb_x), 0);
      rb_define_method(rb_klass, "x=", RUBY_METHOD_FUNC(rb_set_x), 1);
      rb_define_method(rb_klass, "y", RUBY_METHOD_FUNC(rb_y), 0);
      rb_define_method(rb_klass, "y=", RUBY_METHOD_FUNC(rb_set_y), 1);
      rb_define_method(rb_klass, "z", RUBY_METHOD_FUNC(rb_z), 0);
      rb_define_method(rb_klass, "z=", RUBY_METHOD_FUNC(rb_set_z), 1);

      rb_define_method(rb_klass, "to_s", RUBY_METHOD_FUNC(rb_to_s), 0);
      rb_define_method(rb_klass, "to_ary", RUBY_METHOD_FUNC(rb_to_ary), 0);
      rb_define_alias(rb_klass, "to_a", "to_ary");
    }
  }
}
