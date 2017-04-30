/************************************************************

   cvscalar.cpp -

   $Author: lsxi $

   Copyright (C) 2005 Masakazu Yonekura

************************************************************/
#include "cvscalar.h"
#include "cvmat.h"
#include "opencv.h"

/*
 * Document-class: OpenCV::CvScalar
 *
 * Element-value of one pixel.
 * OpenCV supports the image of 4-channels in the maximum.
 * Therefore, CvScalar has 4-values.
 * 
 * C structure is here, very simple.
 *   typdef struct CvScalar {
 *     double val[4];
 *   } CvScalar;
 *
 * If obtain CvScalar-object from the method of CvMat(or IplImage),
 * the channel outside the range is obtained as all 0.
 *
 *   image = IplImage::load("opencv.jpg")  #=> 3-channel 8bit-depth BGR image
 *   pixel = image[10, 20]                 #=> Get pixel value of (10, 20) of image. pixel is CvScalar-object.
 *   blue, green, red = pixel[0], pixel[1], pixel[2]
 *   # pixel[3] always 0.
 *
 * CvColor is alias of CvScalar.
 */
namespace mOpenCV {
  namespace cCvScalar {

    VALUE rb_klass;

    VALUE
    rb_class()
    {
      return rb_klass;
    }

    VALUE
    rb_allocate(VALUE klass)
    {
      CvScalar *ptr;
      return Data_Make_Struct(klass, CvScalar, 0, -1, ptr);
    }

    /*
     * call-seq:
     *   new([d1][,d2][,d3][,d4])
     *
     * Create new Scalar. Argument should be Integer (or nil as 0).
     */
    VALUE
    rb_initialize(int argc, VALUE *argv, VALUE self)
    {
      VALUE val[4];
      rb_scan_args(argc, argv, "04", &val[0], &val[1], &val[2], &val[3]);
      CvScalar* self_ptr = CVSCALAR(self);
      for (int i = 0; i < 4; ++i) {
	self_ptr->val[i] = NIL_P(val[i]) ? 0 : NUM2DBL(val[i]);
      }
      return self;
    }

    /*
     * call-seq:
     *   [<i>index</i>]
     *
     * Return value of <i>index</i> dimension.
     */
    VALUE
    rb_aref(VALUE self, VALUE index)
    {
      int idx = NUM2INT(index);
      if (idx < 0 || idx >= 4) {
	rb_raise(rb_eIndexError, "scalar index should be 0...4");
      }
      return rb_float_new(CVSCALAR(self)->val[idx]);
    }

    /*
     * call-seq:
     *   [<i>index</i>] = <i>value</i>
     *
     * Set value of <i>index</i> dimension to <i>value</i>
     */
    VALUE
    rb_aset(VALUE self, VALUE index, VALUE value)
    {
      int idx = NUM2INT(index);
      if (idx < 0 || idx >= 4) {
	rb_raise(rb_eIndexError, "scalar index should be 0...4");
      }
      CVSCALAR(self)->val[idx] = NUM2DBL(value);
      return self;
    }

    /*
     * call-seq:
     *   sub(val[,mask])
     *
     * Return new CvScalar if <i>val</i> is CvScalar or compatible object.
     *   self[I] - val[I]
     * Or return new CvMat if <i>val</i> is CvMat or subclass.
     */
    VALUE
    rb_sub(int argc, VALUE *argv, VALUE self)
    {
      VALUE val, mask;
      rb_scan_args(argc, argv, "11", &val, &mask);
      if (rb_obj_is_kind_of(val, cCvMat::rb_class())) {
	CvArr *val_ptr = CVARR(val);
	VALUE dest = Qnil;
	try {
	  dest = cCvMat::new_object(cvGetSize(val_ptr), cvGetElemType(val_ptr));
	  cvSubRS(val_ptr, *CVSCALAR(self), CVARR(dest), MASK(mask));
	}
	catch (cv::Exception& e) {
	  raise_cverror(e);
	}
	return dest;
      }
      else {
	CvScalar *src = CVSCALAR(self);
	CvScalar scl = VALUE_TO_CVSCALAR(val);
	return new_object(cvScalar(src->val[0] - scl.val[0],
				   src->val[1] - scl.val[1],
				   src->val[2] - scl.val[2],
				   src->val[3] - scl.val[3]));
      }
    }

