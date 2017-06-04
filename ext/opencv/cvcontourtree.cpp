/************************************************************

   cvcontourtree.cpp -

   $Author: lsxi $

   Copyright (C) 2007 Masakazu Yonekura

************************************************************/
#include "cvcontourtree.h"
#include "cvcontour.h"
#include "cvpoint.h"
#include "cvseq.h"
#include "cvmemstorage.h"
#include "cvtermcriteria.h"
#include "opencv.h"
/*
 * Document-class: OpenCV::CvContourTree
 *
 * Contour tree
 *
 * @see CvContour#create_tree
 */
namespace mOpenCV {
  namespace cCvContourTree {
    VALUE rb_klass;

    VALUE
    rb_class()
    {
      return rb_klass;
    }

#ifdef IS_OPENCV2
    /*
     * Returns the first point of the binary tree root segment
     * @overload p1
     * @return [CvPoint] First point of the binary tree root segment
     */
    VALUE
    rb_p1(VALUE self)
    {
      return REFER_OBJECT(cCvPoint::rb_class(), &CVCONTOURTREE(self)->p1, self);
    }

    /*
     * Returns the last point of the binary tree root segment
     * @overload p2
     * @return [CvPoint] Last point of the binary tree root segment
     */
    VALUE
    rb_p2(VALUE self)
    {
      return REFER_OBJECT(cCvPoint::rb_class(), &CVCONTOURTREE(self)->p2, self);
    }

    /*
     * Restores the contour from its binary tree representation.
     *
     * The parameter +criteria+ determines the accuracy and/or the number of tree levels
     * used for reconstruction, so it is possible to build approximated contour.
     * @overload contour(criteria = 0)
     *   @param criteria [Integer] Criteria, where to stop reconstruction
     * @return [CvContour] Contour tree
     * @opencv_func cvContourFromContourTree
     */
    VALUE
    rb_contour(VALUE self, VALUE criteria)
    {
      VALUE storage = cCvMemStorage::new_object();
      CvSeq *contour = NULL;
      try {
	contour = cvContourFromContourTree(CVCONTOURTREE(self), CVMEMSTORAGE(storage),
					   VALUE_TO_CVTERMCRITERIA(criteria));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return cCvSeq::new_sequence(cCvContour::rb_class(), contour, cCvPoint::rb_class(), storage);
    }
#else
#  define rb_p1 raise_opencv3_unsupported_n
#  define rb_p2 raise_opencv3_unsupported_n
#  define rb_contour raise_opencv3_unsupported_n
#endif

    void
    init_ruby_class()
    {
#if 0
      // For documentation using YARD
      VALUE opencv = rb_define_module("OpenCV");
      VALUE cvseq = rb_define_class_under(opencv, "CvSeq");
#endif
      if (rb_klass)
	return;
      VALUE opencv = rb_module_opencv();
      VALUE cvseq = cCvSeq::rb_class();

      rb_klass = rb_define_class_under(opencv, "CvContourTree", cvseq);
      rb_define_method(rb_klass, "p1", RUBY_METHOD_FUNC(rb_p1), 0);
      rb_define_method(rb_klass, "p2", RUBY_METHOD_FUNC(rb_p2), 0);
      rb_define_method(rb_klass, "contour", RUBY_METHOD_FUNC(rb_contour), 1);
    }
  }
}
