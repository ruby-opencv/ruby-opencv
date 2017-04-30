/************************************************************

   algorithm.h

   $Author: ser1zw $

   Copyright (C) 2013 ser1zw

************************************************************/
#include "opencv.h"

#ifndef RUBY_OPENCV_ALGORITHM_H
#define RUBY_OPENCV_ALGORITHM_H

#include <ruby.h>

#define __NAMESPACE_BEGIN_ALGORITHM namespace cAlgorithm {
#define __NAMESPACE_END_ALGORITM }

__NAMESPACE_BEGIN_OPENCV
__NAMESPACE_BEGIN_ALGORITHM

VALUE rb_class();

void init_ruby_class();

__NAMESPACE_END_ALGORITM

inline cv::Algorithm*
ALGORITHM(VALUE object)
{
  cv::Algorithm *ptr;
  Data_Get_Struct(object, cv::Algorithm, ptr);
  return ptr;
}

__NAMESPACE_END_OPENCV

#endif // RUBY_OPENCV_ALGORITHM_H

