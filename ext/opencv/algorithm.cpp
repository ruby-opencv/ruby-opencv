/************************************************************

   algorithm.cpp -

   $Author: ser1zw $

   Copyright (C) 2013 ser1zw

************************************************************/
#include <stdio.h>
#include <ruby.h>
#include "cvutils.h"
#include "cverror.h"
#include "cvmat.h"
#include "algorithm.h"

/*
 * Document-class: OpenCV::Algorithm
 *
 */
__NAMESPACE_BEGIN_OPENCV
__NAMESPACE_BEGIN_ALGORITHM

VALUE rb_klass;

VALUE
rb_class()
{
  return rb_klass;
}

VALUE
rb_name(VALUE self)
{
  VALUE name = Qnil;
  try {
    name = rb_str_new_cstr(ALGORITHM(self)->getDefaultName().c_str());
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }
  return name;
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
  rb_klass = rb_define_class_under(opencv, "Algorithm", rb_cObject);
  rb_define_method(rb_klass, "name", RUBY_METHOD_FUNC(rb_name), 0);
}

__NAMESPACE_END_ALGORITM
__NAMESPACE_END_OPENCV

