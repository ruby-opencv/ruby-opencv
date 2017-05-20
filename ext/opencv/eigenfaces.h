/************************************************************

   eigenfaces.h

   $Author: ser1zw $

   Copyright (C) 2013 ser1zw

************************************************************/
#ifndef RUBY_OPENCV_EIGENFACES_H
#define RUBY_OPENCV_EIGENFACES_H

#include "ruby.h"

namespace mOpenCV {
  namespace cEigenFaces {
    VALUE rb_class();
    void init_ruby_class();
  }
}
#endif // RUBY_OPENCV_EIGENFACES_H
