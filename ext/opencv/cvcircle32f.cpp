/************************************************************

   cvcircle32f.cpp -

   $Author: lsxi $

   Copyright (C) 2005-2006 Masakazu Yonekura

************************************************************/
#include "cvcircle32f.h"
/*
 * Document-class: OpenCV::CvCircle32f
 *
 * Combination of center and radius.
 *
 * see CvMat#hough_circles
 */
__NAMESPACE_BEGIN_OPENCV
__NAMESPACE_BEGIN_CVCIRCLE32F

VALUE rb_klass;

VALUE
rb_class()
{
  return rb_klass;
}

void
define_ruby_class()
{
  if (rb_klass)
    return;
  /* 
   * opencv = rb_define_module("OpenCV");
   * 
   * note: this comment is used by rdoc.
   */
  VALUE opencv = rb_module_opencv();
  
  rb_klass = rb_define_class_under(opencv, "CvCircle32f", rb_cObject);
  rb_define_alloc_func(rb_klass, rb_allocate);
  rb_define_private_method(rb_klass, "initialize", RUBY_METHOD_FUNC(rb_initialize), -1);
  rb_define_method(rb_klass, "center", RUBY_METHOD_FUNC(rb_center), 0);
  rb_define_method(rb_klass, "radius", RUBY_METHOD_FUNC(rb_radius), 0);
  rb_define_method(rb_klass, "[]", RUBY_METHOD_FUNC(rb_aref), 1);
  rb_define_method(rb_klass, "to_ary", RUBY_METHOD_FUNC(rb_to_ary), 0);
  rb_define_alias(rb_klass, "to_a", "to_ary");
}

VALUE
rb_allocate(VALUE klass)
{
  CvCircle32f *ptr;
  return Data_Make_Struct(klass, CvCircle32f, 0, -1, ptr);
}

VALUE
rb_initialize(int argc, VALUE *argv, VALUE self)
{
  CvCircle32f* self_ptr = CVCIRCLE32F(self);
  switch (argc) {
  case 0:
    break;
  case 2: {
    CvPoint2D32f point = VALUE_TO_CVPOINT2D32F(argv[0]);
    self_ptr->center = point;
    self_ptr->radius = NUM2INT(argv[1]);
    break;
  }
  case 3:
    self_ptr->center = cvPoint2D32f(NUM2DBL(argv[0]), NUM2DBL(argv[1]));
    self_ptr->radius = NUM2INT(argv[2]);
    break;
  default:
    rb_raise(rb_eArgError, "wrong number of arguments (%d for 2..3)", argc);
    break;
  }
  return self;
}

/*
 * Return parameter on center.
 */
VALUE
rb_center(VALUE self)
{
  return cCvPoint2D32f::new_object(CVCIRCLE32F(self)->center);
}

/*
 * Return parameter on radius.
 */
VALUE
rb_radius(VALUE self)
{
  return rb_float_new(CVCIRCLE32F(self)->radius);
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

__NAMESPACE_END_CVCIRCLE32F
__NAMESPACE_END_OPENCV
