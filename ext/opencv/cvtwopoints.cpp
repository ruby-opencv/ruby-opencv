/************************************************************

   cvtwopoints.cpp -

   $Author: lsxi $

   Copyright (C) 2005-2006 Masakazu Yonekura

************************************************************/
#include "cvtwopoints.h"
#include "cvpoint.h"

/*
 * Document-class: OpenCV::CvTwoPoints
 *
 * This class means one twopoints on X axis Y axis. 
 * X and Y takes the value of the Integer. see also CvTwopoints2D32F
 * 
 * C structure is here, very simple.
 *   typdef struct CvTwopoints {
 *     int x;
 *     int y;
 *   }
 */
namespace mOpenCV {
  namespace cCvTwoPoints {
    VALUE rb_klass;

    VALUE
    rb_class()
    {
      return rb_klass;
    }

    VALUE
    rb_allocate(VALUE klass)
    {
      CvTwoPoints *ptr;
      return Data_Make_Struct(klass, CvTwoPoints, 0, -1, ptr);
    }

    /*
     * Return point 1.
     */
    VALUE
    rb_point1(VALUE self)
    {
      return cCvPoint::new_object(CVTWOPOINTS(self)->p1);
    }

    /*
     * Return point2.
     */
    VALUE
    rb_point2(VALUE self)
    {
      return cCvPoint::new_object(CVTWOPOINTS(self)->p2);
    }

    /*
     * call-seq:
     *   [<i>index</i>]
     *
     * Return value of <i>index</i> dimension.
     */
    VALUE
    rb_aref(VALUE self, VALUE index)
    {
      switch (NUM2INT(index)) {
      case 0:
	return cCvPoint::new_object(CVTWOPOINTS(self)->p1);
	break;
      case 1:
	return cCvPoint::new_object(CVTWOPOINTS(self)->p2);
	break;
      default:
	rb_raise(rb_eIndexError, "index should be 0...2");
	break;
      }
      return Qnil;
    }

    /*
     * call-seq:
     *   to_ary -> [self.point1, self.point2]
     *
     * Return 2 point by Array.
     */
    VALUE
    rb_to_ary(VALUE self)
    {
      return rb_ary_new3(2, rb_point1(self), rb_point2(self));
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
  
      rb_klass = rb_define_class_under(opencv, "CvTwoPoints", rb_cObject);
      rb_define_alloc_func(rb_klass, rb_allocate);
      rb_define_method(rb_klass, "point1", RUBY_METHOD_FUNC(rb_point1), 0);
      rb_define_method(rb_klass, "point2", RUBY_METHOD_FUNC(rb_point2), 0);
      rb_define_method(rb_klass, "[]", RUBY_METHOD_FUNC(rb_aref), 1);
      rb_define_method(rb_klass, "to_ary", RUBY_METHOD_FUNC(rb_to_ary), 0);
      rb_define_alias(rb_klass, "to_a", "to_ary");
    }
  }
}
