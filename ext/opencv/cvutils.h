/************************************************************

    cvutils.h -

    $Author: ser1zw $

    Copyright (C) 2011 ser1zw

************************************************************/
#ifndef RUBY_OPENCV_CVUTILS_H
#define RUBY_OPENCV_CVUTILS_H

#include <ruby.h>
#include "opencv2/core/core_c.h"
#include "opencv2/core/core.hpp"
#include "cverror.h"

#define raise_cverror(e) cCvError::raise(e)

void raise_typeerror(VALUE object, VALUE expected_class);
void raise_typeerror(VALUE object, const char* expected_class_name);
void raise_compatible_typeerror(VALUE object, VALUE expected_class);
void raise_compatible_typeerror(VALUE object, const char* expected_class_name);
void* rb_cvAlloc(size_t size);
CvMat* rb_cvCreateMat(int height, int width, int type);
IplImage* rb_cvCreateImage(CvSize size, int depth, int channels);
IplConvKernel* rb_cvCreateStructuringElementEx(int cols, int rows, int anchorX, int anchorY, int shape, int *values);
CvMemStorage* rb_cvCreateMemStorage(int block_size);
VALUE rb_get_option_table(VALUE klass, const char* table_name, VALUE option);

void raise_opencv3_unsupported();
VALUE raise_opencv3_unsupported_n(VALUE, ...);
VALUE raise_opencv3_unsupported_1(int, VALUE*, VALUE);

// Ruby/OpenCV inline functions  
inline CvArr*
CVARR(VALUE object)
{
  CvArr *ptr;
  Data_Get_Struct(object, CvArr, ptr);
  return ptr;
}  

inline CvArr*
CVARR_WITH_CHECK(VALUE object)
{
  Check_Type(object, T_DATA);
  void *ptr = DATA_PTR(object);
  if (CV_IS_IMAGE(ptr) || CV_IS_MAT(ptr) || CV_IS_SEQ(ptr) ||
      CV_IS_MATND(ptr) || CV_IS_SPARSE_MAT(ptr)) {
    return CVARR(object);
  }
  else {
    raise_compatible_typeerror(object, (char*)"CvArr");
  }
  return NULL;
}  

#endif // RUBY_OPENCV_CVUTILS_H