    /*
     * call-seq:
     *   to_s -> "<OpeCV::CvScalar:#{self[0]},#{self[1]},#{self[2]},#{self[3]}>"
     *
     * Return values by String.
     */
    VALUE
    rb_to_s(VALUE self)
    {
      const int i = 6;
      VALUE str[i];
      str[0] = rb_str_new2("<%s:%g,%g,%g,%g>");
      str[1] = rb_str_new2(rb_class2name(CLASS_OF(self)));
      str[2] = rb_aref(self, INT2FIX(0));
      str[3] = rb_aref(self, INT2FIX(1));
      str[4] = rb_aref(self, INT2FIX(2));
      str[5] = rb_aref(self, INT2FIX(3));
      return rb_f_sprintf(i, str);
    }

    /*
     * call-seq:
     *   to_ary -> [self[0],self[1],self[2],self[3]]
     *
     * Return values by Array.
     */
    VALUE
    rb_to_ary(VALUE self)
    {
      return rb_ary_new3(4,
			 rb_aref(self, INT2FIX(0)),
			 rb_aref(self, INT2FIX(1)),
			 rb_aref(self, INT2FIX(2)),
			 rb_aref(self, INT2FIX(3)));
    }

    VALUE
    new_object()
    {
      VALUE object = rb_allocate(rb_klass);
      *CVSCALAR(object) = cvScalar(0);
      return object;
    }

    VALUE
    new_object(CvScalar scalar)
    {
      VALUE object = rb_allocate(rb_klass);
      *CVSCALAR(object) = scalar;
      return object;
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
      /* 
       * opencv = rb_define_module("OpenCV");
       * 
       * note: this comment is used by rdoc.
       */
      VALUE opencv = rb_module_opencv();
  
      rb_klass = rb_define_class_under(opencv, "CvScalar", rb_cObject);
      /* CvScalar: class */
      rb_define_const(opencv, "CvColor", rb_klass);
      rb_define_alloc_func(rb_klass, rb_allocate);      
      rb_define_method(rb_klass, "initialize", RUBY_METHOD_FUNC(rb_initialize), -1);
      rb_define_method(rb_klass, "[]", RUBY_METHOD_FUNC(rb_aref), 1);
      rb_define_method(rb_klass, "[]=", RUBY_METHOD_FUNC(rb_aset), 2);
      rb_define_method(rb_klass, "sub", RUBY_METHOD_FUNC(rb_sub), -1);
      rb_define_alias(rb_klass, "-", "sub");
  
      rb_define_method(rb_klass, "to_s", RUBY_METHOD_FUNC(rb_to_s), 0);
      rb_define_method(rb_klass, "to_ary", RUBY_METHOD_FUNC(rb_to_ary), 0);
      rb_define_alias(rb_klass, "to_a", "to_ary");

      rb_define_const(rb_klass, "Black", cCvScalar::new_object(cvScalar(0x0,0x0,0x0)));
      rb_define_const(rb_klass, "Silver", cCvScalar::new_object(cvScalar(0x0c,0x0c,0x0c)));
      rb_define_const(rb_klass, "Gray", cCvScalar::new_object(cvScalar(0x80,0x80,0x80)));
      rb_define_const(rb_klass, "White", cCvScalar::new_object(cvScalar(0xff,0xff,0xff)));
      rb_define_const(rb_klass, "Maroon", cCvScalar::new_object(cvScalar(0x0,0x0,0x80)));
      rb_define_const(rb_klass, "Red", cCvScalar::new_object(cvScalar(0x0,0x0,0xff)));
      rb_define_const(rb_klass, "Purple", cCvScalar::new_object(cvScalar(0x80,0x0,0x80)));
      rb_define_const(rb_klass, "Fuchsia", cCvScalar::new_object(cvScalar(0xff,0x0,0xff)));
      rb_define_const(rb_klass, "Green", cCvScalar::new_object(cvScalar(0x0,0x80,0x0)));
      rb_define_const(rb_klass, "Lime", cCvScalar::new_object(cvScalar(0x0,0xff,0x0)));
      rb_define_const(rb_klass, "Olive", cCvScalar::new_object(cvScalar(0x0,0x80,0x80)));
      rb_define_const(rb_klass, "Yellow", cCvScalar::new_object(cvScalar(0x0,0xff,0xff)));
      rb_define_const(rb_klass, "Navy", cCvScalar::new_object(cvScalar(0x80,0x0,0x0)));
      rb_define_const(rb_klass, "Blue", cCvScalar::new_object(cvScalar(0xff,0x0,0x0)));
      rb_define_const(rb_klass, "Teal", cCvScalar::new_object(cvScalar(0x80,0x80,0x0)));
      rb_define_const(rb_klass, "Aqua", cCvScalar::new_object(cvScalar(0xff,0xff,0x0)));
    }
  }
}
