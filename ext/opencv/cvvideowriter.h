/************************************************************

   cvvideowriter.h -

   $Author: lsxi $

   Copyright (C) 2005-2006 Masakazu Yonekura

************************************************************/
#ifndef RUBY_OPENCV_CVVIDEOWRITER_H
#define RUBY_OPENCV_CVVIDEOWRITER_H

#include <ruby.h>
#include "opencv2/videoio/videoio_c.h"

namespace mOpenCV {
  namespace cCvVideoWriter {
    VALUE rb_class();
    void init_ruby_class();
    VALUE rb_initialize(int argc, VALUE *argv, VALUE self);
    VALUE rb_write(VALUE self, VALUE frame);
    VALUE rb_close(VALUE self);
  }
 
  inline CvVideoWriter*
    CVVIDEOWRITER(VALUE object)
  {
    return (CvVideoWriter*)DATA_PTR(object);
  }
}
#endif // RUBY_OPENCV_CVVIDEOWRITER_H
