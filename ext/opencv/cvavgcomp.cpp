/************************************************************

   cvavgcomp.cpp -

   $Author: lsxi $

   Copyright (C) 2005-2006 Masakazu Yonekura

************************************************************/
#include <ruby.h>
#include "cvavgcomp.h"
#include "cvrect.h"
#include "opencv.h"

/*
 * Document-class: OpenCV::CvAvgComp
 *
 * CvRect with parameter "neighbors"
 */
namespace mOpenCV {
  namespace cCvAvgComp {

    VALUE rb_klass;

    VALUE
    rb_class()
    {
      return rb_klass;
    }

    VALUE
    rb_allocate(VALUE klass)
    {
      CvAvgComp *ptr;
      return Data_Make_Struct(klass, CvAvgComp, 0, -1, ptr);
    }

    /*
     * Return neighbors
     * @overload neighbors
     * @return [Integer] neighbors
     */
    VALUE
    rb_neighbors(VALUE self)
    {
      return INT2NUM(CVAVGCOMP(self)->neighbors);
    }

    void
    init_ruby_class()
    {
#if 0
      // For documentation using YARD
      VALUE opencv = rb_define_module("OpenCV");
      VALUE cvrect = rb_define_class_under(opencv, "CvRect", rb_cObject);
#endif

      if (rb_klass)
	return;

      VALUE opencv = rb_module_opencv();
      VALUE cvrect = cCvRect::rb_class();
      rb_klass = rb_define_class_under(opencv, "CvAvgComp", cvrect);
      rb_define_alloc_func(rb_klass, rb_allocate);
      rb_define_method(rb_klass, "neighbors", RUBY_METHOD_FUNC(rb_neighbors), 0);
    }
  }
}
