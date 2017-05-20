/************************************************************

   cvsurfparams.cpp -

   $Author: ser1zw $

   Copyright (C) 2011 ser1zw

************************************************************/
#include "cvsurfparams.h"
#include "opencv.h"
/*
 * Document-class: OpenCV::CvSURFParams
 *
 * C structure is here.
 *   typedef struct CvSURFParams {
 *     int extended;
 *     double hessianThreshold;
 *     int nOctaves;
 *     int nOctaveLayers;
 *   } CvSURFParams;
 */
namespace mOpenCV {
  namespace cCvSURFParams {
    VALUE rb_klass;

    VALUE
    rb_class()
    {
      return rb_klass;
    }

#ifdef IS_OPENCV2
    VALUE
    rb_allocate(VALUE klass)
    {
      CvSURFParams *ptr;
      return Data_Make_Struct(klass, CvSURFParams, 0, -1, ptr);
    }

    /*
     * Create a CvSURFParams
     *
     * @overload CvSURFParams.new(hessian_threshold, extended = false, n_octaves = 3, n_octave_layers = 4)
     *   @param hessian_threshold [Number]
     *   @param extended [Boolean] If <tt>true</tt>, exteneded descriptors (128 elements each),
     *     otherwise basic descriptors (64 elements each)
     *   @param n_octaves [Integer] Number of octaves to be used for extraction
     *   @param n_octave_layers [Integer] Number of layers within each octave
     */
    VALUE
    rb_initialize(int argc, VALUE *argv, VALUE self)
    {
      CvSURFParams *self_ptr = CVSURFPARAMS(self);
      VALUE h_thresh, ext, noct, noctl;
      rb_scan_args(argc, argv, "13", &h_thresh, &ext, &noct, &noctl);

      self_ptr->hessianThreshold = NUM2DBL(h_thresh);
      self_ptr->extended = NIL_P(ext) ? 0 : BOOL2INT(ext);
      self_ptr->nOctaves = NIL_P(noct) ? 3 : NUM2INT(noct);
      self_ptr->nOctaveLayers = NIL_P(noctl) ? 4 : NUM2INT(noctl);

      return self;
    }

    /*
     * call-seq:
     *   hessian_threshold -> number
     * Return threshold of hessian
     */
    VALUE
    rb_get_hessian_threshold(VALUE self)
    {
      return DBL2NUM(CVSURFPARAMS(self)->hessianThreshold);
    }

    /*
     * call-seq:
     *   hessian_threshold = <i>value</i>
     *
     * Set threshold of hessian to <i>value</i>
     */
    VALUE
    rb_set_hessian_threshold(VALUE self, VALUE value)
    {
      CVSURFPARAMS(self)->hessianThreshold = NUM2DBL(value);
      return self;
    }

    /*
     * call-seq:
     *   extended -> bool
     * Return the type of descripters
     *   false: basic descriptors (64 elements each)
     *   true : exteneded descriptors (128 elements each)
     */
    VALUE
    rb_get_extended(VALUE self)
    {
      return INT2BOOL(CVSURFPARAMS(self)->extended);
    }

    /*
     * call-seq:
     *   extended = <i>value</i>
     * Set the type of descripters
     *   false: basic descriptors (64 elements each)
     *   true : exteneded descriptors (128 elements each)
     */
    VALUE
    rb_set_extended(VALUE self, VALUE value)
    {
      CVSURFPARAMS(self)->extended = BOOL2INT(value);
      return self;
    }

    /*
     * call-seq:
     *   n_octaves -> fixnum
     * Return the number of octaves to be used for extraction
     */
    VALUE
    rb_get_n_octaves(VALUE self)
    {
      return INT2NUM(CVSURFPARAMS(self)->nOctaves);
    }

    /*
     * call-seq:
     *   n_octaves = <i>value</i>
     * Set the number of octaves to be used for extraction
     */
    VALUE
    rb_set_n_octaves(VALUE self, VALUE value)
    {
      CVSURFPARAMS(self)->nOctaves = NUM2INT(value);
      return self;
    }

    /*
     * call-seq:
     *   n_octave_layers -> fixnum
     * Return the number of layers within each octave
     */
    VALUE
    rb_get_n_octave_layers(VALUE self)
    {
      return INT2NUM(CVSURFPARAMS(self)->nOctaveLayers);
    }

    /*
     * call-seq:
     *   n_octave_layers = <i>value</i>
     * Set the number of layers within each octave
     */
    VALUE
    rb_set_n_octave_layers(VALUE self, VALUE value)
    {
      CVSURFPARAMS(self)->nOctaveLayers = NUM2INT(value);
      return self;
    }

    VALUE
    new_object()
    {
      return rb_allocate(rb_klass);
    }

    VALUE
    new_object(CvSURFParams* cvsurfparams)
    {
      VALUE object = rb_allocate(rb_klass);
      CvSURFParams *ptr = CVSURFPARAMS(object);
      ptr = cvsurfparams;
      return object;
    }
#else
    VALUE rb_initialize(int argc, VALUE *argv, VALUE self)
    {
      raise_opencv3_unsupported();
      return Qnil;
    }
#endif

    void
    init_ruby_class()
    {
#if 0
      // For documentation using YARD
      VALUE opencv = rb_define_module("OpenCV");
#endif

      if (rb_klass)
	return;
      /*
       * opencv = rb_define_module("OpenCV");
       *
       * note: this comment is used by rdoc.
       */
      VALUE opencv = rb_module_opencv();
      rb_klass = rb_define_class_under(opencv, "CvSURFParams", rb_cObject);
      rb_define_method(rb_klass, "initialize", RUBY_METHOD_FUNC(rb_initialize), -1);

#ifdef IS_OPENCV2
      rb_define_alloc_func(rb_klass, rb_allocate);
      rb_define_method(rb_klass, "hessian_threshold", RUBY_METHOD_FUNC(rb_get_hessian_threshold), 0);
      rb_define_method(rb_klass, "hessian_threshold=", RUBY_METHOD_FUNC(rb_set_hessian_threshold), 1);
      rb_define_method(rb_klass, "extended", RUBY_METHOD_FUNC(rb_get_extended), 0);
      rb_define_method(rb_klass, "extended=", RUBY_METHOD_FUNC(rb_set_extended), 1);
      rb_define_method(rb_klass, "n_octaves", RUBY_METHOD_FUNC(rb_get_n_octaves), 0);
      rb_define_method(rb_klass, "n_octaves=", RUBY_METHOD_FUNC(rb_set_n_octaves), 1);
      rb_define_method(rb_klass, "n_octave_layers", RUBY_METHOD_FUNC(rb_get_n_octave_layers), 0);
      rb_define_method(rb_klass, "n_octave_layers=", RUBY_METHOD_FUNC(rb_set_n_octave_layers), 1);
#endif
    }
  }
}
