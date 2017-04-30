/************************************************************

  cvhaarclassifercascade.cpp -

  $Author: lsxi $

  Copyright (C) 2005-2007 Masakazu Yonekura

************************************************************/
#include "cvhaarclassifiercascade.h"
#include "cvseq.h"
#include "cvmemstorage.h"
#include "cvsize.h"
#include "cvavgcomp.h"

/*
 * Document-class: OpenCV::CvHaarClassifierCascade
 *
 * Haar Feature-based Cascade Classifier for Object Detection
 */
namespace mOpenCV {
  namespace cCvHaarClassifierCascade {
    VALUE rb_klass;

    VALUE
    rb_class()
    {
      return rb_klass;
    }

    VALUE
    rb_allocate(VALUE klass)
    {
      return OPENCV_OBJECT(klass, 0);
    }

    void
    cvhaarclassifiercascade_free(void* ptr)
    {
      if (ptr) {
	CvHaarClassifierCascade* cascade = (CvHaarClassifierCascade*)ptr;
	cvReleaseHaarClassifierCascade(&cascade);
      }
    }

    /*
     * Load trained cascade of haar classifers from file.
     *
     * @overload load(filename)
     *   @param filename [String] Haar classifer file name
     * @return [CvHaarClassifierCascade] Object detector
     * @scope class
     * @opencv_func cvLoad
     */
    VALUE
    rb_load(VALUE klass, VALUE path)
    {
      CvHaarClassifierCascade *cascade = NULL;
      try {
	cascade = (CvHaarClassifierCascade*)cvLoad(StringValueCStr(path), 0, 0, 0);
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      if (!CV_IS_HAAR_CLASSIFIER(cascade))
	rb_raise(rb_eArgError, "invalid format haar classifier cascade file.");
      return Data_Wrap_Struct(klass, 0, cvhaarclassifiercascade_free, cascade);
    }

    /*
     * Detects objects of different sizes in the input image.
     *
     * @overload detect_objects(image, options = nil)
     *   @param image [CvMat,IplImage] Matrix of the type CV_8U containing an image where objects are detected.
     *   @param options [Hash] Options
     *   @option options [Number] :scale_factor
     *     Parameter specifying how much the image size is reduced at each image scale.
     *   @option options [Number] :storage
     *      Memory storage to store the resultant sequence of the object candidate rectangles
     *   @option options [Number] :min_neighbors
     *      Parameter specifying how many neighbors each candidate rectangle should have to retain it.
     *   @option options [CvSize] :min_size
     *      Minimum possible object size. Objects smaller than that are ignored.
     *   @option options [CvSize] :max_size
     *      Maximum possible object size. Objects larger than that are ignored.
     * @return [CvSeq<CvAvgComp>] Detected objects as a list of rectangles
     * @opencv_func cvHaarDetectObjects
     */
    VALUE
    rb_detect_objects(int argc, VALUE *argv, VALUE self)
    { 
      VALUE image, options;
      rb_scan_args(argc, argv, "11", &image, &options);

      double scale_factor;
      int flags, min_neighbors;
      CvSize min_size, max_size;
      VALUE storage_val;
      if (NIL_P(options)) {
	scale_factor = 1.1;
	flags = 0;
	min_neighbors = 3;
	min_size = max_size = cvSize(0, 0);
	storage_val = cCvMemStorage::new_object();
      }
      else {
	scale_factor = IF_DBL(LOOKUP_HASH(options, "scale_factor"), 1.1);
	flags = IF_INT(LOOKUP_HASH(options, "flags"), 0);
	min_neighbors = IF_INT(LOOKUP_HASH(options, "min_neighbors"), 3);
	VALUE min_size_val = LOOKUP_HASH(options, "min_size");
	min_size = NIL_P(min_size_val) ? cvSize(0, 0) : VALUE_TO_CVSIZE(min_size_val);
	VALUE max_size_val = LOOKUP_HASH(options, "max_size");
	max_size = NIL_P(max_size_val) ? cvSize(0, 0) : VALUE_TO_CVSIZE(max_size_val);
	storage_val = CHECK_CVMEMSTORAGE(LOOKUP_HASH(options, "storage"));
      }

      VALUE result = Qnil;
      try {
	CvSeq *seq = cvHaarDetectObjects(CVARR_WITH_CHECK(image), CVHAARCLASSIFIERCASCADE(self), CVMEMSTORAGE(storage_val),
					 scale_factor, min_neighbors, flags, min_size, max_size);
	result = cCvSeq::new_sequence(cCvSeq::rb_class(), seq, cCvAvgComp::rb_class(), storage_val);
	if (rb_block_given_p()) {
	  for(int i = 0; i < seq->total; ++i)
	    rb_yield(REFER_OBJECT(cCvAvgComp::rb_class(), cvGetSeqElem(seq, i), storage_val));
	}
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return result;
    }

    void
    init_ruby_class()
    {
#if 0
      // For documentation using YARD
      VALUE opencv = rb_define_module("OpenCV");
#endif

      if (rb_klass)
	return;

      VALUE opencv = rb_module_opencv();
      rb_klass = rb_define_class_under(opencv, "CvHaarClassifierCascade", rb_cObject);
      rb_define_alloc_func(rb_klass, rb_allocate);
      rb_define_singleton_method(rb_klass, "load", RUBY_METHOD_FUNC(rb_load), 1);
      rb_define_method(rb_klass, "detect_objects", RUBY_METHOD_FUNC(rb_detect_objects), -1);
    }
  }
}
