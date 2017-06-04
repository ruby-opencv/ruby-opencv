/************************************************************

  cvcontourtree.h -

  $Author: lsxi $

  Copyright (C) 2007 Masakazu Yonekura

************************************************************/
#ifndef RUBY_OPENCV_CVCONTOURTREE_H
#define RUBY_OPENCV_CVCONTOURTREE_H

#include "ruby.h"
#ifdef IS_OPENCV2
#  include "opencv2/legacy/legacy.hpp"
#endif

namespace mOpenCV {
  namespace cCvContourTree {
    VALUE rb_class();
    void init_ruby_class();
    VALUE new_object();
  }

#ifdef IS_OPENCV2
  inline CvContourTree*
    CVCONTOURTREE(VALUE object){
    CvContourTree *ptr;
    Data_Get_Struct(object, CvContourTree, ptr);
    return ptr;
  }
#endif
}
#endif
