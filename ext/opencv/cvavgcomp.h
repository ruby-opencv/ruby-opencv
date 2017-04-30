/**********************************************************************

   cvavgcomp.h

   $Author: lsxi $

   Copyright (C) 2005-2006 Masakazu Yonekura

**********************************************************************/
#ifndef RUBY_OPENCV_AVGCOMP_H
#define RUBY_OPENCV_AVGCOMP_H

#include "opencv2/objdetect/objdetect_c.h"

namespace mOpenCV {
  namespace cCvAvgComp {
    VALUE rb_class();
    void init_ruby_class();
    VALUE rb_allocate(VALUE klass);
    VALUE rb_neighbors(VALUE self);
  }

  inline CvAvgComp *CVAVGCOMP(VALUE object){
    CvAvgComp *ptr;
    Data_Get_Struct(object, CvAvgComp, ptr);
    return ptr;
  }
}

#endif // RUBY_OPENCV_AVGCOMP_H
