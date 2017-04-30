/************************************************************

   cvchain.cpp -

   $Author: lsxi $

   Copyright (C) 2007 Masakazu Yonekura

************************************************************/
#include <ruby.h>
#include "opencv2/core/core.hpp"
#include "cvchain.h"
#include "cvpoint.h"
#include "curve.h"
#include "cvseq.h"
#include "cvmemstorage.h"
/*
 * Document-class: OpenCV::CvChain
 *
 * Freeman chain code
 */
namespace mOpenCV {
  namespace cCvChain {
#define APPROX_CHAIN_OPTION(op) rb_get_option_table(rb_klass, "APPROX_CHAIN_OPTION", op)
#define APPROX_CHAIN_METHOD(op) CVMETHOD("APPROX_CHAIN_METHOD", LOOKUP_HASH(op, "method"), CV_CHAIN_APPROX_SIMPLE)
#define APPROX_CHAIN_PARAMETER(op) NUM2INT(LOOKUP_HASH(op, "parameter"))
#define APPROX_CHAIN_MINIMAL_PERIMETER(op) NUM2INT(LOOKUP_HASH(op, "minimal_perimeter"))
#define APPROX_CHAIN_RECURSIVE(op) TRUE_OR_FALSE(LOOKUP_HASH(op, "recursive"))

    VALUE rb_klass;

    VALUE
    rb_class()
    {
      return rb_klass;
    }

    VALUE
    rb_allocate(VALUE klass)
    {
      return Data_Wrap_Struct(klass, mark_root_object, unregister_object, NULL);
    }

