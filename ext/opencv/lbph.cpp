/************************************************************

   lbph.cpp -

   $Author: ser1zw $

   Copyright (C) 2013 ser1zw

************************************************************/
#include "ruby.h"
#if IS_OPENCV2
#  include "opencv2/contrib/contrib.hpp"
#endif
#include "facerecognizer.h"
#include "opencv.h"

/*
 * Document-class: OpenCV::LBPH
 *
 */
namespace mOpenCV {
  namespace cLBPH {

    VALUE rb_klass;

    VALUE
    rb_class()
    {
      return rb_klass;
    }

#if IS_OPENCV2
    /* 
     * call-seq:
     *   LBPH.new(radius=1, neighbors=8, grid_x=8, grid_y=8, threshold=DBL_MAX) -> cvmat
     */
    VALUE
    rb_initialize(int argc, VALUE argv[], VALUE self)
    {
      VALUE radius_val, neighbors_val, grid_x_val, grid_y_val, threshold_val;
      rb_scan_args(argc, argv, "05", &radius_val, &neighbors_val, &grid_x_val, &grid_y_val, &threshold_val);

      int radius = NIL_P(radius_val) ? 1 : NUM2INT(radius_val);
      int neighbors = NIL_P(neighbors_val) ? 8 : NUM2INT(neighbors_val);
      int grid_x = NIL_P(grid_x_val) ? 8 : NUM2INT(grid_x_val);
      int grid_y = NIL_P(grid_y_val) ? 8 : NUM2INT(grid_y_val);
      double threshold = NIL_P(threshold_val) ? DBL_MAX : NUM2INT(threshold_val);

      free(DATA_PTR(self));
      cv::Ptr<cv::FaceRecognizer> ptr = cv::createLBPHFaceRecognizer(radius, neighbors, grid_x, grid_y, threshold);
      DATA_PTR(self) = ptr;

      cFaceRecognizer::guard_facerecognizer(DATA_PTR(self), ptr);

      return self;
    }
#else
#  define rb_initialize raise_opencv3_unsupported_1
#endif

    void
    init_ruby_class()
    {
#if 0
      // For documentation using YARD
      VALUE opencv = rb_define_module("OpenCV");
      VALUE alghorithm = rb_define_class_under(opencv, "Algorithm", rb_cObject);
      VALUE face_recognizer = rb_define_class_under(opencv, "FaceRecognizer", alghorithm);
#endif

      if (rb_klass)
	return;
      /* 
       * opencv = rb_define_module("OpenCV");
       * 
       * note: this comment is used by rdoc.
       */
      VALUE opencv = rb_module_opencv();
      VALUE face_recognizer = cFaceRecognizer::rb_class();

      rb_klass = rb_define_class_under(opencv, "LBPH", face_recognizer);
      rb_define_alloc_func(rb_klass, cFaceRecognizer::allocate_facerecognizer);
      rb_define_private_method(rb_klass, "initialize", RUBY_METHOD_FUNC(rb_initialize), -1);
    }
  }
}
