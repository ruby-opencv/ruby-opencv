/************************************************************

   facerecognizer.h

   $Author: ser1zw $

   Copyright (C) 2013 ser1zw

************************************************************/
#ifndef RUBY_OPENCV_FACERECOGNIZER_H
#define RUBY_OPENCV_FACERECOGNIZER_H

#include "ruby.h"
#include "opencv.h"
#include "opencv2/core/core.hpp"
#if IS_OPENCV2
#  include "opencv2/contrib/contrib.hpp"
#else
// Dummy class
namespace cv {
  class FaceRecognizer { };
}
#endif

namespace mOpenCV {
  namespace cFaceRecognizer {
    VALUE rb_class();
    void init_ruby_class();
    void guard_facerecognizer(void* data_ptr, cv::Ptr<cv::FaceRecognizer> ptr);
    VALUE allocate_facerecognizer(VALUE klass);
  }

  inline cv::FaceRecognizer*
  FACERECOGNIZER(VALUE object)
  {
    cv::FaceRecognizer *ptr;
    Data_Get_Struct(object, cv::FaceRecognizer, ptr);
    return ptr;
  }
}
#endif // RUBY_OPENCV_FACERECOGNIZER_H

