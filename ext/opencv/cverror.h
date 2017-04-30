/************************************************************

   cverror.h -

   $Author: lsxi $

   Copyright (C) 2005-2006 Masakazu Yonekura

************************************************************/
#ifndef RUBY_OPENCV_CVERROR_H
#define RUBY_OPENCV_CVERROR_H

#include <ruby.h>
#include "opencv2/core/core.hpp"

namespace mOpenCV {
  namespace cCvError {
    void init_ruby_class();
    VALUE by_code(int error_code);
    void raise(cv::Exception e);
  }
}

#endif