    /*
     * Create a new chain code
     * @overload new(storage=nil)
     *   @param storage [CvMemStorage,nil] Sequence location (If storage is nil, allocates a new storage automatically)
     * @return [CvChain] New CvChain instance
     * @opencv_func cvCreateSeq (seq_flags=CV_SEQ_ELTYPE_CODE)
     */
    VALUE
    rb_initialize(int argc, VALUE *argv, VALUE self)
    {
      CvMemStorage *storage;
      VALUE storage_value;
      if (rb_scan_args(argc, argv, "01", &storage_value) > 0) {
	storage_value = CHECK_CVMEMSTORAGE(storage_value);
	storage = CVMEMSTORAGE(storage_value);
      }
      else
	storage = rb_cvCreateMemStorage(0);
      try {
	DATA_PTR(self) = (CvChain*)cvCreateSeq(CV_SEQ_ELTYPE_CODE, sizeof(CvChain),
					       sizeof(int), storage);
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      CvSeq* self_ptr = CVSEQ(self);
      cCvSeq::register_elem_class(self_ptr, rb_cInteger);
      register_root_object(self_ptr, storage_value);

      return self;
    }

    /*
     * Returns Freeman chain code origin
     * @overload origin
     * @return [CvPoint] Origin of the chain code
     */
    VALUE
    rb_origin(VALUE self)
    {
      return cCvPoint::new_object(CVCHAIN(self)->origin);
    }

    /*
     * Set Freeman chain code origin
     * @overload origin=value
     *   @param value [CvPoint] Origin of the chain code
     * @return [CvChain] self
     */
    VALUE
    rb_set_origin(VALUE self, VALUE origin)
    {
      CVCHAIN(self)->origin = VALUE_TO_CVPOINT(origin);  
      return self;
    }

    /*
     * Returns the chain codes
     * @overload codes
     * @return [Array<Integer>] Chain codes
     * @opencv_func cvStartReadChainPoints
     * @opencv_func CV_READ_SEQ_ELEM
     */
    VALUE
    rb_codes(VALUE self)
    {
      CvChain *chain = CVCHAIN(self);
      CvChainPtReader reader;
      int total = chain->total;
      VALUE ary = rb_ary_new2(total);
      try {
	cvStartReadChainPoints(chain, &reader);
	for (int i = 0; i < total; ++i) {
	  CV_READ_SEQ_ELEM(reader.code, (*((CvSeqReader*)&(reader))));
	  rb_ary_store(ary, i, INT2FIX(reader.code));
	}
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return ary;
    }

    /*
     * Returns the points of the chain codes
     * @overload points
     * @return [Array<CvPoint>] Points of the chain codes
     * @opencv_func cvStartReadChainPoints
     * @opencv_func CV_READ_CHAIN_POINT
     */
    VALUE
    rb_points(VALUE self)
    {
      CvChain *chain = CVCHAIN(self);
      CvChainPtReader reader;
      CvPoint p = chain->origin;
      int total = chain->total;
      VALUE ary = rb_ary_new2(total);
      try {
	cvStartReadChainPoints(chain, &reader);
	for (int i = 0; i < total; ++i) {
	  CV_READ_CHAIN_POINT(p, reader);
	  rb_ary_store(ary, i, cCvPoint::new_object(p));
	}
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return ary;
    }

    /*
     * Approximates Freeman chains with a polygonal curve
     * @overload approx_chain(options)
     *   @param options [Hash] Parameters
     *   @option options [Symbol] :method Approximation method (see the description of CvMat#find_contours)
     *   @option options [Number] :minimal_perimeter Approximates only those contours whose perimeters
     *     are not less than minimal_perimeter. Other chains are removed from the resulting structure.
     *   @option options [Boolean] :recursive Recursion flag. If it is true, the function approximates
     *     all chains that can be obtained from chain by using the h_next or v_next links.
     *     Otherwise, the single input chain is approximated.
     * @return [CvSeq<CvPoint>] Polygonal curve
     * @opencv_func cvApproxChains
     */
    VALUE
    rb_approx_chains(int argc, VALUE *argv, VALUE self)
    {
      VALUE approx_chain_option;
      rb_scan_args(argc, argv, "01", &approx_chain_option);

      approx_chain_option = APPROX_CHAIN_OPTION(approx_chain_option);
      VALUE storage = cCvMemStorage::new_object();
      CvSeq *seq = cvApproxChains(CVSEQ(self), CVMEMSTORAGE(storage),			      
				  APPROX_CHAIN_METHOD(approx_chain_option),
				  APPROX_CHAIN_PARAMETER(approx_chain_option),
				  APPROX_CHAIN_MINIMAL_PERIMETER(approx_chain_option),
				  APPROX_CHAIN_RECURSIVE(approx_chain_option));

      if (seq && seq->total > 0) {
	return cCvSeq::new_sequence(cCvChain::rb_class(), seq, cCvPoint::rb_class(), storage);
      }
      return Qnil;
    }

    VALUE
    new_object()
    {
      VALUE storage = cCvMemStorage::new_object();
      CvSeq *seq = NULL;
      try {
	seq = cvCreateSeq(CV_SEQ_CHAIN_CONTOUR, sizeof(CvChain), sizeof(int), CVMEMSTORAGE(storage));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return cCvSeq::new_sequence(cCvChain::rb_class(), seq, T_FIXNUM, storage);
    }

    void
    init_ruby_class()
    {
#if 0
      // For documentation using YARD
      VALUE opencv = rb_define_module("OpenCV");
      VALUE cvseq = rb_define_class_under(opencv, "CvSeq");
      VALUE curve = rb_define_module_under(opencv, "Curve");
#endif

      if (rb_klass)
	return;

      VALUE opencv = rb_module_opencv();
      VALUE cvseq = cCvSeq::rb_class();
      VALUE curve = mCurve::rb_module();

      rb_klass = rb_define_class_under(opencv, "CvChain", cvseq);
      rb_include_module(rb_klass, curve);
      VALUE approx_chain_option = rb_hash_new();
      rb_define_const(rb_klass, "APPROX_CHAIN_OPTION", approx_chain_option); 
      rb_hash_aset(approx_chain_option, ID2SYM(rb_intern("method")), ID2SYM(rb_intern("approx_simple")));
      rb_hash_aset(approx_chain_option, ID2SYM(rb_intern("parameter")), rb_float_new(0));
      rb_hash_aset(approx_chain_option, ID2SYM(rb_intern("minimal_perimeter")), INT2FIX(0));
      rb_hash_aset(approx_chain_option, ID2SYM(rb_intern("recursive")), Qfalse);

      rb_define_method(rb_klass, "initialize", RUBY_METHOD_FUNC(rb_initialize), -1);
      rb_define_method(rb_klass, "origin", RUBY_METHOD_FUNC(rb_origin), 0);
      rb_define_method(rb_klass, "origin=", RUBY_METHOD_FUNC(rb_set_origin), 1);
      rb_define_method(rb_klass, "codes", RUBY_METHOD_FUNC(rb_codes), 0);
      rb_define_method(rb_klass, "points", RUBY_METHOD_FUNC(rb_points), 0);
      rb_define_method(rb_klass, "approx_chains", RUBY_METHOD_FUNC(rb_approx_chains), -1);
      rb_define_alias(rb_klass, "approx", "approx_chains");
    }
  }
}
