/************************************************************

   algorithm.h

   $Author: ser1zw $

   Copyright (C) 2013 ser1zw

************************************************************/
#ifndef RUBY_OPENCV_ALGORITHM_H
#define RUBY_OPENCV_ALGORITHM_H

#include "ruby.h"
#include "opencv2/core/core.hpp"

namespace mOpenCV {
  namespace cAlgorithm {
    VALUE rb_class();
    void init_ruby_class();
  }

  inline cv::Algorithm*
  ALGORITHM(VALUE object)
  {
    cv::Algorithm *ptr;
    Data_Get_Struct(object, cv::Algorithm, ptr);
    return ptr;
  }
}

#endif // RUBY_OPENCV_ALGORITHM_H
