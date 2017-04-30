/************************************************************

  cvchain.h -

  $Author: lsxi $

  Copyright (C) 2007 Masakazu Yonekura

************************************************************/
#ifndef RUBY_OPENCV_CVCHAIN_H
#define RUBY_OPENCV_CVCHAIN_H

namespace mOpenCV {
  namespace cCvChain {
    VALUE rb_class();
    void init_ruby_class();
    VALUE rb_allocate(VALUE klass);
    VALUE rb_initialize(int argc, VALUE *argv, VALUE self);
    VALUE rb_origin(VALUE self);
    VALUE rb_set_origin(VALUE self, VALUE origin);
    VALUE rb_codes(VALUE self);
    VALUE rb_points(VALUE self);
    VALUE rb_approx_chains(int argc, VALUE *argv, VALUE self);
    VALUE new_object();
  }
  inline CvChain*
    CVCHAIN(VALUE object){
    CvChain *ptr;
    Data_Get_Struct(object, CvChain, ptr);
    return ptr;
  }
}

#endif // RUBY_OPENCV_CVCHAIN_H
