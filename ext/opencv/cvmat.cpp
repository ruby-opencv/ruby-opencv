/************************************************************

   cvmat.cpp -

   $Author: lsxi $

   Copyright (C) 2005-2008 Masakazu Yonekura

************************************************************/
#include "opencv2/core.hpp"
#include "opencv2/calib3d/calib3d.hpp"

#include "opencv2/photo/photo_c.h"

#ifdef IS_OPENCV2
#  include "opencv2/contrib/contrib.hpp"
#  include "opencv2/legacy/legacy.hpp"
#else
#  include "opencv2/video/tracking_c.h"
#  include "opencv2/calib3d/calib3d_c.h"
#endif

#include "cvmat.h"
#include "opencv.h"
#include "iplimage.h"
#include "cvscalar.h"
#include "cvpoint.h"
#include "cvpoint2d32f.h"
#include "cvrect.h"
#include "cvsize.h"
#include "cvtermcriteria.h"
#include "iplconvkernel.h"
#include "cvbox2d.h"
#include "cvconnectedcomp.h"
#include "cvseq.h"
#include "cvchain.h"
#include "cvcontour.h"
#include "cvmemstorage.h"
#include "cvmoments.h"
#include "cvline.h"
#include "cvtwopoints.h"
#include "cvcircle32f.h"
#include "cvbox2d.h"
#include "cvfont.h"
#include "pointset.h"

#ifdef IS_OPENCV2
#  include "cvsurfparams.h"
#  include "cvsurfpoint.h"
#endif

/*
 * Document-class: OpenCV::CvMat
 *
 */
#define DRAWING_OPTION(opt) rb_get_option_table(rb_klass, "DRAWING_OPTION", opt)
#define DO_COLOR(opt) VALUE_TO_CVSCALAR(LOOKUP_HASH(opt, "color"))
#define DO_THICKNESS(opt) NUM2INT(LOOKUP_HASH(opt, "thickness"))
#define DO_LINE_TYPE(opt) rb_drawing_option_line_type(opt)
#define DO_SHIFT(opt) NUM2INT(LOOKUP_HASH(opt, "shift"))
#define DO_IS_CLOSED(opt) TRUE_OR_FALSE(LOOKUP_HASH(opt, "is_closed"))

#define GOOD_FEATURES_TO_TRACK_OPTION(opt) rb_get_option_table(rb_klass, "GOOD_FEATURES_TO_TRACK_OPTION", opt)
#define GF_MAX(opt) NUM2INT(LOOKUP_HASH(opt, "max"))
#define GF_MASK(opt) MASK(LOOKUP_HASH(opt, "mask"))
#define GF_BLOCK_SIZE(opt) NUM2INT(LOOKUP_HASH(opt, "block_size"))
#define GF_USE_HARRIS(opt) TRUE_OR_FALSE(LOOKUP_HASH(opt, "use_harris"))
#define GF_K(opt) NUM2DBL(LOOKUP_HASH(opt, "k"))

#define FLOOD_FILL_OPTION(opt) rb_get_option_table(rb_klass, "FLOOD_FILL_OPTION", opt)
#define FF_CONNECTIVITY(opt) NUM2INT(LOOKUP_HASH(opt, "connectivity"))
#define FF_FIXED_RANGE(opt) TRUE_OR_FALSE(LOOKUP_HASH(opt, "fixed_range"))
#define FF_MASK_ONLY(opt) TRUE_OR_FALSE(LOOKUP_HASH(opt, "mask_only"))

#define FIND_CONTOURS_OPTION(opt) rb_get_option_table(rb_klass, "FIND_CONTOURS_OPTION", opt)
#define FC_MODE(opt) NUM2INT(LOOKUP_HASH(opt, "mode"))
#define FC_METHOD(opt) NUM2INT(LOOKUP_HASH(opt, "method"))
#define FC_OFFSET(opt) VALUE_TO_CVPOINT(LOOKUP_HASH(opt, "offset"))

#define HS_LAMBDA(opt) NUM2DBL(LOOKUP_HASH(opt, "lambda"))
#define HS_CRITERIA(opt) VALUE_TO_CVTERMCRITERIA(LOOKUP_HASH(opt, "criteria"))

#define BM_BLOCK_SIZE(opt) VALUE_TO_CVSIZE(LOOKUP_HASH(opt, "block_size"))
#define BM_SHIFT_SIZE(opt) VALUE_TO_CVSIZE(LOOKUP_HASH(opt, "shift_size"))
#define BM_MAX_RANGE(opt) VALUE_TO_CVSIZE(LOOKUP_HASH(opt, "max_range"))

#define FIND_FUNDAMENTAL_MAT_OPTION(opt) rb_get_option_table(rb_klass, "FIND_FUNDAMENTAL_MAT_OPTION", opt)
#define FFM_WITH_STATUS(opt) TRUE_OR_FALSE(LOOKUP_HASH(opt, "with_status"))
#define FFM_MAXIMUM_DISTANCE(opt) NUM2DBL(LOOKUP_HASH(opt, "maximum_distance"))
#define FFM_DESIRABLE_LEVEL(opt) NUM2DBL(LOOKUP_HASH(opt, "desirable_level"))

namespace mOpenCV {
  namespace cCvMat {
    VALUE rb_klass;

    int
    rb_drawing_option_line_type(VALUE drawing_option)
    {
      VALUE line_type = LOOKUP_HASH(drawing_option, "line_type");
      if (FIXNUM_P(line_type)) {
	return FIX2INT(line_type);
      }
      else if (line_type == ID2SYM(rb_intern("aa"))) {
	return CV_AA;
      }
      return 0;
    }

    int*
    hash_to_format_specific_param(VALUE hash)
    {
      Check_Type(hash, T_HASH);
      const int flags[] = {
	CV_IMWRITE_JPEG_QUALITY,
	CV_IMWRITE_PNG_COMPRESSION,
	CV_IMWRITE_PNG_STRATEGY,
	CV_IMWRITE_PNG_STRATEGY_DEFAULT,
	CV_IMWRITE_PNG_STRATEGY_FILTERED,
	CV_IMWRITE_PNG_STRATEGY_HUFFMAN_ONLY,
	CV_IMWRITE_PNG_STRATEGY_RLE,
	CV_IMWRITE_PNG_STRATEGY_FIXED,
	CV_IMWRITE_PXM_BINARY
      };
      const int flag_size = sizeof(flags) / sizeof(int);

      int* params = (int*)ALLOC_N(int, RHASH_SIZE(hash) * 2);
      for (int i = 0, n = 0; i < flag_size; i++) {
	VALUE val = rb_hash_lookup(hash, INT2FIX(flags[i]));
	if (!NIL_P(val)) {
	  params[n] = flags[i];
	  params[n + 1] = NUM2INT(val);
	  n += 2;
	}
      }

      return params;
    }

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

    /*
     * Creates a matrix
     * @overload new(rows, cols, depth = CV_8U, channels = 3)
     * @param row [Integer] Number of rows in the matrix
     * @param col [Integer] Number of columns in the matrix
     * @param depth [Integer, Symbol] Depth type in the matrix.
     *   The type of the matrix elements in the form of constant <b><tt>CV_<bit depth><S|U|F></tt></b>
     *   or symbol <b><tt>:cv<bit depth><s|u|f></tt></b>, where S=signed, U=unsigned, F=float.
     * @param channels [Integer] Number of channels in the matrix
     * @return [CvMat] Created matrix
     * @opencv_func cvCreateMat
     * @example
     *   mat1 = CvMat.new(3, 4) # Creates a 3-channels 3x4 matrix whose elements are 8bit unsigned.
     *   mat2 = CvMat.new(5, 6, CV_32F, 1) # Creates a 1-channel 5x6 matrix whose elements are 32bit float.
     *   mat3 = CvMat.new(5, 6, :cv32f, 1) # Same as CvMat.new(5, 6, CV_32F, 1)
     */
    VALUE
    rb_initialize(int argc, VALUE *argv, VALUE self)
    {
      VALUE row, column, depth, channel;
      rb_scan_args(argc, argv, "22", &row, &column, &depth, &channel);

      int ch = (argc < 4) ? 3 : NUM2INT(channel);
      CvMat *ptr = rb_cvCreateMat(NUM2INT(row), NUM2INT(column),
				  CV_MAKETYPE(CVMETHOD("DEPTH", depth, CV_8U), ch));
      free(DATA_PTR(self));
      DATA_PTR(self) = ptr;

      return self;
    }

    /*
     * Load an image from the specified file
     * @overload load(filename, iscolor = 1)
     * @param filename [String] Name of file to be loaded
     * @param iscolor [Integer] Flags specifying the color type of a loaded image:
     *   - <b>> 0</b> Return a 3-channel color image.
     *   - <b>= 0</b> Return a grayscale image.
     *   - <b>< 0</b> Return the loaded image as is.
     * @return [CvMat] Loaded image
     * @opencv_func cvLoadImageM
     * @scope class
     */
    VALUE
    rb_load_imageM(int argc, VALUE *argv, VALUE self)
    {
      VALUE filename, iscolor;
      rb_scan_args(argc, argv, "11", &filename, &iscolor);
      Check_Type(filename, T_STRING);

      int _iscolor;
      if (NIL_P(iscolor)) {
	_iscolor = CV_LOAD_IMAGE_COLOR;
      }
      else {
	Check_Type(iscolor, T_FIXNUM);
	_iscolor = FIX2INT(iscolor);
      }

      CvMat *mat = NULL;
      try {
	mat = cvLoadImageM(StringValueCStr(filename), _iscolor);
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      if (mat == NULL) {
	rb_raise(rb_eStandardError, "file does not exist or invalid format image.");
      }
      return OPENCV_OBJECT(rb_klass, mat);
    }

    /*
     * Encodes an image into a memory buffer.
     *
     * @overload encode_image(ext, params = nil)
     *   @param ext [String] File extension that defines the output format ('.jpg', '.png', ...)
     *   @param params [Hash] - Format-specific parameters.
     *   @option params [Integer] CV_IMWRITE_JPEG_QUALITY (95) For JPEG, it can be a quality
     *     ( CV_IMWRITE_JPEG_QUALITY ) from 0 to 100 (the higher is the better).
     *   @option params [Integer] CV_IMWRITE_PNG_COMPRESSION (3) For PNG, it can be the compression
     *     level ( CV_IMWRITE_PNG_COMPRESSION ) from 0 to 9. A higher value means a smaller size
     *     and longer compression time.
     *   @option params [Integer] CV_IMWRITE_PXM_BINARY (1) For PPM, PGM, or PBM, it can be a binary
     *     format flag ( CV_IMWRITE_PXM_BINARY ), 0 or 1.
     * @return [Array<Integer>] Encoded image as array of bytes.
     * @opencv_func cvEncodeImage
     * @example
     *   jpg = CvMat.load('image.jpg')
     *   bytes1 = jpg.encode_image('.jpg') # Encodes a JPEG image which quality is 95
     *   bytes2 = jpg.encode_image('.jpg', CV_IMWRITE_JPEG_QUALITY => 10) # Encodes a JPEG image which quality is 10
     *
     *   png = CvMat.load('image.png')
     *   bytes3 = mat.encode_image('.png', CV_IMWRITE_PNG_COMPRESSION => 1)  # Encodes a PNG image which compression level is 1
     */
    VALUE
    rb_encode_imageM(int argc, VALUE *argv, VALUE self)
    {
      VALUE _ext, _params;
      rb_scan_args(argc, argv, "11", &_ext, &_params);
      Check_Type(_ext, T_STRING);
      const char* ext = RSTRING_PTR(_ext);
      CvMat* buff = NULL;
      int* params = NULL;

      if (!NIL_P(_params)) {
	params = hash_to_format_specific_param(_params);
      }

      try {
	buff = cvEncodeImage(ext, CVARR(self), params);
      }
      catch (cv::Exception& e) {
	if (params != NULL) {
	  free(params);
	  params = NULL;
	}
	raise_cverror(e);
      }
      if (params != NULL) {
	free(params);
	params = NULL;
      }

      const int size = buff->rows * buff->cols;
      VALUE array = rb_ary_new2(size);
      for (int i = 0; i < size; i++) {
	rb_ary_store(array, i, CHR2FIX(CV_MAT_ELEM(*buff, char, 0, i)));
      }

      try {
	cvReleaseMat(&buff);
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }

      return array;
    }

    CvMat*
    prepare_decoding(int argc, VALUE *argv, int* iscolor, int* need_release)
    {
      VALUE _buff, _iscolor;
      rb_scan_args(argc, argv, "11", &_buff, &_iscolor);
      *iscolor = NIL_P(_iscolor) ? CV_LOAD_IMAGE_COLOR : NUM2INT(_iscolor);

      CvMat* buff = NULL;
      *need_release = 0;
      switch (TYPE(_buff)) {
      case T_STRING:
	_buff = rb_funcall(_buff, rb_intern("unpack"), 1, rb_str_new("c*", 2));
      case T_ARRAY: {
	int cols = RARRAY_LEN(_buff);
	*need_release = 1;
	try {
	  buff = rb_cvCreateMat(1, cols, CV_8UC1);
	  VALUE *ary_ptr = RARRAY_PTR(_buff);
	  for (int i = 0; i < cols; i++) {
	    CV_MAT_ELEM(*buff, char, 0, i) = NUM2CHR(ary_ptr[i]);
	  }
	}
	catch (cv::Exception& e) {
	  raise_cverror(e);
	}
	break;
      }
      case T_DATA:
	if (rb_obj_is_kind_of(_buff, cCvMat::rb_class()) == Qtrue) {
	  buff = CVMAT(_buff);
	  break;
	}
      default:
	raise_typeerror(_buff, "CvMat, Array or String");
      }

      return buff;
    }

    /*
     * Reads an image from a buffer in memory.
     * @overload decode_image(buf, iscolor = 1)
     * @param buf [CvMat, Array, String] Input array of bytes
     * @param iscolor [Integer] Flags specifying the color type of a decoded image (the same flags as CvMat#load)
     * @return [CvMat] Loaded matrix
     * @opencv_func cvDecodeImageM
     */
    VALUE
    rb_decode_imageM(int argc, VALUE *argv, VALUE self)
    {
      int iscolor, need_release;
      CvMat* buff = prepare_decoding(argc, argv, &iscolor, &need_release);
      CvMat* mat_ptr = NULL;
      try {
	mat_ptr = cvDecodeImageM(buff, iscolor);
	if (need_release) {
	  cvReleaseMat(&buff);
	}
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }

      return OPENCV_OBJECT(rb_klass, mat_ptr);
    }

    /*
     * nodoc
     */
    VALUE
    rb_method_missing(int argc, VALUE *argv, VALUE self)
    {
      VALUE name, args, method;
      rb_scan_args(argc, argv, "1*", &name, &args);
      method = rb_funcall(name, rb_intern("to_s"), 0);
      if (RARRAY_LEN(args) != 0 || !rb_respond_to(rb_module_opencv(), rb_intern(StringValuePtr(method))))
	return rb_call_super(argc, argv);
      return rb_funcall(rb_module_opencv(), rb_intern(StringValuePtr(method)), 1, self);
    }

    /*
     * @overload to_s
     * @return [String] String representation of the matrix
     */
    VALUE
    rb_to_s(VALUE self)
    {
      const int i = 6;
      VALUE str[i];
      str[0] = rb_str_new2("<%s:%dx%d,depth=%s,channel=%d>");
      str[1] = rb_str_new2(rb_class2name(CLASS_OF(self)));
      str[2] = rb_width(self);
      str[3] = rb_height(self);
      str[4] = rb_depth(self);
      str[5] = rb_channel(self);
      return rb_f_sprintf(i, str);
    }

    /*
     * Tests whether a coordinate or rectangle is inside of the matrix
     * @overload inside?(point)
     *   @param obj [#x, #y] Tested coordinate
     * @overload inside?(rect)
     *   @param obj [#x, #y, #width, #height] Tested rectangle
     * @return [Boolean] If the point or rectangle is inside of the matrix, return <tt>true</tt>.
     *   If not, return <tt>false</tt>.
     */
    VALUE
    rb_inside_q(VALUE self, VALUE object)
    {
      if (cCvPoint::rb_compatible_q(cCvPoint::rb_class(), object)) {
	CvMat *mat = CVMAT(self);
	int x = NUM2INT(rb_funcall(object, rb_intern("x"), 0));
	int y = NUM2INT(rb_funcall(object, rb_intern("y"), 0));
	if (cCvRect::rb_compatible_q(cCvRect::rb_class(), object)) {
	  int width = NUM2INT(rb_funcall(object, rb_intern("width"), 0));
	  int height = NUM2INT(rb_funcall(object, rb_intern("height"), 0));
	  return (x >= 0) && (y >= 0) && (x < mat->width) && ((x + width) < mat->width)
	    && (y < mat->height) && ((y + height) < mat->height) ? Qtrue : Qfalse;
	}
	else {
	  return (x >= 0) && (y >= 0) && (x < mat->width) && (y < mat->height) ? Qtrue : Qfalse;
	}
      }
      rb_raise(rb_eArgError, "argument 1 should have method \"x\", \"y\"");
      return Qnil;
    }

    /*
     * Creates a structuring element from the matrix for morphological operations.
     * @overload to_IplConvKernel(anchor)
     * @param anchor [CvPoint] Anchor position within the element
     * @return [IplConvKernel] Created IplConvKernel
     * @opencv_func cvCreateStructuringElementEx
     */
    VALUE
    rb_to_IplConvKernel(VALUE self, VALUE anchor)
    {
      CvMat *src = CVMAT(self);
      CvPoint p = VALUE_TO_CVPOINT(anchor);
      IplConvKernel *kernel = rb_cvCreateStructuringElementEx(src->cols, src->rows, p.x, p.y,
							      CV_SHAPE_CUSTOM, src->data.i);
      return DEPEND_OBJECT(cIplConvKernel::rb_class(), kernel, self);
    }

    /*
     * Creates a mask (1-channel 8bit unsinged image whose elements are 0) from the matrix.
     * The size of the mask is the same as source matrix.
     * @overload create_mask
     * @return [CvMat] Created mask
     */
    VALUE
    rb_create_mask(VALUE self)
    {
      VALUE mask = cCvMat::new_object(cvGetSize(CVARR(self)), CV_8UC1);
      try {
	cvZero(CVARR(self));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return mask;
    }

    /*
     * Returns number of columns of the matrix.
     * @overload width
     * @return [Integer] Number of columns of the matrix
     */
    VALUE
    rb_width(VALUE self)
    {
      return INT2NUM(CVMAT(self)->width);
    }

    /*
     * Returns number of rows of the matrix.
     * @overload rows
     * @return [Integer] Number of rows of the matrix
     */
    VALUE
    rb_height(VALUE self)
    {
      return INT2NUM(CVMAT(self)->height);
    }

    /*
     * Returns depth type of the matrix
     * @overload depth
     * @return [Symbol] Depth type in the form of symbol <b><tt>:cv<bit depth><s|u|f></tt></b>,
     *   where s=signed, u=unsigned, f=float.
     */
    VALUE
    rb_depth(VALUE self)
    {
      return rb_hash_lookup(rb_funcall(rb_const_get(rb_module_opencv(), rb_intern("DEPTH")), rb_intern("invert"), 0),
			    INT2FIX(CV_MAT_DEPTH(CVMAT(self)->type)));
    }

    /*
     * Returns number of channels of the matrix
     * @overload channel
     * @return [Integer] Number of channels of the matrix
     */
    VALUE
    rb_channel(VALUE self)
    {
      return INT2FIX(CV_MAT_CN(CVMAT(self)->type));
    }

    /*
     * @overload data
     * @deprecated This method will be removed.
     */
    VALUE
    rb_data(VALUE self)
    {
      IplImage *image = IPLIMAGE(self);
      return rb_str_new((char *)image->imageData, image->imageSize);
    }

    /*
     * Makes a clone of an object.
     * @overload clone
     * @return [CvMat] Clone of the object
     * @opencv_func cvClone
     */
    VALUE
    rb_clone(VALUE self)
    {
      VALUE clone = rb_obj_clone(self);
      try {
	DATA_PTR(clone) = cvClone(CVARR(self));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return clone;
    }

    /*
     * Copies one array to another.
     *
     * The function copies selected elements from an input array to an output array:
     *     dst(I) = src(I) if mask(I) != 0
     *
     * @overload copy(dst = nil, mask = nil)
     * @param dst [CvMat] The destination array.
     * @param mask [CvMat] Operation mask, 8-bit single channel array;
     *     specifies elements of the destination array to be changed.
     * @return [CvMat] Copy of the array
     * @opencv_func cvCopy
     */
    VALUE
    rb_copy(int argc, VALUE *argv, VALUE self)
    {
      VALUE _dst, _mask;
      rb_scan_args(argc, argv, "02", &_dst, &_mask);

      CvMat* mask = MASK(_mask);
      CvArr *src = CVARR(self);
      if (NIL_P(_dst)) {
	CvSize size = cvGetSize(src);
	_dst = new_mat_kind_object(size, self);
      }

      try {
	cvCopy(src, CVARR_WITH_CHECK(_dst), mask);
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }

      return _dst;
    }

    VALUE
    copy(VALUE mat)
    {
      return rb_clone(mat);
    }

    inline VALUE
    rb_to_X_internal(VALUE self, int depth)
    {
      CvMat *src = CVMAT(self);
      VALUE dest = new_object(src->rows, src->cols, CV_MAKETYPE(depth, CV_MAT_CN(src->type)));
      try {
	cvConvert(src, CVMAT(dest));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return dest;
    }

    /*
     * Converts the matrix to 8bit unsigned.
     * @overload to_8u
     * @return [CvMat] Converted matrix which depth is 8bit unsigned.
     *   The size and channels of the new matrix are same as the source.
     * @opencv_func cvConvert
     */
    VALUE
    rb_to_8u(VALUE self)
    {
      return rb_to_X_internal(self, CV_8U);
    }

    /*
     * Converts the matrix to 8bit signed.
     * @overload to_8s
     * @return [CvMat] Converted matrix which depth is 8bit signed.
     *   The size and channels of the new matrix are same as the source.
     * @opencv_func cvConvert
     */
    VALUE
    rb_to_8s(VALUE self)
    {
      return rb_to_X_internal(self, CV_8S);
    }

    /*
     * Converts the matrix to 16bit unsigned.
     * @overload to_16u
     * @return [CvMat] Converted matrix which depth is 16bit unsigned.
     *   The size and channels of the new matrix are same as the source.
     * @opencv_func cvConvert
     */
    VALUE rb_to_16u(VALUE self)
    {
      return rb_to_X_internal(self, CV_16U);
    }

    /*
     * Converts the matrix to 16bit signed.
     * @overload to_16s
     * @return [CvMat] Converted matrix which depth is 16bit signed.
     *   The size and channels of the new matrix are same as the source.
     * @opencv_func cvConvert
     */
    VALUE
    rb_to_16s(VALUE self)
    {
      return rb_to_X_internal(self, CV_16S);
    }

    /*
     * Converts the matrix to 32bit signed.
     * @overload to_32s
     * @return [CvMat] Converted matrix which depth is 32bit signed.
     *   The size and channels of the new matrix are same as the source.
     * @opencv_func cvConvert
     */
    VALUE
    rb_to_32s(VALUE self)
    {
      return rb_to_X_internal(self, CV_32S);
    }

    /*
     * Converts the matrix to 32bit float.
     * @overload to_32f
     * @return [CvMat] Converted matrix which depth is 32bit float.
     *   The size and channels of the new matrix are same as the source.
     * @opencv_func cvConvert
     */
    VALUE
    rb_to_32f(VALUE self)
    {
      return rb_to_X_internal(self, CV_32F);
    }

    /*
     * Converts the matrix to 64bit float.
     * @overload to_64f
     * @return [CvMat] Converted matrix which depth is 64bit float.
     *   The size and channels of the new matrix are same as the source.
     * @opencv_func cvConvert
     */
    VALUE
    rb_to_64f(VALUE self)
    {
      return rb_to_X_internal(self, CV_64F);
    }

    /*
     * Returns whether the matrix is a vector.
     * @overload vector?
     * @return [Boolean] If width or height of the matrix is 1, returns <tt>true</tt>.
     *   if not, returns <tt>false</tt>.
     */
    VALUE
    rb_vector_q(VALUE self)
    {
      CvMat *mat = CVMAT(self);
      return (mat->width == 1|| mat->height == 1) ? Qtrue : Qfalse;
    }

    /*
     * Returns whether the matrix is a square.
     * @overload square?
     * @return [Boolean] If width = height, returns <tt>true</tt>.
     *   if not, returns <tt>false</tt>.
     */
    VALUE
    rb_square_q(VALUE self)
    {
      CvMat *mat = CVMAT(self);
      return mat->width == mat->height ? Qtrue : Qfalse;
    }

    /************************************************************
       cxcore function
    ************************************************************/
    /*
     * Converts an object to CvMat
     * @overload to_CvMat
     * @return [CvMat] Converted matrix
     */
    VALUE
    rb_to_CvMat(VALUE self)
    {
      // CvMat#to_CvMat aborts when self's class is CvMat.
      if (CLASS_OF(self) == rb_klass)
	return self;

      CvMat *mat = NULL;
      try {
	mat = cvGetMat(CVARR(self), RB_CVALLOC(CvMat));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return DEPEND_OBJECT(rb_klass, mat, self);
    }

    /*
     * Returns matrix corresponding to the rectangular sub-array of input image or matrix
     *
     * @overload sub_rect(rect)
     *   @param rect [CvRect] Zero-based coordinates of the rectangle of interest.
     * @overload sub_rect(topleft, size)
     *   @param topleft [CvPoint] Top-left coordinates of the rectangle of interest
     *   @param size [CvSize] Size of the rectangle of interest
     * @overload sub_rect(x, y, width, height)
     *   @param x [Integer] X-coordinate of the rectangle of interest
     *   @param y [Integer] Y-coordinate of the rectangle of interest
     *   @param width [Integer] Width of the rectangle of interest
     *   @param height [Integer] Height of the rectangle of interest
     * @return [CvMat] Sub-array of matrix
     * @opencv_func cvGetSubRect
     */
    VALUE
    rb_sub_rect(VALUE self, VALUE args)
    {
      CvRect area;
      CvPoint topleft;
      CvSize size;
      switch(RARRAY_LEN(args)) {
      case 1:
	area = VALUE_TO_CVRECT(RARRAY_PTR(args)[0]);
	break;
      case 2:
	topleft = VALUE_TO_CVPOINT(RARRAY_PTR(args)[0]);
	size = VALUE_TO_CVSIZE(RARRAY_PTR(args)[1]);
	area.x = topleft.x;
	area.y = topleft.y;
	area.width = size.width;
	area.height = size.height;
	break;
      case 4:
	area.x = NUM2INT(RARRAY_PTR(args)[0]);
	area.y = NUM2INT(RARRAY_PTR(args)[1]);
	area.width = NUM2INT(RARRAY_PTR(args)[2]);
	area.height = NUM2INT(RARRAY_PTR(args)[3]);
	break;
      default:
	rb_raise(rb_eArgError, "wrong number of arguments (%ld of 1 or 2 or 4)", RARRAY_LEN(args));
      }

      CvMat* mat = NULL;
      try {
	mat = cvGetSubRect(CVARR(self), RB_CVALLOC(CvMat), area);
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return DEPEND_OBJECT(rb_klass, mat, self);
    }

    void
    rb_get_range_index(VALUE index, int* start, int *end) {
      if (rb_obj_is_kind_of(index, rb_cRange)) {
	*start = NUM2INT(rb_funcall3(index, rb_intern("begin"), 0, NULL));
	*end = NUM2INT(rb_funcall3(index, rb_intern("end"), 0, NULL));
	if (rb_funcall3(index, rb_intern("exclude_end?"), 0, NULL) == Qfalse) {
	  (*end)++;
	}
      }
      else {
	*start = NUM2INT(index);
	*end = *start + 1;
      }
    }

    /*
     * Returns array of row or row span.
     * @overload get_rows(index, delta_row = 1)
     *   @param index [Integer] Zero-based index of the selected row
     *   @param delta_row [Integer] Index step in the row span.
     *   @return [CvMat] Selected row
     * @overload get_rows(range, delta_row = 1)
     *   @param range [Range] Zero-based index range of the selected row
     *   @param delta_row [Integer] Index step in the row span.
     *   @return [CvMat] Selected rows
     * @opencv_func cvGetRows
     */
    VALUE
    rb_get_rows(int argc, VALUE* argv, VALUE self)
    {
      VALUE row_val, delta_val;
      rb_scan_args(argc, argv, "11", &row_val, &delta_val);

      int start, end;
      rb_get_range_index(row_val, &start, &end);
      int delta = NIL_P(delta_val) ? 1 : NUM2INT(delta_val);
      CvMat* submat = RB_CVALLOC(CvMat);
      try {
	cvGetRows(CVARR(self), submat, start, end, delta);
      }
      catch (cv::Exception& e) {
	cvFree(&submat);
	raise_cverror(e);
      }

      return DEPEND_OBJECT(rb_klass, submat, self);
    }

    /*
     * Returns array of column or column span.
     * @overload get_cols(index)
     *   @param index [Integer] Zero-based index of the selected column
     *   @return [CvMat] Selected column
     * @overload get_cols(range)
     *   @param range [Range] Zero-based index range of the selected column
     *   @return [CvMat] Selected columns
     * @opencv_func cvGetCols
     */
    VALUE
    rb_get_cols(VALUE self, VALUE col)
    {
      int start, end;
      rb_get_range_index(col, &start, &end);
      CvMat* submat = RB_CVALLOC(CvMat);
      try {
	cvGetCols(CVARR(self), submat, start, end);
      }
      catch (cv::Exception& e) {
	cvFree(&submat);
	raise_cverror(e);
      }

      return DEPEND_OBJECT(rb_klass, submat, self);
    }

    /*
     * Calls <i>block</i> once for each row in the matrix, passing that
     * row as a parameter.
     * @yield [row] Each row in the matrix
     * @return [CvMat] <tt>self</tt>
     * @opencv_func cvGetRow
     * @todo To return an enumerator if no block is given
     */
    VALUE
    rb_each_row(VALUE self)
    {
      int rows = CVMAT(self)->rows;
      CvMat* row = NULL;
      for (int i = 0; i < rows; ++i) {
	try {
	  row = cvGetRow(CVARR(self), RB_CVALLOC(CvMat), i);
	}
	catch (cv::Exception& e) {
	  if (row != NULL)
	    cvReleaseMat(&row);
	  raise_cverror(e);
	}
	rb_yield(DEPEND_OBJECT(rb_klass, row, self));
      }
      return self;
    }

    /*
     * Calls <i>block</i> once for each column in the matrix, passing that
     * column as a parameter.
     * @yield [col] Each column in the matrix
     * @return [CvMat] <tt>self</tt>
     * @opencv_func cvGetCol
     * @todo To return an enumerator if no block is given
     */
    VALUE
    rb_each_col(VALUE self)
    {
      int cols = CVMAT(self)->cols;
      CvMat *col = NULL;
      for (int i = 0; i < cols; ++i) {
	try {
	  col = cvGetCol(CVARR(self), RB_CVALLOC(CvMat), i);
	}
	catch (cv::Exception& e) {
	  if (col != NULL)
	    cvReleaseMat(&col);
	  raise_cverror(e);
	}
	rb_yield(DEPEND_OBJECT(rb_klass, col, self));
      }
      return self;
    }

    /*
     * Returns a specified diagonal of the matrix
     * @overload diag(val = 0)
     * @param val [Integer] Index of the array diagonal. Zero value corresponds to the main diagonal,
     *   -1 corresponds to the diagonal above the main, 1 corresponds to the diagonal below the main,
     *   and so forth.
     * @return [CvMat] Specified diagonal
     * @opencv_func cvGetDiag
     */
    VALUE
    rb_diag(int argc, VALUE *argv, VALUE self)
    {
      VALUE val;
      if (rb_scan_args(argc, argv, "01", &val) < 1)
	val = INT2FIX(0);
      CvMat* diag = NULL;
      try {
	diag = cvGetDiag(CVARR(self), RB_CVALLOC(CvMat), NUM2INT(val));
      }
      catch (cv::Exception& e) {
	cvReleaseMat(&diag);
	raise_cverror(e);
      }
      return DEPEND_OBJECT(rb_klass, diag, self);
    }

    /*
     * Returns size of the matrix
     * @overload size
     * @return [CvSize] Size of the matrix
     * @opencv_func cvGetSize
     */
    VALUE
    rb_size(VALUE self)
    {
      CvSize size;
      try {
	size = cvGetSize(CVARR(self));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return cCvSize::new_object(size);
    }

    /*
     * Returns array dimensions sizes
     * @overload dims
     * @return [Array<Integer>] Array dimensions sizes.
     *   For 2d arrays the number of rows (height) goes first, number of columns (width) next.
     * @opencv_func cvGetDims
     */
    VALUE
    rb_dims(VALUE self)
    {
      int size[CV_MAX_DIM];
      int dims = 0;
      try {
	dims = cvGetDims(CVARR(self), size);
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      VALUE ary = rb_ary_new2(dims);
      for (int i = 0; i < dims; ++i) {
	rb_ary_store(ary, i, INT2NUM(size[i]));
      }
      return ary;
    }

    /*
     * Returns array size along the specified dimension.
     * @overload dim_size(index)
     * @param index [Intger] Zero-based dimension index
     *   (for matrices 0 means number of rows, 1 means number of columns;
     *   for images 0 means height, 1 means width)
     * @return [Integer] Array size
     * @opencv_func cvGetDimSize
     */
    VALUE
    rb_dim_size(VALUE self, VALUE index)
    {
      int dimsize = 0;
      try {
	dimsize = cvGetDimSize(CVARR(self), NUM2INT(index));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return INT2NUM(dimsize);
    }

    /*
     * Returns a specific array element.
     * @overload [](idx0)
     * @overload [](idx0, idx1)
     * @overload [](idx0, idx1, idx2)
     * @overload [](idx0, idx1, idx2, ...)
     * @param idx-n [Integer] Zero-based component of the element index
     * @return [CvScalar] Array element
     * @opencv_func cvGet1D
     * @opencv_func cvGet2D
     * @opencv_func cvGet3D
     * @opencv_func cvGetND
     */
    VALUE
    rb_aref(VALUE self, VALUE args)
    {
      int index[CV_MAX_DIM];
      for (int i = 0; i < RARRAY_LEN(args); ++i)
	index[i] = NUM2INT(rb_ary_entry(args, i));
  
      CvScalar scalar = cvScalarAll(0);
      try {
	switch (RARRAY_LEN(args)) {
	case 1:
	  scalar = cvGet1D(CVARR(self), index[0]);
	  break;
	case 2:
	  scalar = cvGet2D(CVARR(self), index[0], index[1]);
	  break;
	default:
	  scalar = cvGetND(CVARR(self), index);
	  break;      
	}
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return cCvScalar::new_object(scalar);
    }

    /*
     * Changes the particular array element
     * @overload []=(idx0, value)
     * @overload []=(idx0, idx1, value)
     * @overload []=(idx0, idx1, idx2, value)
     * @overload []=(idx0, idx1, idx2, ..., value)
     * @param idx-n [Integer] Zero-based component of the element index
     * @param value [CvScalar] The assigned value
     * @return [CvMat] <tt>self</tt>
     * @opencv_func cvSet1D
     * @opencv_func cvSet2D
     * @opencv_func cvSet3D
     * @opencv_func cvSetND
     */
    VALUE
    rb_aset(VALUE self, VALUE args)
    {
      CvScalar scalar = VALUE_TO_CVSCALAR(rb_ary_pop(args));
      int index[CV_MAX_DIM];
      for (int i = 0; i < RARRAY_LEN(args); ++i)
	index[i] = NUM2INT(rb_ary_entry(args, i));

      try {
	switch (RARRAY_LEN(args)) {
	case 1:
	  cvSet1D(CVARR(self), index[0], scalar);
	  break;
	case 2:
	  cvSet2D(CVARR(self), index[0], index[1], scalar);
	  break;
	default:
	  cvSetND(CVARR(self), index, scalar);
	  break;
	}
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return self;
    }

    /*
     * Assigns user data to the array header
     * @overload set_data(data)
     * @param data [Array<Integer>] User data
     * @return [CvMat] <tt>self</tt>
     * @opencv_func cvSetData
     */
    VALUE
    rb_set_data(VALUE self, VALUE data)
    {
      data = rb_funcall(data, rb_intern("flatten"), 0);
      const int DATA_LEN = RARRAY_LEN(data);
      CvMat *self_ptr = CVMAT(self);
      int depth = CV_MAT_DEPTH(self_ptr->type);
      void* array = NULL;

      switch (depth) {
      case CV_8U:
	array = rb_cvAlloc(sizeof(uchar) * DATA_LEN);
	for (int i = 0; i < DATA_LEN; ++i)
	  ((uchar*)array)[i] = (uchar)(NUM2INT(rb_ary_entry(data, i)));
	break;
      case CV_8S:
	array = rb_cvAlloc(sizeof(char) * DATA_LEN);
	for (int i = 0; i < DATA_LEN; ++i)
	  ((char*)array)[i] = (char)(NUM2INT(rb_ary_entry(data, i)));
	break;
      case CV_16U:
	array = rb_cvAlloc(sizeof(ushort) * DATA_LEN);
	for (int i = 0; i < DATA_LEN; ++i)
	  ((ushort*)array)[i] = (ushort)(NUM2INT(rb_ary_entry(data, i)));
	break;
      case CV_16S:
	array = rb_cvAlloc(sizeof(short) * DATA_LEN);
	for (int i = 0; i < DATA_LEN; ++i)
	  ((short*)array)[i] = (short)(NUM2INT(rb_ary_entry(data, i)));
	break;
      case CV_32S:
	array = rb_cvAlloc(sizeof(int) * DATA_LEN);
	for (int i = 0; i < DATA_LEN; ++i)
	  ((int*)array)[i] = NUM2INT(rb_ary_entry(data, i));
	break;
      case CV_32F:
	array = rb_cvAlloc(sizeof(float) * DATA_LEN);
	for (int i = 0; i < DATA_LEN; ++i)
	  ((float*)array)[i] = (float)NUM2DBL(rb_ary_entry(data, i));
	break;
      case CV_64F:
	array = rb_cvAlloc(sizeof(double) * DATA_LEN);
	for (int i = 0; i < DATA_LEN; ++i)
	  ((double*)array)[i] = NUM2DBL(rb_ary_entry(data, i));
	break;
      default:
	rb_raise(rb_eArgError, "Invalid CvMat depth");
	break;
      }

      try {
	cvSetData(self_ptr, array, self_ptr->step);    
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }

      return self;
    }

    /*
     * Returns a matrix which is set every element to a given value.
     * The function copies the scalar value to every selected element of the destination array:
     *   mat[I] = value if mask(I) != 0
     *
     * @overload set(value, mask = nil) Fill value
     * @param value [CvScalar] Fill value
     * @param mask [CvMat] Operation mask, 8-bit single channel array;
     *   specifies elements of the destination array to be changed
     * @return [CvMat] Matrix which is set every element to a given value.
     * @opencv_func cvSet
     */
    VALUE
    rb_set(int argc, VALUE *argv, VALUE self)
    {
      return rb_set_bang(argc, argv, copy(self));
    }

    /*
     * Sets every element of the matrix to a given value.
     * The function copies the scalar value to every selected element of the destination array:
     *   mat[I] = value if mask(I) != 0
     *
     * @overload set!(value, mask = nil)
     * @param (see #set)
     * @return [CvMat] <tt>self</tt>
     * @opencv_func cvSet
     */
    VALUE
    rb_set_bang(int argc, VALUE *argv, VALUE self)
    {
      VALUE value, mask;
      rb_scan_args(argc, argv, "11", &value, &mask);
      try {
	cvSet(CVARR(self), VALUE_TO_CVSCALAR(value), MASK(mask));    
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return self;
    }

    /*
     * Saves an image to a specified file.
     * The image format is chosen based on the filename extension.
     * @overload save_image(filename)
     * @param filename [String] Name of the file
     * @return [CvMat] <tt>self</tt>
     * @opencv_func cvSaveImage 
     */
    VALUE
    rb_save_image(int argc, VALUE *argv, VALUE self)
    {
      VALUE _filename, _params;
      rb_scan_args(argc, argv, "11", &_filename, &_params);
      Check_Type(_filename, T_STRING);
      int *params = NULL;
      if (!NIL_P(_params)) {
	params = hash_to_format_specific_param(_params);
      }

      try {
	cvSaveImage(StringValueCStr(_filename), CVARR(self), params);
      }
      catch (cv::Exception& e) {
	if (params != NULL) {
	  free(params);
	  params = NULL;
	}
	raise_cverror(e);
      }
      if (params != NULL) {
	free(params);
	params = NULL;
      }

      return self;
    }

    /*
     * Returns cleared array.
     * @overload set_zero
     * @return [CvMat] Cleared array
     * @opencv_func cvSetZero
     */
    VALUE
    rb_set_zero(VALUE self)
    {
      return rb_set_zero_bang(copy(self));
    }

    /*
     * Clears the array.
     * @overload set_zero!
     * @return [CvMat] <tt>self</tt>
     * @opencv_func cvSetZero
     */
    VALUE
    rb_set_zero_bang(VALUE self)
    {
      try {
	cvSetZero(CVARR(self));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return self;
    }

    /*
     * Returns a scaled identity matrix.
     *   arr(i, j) = value if i = j, 0 otherwise
     * @overload identity(value)
     * @param value [CvScalar] Value to assign to diagonal elements.
     * @return [CvMat] Scaled identity matrix.
     * @opencv_func cvSetIdentity
     */
    VALUE
    rb_set_identity(int argc, VALUE *argv, VALUE self)
    {
      return rb_set_identity_bang(argc, argv, copy(self));
    }

    /*
     * Initializes a scaled identity matrix.
     *   arr(i, j) = value if i = j, 0 otherwise
     * @overload identity!(value)
     * @param (see #identity)
     * @return [CvMat] <tt>self</tt>
     * @opencv_func cvSetIdentity
     */
    VALUE
    rb_set_identity_bang(int argc, VALUE *argv, VALUE self)
    {
      VALUE val;
      CvScalar value;
      if (rb_scan_args(argc, argv, "01",  &val) < 1)
	value = cvRealScalar(1);
      else
	value = VALUE_TO_CVSCALAR(val);

      try {
	cvSetIdentity(CVARR(self), value);
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return self;
    }

    /*
     * Returns initialized matrix as following:
     *   arr(i,j)=(end-start)*(i*cols(arr)+j)/(cols(arr)*rows(arr))
     * @overload range(start, end)
     * @param start [Number] The lower inclusive boundary of the range
     * @param end [Number] The upper exclusive boundary of the range
     * @return [CvMat] Initialized matrix
     * @opencv_func cvRange
     */
    VALUE
    rb_range(VALUE self, VALUE start, VALUE end)
    {
      return rb_range_bang(copy(self), start, end);
    }

    /*
     * Initializes the matrix as following:
     *   arr(i,j)=(end-start)*(i*cols(arr)+j)/(cols(arr)*rows(arr))
     * @overload range!(start, end)
     * @param (see #range)
     * @return [CvMat] <tt>self</tt>
     * @opencv_func cvRange
     */
    VALUE
    rb_range_bang(VALUE self, VALUE start, VALUE end)
    {
      try {
	cvRange(CVARR(self), NUM2DBL(start), NUM2DBL(end));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return self;
    }

    /*
     * Changes shape of matrix/image without copying data.
     * @overload reshape(cn, rows=0)
     *   @param cn [Integer] New number of channels. If the parameter is 0, the number of channels remains the same.
     *   @param rows [Integer] New number of rows. If the parameter is 0, the number of rows remains the same.
     * @return [CvMat] Changed matrix
     * @opencv_func cvReshape
     * @example
     *   mat = CvMat.new(3, 3, CV_8U, 3)  #=> 3x3 3-channel matrix
     *   vec = mat.reshape(:rows => 1)    #=> 1x9 3-channel matrix
     *   ch1 = mat.reshape(:channel => 1) #=> 9x3 1-channel matrix
     */
    VALUE
    rb_reshape(int argc, VALUE *argv, VALUE self)
    {
      VALUE cn, rows;
      CvMat *mat = NULL;
      rb_scan_args(argc, argv, "11", &cn, &rows);
      try {
	mat = cvReshape(CVARR(self), RB_CVALLOC(CvMat), NUM2INT(cn), IF_INT(rows, 0));
      }
      catch (cv::Exception& e) {
	if (mat != NULL)
	  cvReleaseMat(&mat);
	raise_cverror(e);
      }
      return DEPEND_OBJECT(rb_klass, mat, self);
    }

    /*
     * Fills the destination array with repeated copies of the source array.
     *
     * @overload repeat(dst)
     * @param dst [CvMat] Destination array of the same type as <tt>self</tt>.
     * @return [CvMat] Destination array
     * @opencv_func cvRepeat
     */
    VALUE
    rb_repeat(VALUE self, VALUE object)
    {
      try {
	cvRepeat(CVARR(self), CVARR_WITH_CHECK(object));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return object;
    }

    /*
     * Returns a fliped 2D array around vertical, horizontal, or both axes.
     *
     * @overload flip(flip_mode)
     * @param flip_mode [Symbol] Flag to specify how to flip the array.
     *   - <tt>:x</tt> - Flipping around the x-axis.
     *   - <tt>:y</tt> - Flipping around the y-axis.
     *   - <tt>:xy</tt> - Flipping around both axes.
     * @return [CvMat] Flipped array
     * @opencv_func cvFlip
     */
    VALUE
    rb_flip(int argc, VALUE *argv, VALUE self)
    {
      return rb_flip_bang(argc, argv, copy(self));
    }

    /*
     * Flips a 2D array around vertical, horizontal, or both axes.
     *
     * @overload flip!(flip_mode)
     * @param (see #flip)
     * @return (see #flip)
     * @opencv_func (see #flip)
     */
    VALUE
    rb_flip_bang(int argc, VALUE *argv, VALUE self)
    {
      VALUE format;
      int mode = 1;
      if (rb_scan_args(argc, argv, "01", &format) > 0) {
	Check_Type(format, T_SYMBOL);
	ID flip_mode = rb_to_id(format);
	if (flip_mode == rb_intern("x")) {
	  mode = 1;
	}
	else if (flip_mode == rb_intern("y")) {
	  mode = 0;
	}
	else if (flip_mode == rb_intern("xy")) {
	  mode = -1;
	}
      }
      try {
	cvFlip(CVARR(self), NULL, mode);
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return self;
    }

    /*
     * Divides a multi-channel array into several single-channel arrays.
     *
     * @overload split
     * @return [Array<CvMat>] Array of single-channel arrays
     * @opencv_func cvSplit
     * @see merge
     * @example
     *   img = CvMat.new(640, 480, CV_8U, 3) #=> 3-channel image
     *   a = img.split                       #=> [img-ch1, img-ch2, img-ch3]
     */
    VALUE
    rb_split(VALUE self)
    {
      CvArr* self_ptr = CVARR(self);
      int type = cvGetElemType(self_ptr);
      int depth = CV_MAT_DEPTH(type), channel = CV_MAT_CN(type);
      VALUE dest = rb_ary_new2(channel);
      try {
	CvArr *dest_ptr[] = { NULL, NULL, NULL, NULL };
	CvSize size = cvGetSize(self_ptr);
	for (int i = 0; i < channel; ++i) {
	  VALUE tmp = new_mat_kind_object(size, self, depth, 1);
	  rb_ary_store(dest, i, tmp);
	  dest_ptr[i] = CVARR(tmp);
	}
	cvSplit(self_ptr, dest_ptr[0], dest_ptr[1], dest_ptr[2], dest_ptr[3]);
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }

      return dest;
    }

    /*
     * Composes a multi-channel array from several single-channel arrays.
     *
     * @overload merge(src1 = nil, src2 = nil, src3 = nil, src4 = nil)
     * @param src-n [CvMat] Source arrays to be merged.
     *     All arrays must have the same size and the same depth.
     * @return [CvMat] Merged array
     * @opencv_func cvMerge
     * @see split
     * @scope class
     */
    VALUE
    rb_merge(VALUE klass, VALUE args)
    {
      int len = RARRAY_LEN(args);
      if (len <= 0 || len > 4) {
	rb_raise(rb_eArgError, "wrong number of argument (%d for 1..4)", len);
      }
      CvMat *src[] = { NULL, NULL, NULL, NULL }, *prev_src = NULL;
      for (int i = 0; i < len; ++i) {
	VALUE object = rb_ary_entry(args, i);
	if (NIL_P(object))
	  src[i] = NULL;
	else {
	  src[i] = CVMAT_WITH_CHECK(object);
	  if (CV_MAT_CN(src[i]->type) != 1)
	    rb_raise(rb_eArgError, "image should be single-channel CvMat.");
	  if (prev_src == NULL)
	    prev_src = src[i];
	  else {
	    if (!CV_ARE_SIZES_EQ(prev_src, src[i]))
	      rb_raise(rb_eArgError, "image size should be same.");
	    if (!CV_ARE_DEPTHS_EQ(prev_src, src[i]))
	      rb_raise(rb_eArgError, "image depth should be same.");
	  }
	}
      }
      // TODO: adapt IplImage
      VALUE dest = Qnil;
      try {
	dest = new_object(cvGetSize(src[0]), CV_MAKETYPE(CV_MAT_DEPTH(src[0]->type), len));
	cvMerge(src[0], src[1], src[2], src[3], CVARR(dest));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return dest;
    }

    /*
     * Returns shuffled matrix by swapping randomly chosen pairs of the matrix elements on each iteration 
     * (where each element may contain several components in case of multi-channel arrays)
     *
     * @overload rand_shuffle(seed = -1, iter_factor = 1)
     * @param seed [Integer] Integer value used to initiate a random sequence
     * @param iter_factor [Integer] The relative parameter that characterizes intensity of
     *     the shuffling performed. The number of iterations (i.e. pairs swapped) is
     *     round(iter_factor*rows(mat)*cols(mat)), so <tt>iter_factor</tt> = 0 means that no shuffling is done,
     *     <tt>iter_factor</tt> = 1 means that the function swaps rows(mat)*cols(mat) random pairs etc
     * @return [CvMat] Shuffled matrix
     * @opencv_func cvRandShuffle
     */
    VALUE
    rb_rand_shuffle(int argc, VALUE *argv, VALUE self)
    {
      return rb_rand_shuffle_bang(argc, argv, copy(self));
    }

    /*
     * Shuffles the matrix by swapping randomly chosen pairs of the matrix elements on each iteration 
     * (where each element may contain several components in case of multi-channel arrays)
     *
     * @overload rand_shuffle!(seed = -1, iter_factor = 1)
     * @param (see #rand_shuffle)
     * @return (see #rand_shuffle)
     * @opencv_func (see #rand_shuffle)
     */
    VALUE
    rb_rand_shuffle_bang(int argc, VALUE *argv, VALUE self)
    {
      VALUE seed, iter;
      rb_scan_args(argc, argv, "02", &seed, &iter);
      try {
	if (NIL_P(seed))
	  cvRandShuffle(CVARR(self), NULL, IF_INT(iter, 1));
	else {
	  CvRNG rng = cvRNG(rb_num2ll(seed));
	  cvRandShuffle(CVARR(self), &rng, IF_INT(iter, 1));
	}
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return self;
    }

    /*
     * Performs a look-up table transform of an array.
     *
     * @overload lut(lut)
     * @param lut [CvMat] Look-up table of 256 elements. In case of multi-channel source array,
     *     the table should either have a single channel (in this case the same table is used
     *     for all channels) or the same number of channels as in the source array.
     * @return [CvMat] Transformed array
     * @opencv_func cvLUT
     */
    VALUE
    rb_lut(VALUE self, VALUE lut)
    {
      VALUE dest = copy(self);
      try {
	cvLUT(CVARR(self), CVARR(dest), CVARR_WITH_CHECK(lut));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return dest;
    }

    /*
     * Converts one array to another with optional linear transformation.
     *
     * @overload convert_scale(params)
     *   @param params [Hash] Transform parameters
     *   @option params [Integer] :depth (same as self) Depth of the destination array
     *   @option params [Number] :scale (1.0) Scale factor
     *   @option params [Number] :shift (0.0) Value added to the scaled source array elements
     * @return [CvMat] Converted array
     * @opencv_func cvConvertScale
     */
    VALUE
    rb_convert_scale(VALUE self, VALUE hash)
    {
      Check_Type(hash, T_HASH);
      CvMat* self_ptr = CVMAT(self);
      VALUE depth = LOOKUP_HASH(hash, "depth");
      VALUE scale = LOOKUP_HASH(hash, "scale");
      VALUE shift = LOOKUP_HASH(hash, "shift");

      VALUE dest = Qnil;
      try {
	dest = new_mat_kind_object(cvGetSize(self_ptr), self,
				   CVMETHOD("DEPTH", depth, CV_MAT_DEPTH(self_ptr->type)),
				   CV_MAT_CN(self_ptr->type));
	cvConvertScale(self_ptr, CVARR(dest), IF_DBL(scale, 1.0), IF_DBL(shift, 0.0));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return dest;
    }

    /*
     * Scales, computes absolute values, and converts the result to 8-bit.
     *
     * @overload convert_scale_abs(params)
     *   @param params [Hash] Transform parameters
     *   @option params [Number] :scale (1.0) Scale factor
     *   @option params [Number] :shift (0.0) Value added to the scaled source array elements
     * @return [CvMat] Converted array
     * @opencv_func cvConvertScaleAbs
     */
    VALUE
    rb_convert_scale_abs(VALUE self, VALUE hash)
    {
      Check_Type(hash, T_HASH);
      CvMat* self_ptr = CVMAT(self);
      VALUE scale = LOOKUP_HASH(hash, "scale");
      VALUE shift = LOOKUP_HASH(hash, "shift");
      VALUE dest = Qnil;
      try {
	dest = new_mat_kind_object(cvGetSize(self_ptr), self, CV_8U, CV_MAT_CN(CVMAT(self)->type));
	cvConvertScaleAbs(self_ptr, CVARR(dest), IF_DBL(scale, 1.0), IF_DBL(shift, 0.0));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return dest;
    }

    /*
     * Computes the per-element sum of two arrays or an array and a scalar.
     *
     * @overload add(val, mask = nil)
     * @param val [CvMat, CvScalar] Array or scalar to add
     * @param mask [CvMat] Optional operation mask, 8-bit single channel array, 
     *     that specifies elements of the destination array to be changed.
     * @return [CvMat] Result array
     * @opencv_func cvAdd
     * @opencv_func cvAddS
     */
    VALUE
    rb_add(int argc, VALUE *argv, VALUE self)
    {
      VALUE val, mask, dest;
      rb_scan_args(argc, argv, "11", &val, &mask);
      dest = copy(self);
      try {
	if (rb_obj_is_kind_of(val, rb_klass))
	  cvAdd(CVARR(self), CVARR(val), CVARR(dest), MASK(mask));
	else
	  cvAddS(CVARR(self), VALUE_TO_CVSCALAR(val), CVARR(dest), MASK(mask));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return dest;
    }

    /*
     * Calculates the per-element difference between two arrays or array and a scalar.
     *
     * @overload sub(val, mask = nil)
     * @param val [CvMat, CvScalar] Array or scalar to subtract
     * @param mask [CvMat] Optional operation mask, 8-bit single channel array, 
     *     that specifies elements of the destination array to be changed.
     * @return [CvMat] Result array
     * @opencv_func cvSub
     * @opencv_func cvSubS
     */
    VALUE
    rb_sub(int argc, VALUE *argv, VALUE self)
    {
      VALUE val, mask, dest;
      rb_scan_args(argc, argv, "11", &val, &mask);
      dest = copy(self);
      try {
	if (rb_obj_is_kind_of(val, rb_klass))
	  cvSub(CVARR(self), CVARR(val), CVARR(dest), MASK(mask));
	else
	  cvSubS(CVARR(self), VALUE_TO_CVSCALAR(val), CVARR(dest), MASK(mask));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return dest;
    }

    /*
     * Calculates the per-element scaled product of two arrays.
     *
     * @overload mul(val, scale = 1.0)
     * @param val [CvMat, CvScalar] Array or scalar to multiply
     * @param scale [Number] Optional scale factor.
     * @return [CvMat] Result array
     * @opencv_func cvMul
     */
    VALUE
    rb_mul(int argc, VALUE *argv, VALUE self)
    {
      VALUE val, scale, dest;
      if (rb_scan_args(argc, argv, "11", &val, &scale) < 2)
	scale = rb_float_new(1.0);
      dest = new_mat_kind_object(cvGetSize(CVARR(self)), self);
      try {
	if (rb_obj_is_kind_of(val, rb_klass))
	  cvMul(CVARR(self), CVARR(val), CVARR(dest), NUM2DBL(scale));
	else {
	  CvScalar scl = VALUE_TO_CVSCALAR(val);
	  VALUE mat = new_object(cvGetSize(CVARR(self)), cvGetElemType(CVARR(self)));
	  cvSet(CVARR(mat), scl);
	  cvMul(CVARR(self), CVARR(mat), CVARR(dest), NUM2DBL(scale));
	}
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return dest;
    }

    /*
     * Calculates the product of two arrays.
     *   dst = self * val + shiftvec
     *
     * @overload mat_mul(val, shiftvec = nil)
     * @param val [CvMat] Array to multiply
     * @param shiftvec [CvMat] Optional translation vector
     * @return [CvMat] Result array
     * @opencv_func cvMatMul
     * @opencv_func cvMatMulAdd
     */
    VALUE
    rb_mat_mul(int argc, VALUE *argv, VALUE self)
    {
      VALUE val, shiftvec, dest;
      rb_scan_args(argc, argv, "11", &val, &shiftvec);
      CvArr* self_ptr = CVARR(self);  
      dest = new_mat_kind_object(cvGetSize(self_ptr), self);
      try {
	if (NIL_P(shiftvec))
	  cvMatMul(self_ptr, CVARR_WITH_CHECK(val), CVARR(dest));
	else
	  cvMatMulAdd(self_ptr, CVARR_WITH_CHECK(val), CVARR_WITH_CHECK(shiftvec), CVARR(dest));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return dest;
    }

    /*
     * Performs per-element division of two arrays or a scalar by an array.
     *
     * @overload div(val, scale = 1.0)
     * @param val [CvMat, CvScalar] Array or scalar to divide
     * @param scale [Number] Scale factor
     * @return [CvMat] Result array
     * @opencv_func cvDiv
     */
    VALUE
    rb_div(int argc, VALUE *argv, VALUE self)
    {
      VALUE val, scale;
      if (rb_scan_args(argc, argv, "11", &val, &scale) < 2)
	scale = rb_float_new(1.0);
      CvArr* self_ptr = CVARR(self);
      VALUE dest = Qnil;
      try {
	dest = new_mat_kind_object(cvGetSize(self_ptr), self);
	if (rb_obj_is_kind_of(val, rb_klass))
	  cvDiv(self_ptr, CVARR(val), CVARR(dest), NUM2DBL(scale));
	else {
	  CvScalar scl = VALUE_TO_CVSCALAR(val);
	  VALUE mat = new_mat_kind_object(cvGetSize(self_ptr), self);
	  CvArr* mat_ptr = CVARR(mat);
	  cvSet(mat_ptr, scl);
	  cvDiv(self_ptr, mat_ptr, CVARR(dest), NUM2DBL(scale));
	}
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return dest;
    }

    /*
     * Computes the weighted sum of two arrays.
     * This function calculates the weighted sum of two arrays as follows:
     *   dst(I) = src1(I) * alpha + src2(I) * beta + gamma
     *
     * @overload add_weighted(src1, alpha, src2, beta, gamma)
     * @param src1 [CvMat] The first source array.
     * @param alpha [Number] Weight for the first array elements.
     * @param src2 [CvMat] The second source array.
     * @param beta [Number] Weight for the second array elements.
     * @param gamma [Number] Scalar added to each sum.
     * @return [CvMat] Result array
     * @opencv_func cvAddWeighted
     */
    VALUE
    rb_add_weighted(VALUE klass, VALUE src1, VALUE alpha, VALUE src2, VALUE beta, VALUE gamma)
    {
      CvArr* src1_ptr = CVARR_WITH_CHECK(src1);
      VALUE dst = new_mat_kind_object(cvGetSize(src1_ptr), src1);
      try {
	cvAddWeighted(src1_ptr, NUM2DBL(alpha),
		      CVARR_WITH_CHECK(src2), NUM2DBL(beta),
		      NUM2DBL(gamma), CVARR(dst));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return dst;
    }

    /*
     * Calculates the per-element bit-wise conjunction of two arrays or an array and a scalar.
     *
     * @overload and(val, mask = nil)
     * @param val [CvMat, CvScalar] Array or scalar to calculate bit-wise conjunction
     * @param mask [CvMat] Optional operation mask, 8-bit single channel array, that specifies
     *     elements of the destination array to be changed.
     * @return [CvMat] Result array
     * @opencv_func cvAnd
     * @opencv_func cvAndS
     */
    VALUE
    rb_and(int argc, VALUE *argv, VALUE self)
    {
      VALUE val, mask, dest;
      rb_scan_args(argc, argv, "11", &val, &mask);
      dest = copy(self);
      try {
	if (rb_obj_is_kind_of(val, rb_klass))
	  cvAnd(CVARR(self), CVARR(val), CVARR(dest), MASK(mask));
	else
	  cvAndS(CVARR(self), VALUE_TO_CVSCALAR(val), CVARR(dest), MASK(mask));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return dest;
    }

    /*
     * Calculates the per-element bit-wise disjunction of two arrays or an array and a scalar.
     *
     * @overload or(val, mask = nil)
     * @param val [CvMat, CvScalar] Array or scalar to calculate bit-wise disjunction
     * @param mask [CvMat] Optional operation mask, 8-bit single channel array, that specifies
     *     elements of the destination array to be changed.
     * @return [CvMat] Result array
     * @opencv_func cvOr
     * @opencv_func cvOrS
     */
    VALUE
    rb_or(int argc, VALUE *argv, VALUE self)
    {
      VALUE val, mask, dest;
      rb_scan_args(argc, argv, "11", &val, &mask);
      dest = copy(self);
      try {
	if (rb_obj_is_kind_of(val, rb_klass))
	  cvOr(CVARR(self), CVARR(val), CVARR(dest), MASK(mask));
	else
	  cvOrS(CVARR(self), VALUE_TO_CVSCALAR(val), CVARR(dest), MASK(mask));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return dest;
    }

    /*
     * Calculates the per-element bit-wise "exclusive or" operation on two arrays or an array and a scalar.
     *
     * @overload xor(val, mask = nil)
     * @param val [CvMat, CvScalar] Array or scalar to calculate bit-wise xor operation.
     * @param mask [CvMat] Optional operation mask, 8-bit single channel array, that specifies
     *     elements of the destination array to be changed.
     * @return [CvMat] Result array
     * @opencv_func cvXor
     * @opencv_func cvXorS
     */
    VALUE
    rb_xor(int argc, VALUE *argv, VALUE self)
    {
      VALUE val, mask, dest;
      rb_scan_args(argc, argv, "11", &val, &mask);
      dest = copy(self);
      try {
	if (rb_obj_is_kind_of(val, rb_klass))
	  cvXor(CVARR(self), CVARR(val), CVARR(dest), MASK(mask));
	else
	  cvXorS(CVARR(self), VALUE_TO_CVSCALAR(val), CVARR(dest), MASK(mask));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return dest;
    }

    /*
     * Returns an array which elements are bit-wise invertion of source array.
     *
     * @overload not
     * @return [CvMat] Result array
     * @opencv_func cvNot
     */
    VALUE
    rb_not(VALUE self)
    {
      return rb_not_bang(copy(self));
    }

    /*
     * Inverts every bit of an array.
     *
     * @overload not!
     * @return [CvMat] Result array
     * @opencv_func cvNot
     */
    VALUE
    rb_not_bang(VALUE self)
    {
      try {
	cvNot(CVARR(self), CVARR(self));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return self;
    }

    VALUE
    rb_cmp_internal(VALUE self, VALUE val, int operand)
    {
      CvArr* self_ptr = CVARR(self);
      VALUE dest = new_mat_kind_object(cvGetSize(self_ptr), self, CV_8U, 1);
      try {
	if (rb_obj_is_kind_of(val, rb_klass))
	  cvCmp(self_ptr, CVARR(val), CVARR(dest), operand);
	else if (CV_MAT_CN(cvGetElemType(self_ptr)) == 1 && rb_obj_is_kind_of(val, rb_cNumeric))
	  cvCmpS(self_ptr, NUM2DBL(val), CVARR(dest), operand);
	else {
	  VALUE mat = new_mat_kind_object(cvGetSize(CVARR(self)), self);
	  cvSet(CVARR(mat), VALUE_TO_CVSCALAR(val));
	  cvCmp(self_ptr, CVARR(mat), CVARR(dest), operand);
	}
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return dest;
    }

    /*
     * Performs the per-element comparison "equal" of two arrays or an array and scalar value.
     *
     * @overload eq(val)
     * @param val [CvMat, CvScalar, Number] Array, scalar or number to compare
     * @return [CvMat] Result array
     * @opencv_func cvCmp
     * @opencv_func cvCmpS
     */
    VALUE
    rb_eq(VALUE self, VALUE val)
    {
      return rb_cmp_internal(self, val, CV_CMP_EQ);
    }

    /*
     * Performs the per-element comparison "greater than" of two arrays or an array and scalar value.
     *
     * @overload gt(val)
     * @param val [CvMat, CvScalar, Number] Array, scalar or number to compare
     * @return [CvMat] Result array
     * @opencv_func cvCmp
     * @opencv_func cvCmpS
     */
    VALUE
    rb_gt(VALUE self, VALUE val)
    {
      return rb_cmp_internal(self, val, CV_CMP_GT);
    }

    /*
     * Performs the per-element comparison "greater than or equal" of two arrays or an array and scalar value.
     *
     * @overload ge(val)
     * @param val [CvMat, CvScalar, Number] Array, scalar or number to compare
     * @return [CvMat] Result array
     * @opencv_func cvCmp
     * @opencv_func cvCmpS
     */
    VALUE
    rb_ge(VALUE self, VALUE val)
    {
      return rb_cmp_internal(self, val, CV_CMP_GE);
    }

    /*
     * Performs the per-element comparison "less than" of two arrays or an array and scalar value.
     *
     * @overload lt(val)
     * @param val [CvMat, CvScalar, Number] Array, scalar or number to compare
     * @return [CvMat] Result array
     * @opencv_func cvCmp
     * @opencv_func cvCmpS
     */
    VALUE
    rb_lt(VALUE self, VALUE val)
    {
      return rb_cmp_internal(self, val, CV_CMP_LT);
    }

    /*
     * Performs the per-element comparison "less than or equal" of two arrays or an array and scalar value.
     *
     * @overload le(val)
     * @param val [CvMat, CvScalar, Number] Array, scalar or number to compare
     * @return [CvMat] Result array
     * @opencv_func cvCmp
     * @opencv_func cvCmpS
     */
    VALUE
    rb_le(VALUE self, VALUE val)
    {
      return rb_cmp_internal(self, val, CV_CMP_LE);
    }

    /*
     * Performs the per-element comparison "not equal" of two arrays or an array and scalar value.
     *
     * @overload ne(val)
     * @param val [CvMat, CvScalar, Number] Array, scalar or number to compare
     * @return [CvMat] Result array
     * @opencv_func cvCmp
     * @opencv_func cvCmpS
     */
    VALUE
    rb_ne(VALUE self, VALUE val)
    {
      return rb_cmp_internal(self, val, CV_CMP_NE);
    }

    /*
     * Checks if array elements lie between the elements of two other arrays.
     *
     * @overload in_range(min, max)
     * @param min [CvMat, CvScalar] Inclusive lower boundary array or a scalar.
     * @param max [CvMat, CvScalar] Inclusive upper boundary array or a scalar.
     * @return [CvMat] Result array
     * @opencv_func cvInRange
     * @opencv_func cvInRangeS
     */
    VALUE
    rb_in_range(VALUE self, VALUE min, VALUE max)
    {
      CvArr* self_ptr = CVARR(self);
      CvSize size = cvGetSize(self_ptr);
      VALUE dest = new_object(size, CV_8UC1);
      try {
	if (rb_obj_is_kind_of(min, rb_klass) && rb_obj_is_kind_of(max, rb_klass))
	  cvInRange(self_ptr, CVARR(min), CVARR(max), CVARR(dest));
	else if (rb_obj_is_kind_of(min, rb_klass)) {
	  VALUE tmp = new_object(size, cvGetElemType(self_ptr));
	  cvSet(CVARR(tmp), VALUE_TO_CVSCALAR(max));
	  cvInRange(self_ptr, CVARR(min), CVARR(tmp), CVARR(dest));
	}
	else if (rb_obj_is_kind_of(max, rb_klass)) {
	  VALUE tmp = new_object(size, cvGetElemType(self_ptr));
	  cvSet(CVARR(tmp), VALUE_TO_CVSCALAR(min));
	  cvInRange(self_ptr, CVARR(tmp), CVARR(max), CVARR(dest));
	}
	else
	  cvInRangeS(self_ptr, VALUE_TO_CVSCALAR(min), VALUE_TO_CVSCALAR(max), CVARR(dest));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return dest;
    }

    /*
     * Computes the per-element absolute difference between two arrays or between an array and a scalar.
     *
     * @overload abs_diff(val)
     * @param val [CvMat, CvScalar] Array or scalar to compute absolute difference
     * @return [CvMat] Result array
     * @opencv_func cvAbsDiff
     * @opencv_func cvAbsDiffS
     */
    VALUE
    rb_abs_diff(VALUE self, VALUE val)
    {
      CvArr* self_ptr = CVARR(self);
      VALUE dest = new_mat_kind_object(cvGetSize(self_ptr), self);
      try {
	if (rb_obj_is_kind_of(val, rb_klass))
	  cvAbsDiff(self_ptr, CVARR(val), CVARR(dest));
	else
	  cvAbsDiffS(self_ptr, CVARR(dest), VALUE_TO_CVSCALAR(val));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return dest;
    }

    /*
     * Normalizes the norm or value range of an array.
     *
     * @overload normalize(alpha = 1.0, beta = 0.0, norm_type = NORM_L2, dtype = -1, mask = nil)
     *   @param alpha [Number] Norm value to normalize to or the lower range boundary
     *     in case of the range normalization.
     *   @param beta [Number] Upper range boundary in case of the range normalization.
     *     It is not used for the norm normalization.
     *   @param norm_type [Integer] Normalization type.
     *   @param dtype [Integer] when negative, the output array has the same type as src;
     *     otherwise, it has the same number of channels as src and the depth
     *   @param mask [CvMat] Optional operation mask.
     * @return [CvMat] Normalized array.
     * @opencv_func cv::normalize
     */
    VALUE
    rb_normalize(int argc, VALUE *argv, VALUE self)
    {
      VALUE alpha_val, beta_val, norm_type_val, dtype_val, mask_val;
      rb_scan_args(argc, argv, "05", &alpha_val, &beta_val, &norm_type_val, &dtype_val, &mask_val);

      double alpha = NIL_P(alpha_val) ? 1.0 : NUM2DBL(alpha_val);
      double beta = NIL_P(beta_val) ? 0.0 : NUM2DBL(beta_val);
      int norm_type = NIL_P(norm_type_val) ? cv::NORM_L2 : NUM2INT(norm_type_val);
      int dtype = NIL_P(dtype_val) ? -1 : NUM2INT(dtype_val);
      VALUE dst;

      try {
	cv::Mat self_mat = cv::cvarrToMat(CVMAT(self));
	cv::Mat dst_mat;

	if (NIL_P(mask_val)) {
	  cv::normalize(self_mat, dst_mat, alpha, beta, norm_type, dtype);
	}
	else {
	  cv::Mat mask = cv::cvarrToMat(MASK(mask_val));
	  cv::normalize(self_mat, dst_mat, alpha, beta, norm_type, dtype, mask);
	}
	dst = new_mat_kind_object(cvGetSize(CVARR(self)), self, dst_mat.depth(), dst_mat.channels());

	CvMat tmp = dst_mat;
	cvCopy(&tmp, CVMAT(dst));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }

      return dst;
    }

    /*
     * Counts non-zero array elements.
     *
     * @overload count_non_zero
     * @return [Integer] The number of non-zero elements.
     * @opencv_func cvCountNonZero
     */
    VALUE
    rb_count_non_zero(VALUE self)
    {
      int n = 0;
      try {
	n = cvCountNonZero(CVARR(self));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return INT2NUM(n);
    }

    /*
     * Calculates the sum of array elements.
     *
     * @overload sum
     * @return [CvScalar] The sum of array elements.
     * @opencv_func cvSum
     */
    VALUE
    rb_sum(VALUE self)
    {
      CvScalar sum;
      try {
	sum = cvSum(CVARR(self));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return cCvScalar::new_object(sum);
    }

    /*
     * Calculates an average (mean) of array elements.
     * @overload avg(mask = nil)
     * @param mask [CvMat] Optional operation mask.
     * @return [CvScalar] The average of array elements.
     * @opencv_func cvAvg
     */
    VALUE
    rb_avg(int argc, VALUE *argv, VALUE self)
    {
      VALUE mask;
      rb_scan_args(argc, argv, "01", &mask);
      CvScalar avg;
      try {
	avg = cvAvg(CVARR(self), MASK(mask));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return cCvScalar::new_object(avg);
    }

    /*
     * Calculates a mean and standard deviation of array elements.
     * @overload avg_sdv(mask = nil)
     * @param mask [CvMat] Optional operation mask.
     * @return [Array<CvScalar>] <tt>[mean, stddev]</tt>,
     *     where <tt>mean</tt> is the computed mean value and <tt>stddev</tt> is the computed standard deviation.
     * @opencv_func cvAvgSdv
     */
    VALUE
    rb_avg_sdv(int argc, VALUE *argv, VALUE self)
    {
      VALUE mask, mean, std_dev;
      rb_scan_args(argc, argv, "01", &mask);
      mean = cCvScalar::new_object();
      std_dev = cCvScalar::new_object();
      try {
	cvAvgSdv(CVARR(self), CVSCALAR(mean), CVSCALAR(std_dev), MASK(mask));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return rb_ary_new3(2, mean, std_dev);
    }

    /*
     * Calculates a standard deviation of array elements.
     * @overload sdv(mask = nil)
     * @param mask [CvMat] Optional operation mask.
     * @return [CvScalar] The standard deviation of array elements.
     * @opencv_func cvAvgSdv
     */
    VALUE
    rb_sdv(int argc, VALUE *argv, VALUE self)
    {
      VALUE mask, std_dev;
      rb_scan_args(argc, argv, "01", &mask);
      std_dev = cCvScalar::new_object();
      try {
	cvAvgSdv(CVARR(self), NULL, CVSCALAR(std_dev), MASK(mask));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return std_dev;
    }

    /*
     * Finds the global minimum and maximum in an array.
     *
     * @overload min_max_loc(mask = nil)
     * @param mask [CvMat] Optional mask used to select a sub-array.
     * @return [Array<Number, CvPoint>] <tt>[min_val, max_val, min_loc, max_loc]</tt>, where
     *   <tt>min_val</tt>, <tt>max_val</tt> are minimum, maximum values as <tt>Number</tt> and 
     *   <tt>min_loc</tt>, <tt>max_loc</tt> are minimum, maximum locations as <tt>CvPoint</tt>, respectively.
     * @opencv_func cvMinMaxLoc
     */
    VALUE
    rb_min_max_loc(int argc, VALUE *argv, VALUE self)
    {
      VALUE mask, min_loc, max_loc;
      double min_val = 0.0, max_val = 0.0;
      rb_scan_args(argc, argv, "01", &mask);
      min_loc = cCvPoint::new_object();
      max_loc = cCvPoint::new_object();
      try {
	cvMinMaxLoc(CVARR(self), &min_val, &max_val, CVPOINT(min_loc), CVPOINT(max_loc), MASK(mask));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return rb_ary_new3(4, rb_float_new(min_val), rb_float_new(max_val), min_loc, max_loc);
    }

    /*
     * Calculates an absolute array norm, an absolute difference norm, or a relative difference norm.
     *
     * @overload norm(src1, src2=nil, norm_type=NORM_L2, mask=nil)
     * @param src1 [CvMat] First input array.
     * @param src2 [CvMat] Second input array of the same size and the same type as <tt>src1</tt>.
     * @param norm_type [Integer] Type of the norm.
     * @param mask [CvMat] Optional operation mask; it must have the same size as <tt>src1</tt> and <tt>CV_8UC1</tt> type.
     * @return [Number] The norm of two arrays.
     * @opencv_func cvNorm
     * @scope class
     */
    VALUE
    rb_norm(int argc, VALUE *argv, VALUE self)
    {
      VALUE src1, src2, norm_type_val, mask_val;
      rb_scan_args(argc, argv, "13", &src1, &src2, &norm_type_val, &mask_val);

      CvMat *src1_ptr = NULL;
      CvMat *src2_ptr = NULL;
      int norm_type = NIL_P(norm_type_val) ? cv::NORM_L2 : NUM2INT(norm_type_val);
      CvMat *mask = NULL;
      double norm = 0.0;

      try {
	src1_ptr = CVMAT_WITH_CHECK(src1);
	if (!NIL_P(src2)) {
	  src2_ptr = CVMAT_WITH_CHECK(src2);
	}
	if (!NIL_P(mask_val)) {
	  mask = CVMAT_WITH_CHECK(mask_val);
	}
	norm = cvNorm(src1_ptr, src2_ptr, norm_type, mask);
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }

      return DBL2NUM(norm);
    }

    /*
     * Calculates the dot product of two arrays in Euclidean metrics.
     *
     * @overload dot_product(mat)
     * @param mat [CvMat] An array to calculate the dot product.
     * @return [Number] The dot product of two arrays.
     * @opencv_func cvDotProduct
     */
    VALUE
    rb_dot_product(VALUE self, VALUE mat)
    {
      double result = 0.0;
      try {
	result = cvDotProduct(CVARR(self), CVARR_WITH_CHECK(mat));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return rb_float_new(result);
    }

    /*
     * Calculates the cross product of two 3D vectors.
     *
     * @overload cross_product(mat)
     * @param mat [CvMat] A vector to calculate the cross product.
     * @return [CvMat] The cross product of two vectors.
     * @opencv_func cvCrossProduct
     */
    VALUE
    rb_cross_product(VALUE self, VALUE mat)
    {
      CvArr* self_ptr = CVARR(self);
      VALUE dest = Qnil;
      try {
	dest = new_mat_kind_object(cvGetSize(self_ptr), self);
	cvCrossProduct(self_ptr, CVARR_WITH_CHECK(mat), CVARR(dest));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return dest;
    }

    /*
     * Performs the matrix transformation of every array element.
     *
     * @overload transform(transmat, shiftvec = nil)
     * @param transmat [CvMat] Transformation 2x2 or 2x3 floating-point matrix.
     * @param shiftvec [CvMat] Optional translation vector.
     * @return [CvMat] Transformed array.
     * @opencv_func cvTransform
     */
    VALUE
    rb_transform(int argc, VALUE *argv, VALUE self)
    {
      VALUE transmat, shiftvec;
      rb_scan_args(argc, argv, "11", &transmat, &shiftvec);
      CvArr* self_ptr = CVARR(self);
      VALUE dest = Qnil;
      try {
	dest = new_mat_kind_object(cvGetSize(self_ptr), self);
	cvTransform(self_ptr, CVARR(dest), CVMAT_WITH_CHECK(transmat),
		    NIL_P(shiftvec) ? NULL : CVMAT_WITH_CHECK(shiftvec));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return dest;
    }

    /*
     * Performs the perspective matrix transformation of vectors.
     *
     * @overload perspective_transform(mat)
     * @param mat [CvMat] 3x3 or 4x4 floating-point transformation matrix.
     * @return [CvMat] Transformed vector.
     * @opencv_func cvPerspectiveTransform
     */
    VALUE
    rb_perspective_transform(VALUE self, VALUE mat)
    {
      CvArr* self_ptr = CVARR(self);
      VALUE dest = Qnil;
      try {
	dest = new_mat_kind_object(cvGetSize(self_ptr), self);
	cvPerspectiveTransform(self_ptr, CVARR(dest), CVMAT_WITH_CHECK(mat));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return dest;
    }

    /*
     * Calculates the product of a matrix and its transposition.
     *
     * This function calculates the product of <tt>self</tt> and its transposition:
     *   if :order = 0
     *     dst = scale * (self - delta) * (self - delta)T
     *   otherwise
     *     dst = scale * (self - delta)T * (self - delta)
     *
     * @overload mul_transposed(options)
     *   @param options [Hash] Options
     *   @option options [Integer] :order (0) Flag specifying the multiplication ordering, should be 0 or 1.
     *   @option options [CvMat] :delta (nil) Optional delta matrix subtracted from source before the multiplication.
     *   @option options [Number] :scale (1.0) Optional scale factor for the matrix product.
     * @return [CvMat] Result array.
     * @opencv_func cvMulTransposed
     */
    VALUE
    rb_mul_transposed(int argc, VALUE *argv, VALUE self)
    {
      VALUE options = Qnil;
      VALUE _delta = Qnil, _scale = Qnil, _order = Qnil;

      if (rb_scan_args(argc, argv, "01", &options) > 0) {
	Check_Type(options, T_HASH);
	_delta = LOOKUP_HASH(options, "delta");
	_scale = LOOKUP_HASH(options, "scale");
	_order = LOOKUP_HASH(options, "order");
      }

      CvArr* delta = NIL_P(_delta) ? NULL : CVARR_WITH_CHECK(_delta);
      double scale = NIL_P(_scale) ? 1.0 : NUM2DBL(_scale);
      int order = NIL_P(_order) ? 0 : NUM2INT(_order);
      CvArr* self_ptr = CVARR(self);
      VALUE dest = new_mat_kind_object(cvGetSize(self_ptr), self);
      try {
	cvMulTransposed(self_ptr, CVARR(dest), order, delta, scale);
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }

      return dest;
    }


    /*
     * Returns the trace of a matrix.
     *
     * @overload trace
     * @return [CvScalar] The trace of a matrix.
     * @opencv_func cvTrace
     */
    VALUE
    rb_trace(VALUE self)
    {
      CvScalar scalar;
      try {
	scalar = cvTrace(CVARR(self));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return cCvScalar::new_object(scalar);
    }

    /*
     * Transposes a matrix.
     *
     * @overload transpose
     * @return [CvMat] Transposed matrix.
     * @opencv_func cvTranspose
     */
    VALUE
    rb_transpose(VALUE self)
    {
      CvMat* self_ptr = CVMAT(self);
      VALUE dest = new_mat_kind_object(cvSize(self_ptr->rows, self_ptr->cols), self);
      try {
	cvTranspose(self_ptr, CVARR(dest));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return dest;
    }

    /*
     * Returns the determinant of a square floating-point matrix.
     *
     * @overload det
     * @return [Number] The determinant of the matrix.
     * @opencv_func cvDet
     */
    VALUE
    rb_det(VALUE self)
    {
      double det = 0.0;
      try {
	det = cvDet(CVARR(self));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return rb_float_new(det);
    }

    /*
     * Finds inverse or pseudo-inverse of matrix.
     *
     * @overload invert(inversion_method = :lu)
     * @param inversion_method [Symbol] Inversion method.
     *   * <tt>:lu</tt> - Gaussian elimincation with optimal pivot element chose.
     *   * <tt>:svd</tt> - Singular value decomposition(SVD) method.
     *   * <tt>:svd_sym</tt> - SVD method for a symmetric positively-defined matrix.
     * @return [Number] Inverse or pseudo-inverse of matrix.
     * @opencv_func cvInvert
     */
    VALUE
    rb_invert(int argc, VALUE *argv, VALUE self)
    {
      VALUE symbol;
      rb_scan_args(argc, argv, "01", &symbol);
      int method = CVMETHOD("INVERSION_METHOD", symbol, CV_LU);
      VALUE dest = Qnil;
      CvArr* self_ptr = CVARR(self);
      try {
	dest = new_mat_kind_object(cvGetSize(self_ptr), self);
	cvInvert(self_ptr, CVARR(dest), method);
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return dest;
    }

    /*
     * Solves one or more linear systems or least-squares problems.
     *
     * @overload solve(src1, src2, inversion_method = :lu)
     * @param src1 [CvMat] Input matrix on the left-hand side of the system.
     * @param src2 [CvMat] Input matrix on the right-hand side of the system.
     * @param inversion_method [Symbol] Inversion method.
     *   * <tt>:lu</tt> - Gaussian elimincation with optimal pivot element chose.
     *   * <tt>:svd</tt> - Singular value decomposition(SVD) method.
     *   * <tt>:svd_sym</tt> - SVD method for a symmetric positively-defined matrix.
     * @return [Number] Output solution.
     * @scope class
     * @opencv_func cvSolve
     */
    VALUE
    rb_solve(int argc, VALUE *argv, VALUE self)
    {
      VALUE src1, src2, symbol;
      rb_scan_args(argc, argv, "21", &src1, &src2, &symbol);
      VALUE dest = Qnil;
      CvArr* src2_ptr = CVARR_WITH_CHECK(src2);
      try {
	dest = new_mat_kind_object(cvGetSize(src2_ptr), src2);
	cvSolve(CVARR_WITH_CHECK(src1), src2_ptr, CVARR(dest), CVMETHOD("INVERSION_METHOD", symbol, CV_LU));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return dest;
    }

    /*
     * Performs SVD of a matrix
     * @overload svd(flag = 0)
     * @param flag [Integer] Operation flags.
     *   * <tt>CV_SVD_MODIFY_A</tt> - Use the algorithm to modify the decomposed matrix. It can save space and speed up processing.
     *   * <tt>CV_SVD_U_T</tt> - Indicate that only a vector of singular values <tt>w</tt> is to be computed, while <tt>u</tt> and <tt>v</tt> will be set to empty matrices.
     *   * <tt>CV_SVD_V_T</tt> - When the matrix is not square, by default the algorithm produces <tt>u</tt> and <tt>v</tt> matrices of sufficiently large size for the further A reconstruction. If, however, <tt>CV_SVD_V_T</tt> flag is specified, <tt>u</tt> and <tt>v</tt> will be full-size square orthogonal matrices.
     * @return [Array<CvMat>] Array of the computed values <tt>[w, u, v]</tt>, where
     *   * <tt>w</tt> - Computed singular values
     *   * <tt>u</tt> - Computed left singular vectors
     *   * <tt>v</tt> - Computed right singular vectors
     * @opencv_func cvSVD
     */
    VALUE
    rb_svd(int argc, VALUE *argv, VALUE self)
    {
      VALUE _flag = Qnil;
      int flag = 0;
      if (rb_scan_args(argc, argv, "01", &_flag) > 0) {
	flag = NUM2INT(_flag);
      }

      CvMat* self_ptr = CVMAT(self);
      VALUE w = new_mat_kind_object(cvSize(self_ptr->cols, self_ptr->rows), self);
  
      int rows = 0;
      int cols = 0;
      if (flag & CV_SVD_U_T) {
	rows = MIN(self_ptr->rows, self_ptr->cols);
	cols = self_ptr->rows;
      }
      else {
	rows = self_ptr->rows;
	cols = MIN(self_ptr->rows, self_ptr->cols);
      }
      VALUE u = new_mat_kind_object(cvSize(cols, rows), self);

      if (flag & CV_SVD_V_T) {
	rows = MIN(self_ptr->rows, self_ptr->cols);
	cols = self_ptr->cols;
      }
      else {
	rows = self_ptr->cols;
	cols = MIN(self_ptr->rows, self_ptr->cols);
      }
      VALUE v = new_mat_kind_object(cvSize(cols, rows), self);

      cvSVD(self_ptr, CVARR(w), CVARR(u), CVARR(v), flag);

      return rb_ary_new3(3, w, u, v);
    }

    /*
     * Computes eigenvalues and eigenvectors of symmetric matrix.
     * <i>self</i> should be symmetric square matrix. <i>self</i> is modified during the processing.
     *
     * @overload eigenvv
     * @return [Array<CvMat>] Array of <tt>[eigenvalues, eigenvectors]</tt>
     * @opencv_func cvEigenVV
     */
    VALUE
    rb_eigenvv(int argc, VALUE *argv, VALUE self)
    {
      VALUE epsilon, lowindex, highindex;
      rb_scan_args(argc, argv, "03", &epsilon, &lowindex, &highindex);
      double eps = (NIL_P(epsilon)) ? 0.0 : NUM2DBL(epsilon);
      int lowidx = (NIL_P(lowindex)) ? -1 : NUM2INT(lowindex);
      int highidx = (NIL_P(highindex)) ? -1 : NUM2INT(highindex);
      VALUE eigen_vectors = Qnil, eigen_values = Qnil;
      CvArr* self_ptr = CVARR(self);
      try {
	CvSize size = cvGetSize(self_ptr);
	int type = cvGetElemType(self_ptr);
	eigen_vectors = new_object(size, type);
	eigen_values = new_object(size.height, 1, type);
	// NOTE: eps, lowidx, highidx are ignored in the current OpenCV implementation.
	cvEigenVV(self_ptr, CVARR(eigen_vectors), CVARR(eigen_values), eps, lowidx, highidx);
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return rb_ary_new3(2, eigen_vectors, eigen_values);
    }


    /*
     * Performs a forward or inverse Discrete Fourier transform of a 1D or 2D floating-point array.
     *
     * @overload dft(flags = CV_DXT_FORWARD, nonzero_rows = 0)
     *   @param flags [Integer] transformation flags, representing a combination of the following values:
     *     * <tt>CV_DXT_FORWARD</tt> - Performs a 1D or 2D transform.
     *     * <tt>CV_DXT_INVERSE</tt> - Performs an inverse 1D or 2D transform instead of the default forward transform.
     *     * <tt>CV_DXT_SCALE</tt> - Scales the result: divide it by the number of array elements. 
     *       Normally, it is combined with <tt>CV_DXT_INVERSE</tt>.
     *     * <tt>CV_DXT_INV_SCALE</tt> - <tt>CV_DXT_INVERSE</tt> + <tt>CV_DXT_SCALE</tt>
     *   @param nonzero_rows [Integer] when the parameter is not zero, the function assumes that only
     *       the first <i>nonzero_rows</i> rows of the input array (CV_DXT_INVERSE is not set)
     *       or only the first nonzero_rows of the output array (CV_DXT_INVERSE is set) contain non-zeros.
     * @return [CvMat] Output array
     * @opencv_func cvDFT
     */
    VALUE
    rb_dft(int argc, VALUE *argv, VALUE self)
    {
      VALUE flag_value, nonzero_row_value;
      rb_scan_args(argc, argv, "02", &flag_value, &nonzero_row_value);

      int flags = NIL_P(flag_value) ? CV_DXT_FORWARD : NUM2INT(flag_value);
      int nonzero_rows = NIL_P(nonzero_row_value) ? 0 : NUM2INT(nonzero_row_value);
      CvArr* self_ptr = CVARR(self);
      VALUE dest = Qnil;
      try {
	dest = new_mat_kind_object(cvGetSize(self_ptr), self);
	cvDFT(self_ptr, CVARR(dest), flags, nonzero_rows);
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return dest;
    }

    /*
     * Performs forward or inverse Discrete Cosine Transform(DCT) of 1D or 2D floating-point array.
     *
     * @overload dct(flags = CV_DXT_FORWARD)
     *   @param flags [Integer] transformation flags, representing a combination of the following values:
     *     * <tt>CV_DXT_FORWARD</tt> - Performs a 1D or 2D transform.
     *     * <tt>CV_DXT_INVERSE</tt> - Performs an inverse 1D or 2D transform instead of the default forward transform.
     *     * <tt>CV_DXT_ROWS</tt> - Performs a forward or inverse transform of every individual row of the input matrix.
     *       This flag enables you to transform multiple vectors simultaneously and can be used to decrease
     *       the overhead (which is sometimes several times larger than the processing itself) to perform 3D
     *       and higher-dimensional transforms and so forth.
     * @return [CvMat] Output array
     * @opencv_func cvDCT
     */
    VALUE
    rb_dct(int argc, VALUE *argv, VALUE self)
    {
      VALUE flag_value;
      rb_scan_args(argc, argv, "01", &flag_value);

      int flags = NIL_P(flag_value) ? CV_DXT_FORWARD : NUM2INT(flag_value);
      CvArr* self_ptr = CVARR(self);
      VALUE dest = Qnil;
      try {
	dest = new_mat_kind_object(cvGetSize(self_ptr), self);
	cvDCT(self_ptr, CVARR(dest), flags);
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return dest;
    }

    /*
     * Returns an image that is drawn a line segment connecting two points.
     *
     * @overload line(p1, p2, options = nil)
     *   @param p1 [CvPoint] First point of the line segment.
     *   @param p2 [CvPoint] Second point of the line segment.
     *   @param options [Hash] Drawing options
     *   @option options [CvScalar] :color Line color.
     *   @option options [Integer] :thickness Line thickness.
     *   @option options [Integer] :line_type Type of the line.
     *     * 8 - 8-connected line.
     *     * 4 - 4-connected line.
     *     * <tt>CV_AA</tt> - Antialiased line.
     *   @option options [Integer] :shift Number of fractional bits in the point coordinates.
     * @return [CvMat] Output image
     * @opencv_func cvLine
     */
    VALUE
    rb_line(int argc, VALUE *argv, VALUE self)
    {
      return rb_line_bang(argc, argv, rb_clone(self));
    }

    /*
     * Draws a line segment connecting two points.
     *
     * @overload line!(p1, p2, options = nil)
     *   @param p1 [CvPoint] First point of the line segment.
     *   @param p2 [CvPoint] Second point of the line segment.
     *   @param options [Hash] Drawing options
     *   @option options [CvScalar] :color Line color.
     *   @option options [Integer] :thickness Line thickness.
     *   @option options [Integer] :line_type Type of the line.
     *     * 8 - 8-connected line.
     *     * 4 - 4-connected line.
     *     * <tt>CV_AA</tt> - Antialiased line.
     *   @option options [Integer] :shift Number of fractional bits in the point coordinates.
     * @return [CvMat] <tt>self</tt>
     * @opencv_func (see #line)
     */
    VALUE
    rb_line_bang(int argc, VALUE *argv, VALUE self)
    {
      VALUE p1, p2, drawing_option;
      rb_scan_args(argc, argv, "21", &p1, &p2, &drawing_option);
      drawing_option = DRAWING_OPTION(drawing_option);
      try {
	cvLine(CVARR(self), VALUE_TO_CVPOINT(p1), VALUE_TO_CVPOINT(p2),
	       DO_COLOR(drawing_option),
	       DO_THICKNESS(drawing_option),
	       DO_LINE_TYPE(drawing_option),
	       DO_SHIFT(drawing_option));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return self;
    }

    /*
     * Returns an image that is drawn a simple, thick, or filled up-right rectangle.
     *
     * @overload rectangle(p1, p2, options = nil)
     *   @param p1 [CvPoint] Vertex of the rectangle.
     *   @param p2 [CvPoint] Vertex of the rectangle opposite to <tt>p1</tt>.
     *   @param options [Hash] Drawing options
     *   @option options [CvScalar] :color Line color.
     *   @option options [Integer] :thickness Line thickness.
     *   @option options [Integer] :line_type Type of the line.
     *     * 8 - 8-connected line.
     *     * 4 - 4-connected line.
     *     * <tt>CV_AA</tt> - Antialiased line.
     *   @option options [Integer] :shift Number of fractional bits in the point coordinates.
     * @return [CvMat] Output image
     * @opencv_func cvRectangle
     */
    VALUE
    rb_rectangle(int argc, VALUE *argv, VALUE self)
    {
      return rb_rectangle_bang(argc, argv, rb_clone(self));
    }

    /*
     * Draws a simple, thick, or filled up-right rectangle.
     *
     * @overload rectangle!(p1, p2, options = nil)
     *   @param p1 [CvPoint] Vertex of the rectangle.
     *   @param p2 [CvPoint] Vertex of the rectangle opposite to <tt>p1</tt>.
     *   @param options [Hash] Drawing options
     *   @option options [CvScalar] :color Line color.
     *   @option options [Integer] :thickness Line thickness.
     *   @option options [Integer] :line_type Type of the line.
     *     * 8 - 8-connected line.
     *     * 4 - 4-connected line.
     *     * <tt>CV_AA</tt> - Antialiased line.
     *   @option options [Integer] :shift Number of fractional bits in the point coordinates.
     * @return [CvMat] <tt>self</tt>
     * @opencv_func cvRectangle
     */
    VALUE
    rb_rectangle_bang(int argc, VALUE *argv, VALUE self)
    {
      VALUE p1, p2, drawing_option;
      rb_scan_args(argc, argv, "21", &p1, &p2, &drawing_option);
      drawing_option = DRAWING_OPTION(drawing_option);
      try {
	cvRectangle(CVARR(self), VALUE_TO_CVPOINT(p1), VALUE_TO_CVPOINT(p2),
		    DO_COLOR(drawing_option),
		    DO_THICKNESS(drawing_option),
		    DO_LINE_TYPE(drawing_option),
		    DO_SHIFT(drawing_option));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return self;
    }

    /*
     * Returns an image that is drawn a circle
     *
     * @overload circle(center, radius, options = nil)
     *   @param center [CvPoint] Center of the circle.
     *   @param radius [Integer] Radius of the circle.
     *   @param options [Hash] Drawing options
     *   @option options [CvScalar] :color Line color.
     *   @option options [Integer] :thickness Line thickness.
     *   @option options [Integer] :line_type Type of the line.
     *     * 8 - 8-connected line.
     *     * 4 - 4-connected line.
     *     * <tt>CV_AA</tt> - Antialiased line.
     *   @option options [Integer] :shift Number of fractional bits in the point coordinates.
     * @return [CvMat] Output image
     * @opencv_func cvCircle
     */
    VALUE
    rb_circle(int argc, VALUE *argv, VALUE self)
    {
      return rb_circle_bang(argc, argv, rb_clone(self));
    }

    /*
     * Draws a circle
     *
     * @overload circle!(center, radius, options = nil)
     *   @param center [CvPoint] Center of the circle.
     *   @param radius [Integer] Radius of the circle.
     *   @param options [Hash] Drawing options
     *   @option options [CvScalar] :color Line color.
     *   @option options [Integer] :thickness Line thickness.
     *   @option options [Integer] :line_type Type of the line.
     *     * 8 - 8-connected line.
     *     * 4 - 4-connected line.
     *     * <tt>CV_AA</tt> - Antialiased line.
     *   @option options [Integer] :shift Number of fractional bits in the point coordinates.
     * @return [CvMat] <tt>self</tt>
     * @opencv_func cvCircle
     */
    VALUE
    rb_circle_bang(int argc, VALUE *argv, VALUE self)
    {
      VALUE center, radius, drawing_option;
      rb_scan_args(argc, argv, "21", &center, &radius, &drawing_option);
      drawing_option = DRAWING_OPTION(drawing_option);
      try {
	cvCircle(CVARR(self), VALUE_TO_CVPOINT(center), NUM2INT(radius),
		 DO_COLOR(drawing_option),
		 DO_THICKNESS(drawing_option),
		 DO_LINE_TYPE(drawing_option),
		 DO_SHIFT(drawing_option));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return self;
    }

    /*
     * Returns an image that is drawn a simple or thick elliptic arc or fills an ellipse sector.
     *
     * @overload ellipse(center, axes, angle, start_angle, end_angle, options = nil)
     *   @param center [CvPoint] Center of the ellipse.
     *   @param axes [CvSize] Length of the ellipse axes.
     *   @param angle [Number] Ellipse rotation angle in degrees.
     *   @param start_angle [Number] Starting angle of the elliptic arc in degrees.
     *   @param end_angle [Number] Ending angle of the elliptic arc in degrees.
     *   @param options [Hash] Drawing options
     *   @option options [CvScalar] :color Line color.
     *   @option options [Integer] :thickness Line thickness.
     *   @option options [Integer] :line_type Type of the line.
     *     * 8 - 8-connected line.
     *     * 4 - 4-connected line.
     *     * <tt>CV_AA</tt> - Antialiased line.
     *   @option options [Integer] :shift Number of fractional bits in the point coordinates.
     * @return [CvMat] Output image
     * @opencv_func cvEllipse
     */
    VALUE
    rb_ellipse(int argc, VALUE *argv, VALUE self)
    {
      return rb_ellipse_bang(argc, argv, rb_clone(self));
    }

    /*
     * Draws a simple or thick elliptic arc or fills an ellipse sector.
     *
     * @overload ellipse!(center, axes, angle, start_angle, end_angle, options = nil)
     *   @param center [CvPoint] Center of the ellipse.
     *   @param axes [CvSize] Length of the ellipse axes.
     *   @param angle [Number] Ellipse rotation angle in degrees.
     *   @param start_angle [Number] Starting angle of the elliptic arc in degrees.
     *   @param end_angle [Number] Ending angle of the elliptic arc in degrees.
     *   @param options [Hash] Drawing options
     *   @option options [CvScalar] :color Line color.
     *   @option options [Integer] :thickness Line thickness.
     *   @option options [Integer] :line_type Type of the line.
     *     * 8 - 8-connected line.
     *     * 4 - 4-connected line.
     *     * <tt>CV_AA</tt> - Antialiased line.
     *   @option options [Integer] :shift Number of fractional bits in the point coordinates.
     * @return [CvMat] <tt>self</tt>
     * @opencv_func cvEllipse
     */
    VALUE
    rb_ellipse_bang(int argc, VALUE *argv, VALUE self)
    {
      VALUE center, axis, angle, start_angle, end_angle, drawing_option;
      rb_scan_args(argc, argv, "51", &center, &axis, &angle, &start_angle, &end_angle, &drawing_option);
      drawing_option = DRAWING_OPTION(drawing_option);
      try {
	cvEllipse(CVARR(self), VALUE_TO_CVPOINT(center),
		  VALUE_TO_CVSIZE(axis),
		  NUM2DBL(angle), NUM2DBL(start_angle), NUM2DBL(end_angle),
		  DO_COLOR(drawing_option),
		  DO_THICKNESS(drawing_option),
		  DO_LINE_TYPE(drawing_option),
		  DO_SHIFT(drawing_option));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return self;
    }

    /*
     * Returns an image that is drawn a simple or thick elliptic arc or fills an ellipse sector.
     *
     * @overload ellipse_box(box, options = nil)
     *   @param box [CvBox2D] Alternative ellipse representation via <tt>CvBox2D</tt>. This means that
     *     the function draws an ellipse inscribed in the rotated rectangle.
     *   @param options [Hash] Drawing options
     *   @option options [CvScalar] :color Line color.
     *   @option options [Integer] :thickness Line thickness.
     *   @option options [Integer] :line_type Type of the line.
     *     * 8 - 8-connected line.
     *     * 4 - 4-connected line.
     *     * <tt>CV_AA</tt> - Antialiased line.
     *   @option options [Integer] :shift Number of fractional bits in the point coordinates.
     * @return [CvMat] Output image
     * @opencv_func cvEllipseBox
     */
    VALUE
    rb_ellipse_box(int argc, VALUE *argv, VALUE self)
    {
      return rb_ellipse_box_bang(argc, argv, rb_clone(self));
    }

    /*
     * Draws a simple or thick elliptic arc or fills an ellipse sector.
     *
     * @overload ellipse_box!(box, options = nil)
     *   @param box [CvBox2D] Alternative ellipse representation via <tt>CvBox2D</tt>. This means that
     *     the function draws an ellipse inscribed in the rotated rectangle.
     *   @param options [Hash] Drawing options
     *   @option options [CvScalar] :color Line color.
     *   @option options [Integer] :thickness Line thickness.
     *   @option options [Integer] :line_type Type of the line.
     *     * 8 - 8-connected line.
     *     * 4 - 4-connected line.
     *     * <tt>CV_AA</tt> - Antialiased line.
     *   @option options [Integer] :shift Number of fractional bits in the point coordinates.
     * @return [CvMat] <tt>self</tt>
     * @opencv_func cvEllipseBox
     */
    VALUE
    rb_ellipse_box_bang(int argc, VALUE *argv, VALUE self)
    {
      VALUE box, drawing_option;
      rb_scan_args(argc, argv, "11", &box, &drawing_option);
      drawing_option = DRAWING_OPTION(drawing_option);
      try {
	cvEllipseBox(CVARR(self), VALUE_TO_CVBOX2D(box),
		     DO_COLOR(drawing_option),
		     DO_THICKNESS(drawing_option),
		     DO_LINE_TYPE(drawing_option),
		     DO_SHIFT(drawing_option));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return self;
    }

    /*
     * Returns an image that is filled the area bounded by one or more polygons.
     *
     * @overload fill_poly(points, options = nil)
     *   @param points [Array<CvPoint>] Array of polygons where each polygon is represented as an array of points.
     *   @param options [Hash] Drawing options
     *   @option options [CvScalar] :color Line color.
     *   @option options [Integer] :thickness Line thickness.
     *   @option options [Integer] :line_type Type of the line.
     *     * 8 - 8-connected line.
     *     * 4 - 4-connected line.
     *     * <tt>CV_AA</tt> - Antialiased line.
     *   @option options [Integer] :shift Number of fractional bits in the point coordinates.
     * @return [CvMat] Output image
     * @opencv_func cvFillPoly
     */
    VALUE
    rb_fill_poly(int argc, VALUE *argv, VALUE self)
    {
      return rb_fill_poly_bang(argc, argv, self);
    }

    /*
     * Fills the area bounded by one or more polygons.
     *
     * @overload fill_poly!(points, options = nil)
     *   @param points [Array<CvPoint>] Array of polygons where each polygon is represented as an array of points.
     *   @param options [Hash] Drawing options
     *   @option options [CvScalar] :color Line color.
     *   @option options [Integer] :thickness Line thickness.
     *   @option options [Integer] :line_type Type of the line.
     *     * 8 - 8-connected line.
     *     * 4 - 4-connected line.
     *     * <tt>CV_AA</tt> - Antialiased line.
     *   @option options [Integer] :shift Number of fractional bits in the point coordinates.
     * @return [CvMat] <tt>self</tt>
     * @opencv_func cvFillPoly
     */
    VALUE
    rb_fill_poly_bang(int argc, VALUE *argv, VALUE self)
    {
      VALUE polygons, drawing_option;
      VALUE points;
      int i, j;
      int num_polygons;
      int *num_points;
      CvPoint **p;

      rb_scan_args(argc, argv, "11", &polygons, &drawing_option);
      Check_Type(polygons, T_ARRAY);
      drawing_option = DRAWING_OPTION(drawing_option);
      num_polygons = RARRAY_LEN(polygons);
      num_points = ALLOCA_N(int, num_polygons);

      p = ALLOCA_N(CvPoint*, num_polygons);
      for (j = 0; j < num_polygons; ++j) {
	points = rb_ary_entry(polygons, j);
	Check_Type(points, T_ARRAY);
	num_points[j] = RARRAY_LEN(points);
	p[j] = ALLOCA_N(CvPoint, num_points[j]);
	for (i = 0; i < num_points[j]; ++i) {
	  p[j][i] = VALUE_TO_CVPOINT(rb_ary_entry(points, i));
	}
      }
      try {
	cvFillPoly(CVARR(self), p, num_points, num_polygons,
		   DO_COLOR(drawing_option),
		   DO_LINE_TYPE(drawing_option),
		   DO_SHIFT(drawing_option));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return self;
    }

    /*
     * Returns an image that is filled a convex polygon.
     *
     * @overload fill_convex_poly(points, options = nil)
     *   @param points [Array<CvPoint>] Polygon vertices.
     *   @param options [Hash] Drawing options
     *   @option options [CvScalar] :color Line color.
     *   @option options [Integer] :thickness Line thickness.
     *   @option options [Integer] :line_type Type of the line.
     *     * 8 - 8-connected line.
     *     * 4 - 4-connected line.
     *     * <tt>CV_AA</tt> - Antialiased line.
     *   @option options [Integer] :shift Number of fractional bits in the point coordinates.
     * @return [CvMat] Output image
     * @opencv_func cvFillConvexPoly
     */
    VALUE
    rb_fill_convex_poly(int argc, VALUE *argv, VALUE self)
    {
      return rb_fill_convex_poly_bang(argc, argv, rb_clone(self));
    }

    /*
     * Fills a convex polygon.
     *
     * @overload fill_convex_poly!(points, options = nil)
     *   @param points [Array<CvPoint>] Polygon vertices.
     *   @param options [Hash] Drawing options
     *   @option options [CvScalar] :color Line color.
     *   @option options [Integer] :thickness Line thickness.
     *   @option options [Integer] :line_type Type of the line.
     *     * 8 - 8-connected line.
     *     * 4 - 4-connected line.
     *     * <tt>CV_AA</tt> - Antialiased line.
     *   @option options [Integer] :shift Number of fractional bits in the point coordinates.
     * @return [CvMat] <tt>self</tt>
     * @opencv_func cvFillConvexPoly
     */
    VALUE
    rb_fill_convex_poly_bang(int argc, VALUE *argv, VALUE self)
    {
      VALUE points, drawing_option;
      int i, num_points;
      CvPoint *p;

      rb_scan_args(argc, argv, "11", &points, &drawing_option);
      Check_Type(points, T_ARRAY);
      drawing_option = DRAWING_OPTION(drawing_option);
      num_points = RARRAY_LEN(points);
      p = ALLOCA_N(CvPoint, num_points);
      for (i = 0; i < num_points; ++i)
	p[i] = VALUE_TO_CVPOINT(rb_ary_entry(points, i));

      try {
	cvFillConvexPoly(CVARR(self), p, num_points,
			 DO_COLOR(drawing_option),
			 DO_LINE_TYPE(drawing_option),
			 DO_SHIFT(drawing_option));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return self;
    }

    /*
     * Returns an image that is drawn several polygonal curves.
     *
     * @overload poly_line(points, options = nil)
     *   @param points [Array<CvPoint>] Array of polygonal curves.
     *   @param options [Hash] Drawing options
     *   @option options [CvScalar] :color Line color.
     *   @option options [Integer] :thickness Line thickness.
     *   @option options [Integer] :line_type Type of the line.
     *     * 8 - 8-connected line.
     *     * 4 - 4-connected line.
     *     * <tt>CV_AA</tt> - Antialiased line.
     *   @option options [Boolean] :is_closed
     *     Indicates whether the polylines must be drawn closed.
     *     If closed, the method draws the line from the last vertex
     *     of every contour to the first vertex.
     *   @option options [Integer] :shift Number of fractional bits in the point coordinates.
     * @return [CvMat] Output image
     * @opencv_func cvPolyLine
     */
    VALUE
    rb_poly_line(int argc, VALUE *argv, VALUE self)
    {
      return rb_poly_line_bang(argc, argv, rb_clone(self));
    }

    /*
     * Draws several polygonal curves.
     *
     * @overload poly_line!(points, options = nil)
     *   @param points [Array<CvPoint>] Array of polygonal curves.
     *   @param options [Hash] Drawing options
     *   @option options [CvScalar] :color Line color.
     *   @option options [Integer] :thickness Line thickness.
     *   @option options [Integer] :line_type Type of the line.
     *     * 8 - 8-connected line.
     *     * 4 - 4-connected line.
     *     * <tt>CV_AA</tt> - Antialiased line.
     *   @option options [Boolean] :is_closed
     *     Indicates whether the polylines must be drawn closed.
     *     If closed, the method draws the line from the last vertex
     *     of every contour to the first vertex.
     *   @option options [Integer] :shift Number of fractional bits in the point coordinates.
     * @return [CvMat] <tt>self</tt>
     * @opencv_func cvPolyLine
     */
    VALUE
    rb_poly_line_bang(int argc, VALUE *argv, VALUE self)
    {
      VALUE polygons, drawing_option;
      VALUE points;
      int i, j;
      int num_polygons;
      int *num_points;
      CvPoint **p;

      rb_scan_args(argc, argv, "11", &polygons, &drawing_option);
      Check_Type(polygons, T_ARRAY);
      drawing_option = DRAWING_OPTION(drawing_option);
      num_polygons = RARRAY_LEN(polygons);
      num_points = ALLOCA_N(int, num_polygons);
      p = ALLOCA_N(CvPoint*, num_polygons);

      for (j = 0; j < num_polygons; ++j) {
	points = rb_ary_entry(polygons, j);
	Check_Type(points, T_ARRAY);
	num_points[j] = RARRAY_LEN(points);
	p[j] = ALLOCA_N(CvPoint, num_points[j]);
	for (i = 0; i < num_points[j]; ++i) {
	  p[j][i] = VALUE_TO_CVPOINT(rb_ary_entry(points, i));
	}
      }

      try {
	cvPolyLine(CVARR(self), p, num_points, num_polygons,
		   DO_IS_CLOSED(drawing_option),
		   DO_COLOR(drawing_option),
		   DO_THICKNESS(drawing_option),
		   DO_LINE_TYPE(drawing_option),
		   DO_SHIFT(drawing_option));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }

      return self;
    }


    /*
     * Returns an image which is drawn a text string.
     *
     * @overload put_text(text, org, font, color = CvColor::Black)
     * @param text [String] Text string to be drawn.
     * @param org [CvPoint] Bottom-left corner of the text string in the image.
     * @param font [CvFont] <tt>CvFont</tt> object.
     * @param color [CvScalar] Text color.
     * @return [CvMat] Output image
     * @opencv_func cvPutText
     */
    VALUE
    rb_put_text(int argc, VALUE* argv, VALUE self)
    {
      return rb_put_text_bang(argc, argv, rb_clone(self));
    }

    /*
     * Draws a text string.
     *
     * @overload put_text!(text, org, font, color = CvColor::Black)
     * @param text [String] Text string to be drawn.
     * @param org [CvPoint] Bottom-left corner of the text string in the image.
     * @param font [CvFont] <tt>CvFont</tt> object.
     * @param color [CvScalar] Text color.
     * @return [CvMat] <tt>self</tt>
     * @opencv_func cvPutText
     */
    VALUE
    rb_put_text_bang(int argc, VALUE* argv, VALUE self)
    {
      VALUE _text, _point, _font, _color;
      rb_scan_args(argc, argv, "31", &_text, &_point, &_font, &_color);
      CvScalar color = NIL_P(_color) ? CV_RGB(0, 0, 0) : VALUE_TO_CVSCALAR(_color);
      try {
	cvPutText(CVARR(self), StringValueCStr(_text), VALUE_TO_CVPOINT(_point),
		  CVFONT_WITH_CHECK(_font), color);
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return self;
    }

    /*
     * Calculates the first, second, third, or mixed image derivatives using an extended Sobel operator.
     *
     * @overload sobel(xorder, yorder, aperture_size = 3)
     * @param xorder [Integer] Order of the derivative x.
     * @param yorder [Integer] Order of the derivative y.
     * @param aperture_size [Integer] Size of the extended Sobel kernel; it must be 1, 3, 5, or 7.
     * @return [CvMat] Output image.
     * @opencv_func cvSovel
     */
    VALUE
    rb_sobel(int argc, VALUE *argv, VALUE self)
    {
      VALUE xorder, yorder, aperture_size, dest;
      if (rb_scan_args(argc, argv, "21", &xorder, &yorder, &aperture_size) < 3)
	aperture_size = INT2FIX(3);
      CvMat* self_ptr = CVMAT(self);
      switch(CV_MAT_DEPTH(self_ptr->type)) {
      case CV_8U:
	dest = new_mat_kind_object(cvGetSize(self_ptr), self, CV_16S, 1);
	break;
      case CV_32F:
	dest = new_mat_kind_object(cvGetSize(self_ptr), self, CV_32F, 1);
	break;
      default:
	rb_raise(rb_eArgError, "source depth should be CV_8U or CV_32F.");
	break;
      }

      try {
	cvSobel(self_ptr, CVARR(dest), NUM2INT(xorder), NUM2INT(yorder), NUM2INT(aperture_size));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return dest;
    }

    /*
     * Calculates the Laplacian of an image.
     *
     * @overload laplace(aperture_size = 3)
     * @param aperture_size [Integer] Aperture size used to compute the second-derivative filters.
     *     The size must be positive and odd.
     * @return Output image.
     * @opencv_func cvLaplace
     */
    VALUE
    rb_laplace(int argc, VALUE *argv, VALUE self)
    {
      VALUE aperture_size, dest;
      if (rb_scan_args(argc, argv, "01", &aperture_size) < 1)
	aperture_size = INT2FIX(3);
      CvMat* self_ptr = CVMAT(self);
      switch(CV_MAT_DEPTH(self_ptr->type)) {
      case CV_8U:
	dest = new_mat_kind_object(cvGetSize(self_ptr), self, CV_16S, 1);
	break;
      case CV_32F:
	dest = new_mat_kind_object(cvGetSize(self_ptr), self, CV_32F, 1);
	break;
      default:
	rb_raise(rb_eArgError, "source depth should be CV_8U or CV_32F.");
      }

      try {
	cvLaplace(self_ptr, CVARR(dest), NUM2INT(aperture_size));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return dest;
    }

    /*
     * Finds edges in an image using the [Canny86] algorithm.
     *
     * Canny86: J. Canny. A Computational Approach to Edge Detection, IEEE Trans. on Pattern Analysis
     * and Machine Intelligence, 8(6), pp. 679-698 (1986).
     *
     * @overload canny(thresh1, thresh2, aperture_size = 3)
     * @param thresh1 [Number] First threshold for the hysteresis procedure.
     * @param thresh2 [Number] Second threshold for the hysteresis procedure.
     * @param aperture_size [Integer] Aperture size for the sobel operator.
     * @return [CvMat] Output edge map
     * @opencv_func cvCanny
     */
    VALUE
    rb_canny(int argc, VALUE *argv, VALUE self)
    {
      VALUE thresh1, thresh2, aperture_size;
      if (rb_scan_args(argc, argv, "21", &thresh1, &thresh2, &aperture_size) < 3)
	aperture_size = INT2FIX(3);
      CvArr* self_ptr = CVARR(self);
      VALUE dest = new_mat_kind_object(cvGetSize(self_ptr), self);
  
      try {
	cvCanny(self_ptr, CVARR(dest), NUM2INT(thresh1), NUM2INT(thresh2), NUM2INT(aperture_size));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return dest;
    }

    /*
     * Calculates a feature map for corner detection.
     *
     * @overload pre_corner_detect(aperture_size = 3)
     * @param aperture_size [Integer] Aperture size for the sobel operator.
     * @return [CvMat] Output image
     * @opencv_func cvPreCornerDetect
     */
    VALUE
    rb_pre_corner_detect(int argc, VALUE *argv, VALUE self)
    {
      VALUE aperture_size, dest = Qnil;
      if (rb_scan_args(argc, argv, "01", &aperture_size) < 1)
	aperture_size = INT2FIX(3);

      CvArr *self_ptr = CVARR(self);
      try {
	dest = new_object(cvGetSize(self_ptr), CV_MAKETYPE(CV_32F, 1));
	cvPreCornerDetect(self_ptr, CVARR(dest), NUM2INT(aperture_size));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return dest;
    }

    /*
     * Calculates eigenvalues and eigenvectors of image blocks for corner detection.
     *
     * @overload corner_eigenvv(block_size, aperture_size = 3)
     * @param block_size [Integer] Neighborhood size.
     * @param aperture_size [Integer] Aperture parameter for the sobel operator.
     * @return [CvMat] Result array.
     * @opencv_func cvCornerEigenValsAndVecs
     */
    VALUE
    rb_corner_eigenvv(int argc, VALUE *argv, VALUE self)
    {
      VALUE block_size, aperture_size, dest;
      if (rb_scan_args(argc, argv, "11", &block_size, &aperture_size) < 2)
	aperture_size = INT2FIX(3);
      CvMat* self_ptr = CVMAT(self);
      dest = new_object(cvSize(self_ptr->cols * 6, self_ptr->rows), CV_MAKETYPE(CV_32F, 1));
      try {
	cvCornerEigenValsAndVecs(self_ptr, CVARR(dest), NUM2INT(block_size), NUM2INT(aperture_size));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return dest;
    }

    /*
     * Calculates the minimal eigenvalue of gradient matrices for corner detection.
     *
     * @overload corner_min_eigen_val(block_size, aperture_size = 3)
     * @param block_size [Integer] Neighborhood size.
     * @param aperture_size [Integer] Aperture parameter for the sobel operator.
     * @return [CvMat] Result array.
     * @opencv_func cvCornerMinEigenVal
     */
    VALUE
    rb_corner_min_eigen_val(int argc, VALUE *argv, VALUE self)
    {
      VALUE block_size, aperture_size, dest;
      if (rb_scan_args(argc, argv, "11", &block_size, &aperture_size) < 2)
	aperture_size = INT2FIX(3);
      CvArr* self_ptr = CVARR(self);
      dest = new_object(cvGetSize(self_ptr), CV_MAKETYPE(CV_32F, 1));
      try {
	cvCornerMinEigenVal(self_ptr, CVARR(dest), NUM2INT(block_size), NUM2INT(aperture_size));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return dest;
    }

    /*
     * Harris edge detector.
     *
     * @overload corner_harris(block_size, aperture_size = 3, k = 0.04)
     * @param block_size [Integer] Neighborhood size.
     * @param aperture_size [Integer] Aperture parameter for the sobel operator.
     * @param k [Number] Harris detector free parameter.
     * @return [CvMat] The Harris detector responses.
     * @opencv_func cvCornerHarris
     */
    VALUE
    rb_corner_harris(int argc, VALUE *argv, VALUE self)
    {
      VALUE block_size, aperture_size, k, dest;
      rb_scan_args(argc, argv, "12", &block_size, &aperture_size, &k);
      CvArr* self_ptr = CVARR(self);
      dest = new_object(cvGetSize(self_ptr), CV_MAKETYPE(CV_32F, 1));
      try {
	cvCornerHarris(self_ptr, CVARR(dest), NUM2INT(block_size), IF_INT(aperture_size, 3), IF_DBL(k, 0.04));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return dest;
    }

    /*
     * Finds the positions of internal corners of the chessboard.
     *
     * @overload find_chessboard_corners(pattern_size, flag = CV_CALIB_CB_ADAPTIVE_THRESH)
     * @param pattern_size [CvSize] Number of inner corners per a chessboard row and column.
     * @param flags [Integer] Various operation flags that can be zero or a combination of the following values.
     *   * CV_CALIB_CB_ADAPTIVE_THRESH
     *     * Use adaptive thresholding to convert the image to black and white, rather than
     *       a fixed threshold level (computed from the average image brightness).
     *   * CV_CALIB_CB_NORMALIZE_IMAGE
     *     * Normalize the image gamma with CvMat#equalize_hist() before applying fixed or adaptive thresholding.
     *   * CV_CALIB_CB_FILTER_QUADS
     *     * Use additional criteria (like contour area, perimeter, square-like shape) to
     *       filter out false quads extracted at the contour retrieval stage.
     *   * CALIB_CB_FAST_CHECK
     *     * Run a fast check on the image that looks for chessboard corners, and shortcut the call
     *       if none is found. This can drastically speed up the call in the degenerate condition
     *       when no chessboard is observed.
     * @return [Array<Array<CvPoint2D32f>, Boolean>] An array which includes the positions of internal corners
     *     of the chessboard, and a parameter indicating whether the complete board was found or not.
     * @opencv_func cvFindChessboardCorners
     * @example
     *   mat = CvMat.load('chessboard.jpg', 1)
     *   gray = mat.BGR2GRAY
     *   pattern_size = CvSize.new(4, 4)
     *   corners, found = gray.find_chessboard_corners(pattern_size, CV_CALIB_CB_ADAPTIVE_THRESH)
     *
     *   if found
     *     corners = gray.find_corner_sub_pix(corners, CvSize.new(3, 3), CvSize.new(-1, -1), CvTermCriteria.new(20, 0.03))
     *   end
     *
     *   result = mat.draw_chessboard_corners(pattern_size, corners, found)
     *   w = GUI::Window.new('Result')
     *   w.show result
     *   GUI::wait_key
     */
    VALUE
    rb_find_chessboard_corners(int argc, VALUE *argv, VALUE self)
    {
      VALUE pattern_size_val, flag_val;
      rb_scan_args(argc, argv, "11", &pattern_size_val, &flag_val);

      int flag = NIL_P(flag_val) ? CV_CALIB_CB_ADAPTIVE_THRESH : NUM2INT(flag_val);
      CvSize pattern_size = VALUE_TO_CVSIZE(pattern_size_val);
      CvPoint2D32f* corners = ALLOCA_N(CvPoint2D32f, pattern_size.width * pattern_size.height);
      int num_found_corners = 0;
      int pattern_was_found = 0;
      try {
	pattern_was_found = cvFindChessboardCorners(CVARR(self), pattern_size, corners, &num_found_corners, flag);
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }

      VALUE found_corners = rb_ary_new2(num_found_corners);
      for (int i = 0; i < num_found_corners; i++) {
	rb_ary_store(found_corners, i, cCvPoint2D32f::new_object(corners[i]));
      }

      VALUE found = (pattern_was_found > 0) ? Qtrue : Qfalse;
      return rb_assoc_new(found_corners, found);
    }

    /*
     * Refines the corner locations.
     * 
     * @overload find_corner_sub_pix(corners, win_size, zero_zone, criteria)
     * @param corners [Array<CvPoint>] Initial coordinates of the input corners.
     * @param win_size [CvSize] Half of the side length of the search window.
     * @param zero_zone [CvSize] Half of the size of the dead region in the middle of the search zone over
     *   which the summation in the formula below is not done.
     * @param criteria [CvTermCriteria] Criteria for termination of the iterative process of corner refinement.
     * @return [Array<CvPoint2D32f>] Refined corner coordinates.
     * @opencv_func cvFindCornerSubPix
     */
    VALUE
    rb_find_corner_sub_pix(VALUE self, VALUE corners, VALUE win_size, VALUE zero_zone, VALUE criteria)
    {
      Check_Type(corners, T_ARRAY);
      int count = RARRAY_LEN(corners);
      CvPoint2D32f* corners_buff = ALLOCA_N(CvPoint2D32f, count);
      VALUE* corners_ptr = RARRAY_PTR(corners);

      for (int i = 0; i < count; i++) {
	corners_buff[i] = *(CVPOINT2D32F(corners_ptr[i]));
      }

      try {
	cvFindCornerSubPix(CVARR(self), corners_buff, count, VALUE_TO_CVSIZE(win_size),
			   VALUE_TO_CVSIZE(zero_zone), VALUE_TO_CVTERMCRITERIA(criteria));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }

      VALUE refined_corners = rb_ary_new2(count);
      for (int i = 0; i < count; i++) {
	rb_ary_store(refined_corners, i, cCvPoint2D32f::new_object(corners_buff[i]));
      }

      return refined_corners;
    }

    /*
     * Determines strong corners on an image.
     *
     * @overload good_features_to_track(quality_level, min_distance, good_features_to_track_option = {})
     *   @param quality_level [Number] Parameter characterizing the minimal accepted quality of image corners.
     *     The parameter value is multiplied by the best corner quality measure, which is the minimal eigenvalue
     *     or the Harris function response.
     *   @param min_distance [Number] Minimum possible Euclidean distance between the returned corners.
     *   @param good_features_to_track_option [Hash] Options.
     *   @option good_features_to_track_option [CvMat] :mask (nil) Optional region of interest.
     *     If the image is not empty (it needs to have the type CV_8UC1 and the same size as image),
     *     it specifies the region in which the corners are detected.
     *   @option good_features_to_track_option [Integer] :block_size (3) Size of an average block for computing
     *     a derivative covariation matrix over each pixel neighborhood.
     *   @option good_features_to_track_option [Boolean] :use_harris (false) Parameter indicating whether
     *     to use a Harris detector.
     *   @option good_features_to_track_option [Number] :k (0.04) Free parameter of the Harris detector.
     * @return [Array<CvPoint2D32f>] Output vector of detected corners.
     * @opencv_func cvGoodFeaturesToTrack
     */
    VALUE
    rb_good_features_to_track(int argc, VALUE *argv, VALUE self)
    {
      VALUE quality_level, min_distance, good_features_to_track_option;
      rb_scan_args(argc, argv, "21", &quality_level, &min_distance, &good_features_to_track_option);
      good_features_to_track_option = GOOD_FEATURES_TO_TRACK_OPTION(good_features_to_track_option);
      int np = GF_MAX(good_features_to_track_option);
      if (np <= 0)
	rb_raise(rb_eArgError, "option :max should be positive value.");

      CvMat *self_ptr = CVMAT(self);
      CvPoint2D32f *p32 = (CvPoint2D32f*)rb_cvAlloc(sizeof(CvPoint2D32f) * np);
      int type = CV_MAKETYPE(CV_32F, 1);
      CvMat* eigen = rb_cvCreateMat(self_ptr->rows, self_ptr->cols, type);
      CvMat* tmp = rb_cvCreateMat(self_ptr->rows, self_ptr->cols, type);
      try {
	cvGoodFeaturesToTrack(self_ptr, &eigen, &tmp, p32, &np, NUM2DBL(quality_level), NUM2DBL(min_distance),
			      GF_MASK(good_features_to_track_option),
			      GF_BLOCK_SIZE(good_features_to_track_option),
			      GF_USE_HARRIS(good_features_to_track_option),
			      GF_K(good_features_to_track_option));
      }
      catch (cv::Exception& e) {
	if (eigen != NULL)
	  cvReleaseMat(&eigen);
	if (tmp != NULL)
	  cvReleaseMat(&tmp);
	if (p32 != NULL)
	  cvFree(&p32);
	raise_cverror(e);
      }
      VALUE corners = rb_ary_new2(np);
      for (int i = 0; i < np; ++i)
	rb_ary_store(corners, i, cCvPoint2D32f::new_object(p32[i]));
      cvFree(&p32);
      cvReleaseMat(&eigen);
      cvReleaseMat(&tmp);
      return corners;
    }

    /*
     * Retrieves a pixel rectangle from an image with sub-pixel accuracy.
     *
     * @overload rect_sub_pix(center, size = self.size)
     * @param center [CvPoint2D32f] Floating point coordinates of the center of the extracted rectangle within
     *     the source image. The center must be inside the image.
     * @param size [CvSize] Size of the extracted patch.
     * @return [CvMat] Extracted patch that has the size <tt>size</tt> and the same number of channels as <tt>self</tt>.
     * @opencv_func cvGetRectSubPix
     */
    VALUE
    rb_rect_sub_pix(int argc, VALUE *argv, VALUE self)
    {
      VALUE center, size;
      VALUE dest = Qnil;
      CvSize _size;
      CvArr* self_ptr = CVARR(self);
      try {
	if (rb_scan_args(argc, argv, "11", &center, &size) < 2)
	  _size = cvGetSize(self_ptr);
	else
	  _size = VALUE_TO_CVSIZE(size);
	dest = new_mat_kind_object(_size, self);
	cvGetRectSubPix(self_ptr, CVARR(dest), VALUE_TO_CVPOINT2D32F(center));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return dest;
    }

    /*
     * Applies an affine transformation to an image.
     * 
     * @overload quadrangle_sub_pix(map_matrix, size = self.size)
     * @param map_matrix [CvMat] 2x3 transformation matrix.
     * @param size [CvSize] Size of the output image.
     * @return [CvMat] Output image that has the size <tt>size</tt> and the same type as <tt>self</tt>.
     * @opencv_func cvGetQuadrangleSubPix
     * @note <tt>CvMat#quadrangle_sub_pix</tt> is similar to <tt>CvMat#warp_affine</tt>, but the outliers are
     *     extrapolated using replication border mode.
     */
    VALUE
    rb_quadrangle_sub_pix(int argc, VALUE *argv, VALUE self)
    {
      VALUE map_matrix, size;
      VALUE dest = Qnil;
      CvSize _size;
      CvArr* self_ptr = CVARR(self);
      try {
	if (rb_scan_args(argc, argv, "11", &map_matrix, &size) < 2)
	  _size = cvGetSize(self_ptr);
	else
	  _size = VALUE_TO_CVSIZE(size);
	dest = new_mat_kind_object(_size, self);
	cvGetQuadrangleSubPix(self_ptr, CVARR(dest), CVMAT_WITH_CHECK(map_matrix));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return dest;
    }

    /*
     * Resizes an image.
     *
     * @overload resize(size, interpolation = :linear)
     * @param size [CvSize] Output image size.
     * @param interpolation [Symbol] Interpolation method:
     *     * <tt>CV_INTER_NN</tt> - A nearest-neighbor interpolation
     *     * <tt>CV_INTER_LINEAR</tt> - A bilinear interpolation (used by default)
     *     * <tt>CV_INTER_AREA</tt> - Resampling using pixel area relation. It may be a preferred method for
     *       image decimation, as it gives moire'-free results. But when the image is zoomed,
     *       it is similar to the <tt>:nn</tt> method.
     *     * <tt>CV_INTER_CUBIC</tt> - A bicubic interpolation over 4x4 pixel neighborhood
     *     * <tt>CV_INTER_LANCZOS4</tt> - A Lanczos interpolation over 8x8 pixel neighborhood
     * @return [CvMat] Output image.
     * @opencv_func cvResize
     */
    VALUE
    rb_resize(int argc, VALUE *argv, VALUE self)
    {
      VALUE size, interpolation;
      rb_scan_args(argc, argv, "11", &size, &interpolation);
      VALUE dest = new_mat_kind_object(VALUE_TO_CVSIZE(size), self);
      int method = NIL_P(interpolation) ? CV_INTER_LINEAR : NUM2INT(interpolation);

      try {
	cvResize(CVARR(self), CVARR(dest), method);
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return dest;
    }

    /*
     * Applies an affine transformation to an image.
     *
     * @overload warp_affine(map_matrix, flags = CV_INTER_LINEAR | CV_WARP_FILL_OUTLIERS, fillval = 0)
     * @param map_matrix [CvMat] 2x3 transformation matrix.
     * @param flags [Integer] Combination of interpolation methods (#see resize) and the optional
     *     flag <tt>WARP_INVERSE_MAP</tt> that means that <tt>map_matrix</tt> is the inverse transformation.
     * @return [CvMat] Output image that has the size <tt>size</tt> and the same type as <tt>self</tt>.
     * @param fillval [Number, CvScalar] Value used in case of a constant border.
     * @opencv_func cvWarpAffine
     */
    VALUE
    rb_warp_affine(int argc, VALUE *argv, VALUE self)
    {
      VALUE map_matrix, flags_val, fill_value;
      VALUE dest = Qnil;
      if (rb_scan_args(argc, argv, "12", &map_matrix, &flags_val, &fill_value) < 3)
	fill_value = INT2FIX(0);
      CvArr* self_ptr = CVARR(self);
      int flags = NIL_P(flags_val) ? (CV_INTER_LINEAR | CV_WARP_FILL_OUTLIERS) : NUM2INT(flags_val);
      try {
	dest = new_mat_kind_object(cvGetSize(self_ptr), self);
	cvWarpAffine(self_ptr, CVARR(dest), CVMAT_WITH_CHECK(map_matrix),
		     flags, VALUE_TO_CVSCALAR(fill_value));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return dest;
    }

    /*
     * Finds a perspective transformation between two planes.
     *
     * @overload find_homography(src_points, dst_points, method = :all, ransac_reproj_threshold = 3, get_mask = false)
     *   @param src_points [CvMat] Coordinates of the points in the original plane.
     *   @param dst_points [CvMat] Coordinates of the points in the target plane.
     *   @param method [Symbol] Method used to computed a homography matrix. The following methods are possible:
     *     * <tt>:all</tt> - a regular method using all the points
     *     * <tt>:ransac</tt> - RANSAC-based robust method
     *     * <tt>:lmeds</tt> - Least-Median robust method
     *   @param ransac_reproj_threshold [Number] Maximum allowed reprojection error to treat a point pair as
     *     an inlier (used in the RANSAC method only).
     *   @param get_mask [Boolean] If <tt>true</tt>, the optional output mask set by
     *     a robust method (<tt>:ransac</tt> or <tt>:lmeds</tt>) is returned additionally.
     * @return [CvMat, Array<CvMat>] The perspective transformation <tt>H</tt> between the source and the destination
     *   planes in <tt>CvMat</tt>.
     *   If <tt>method</tt> is <tt>:ransac</tt> or <tt>:lmeds</tt> and <tt>get_mask</tt> is <tt>true</tt>, the output mask
     *   is also returned in the form of an array <tt>[H, output_mask]</tt>.
     * @scope class
     * @opencv_func cvFindHomography
     */
    VALUE
    rb_find_homography(int argc, VALUE *argv, VALUE self)
    {
      VALUE src_points, dst_points, method, ransac_reproj_threshold, get_status;
      rb_scan_args(argc, argv, "23", &src_points, &dst_points, &method, &ransac_reproj_threshold, &get_status);

      VALUE homography = new_object(cvSize(3, 3), CV_32FC1);
      int _method = CVMETHOD("HOMOGRAPHY_CALC_METHOD", method, 0);
      double _ransac_reproj_threshold = NIL_P(ransac_reproj_threshold) ? 0.0 : NUM2DBL(ransac_reproj_threshold);

      if ((_method != 0) && (!NIL_P(get_status)) && IF_BOOL(get_status, 1, 0, 0)) {
	CvMat *src = CVMAT_WITH_CHECK(src_points);
	int num_points = MAX(src->rows, src->cols);
	VALUE status = new_object(cvSize(num_points, 1), CV_8UC1);
	try {
	  cvFindHomography(src, CVMAT_WITH_CHECK(dst_points), CVMAT(homography),
			   _method, _ransac_reproj_threshold, CVMAT(status));
	}
	catch (cv::Exception& e) {
	  raise_cverror(e);
	}
	return rb_assoc_new(homography, status);
      }
      else {
	try {
	  cvFindHomography(CVMAT(src_points), CVMAT(dst_points), CVMAT(homography),
			   _method, _ransac_reproj_threshold, NULL);
	}
	catch (cv::Exception& e) {
	  raise_cverror(e);
	}
	return homography;
      }
    }

    /*
     * Calculates an affine matrix of 2D rotation.
     *
     * @overload rotation_matrix2D(center, angle, scale)
     * @param center [CvPoint2D32f] Center of the rotation in the source image.
     * @param angle [Number] Rotation angle in degrees. Positive values mean counter-clockwise rotation
     *   (the coordinate origin is assumed to be the top-left corner).
     * @param scale [Number] Isotropic scale factor.
     * @return [CvMat] The output affine transformation, 2x3 floating-point matrix.
     * @scope class
     * @opencv_func cv2DRotationMatrix
     */
    VALUE
    rb_rotation_matrix2D(VALUE self, VALUE center, VALUE angle, VALUE scale)
    {
      VALUE map_matrix = new_object(cvSize(3, 2), CV_MAKETYPE(CV_32F, 1));
      try {
	cv2DRotationMatrix(VALUE_TO_CVPOINT2D32F(center), NUM2DBL(angle), NUM2DBL(scale), CVMAT(map_matrix));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return map_matrix;
    }

    /*
     * Calculates a perspective transform from four pairs of the corresponding points.
     *
     * @overload get_perspective_transform(src, dst)
     *   @param src [Array<CvPoint>] Coordinates of quadrangle vertices in the source image.
     *   @param dst [Array<CvPoint>] Coordinates of the corresponding quadrangle vertices in the destination image.
     * @return [CvMat] Map matrix
     * @scope class
     * @opencv_func cvGetPerspectiveTransform
     */
    VALUE
    rb_get_perspective_transform(VALUE self, VALUE source, VALUE dest)
    {
      Check_Type(source, T_ARRAY);
      Check_Type(dest, T_ARRAY);

      int count = RARRAY_LEN(source);

      CvPoint2D32f* source_buff = ALLOCA_N(CvPoint2D32f, count);
      CvPoint2D32f* dest_buff = ALLOCA_N(CvPoint2D32f, count);

      for (int i = 0; i < count; i++) {
	source_buff[i] = *(CVPOINT2D32F(RARRAY_PTR(source)[i]));
	dest_buff[i] = *(CVPOINT2D32F(RARRAY_PTR(dest)[i]));
      }

      VALUE map_matrix = new_object(cvSize(3, 3), CV_MAKETYPE(CV_32F, 1));

      try {
	cvGetPerspectiveTransform(source_buff, dest_buff, CVMAT(map_matrix));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return map_matrix;
    }

    /*
     * Applies a perspective transformation to an image.
     *
     * @overload warp_perspective(map_matrix, flags = CV_INTER_LINEAR | CV_WARP_FILL_OUTLIERS, fillval = 0)
     *   @param map_matrix [CvMat] 3x3 transformation matrix.
     *   @param flags [Integer] Combination of interpolation methods (<tt>CV_INTER_LINEAR</tt> or <tt>CV_INTER_NEAREST</tt>)
     *     and the optional flag <tt>CV_WARP_INVERSE_MAP</tt>, that sets <tt>map_matrix</tt> as the inverse transformation.
     *   @param fillval [Number, CvScalar] Value used in case of a constant border.
     * @return [CvMat] Output image.
     * @opencv_func cvWarpPerspective
     */
    VALUE
    rb_warp_perspective(int argc, VALUE *argv, VALUE self)
    {
      VALUE map_matrix, flags_val, option, fillval;
      if (rb_scan_args(argc, argv, "13", &map_matrix, &flags_val, &option, &fillval) < 4)
	fillval = INT2FIX(0);
      CvArr* self_ptr = CVARR(self);
      VALUE dest = Qnil;
      int flags = NIL_P(flags_val) ? (CV_INTER_LINEAR | CV_WARP_FILL_OUTLIERS) : NUM2INT(flags_val);
      try {
	dest = new_mat_kind_object(cvGetSize(self_ptr), self);
	cvWarpPerspective(self_ptr, CVARR(dest), CVMAT_WITH_CHECK(map_matrix),
			  flags, VALUE_TO_CVSCALAR(fillval));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return dest;
    }

    /*
     * Applies a generic geometrical transformation to an image.
     *
     * @overload remap(mapx, mapy, flags = CV_INTER_LINEAR | CV_WARP_FILL_OUTLIERS, fillval = 0)
     *   @param mapx [CvMat] The first map of either <tt>(x,y)</tt> points or just x values having the type
     *     <tt>CV_16SC2</tt>, <tt>CV_32FC1</tt>, or <tt>CV_32FC2</tt>.
     *   @param mapy [CvMat] The second map of y values having the type <tt>CV_16UC1</tt>, <tt>CV_32FC1</tt>, or none
     *     (empty map if <tt>mapx</tt> is <tt>(x,y)</tt> points), respectively.
     *   @param flags [Integer] Combination of interpolation methods (<tt>CV_INTER_LINEAR</tt> or <tt>CV_INTER_NEAREST</tt>)
     *     and the optional flag <tt>CV_WARP_INVERSE_MAP</tt>, that sets <tt>map_matrix</tt> as the inverse transformation.
     *   @param fillval [Number, CvScalar] Value used in case of a constant border.
     * @return [CvMat] Output image.
     * @opencv_func cvRemap
     */
    VALUE
    rb_remap(int argc, VALUE *argv, VALUE self)
    {
      VALUE mapx, mapy, flags_val, option, fillval;
      if (rb_scan_args(argc, argv, "23", &mapx, &mapy, &flags_val, &option, &fillval) < 5)
	fillval = INT2FIX(0);
      CvArr* self_ptr = CVARR(self);
      VALUE dest = Qnil;
      int flags = NIL_P(flags_val) ? (CV_INTER_LINEAR | CV_WARP_FILL_OUTLIERS) : NUM2INT(flags_val);
      try {
	dest = new_mat_kind_object(cvGetSize(self_ptr), self);
	cvRemap(self_ptr, CVARR(dest), CVARR_WITH_CHECK(mapx), CVARR_WITH_CHECK(mapy),
		flags, VALUE_TO_CVSCALAR(fillval));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return dest;
    }

    /*
     * Remaps an image to log-polar space.
     *
     * @overload log_polar(size, center, magnitude, flags = CV_INTER_LINEAR | CV_WARP_FILL_OUTLIERS)
     *   @param size [CvSize] Size of the destination image.
     *   @param center [CvPoint2D32f] The transformation center; where the output precision is maximal.
     *   @param magnitude [Number] Magnitude scale parameter.
     *   @param flags [Integer] A combination of interpolation methods and the following optional flags:
     *     * <tt>CV_WARP_FILL_OUTLIERS</tt> - fills all of the destination image pixels. If some of them
     *       correspond to outliers in the source image, they are set to zero.
     *     * <tt>CV_WARP_INVERSE_MAP</tt> - performs inverse transformation.
     * @return [CvMat] Destination image.
     * @opencv_func cvLogPolar
     */
    VALUE
    rb_log_polar(int argc, VALUE *argv, VALUE self)
    {
      VALUE dst_size, center, m, flags;
      rb_scan_args(argc, argv, "31", &dst_size, &center, &m, &flags);
      int _flags = NIL_P(flags) ? (CV_INTER_LINEAR | CV_WARP_FILL_OUTLIERS) : NUM2INT(flags);
      VALUE dest = new_mat_kind_object(VALUE_TO_CVSIZE(dst_size), self);
      try {
	cvLogPolar(CVARR(self), CVARR(dest), VALUE_TO_CVPOINT2D32F(center), NUM2DBL(m), _flags);
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return dest;
    }

    /*
     * call-seq:
     *   erode([element = nil, iteration = 1]) -> cvmat
     *
     * Create erodes image by using arbitrary structuring element.
     * <i>element</i> is structuring element used for erosion.
     * <i>element</i> should be IplConvKernel. If it is nil, a 3x3 rectangular structuring element is used.
     * <i>iterations</i> is number of times erosion is applied.
     */
    VALUE
    rb_erode(int argc, VALUE *argv, VALUE self)
    {
      return rb_erode_bang(argc, argv, rb_clone(self));
    }

    /*
     * call-seq:
     *   erode!([element = nil][,iteration = 1]) -> self
     *
     * Erodes image by using arbitrary structuring element.
     * see also #erode.
     */
    VALUE
    rb_erode_bang(int argc, VALUE *argv, VALUE self)
    {
      VALUE element, iteration;
      rb_scan_args(argc, argv, "02", &element, &iteration);
      IplConvKernel* kernel = NIL_P(element) ? NULL : IPLCONVKERNEL_WITH_CHECK(element);
      try {
	cvErode(CVARR(self), CVARR(self), kernel, IF_INT(iteration, 1));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return self;
    }

    /*
     * call-seq:
     *   dilate([element = nil][,iteration = 1]) -> cvmat
     *
     * Create dilates image by using arbitrary structuring element.
     * <i>element</i> is structuring element used for erosion.
     * <i>element</i> should be IplConvKernel. If it is nil, a 3x3 rectangular structuring element is used.
     * <i>iterations</i> is number of times erosion is applied.
     */
    VALUE
    rb_dilate(int argc, VALUE *argv, VALUE self)
    {
      return rb_dilate_bang(argc, argv, rb_clone(self));
    }

    /*
     * call-seq:
     *   dilate!([element = nil][,iteration = 1]) -> self
     *
     * Dilate image by using arbitrary structuring element.
     * see also #dilate.
     */
    VALUE
    rb_dilate_bang(int argc, VALUE *argv, VALUE self)
    {
      VALUE element, iteration;
      rb_scan_args(argc, argv, "02", &element, &iteration);
      IplConvKernel* kernel = NIL_P(element) ? NULL : IPLCONVKERNEL_WITH_CHECK(element);
      try {
	cvDilate(CVARR(self), CVARR(self), kernel, IF_INT(iteration, 1));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return self;
    }

    /*
     * Performs advanced morphological transformations using erosion and dilation as basic operations.
     *
     * @overload morphology(operation, element = nil, iteration = 1)
     * @param operation [Integer] Type of morphological operation.
     *   * <tt>CV_MOP_OPEN</tt> - Opening
     *   * <tt>CV_MOP_CLOSE</tt> - Closing
     *   * <tt>CV_MOP_GRADIENT</tt> - Morphological gradient
     *   * <tt>CV_MOP_TOPHAT</tt> - Top hat
     *   * <tt>CV_MOP_BLACKHAT</tt> - Black hat
     * @param element [IplConvKernel] Structuring element.
     * @param iteration [Integer] Number of times erosion and dilation are applied.
     * @return [CvMat] Result array
     * @opencv_func cvMorphologyEx
     */
    VALUE
    rb_morphology(int argc, VALUE *argv, VALUE self)
    {
      VALUE element, iteration, operation_val;
      rb_scan_args(argc, argv, "12", &operation_val, &element, &iteration);

      int operation = CVMETHOD("MORPHOLOGICAL_OPERATION", operation_val, -1);
      CvArr* self_ptr = CVARR(self);
      CvSize size = cvGetSize(self_ptr);
      VALUE dest = new_mat_kind_object(size, self);
      IplConvKernel* kernel = NIL_P(element) ? NULL : IPLCONVKERNEL_WITH_CHECK(element);
      try {
	if (operation == CV_MOP_GRADIENT) {
	  CvMat* temp = rb_cvCreateMat(size.height, size.width, cvGetElemType(self_ptr));
	  cvMorphologyEx(self_ptr, CVARR(dest), temp, kernel, CV_MOP_GRADIENT, IF_INT(iteration, 1));
	  cvReleaseMat(&temp);
	}
	else {
	  cvMorphologyEx(self_ptr, CVARR(dest), 0, kernel, operation, IF_INT(iteration, 1));
	}
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }

      return dest;
    }

    /*
     * call-seq:
     *   smooth_blur_no_scale([p1 = 3, p2 = 3]) -> cvmat
     *
     * Smooths the image by simple blur with no scaling.
     * * 8bit unsigned -> return 16bit unsigned
     * * 32bit floating point -> return 32bit floating point
     * <b>support single-channel image only.</b>
     */
    VALUE
    rb_smooth_blur_no_scale(int argc, VALUE *argv, VALUE self)
    {
      VALUE p1, p2, dest;
      rb_scan_args(argc, argv, "02", &p1, &p2);
      CvArr* self_ptr = CVARR(self);
      int type = cvGetElemType(self_ptr), dest_type;
      switch (CV_MAT_DEPTH(type)) {
      case CV_8U:
	dest_type = CV_16U;
	break;
      case CV_32F:
	dest_type = CV_32F;
	break;
      default:
	rb_raise(rb_eNotImpError, "unsupport format. (support 8bit unsigned/signed or 32bit floating point only)");
      }
      dest = new_mat_kind_object(cvGetSize(self_ptr), self, dest_type, CV_MAT_CN(type));
      cvSmooth(self_ptr, CVARR(dest), CV_BLUR_NO_SCALE, IF_INT(p1, 3), IF_INT(p2, 3));
      return dest;
    }

    /*
     * call-seq:
     *   smooth_blur([p1 = 3, p2 = 3]) -> cvmat
     *
     * Smooths the image by simple blur.
     * Summation over a pixel <i>p1</i> x <i>p2</i> neighborhood with subsequent scaling by 1 / (p1*p2).
     */
    VALUE
    rb_smooth_blur(int argc, VALUE *argv, VALUE self)
    {
      VALUE p1, p2, dest;
      rb_scan_args(argc, argv, "02", &p1, &p2);
      CvArr* self_ptr = CVARR(self);
      dest = new_mat_kind_object(cvGetSize(self_ptr), self);
      cvSmooth(self_ptr, CVARR(dest), CV_BLUR, IF_INT(p1, 3), IF_INT(p2, 3));
      return dest;
    }

    /*
     * call-seq:
     *   smooth_gaussian([p1 = 3, p2 = 3, p3 = 0.0, p4 = 0.0]) -> cvmat
     *
     * Smooths the image by gaussian blur.
     * Convolving image with <i>p1</i> x <i>p2</i> Gaussian kernel.
     *
     * <i>p3</i> may specify Gaussian sigma (standard deviation).
     * If it is zero, it is calculated from the kernel size:
     *    sigma = (n/2 - 1)*0.3 + 0.8, where n = p1 for horizontal kernel,
     *                                       n = p2 for vertical kernel.
     *
     * <i>p4</i> is in case of non-square Gaussian kernel the parameter.
     * It may be used to specify a different (from p3) sigma in the vertical direction.
     */
    VALUE
    rb_smooth_gaussian(int argc, VALUE *argv, VALUE self)
    {
      VALUE p1, p2, p3, p4, dest;
      rb_scan_args(argc, argv, "04", &p1, &p2, &p3, &p4);
      CvArr* self_ptr = CVARR(self);
      dest = new_mat_kind_object(cvGetSize(self_ptr), self);
      cvSmooth(self_ptr, CVARR(dest), CV_GAUSSIAN, IF_INT(p1, 3), IF_INT(p2, 3), IF_DBL(p3, 0.0), IF_DBL(p4, 0.0));
      return dest;
    }

    /*
     * call-seq:
     *   smooth_median([p1 = 3]) -> cvmat
     *
     * Smooths the image by median blur.
     * Finding median of <i>p1</i> x <i>p1</i> neighborhood (i.e. the neighborhood is square).
     */
    VALUE
    rb_smooth_median(int argc, VALUE *argv, VALUE self)
    {
      VALUE p1, dest;
      rb_scan_args(argc, argv, "01", &p1);
      CvArr* self_ptr = CVARR(self);
      dest = new_mat_kind_object(cvGetSize(self_ptr), self);
      cvSmooth(self_ptr, CVARR(dest), CV_MEDIAN, IF_INT(p1, 3));
      return dest;
    }

    /*
     * call-seq:
     *   smooth_bilateral([p1 = 3][p2 = 3]) -> cvmat
     *
     * Smooths the image by bilateral filter.
     * Applying bilateral 3x3 filtering with color sigma=<i>p1</i> and space sigma=<i>p2</i>.
     */
    VALUE
    rb_smooth_bilateral(int argc, VALUE *argv, VALUE self)
    {
      VALUE p1, p2, dest;
      rb_scan_args(argc, argv, "02", &p1, &p2);
      CvArr* self_ptr = CVARR(self);
      dest = new_mat_kind_object(cvGetSize(self_ptr), self);
      cvSmooth(self_ptr, CVARR(dest), CV_BILATERAL, IF_INT(p1, 3), IF_INT(p2, 3));
      return dest;
    }

    /**
     * Smooths the image in one of several ways.
     *
     * @overload smooth(smoothtype, size1 = 3, size2 = 0, sigma1 = 0, sigma2 = 0)
     *   @param smoothtype [Integer] Type of the smoothing.
     *     * CV_BLUR_NO_SCALE - linear convolution with <tt>size1 x size2</tt> box kernel (all 1's).
     *       If you want to smooth different pixels with different-size box kernels,
     *       you can use the integral image that is computed using <tt>CvMat#integral</tt>.
     *     * CV_BLUR - linear convolution with <tt>size1 x size2</tt> box kernel (all 1's)
     *       with subsequent scaling by <tt>1 / (size1 x size1)</tt>.
     *     * CV_GAUSSIAN - linear convolution with a <tt>size1 x size2</tt> Gaussian kernel.
     *     * CV_MEDIAN - median filter with a <tt>size1 x size1</tt> square aperture
     *     * CV_BILATERAL - bilateral filter with a <tt>size1 x size1</tt> square aperture,
     *       color sigma = <tt>sigma1</tt> and spatial sigma = <tt>sigma2</tt>.
     *       If <tt>size1</tt> = 0, the aperture square side is set to <tt>CvMat#round(sigma2 * 1.5) * 2 + 1</tt>.
     *   @param size1 [Integer] The first parameter of the smoothing operation, the aperture width.
     *     Must be a positive odd number (1, 3, 5, ...)
     *   @param size2 [Integer] The second parameter of the smoothing operation, the aperture height.
     *     Ignored by <tt>CV_MEDIAN</tt> and <tt>CV_BILATERAL</tt> methods. In the case of simple
     *     scaled/non-scaled and Gaussian blur if <tt>size2</tt> is zero, it is set to <tt>size1</tt>.
     *     Otherwise it must be a positive odd number.
     *   @param sigma1 [Integer] In the case of a Gaussian parameter this parameter may specify
     *     Gaussian sigma (standard deviation). If it is zero, it is calculated from the kernel size.
     * @return [CvMat] The destination image.
     * @opencv_func cvSmooth
     */
    VALUE
    rb_smooth(int argc, VALUE *argv, VALUE self)
    {
      VALUE smoothtype, p1, p2, p3, p4;
      rb_scan_args(argc, argv, "14", &smoothtype, &p1, &p2, &p3, &p4);
      int _smoothtype = CVMETHOD("SMOOTHING_TYPE", smoothtype, -1);
  
      VALUE (*smooth_func)(int c, VALUE* v, VALUE s);
      argc--;
      switch (_smoothtype) {
      case CV_BLUR_NO_SCALE:
	smooth_func = rb_smooth_blur_no_scale;
	argc = (argc > 2) ? 2 : argc;
	break;
      case CV_BLUR:
	smooth_func = rb_smooth_blur;
	argc = (argc > 2) ? 2 : argc;
	break;
      case CV_GAUSSIAN:
	smooth_func = rb_smooth_gaussian;
	break;
      case CV_MEDIAN:
	smooth_func = rb_smooth_median;
	argc = (argc > 1) ? 1 : argc;
	break;
      case CV_BILATERAL:
	smooth_func = rb_smooth_bilateral;
	argc = (argc > 2) ? 2 : argc;
	break;
      default:
	smooth_func = rb_smooth_gaussian;
	break;
      }
      VALUE result = Qnil;
      try {
	result = (*smooth_func)(argc, argv + 1, self);
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return result;
    }

    /*
     * call-seq:
     *   filter2d(kernel[,anchor]) -> cvmat
     *
     * Convolves image with the kernel.
     * Convolution kernel, single-channel floating point matrix (or same depth of self's).
     * If you want to apply different kernels to different channels,
     * split the image using CvMat#split into separate color planes and process them individually.
     */
    VALUE
    rb_filter2d(int argc, VALUE *argv, VALUE self)
    {
      VALUE _kernel, _anchor;
      rb_scan_args(argc, argv, "11", &_kernel, &_anchor);
      CvMat* kernel = CVMAT_WITH_CHECK(_kernel);
      CvArr* self_ptr = CVARR(self);
      VALUE _dest = Qnil;
      try {
	_dest = new_mat_kind_object(cvGetSize(self_ptr), self);
	cvFilter2D(self_ptr, CVARR(_dest), kernel, NIL_P(_anchor) ? cvPoint(-1,-1) : VALUE_TO_CVPOINT(_anchor));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return _dest;
    }

    /*
     * call-seq:
     *   copy_make_border(border_type, size, offset[,value = CvScalar.new(0)])
     *
     * Copies image and makes border around it.
     * <i>border_type</i>:
     * - IPL_BORDER_CONSTANT, :constant
     *     border is filled with the fixed value, passed as last parameter of the function.
     * - IPL_BORDER_REPLICATE, :replicate
     *     the pixels from the top and bottom rows, the left-most and right-most columns are replicated to fill the border
     * <i>size</i>: The destination image size
     * <i>offset</i>: Coordinates of the top-left corner (or bottom-left in the case of images with bottom-left origin) of the destination image rectangle.
     * <i>value</i>: Value of the border pixels if bordertype is IPL_BORDER_CONSTANT or :constant.
     */
    VALUE
    rb_copy_make_border(int argc, VALUE *argv, VALUE self)
    {
      VALUE border_type, size, offset, value, dest;
      rb_scan_args(argc, argv, "31", &border_type, &size, &offset, &value);
      dest = new_mat_kind_object(VALUE_TO_CVSIZE(size), self);

      int type = 0;
      if (SYMBOL_P(border_type)) {
	ID type_id = rb_to_id(border_type);
	if (type_id == rb_intern("constant"))
	  type = IPL_BORDER_CONSTANT;
	else if (type_id == rb_intern("replicate"))
	  type = IPL_BORDER_REPLICATE;
	else
	  rb_raise(rb_eArgError, "Invalid border_type (should be :constant or :replicate)");
      }
      else
	type = NUM2INT(border_type);

      try {
	cvCopyMakeBorder(CVARR(self), CVARR(dest), VALUE_TO_CVPOINT(offset), type,
			 NIL_P(value) ? cvScalar(0) : VALUE_TO_CVSCALAR(value));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return dest;
    }

    /*
     * call-seq:
     *   integral(need_sqsum = false, need_tilted_sum = false) -> [cvmat, cvmat or nil, cvmat or nil]
     *
     * Calculates integral images.
     * If <i>need_sqsum</i> = true, calculate the integral image for squared pixel values.
     * If <i>need_tilted_sum</i> = true, calculate the integral for the image rotated by 45 degrees.
     *
     *   sum(X,Y)=sumx<X,y<Yimage(x,y)
     *   sqsum(X,Y)=sumx<X,y<Yimage(x,y)2
     *   tilted_sum(X,Y)=sumy<Y,abs(x-X)<yimage(x,y)
     *
     * Using these integral images, one may calculate sum, mean, standard deviation over arbitrary up-right or rotated rectangular region of the image in a constant time.
     */
    VALUE
    rb_integral(int argc, VALUE *argv, VALUE self)
    {
      VALUE need_sqsum = Qfalse, need_tiled_sum = Qfalse;
      rb_scan_args(argc, argv, "02", &need_sqsum, &need_tiled_sum);

      VALUE sum = Qnil;
      VALUE sqsum = Qnil;
      VALUE tiled_sum = Qnil;
      CvArr* self_ptr = CVARR(self);
      try {
	CvSize self_size = cvGetSize(self_ptr);
	CvSize size = cvSize(self_size.width + 1, self_size.height + 1);
	int type_cv64fcn = CV_MAKETYPE(CV_64F, CV_MAT_CN(cvGetElemType(self_ptr)));
	sum = cCvMat::new_object(size, type_cv64fcn);
	sqsum = (need_sqsum == Qtrue ? cCvMat::new_object(size, type_cv64fcn) : Qnil);
	tiled_sum = (need_tiled_sum == Qtrue ? cCvMat::new_object(size, type_cv64fcn) : Qnil);
	cvIntegral(self_ptr, CVARR(sum), (need_sqsum == Qtrue) ? CVARR(sqsum) : NULL,
		   (need_tiled_sum == Qtrue) ? CVARR(tiled_sum) : NULL);
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
  
      if ((need_sqsum != Qtrue) && (need_tiled_sum != Qtrue))
	return sum;
      else {
	VALUE dest = rb_ary_new3(1, sum);
	if (need_sqsum == Qtrue)
	  rb_ary_push(dest, sqsum);
	if (need_tiled_sum == Qtrue)
	  rb_ary_push(dest, tiled_sum);
	return dest;
      }
    }

    inline VALUE
    rb_threshold_internal(int threshold_type, VALUE threshold, VALUE max_value, VALUE use_otsu, VALUE self)
    {
      CvArr* self_ptr = CVARR(self);
      int otsu = (use_otsu == Qtrue) && ((threshold_type & CV_THRESH_OTSU) == 0);
      int type = threshold_type | (otsu ? CV_THRESH_OTSU : 0);
      VALUE dest = Qnil;
      double otsu_threshold = 0;
      try {
	dest = new_mat_kind_object(cvGetSize(self_ptr), self);
	otsu_threshold = cvThreshold(self_ptr, CVARR(dest), NUM2DBL(threshold), NUM2DBL(max_value), type);
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }

      if ((use_otsu == Qtrue) || (threshold_type & CV_THRESH_OTSU))
	return rb_assoc_new(dest, DBL2NUM(otsu_threshold));
      else
	return dest;
    }

    /*
     * Applies a fixed-level threshold to each array element.
     *
     * @overload threshold(threshold, max_value, threshold_type)
     *   @param threshold [Number] Threshold value.
     *   @param max_value [Number] Maximum value to use with the <tt>CV_THRESH_BINARY</tt>
     *     and <tt>CV_THRESH_BINARY_INV</tt> thresholding types.
     *   @param threshold_type [Integer] Thresholding type
     *     * CV_THRESH_BINARY
     *     * CV_THRESH_BINARY_INV
     *     * CV_THRESH_TRUNC
     *     * CV_THRESH_TOZERO
     *     * CV_THRESH_TOZERO_INV
     *   @return [CvMat] Output array of the same size and type as <tt>self</tt>.
     * @overload threshold(threshold, max_value, threshold_type, use_otsu)
     *   @param threshold [Number] Threshold value.
     *   @param max_value [Number] Maximum value to use with the <tt>CV_THRESH_BINARY</tt>
     *     and <tt>CV_THRESH_BINARY_INV</tt> thresholding types.
     *   @param threshold_type [Integer] Thresholding type
     *     * CV_THRESH_BINARY
     *     * CV_THRESH_BINARY_INV
     *     * CV_THRESH_TRUNC
     *     * CV_THRESH_TOZERO
     *     * CV_THRESH_TOZERO_INV
     *   @param use_otsu [Boolean] Determines the optimal threshold value using the Otsu's algorithm
     *   @return [Array<CvMat, Number>] Output array and Otsu's threshold.
     * @opencv_func cvThreshold
     * @example
     *   mat = CvMat.new(3, 3, CV_8U, 1)
     *   mat.set_data([1, 2, 3, 4, 5, 6, 7, 8, 9])
     *   mat #=> [1, 2, 3,
     *            4, 5, 6,
     *            7, 8, 9]
     *   result = mat.threshold(4, 7, CV_THRESH_BINARY)
     *   result #=> [0, 0, 0,
     *               0, 7, 7,
     *               7, 7, 7]
     */
    VALUE
    rb_threshold(int argc, VALUE *argv, VALUE self)
    {
      VALUE threshold, max_value, threshold_type, use_otsu;
      rb_scan_args(argc, argv, "31", &threshold, &max_value, &threshold_type, &use_otsu);
      const int INVALID_TYPE = -1;
      int type = CVMETHOD("THRESHOLD_TYPE", threshold_type, INVALID_TYPE);
      if (type == INVALID_TYPE)
	rb_raise(rb_eArgError, "Invalid threshold type.");
  
      return rb_threshold_internal(type, threshold, max_value, use_otsu, self);
    }

    /*
     * Applies an adaptive threshold to an array.
     *
     * @overload adaptive_threshold(max_value, options)
     *   @param max_value [Number] Non-zero value assigned to the pixels for which the condition is satisfied.
     *   @param options [Hash] Threshold option
     *   @option options [Integer, Symbol] :threshold_type (CV_THRESH_BINARY) Thresholding type;
     *     must be one of <tt>CV_THRESH_BINARY</tt> or <tt>:binary</tt>, <tt>CV_THRESH_BINARY_INV</tt> or <tt>:binary_inv</tt>.
     *   @option options [Integer, Symbol] :adaptive_method (CV_ADAPTIVE_THRESH_MEAN_C) Adaptive thresholding algorithm to use:
     *     <tt>CV_ADAPTIVE_THRESH_MEAN_C</tt> or <tt>:mean_c</tt>, <tt>CV_ADAPTIVE_THRESH_GAUSSIAN_C</tt> or <tt>:gaussian_c</tt>.
     *   @option options [Integer] :block_size (3) The size of a pixel neighborhood that is used to calculate a threshold value
     *     for the pixel: 3, 5, 7, and so on.
     *   @option options :param1 [Number] (5) The method-dependent parameter. For the methods <tt>CV_ADAPTIVE_THRESH_MEAN_C</tt>
     *     and <tt>CV_ADAPTIVE_THRESH_GAUSSIAN_C</tt> it is a constant subtracted from the mean or weighted mean, though it may be negative
     *   @return [CvMat] Destination image of the same size and the same type as <tt>self</tt>.
     * @opencv_func cvAdaptiveThreshold
     * @example
     *   mat = CvMat.new(3, 3, CV_8U, 1)
     *   mat.set_data([1, 2, 3, 4, 5, 6, 7, 8, 9])
     *   mat #=> [1, 2, 3,
     *            4, 5, 6,
     *            7, 8, 9]
     *   result = mat.adaptive_threshold(7, threshold_type: CV_THRESH_BINARY,
     *                                   adaptive_method: CV_ADAPTIVE_THRESH_MEAN_C,
     *                                   block_size: 3, param1: 1)
     *   result #=> [0, 0, 0,
     *               7, 7, 7,
     *               7, 7, 7]
     */
    VALUE
    rb_adaptive_threshold(int argc, VALUE *argv, VALUE self)
    {
      VALUE max_value, options;
      rb_scan_args(argc, argv, "11", &max_value, &options);

      int threshold_type = CV_THRESH_BINARY;
      int adaptive_method = CV_ADAPTIVE_THRESH_MEAN_C;
      int block_size = 3;
      double param1 = 5;
      if (!NIL_P(options)) {
	Check_Type(options, T_HASH);
	threshold_type = CVMETHOD("THRESHOLD_TYPE", LOOKUP_HASH(options, "threshold_type"),
				  CV_THRESH_BINARY);
	adaptive_method = CVMETHOD("ADAPTIVE_METHOD", LOOKUP_HASH(options, "adaptive_method"),
				   CV_ADAPTIVE_THRESH_MEAN_C);
	VALUE _block_size = LOOKUP_HASH(options, "block_size");
	if (!NIL_P(_block_size)) {
	  block_size = NUM2INT(_block_size);
	}
	VALUE _param1 = LOOKUP_HASH(options, "param1");
	if (!NIL_P(_param1)) {
	  param1 = NUM2INT(_param1);
	}
      }
      CvArr* self_ptr = CVARR(self);
      VALUE dst = new_mat_kind_object(cvGetSize(self_ptr), self);
      try {
	cvAdaptiveThreshold(self_ptr, CVARR(dst), NUM2DBL(max_value), adaptive_method, threshold_type,
			    block_size, param1);
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
  
      return dst;
    }

    /*
     * call-seq:
     *   pyr_down([filter = :gaussian_5x5]) -> cvmat
     *
     * Return downsamples image.
     *
     * This operation performs downsampling step of Gaussian pyramid decomposition.
     * First it convolves source image with the specified filter and then downsamples the image
     * by rejecting even rows and columns.
     *
     * note: filter - only :gaussian_5x5 is currently supported.
     */
    VALUE
    rb_pyr_down(int argc, VALUE *argv, VALUE self)
    {
      int filter = CV_GAUSSIAN_5x5;
      if (argc > 0) {
	VALUE filter_type = argv[0];
	switch (TYPE(filter_type)) {
	case T_SYMBOL:
	  // currently suport CV_GAUSSIAN_5x5 only.
	  break;
	default:
	  raise_typeerror(filter_type, rb_cSymbol);
	}
      }
      CvArr* self_ptr = CVARR(self);
      VALUE dest = Qnil;
      try {
	CvSize original_size = cvGetSize(self_ptr);
	CvSize size = cvSize(original_size.width >> 1, original_size.height >> 1);
	dest = new_mat_kind_object(size, self);
	cvPyrDown(self_ptr, CVARR(dest), filter);
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return dest;
    }

    /*
     * call-seq:
     *   pyr_up([filter = :gaussian_5x5]) -> cvmat
     *
     * Return upsamples image.
     *
     * This operation performs up-sampling step of Gaussian pyramid decomposition.
     * First it upsamples the source image by injecting even zero rows and columns and
     * then convolves result with the specified filter multiplied by 4 for interpolation.
     * So the destination image is four times larger than the source image.
     *
     * note: filter - only :gaussian_5x5 is currently supported.
     */
    VALUE
    rb_pyr_up(int argc, VALUE *argv, VALUE self)
    {
      VALUE filter_type;
      rb_scan_args(argc, argv, "01", &filter_type);
      int filter = CV_GAUSSIAN_5x5;
      if (argc > 0) {
	switch (TYPE(filter_type)) {
	case T_SYMBOL:
	  // currently suport CV_GAUSSIAN_5x5 only.
	  break;
	default:
	  raise_typeerror(filter_type, rb_cSymbol);
	}
      }
      CvArr* self_ptr = CVARR(self);
      VALUE dest = Qnil;
      try {
	CvSize original_size = cvGetSize(self_ptr);
	CvSize size = cvSize(original_size.width << 1, original_size.height << 1);
	dest = new_mat_kind_object(size, self);
	cvPyrUp(self_ptr, CVARR(dest), filter);
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return dest;
    }

    /*
     * Fills a connected component with the given color.
     *
     * @overload flood_fill(seed_point, new_val, lo_diff = CvScalar.new(0), up_diff = CvScalar.new(0), flood_fill_option = nil)
     *   @param seed_point [CvPoint] Starting point.
     *   @param new_val [CvScalar] New value of the repainted domain pixels.
     *   @param lo_diff [CvScalar] Maximal lower brightness/color difference between the currently observed pixel
     *     and one of its neighbor belong to the component or seed pixel to add the pixel to component.
     *     In case of 8-bit color images it is packed value.
     *   @param up_diff [CvScalar] Maximal upper brightness/color difference between the currently observed pixel and
     *     one of its neighbor belong to the component or seed pixel to add the pixel to component.
     *     In case of 8-bit color images it is packed value.
     *   @param flood_fill_option [Hash] 
     *   @option flood_fill_option [Integer] :connectivity (4)
     *     Connectivity determines which neighbors of a pixel are considered (4 or 8).
     *   @option flood_fill_option [Boolean] :fixed_range (false)
     *     If set the difference between the current pixel and seed pixel is considered, otherwise difference between
     *     neighbor pixels is considered (the range is floating).
     *   @option flood_fill_option [Boolean] :mask_only (false)
     *     If set, the function does not fill the image(new_val is ignored), but the fills mask.
     * @return [Array<CvMat, CvConnectedComp>] Array of output image, connected component and mask.
     * @opencv_func cvFloodFill
     */
    VALUE
    rb_flood_fill(int argc, VALUE *argv, VALUE self)
    {
      return rb_flood_fill_bang(argc, argv, copy(self));
    }

    /*
     * Fills a connected component with the given color.
     *
     * @overload flood_fill!(seed_point, new_val, lo_diff = CvScalar.new(0), up_diff = CvScalar.new(0), flood_fill_option = nil)
     *   @param (see #flood_fill)
     *   @return (see #flood_fill)
     * @opencv_func (see #flood_fill)
     */
    VALUE
    rb_flood_fill_bang(int argc, VALUE *argv, VALUE self)
    {
      VALUE seed_point, new_val, lo_diff, up_diff, flood_fill_option;
      rb_scan_args(argc, argv, "23", &seed_point, &new_val, &lo_diff, &up_diff, &flood_fill_option);
      flood_fill_option = FLOOD_FILL_OPTION(flood_fill_option);
      int flags = FF_CONNECTIVITY(flood_fill_option);
      if (FF_FIXED_RANGE(flood_fill_option)) {
	flags |= CV_FLOODFILL_FIXED_RANGE;
      }
      if (FF_MASK_ONLY(flood_fill_option)) {
	flags |= CV_FLOODFILL_MASK_ONLY;
      }
      CvArr* self_ptr = CVARR(self);
      VALUE comp = cCvConnectedComp::new_object();
      VALUE mask = Qnil;
      try {
	CvSize size = cvGetSize(self_ptr);
	// TODO: Change argument format to set mask
	mask = new_object(size.height + 2, size.width + 2, CV_MAKETYPE(CV_8U, 1));
	CvMat* mask_ptr = CVMAT(mask);
	cvSetZero(mask_ptr);
	cvFloodFill(self_ptr,
		    VALUE_TO_CVPOINT(seed_point),
		    VALUE_TO_CVSCALAR(new_val),
		    NIL_P(lo_diff) ? cvScalar(0) : VALUE_TO_CVSCALAR(lo_diff),
		    NIL_P(up_diff) ? cvScalar(0) : VALUE_TO_CVSCALAR(up_diff),
		    CVCONNECTEDCOMP(comp),
		    flags,
		    mask_ptr);
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return rb_ary_new3(3, self, comp, mask);
    }

    /*
     * Finds contours in binary image.
     *
     * @overload find_contours(find_contours_options)
     *   @param find_contours_options [Hash] Options
     *   @option find_contours_options [Symbol] :mode (:list) Retrieval mode.
     *      * :external - retrive only the extreme outer contours
     *      * :list - retrieve all the contours and puts them in the list.
     *      * :ccomp - retrieve all the contours and organizes them into two-level hierarchy:
     *        top level are external boundaries of the components, second level are bounda boundaries of the holes
     *      * :tree - retrieve all the contours and reconstructs the full hierarchy of nested contours
     *        Connectivity determines which neighbors of a pixel are considered.
     *   @option find_contours_options [Symbol] :method (:approx_simple) Approximation method.
     *      * :code - output contours in the Freeman chain code. All other methods output polygons (sequences of vertices).
     *      * :approx_none - translate all the points from the chain code into points;
     *      * :approx_simple - compress horizontal, vertical, and diagonal segments, that is, the function leaves only their ending points;
     *      * :approx_tc89_l1, :approx_tc89_kcos - apply one of the flavors of Teh-Chin chain approximation algorithm.
     *   @option find_contours_options [CvPoint] :offset (CvPoint.new(0, 0)) Offset, by which every contour point is shifted.
     * @return [CvContour, CvChain] Detected contours. If <tt>:method</tt> is <tt>:code</tt>,
     *   returns as <tt>CvChain</tt>, otherwise <tt>CvContour</tt>.
     * @opencv_func cvFindContours
     */
    VALUE
    rb_find_contours(int argc, VALUE *argv, VALUE self)
    {
      return rb_find_contours_bang(argc, argv, copy(self));
    }

    /*
     * Finds contours in binary image.
     *
     * @overload find_contours!(find_contours_options)
     *   @param (see #find_contours)
     * @return (see #find_contours)
     * @opencv_func (see #find_contours)
     */
    VALUE
    rb_find_contours_bang(int argc, VALUE *argv, VALUE self)
    {
      VALUE find_contours_option, klass, element_klass, storage;
      rb_scan_args(argc, argv, "01", &find_contours_option);
      CvSeq *contour = NULL;
      find_contours_option = FIND_CONTOURS_OPTION(find_contours_option);
      int mode = FC_MODE(find_contours_option);
      int method = FC_METHOD(find_contours_option);
      int header_size;
      if (method == CV_CHAIN_CODE) {
	klass = cCvChain::rb_class();
	element_klass = T_FIXNUM;
	header_size = sizeof(CvChain);
      }
      else {
	klass = cCvContour::rb_class();
	element_klass = cCvPoint::rb_class();
	header_size = sizeof(CvContour);
      }
      storage = cCvMemStorage::new_object();

      int count = 0;
      try {
	count = cvFindContours(CVARR(self), CVMEMSTORAGE(storage), &contour, header_size,
			       mode, method, FC_OFFSET(find_contours_option));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      if (count == 0)
	return Qnil;
      else
	return cCvSeq::new_sequence(klass, contour, element_klass, storage);
    }

    /*
     * call-seq:
     *   draw_contours(contour, external_color, hole_color, max_level, options) -> cvmat
     *
     * Draws contour outlines or interiors in an image.
     *
     * * <i>contour</i> (CvContour) - Pointer to the first contour
     * * <i>external_color</i> (CvScalar) -  Color of the external contours
     * * <i>hole_color</i> (CvScalar) - Color of internal contours (holes)
     * * <i>max_level</i> (Integer) - Maximal level for drawn contours. If 0, only contour is drawn. If 1, the contour and all contours following it on the same level are drawn. If 2, all contours following and all contours one level below the contours are drawn, and so forth. If the value is negative, the function does not draw the contours following after contour but draws the child contours of contour up to the |max_level| - 1 level.
     * * <i>options</i> (Hash) - Drawing options.
     *   * :thickness (Integer) - Thickness of lines the contours are drawn with. If it is negative, the contour interiors are drawn (default: 1).
     *   * :line_type (Integer or Symbol) - Type of the contour segments, see CvMat#line description (default: 8).
     */
    VALUE
    rb_draw_contours(int argc, VALUE *argv, VALUE self)
    {
      return rb_draw_contours_bang(argc, argv, copy(self));
    }

    /*
     * call-seq:
     *   draw_contours!(contour, external_color, hole_color, max_level, options) -> cvmat
     *
     * Draws contour outlines or interiors in an image.
     *
     * see CvMat#draw_contours
     */
    VALUE
    rb_draw_contours_bang(int argc, VALUE *argv, VALUE self)
    {
      VALUE contour, external_color, hole_color, max_level, options;
      rb_scan_args(argc, argv, "41", &contour, &external_color, &hole_color, &max_level, &options);
      options = DRAWING_OPTION(options);
      try {
	cvDrawContours(CVARR(self), CVSEQ_WITH_CHECK(contour), VALUE_TO_CVSCALAR(external_color),
		       VALUE_TO_CVSCALAR(hole_color), NUM2INT(max_level),
		       DO_THICKNESS(options), DO_LINE_TYPE(options));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return self;
    }

    /*
     * call-seq:
     *   draw_chessboard_corners(pattern_size, corners, pattern_was_found) -> nil
     *
     * Returns an image which is rendered the detected chessboard corners.
     *
     * pattern_size (CvSize) - Number of inner corners per a chessboard row and column.
     * corners (Array<CvPoint2D32f>) - Array of detected corners, the output of CvMat#find_chessboard_corners.
     * pattern_was_found (Boolean)- Parameter indicating whether the complete board was found or not.
     */
    VALUE
    rb_draw_chessboard_corners(VALUE self, VALUE pattern_size, VALUE corners, VALUE pattern_was_found)
    {
      return rb_draw_chessboard_corners_bang(copy(self), pattern_size, corners, pattern_was_found);
    }

    /*
     * call-seq:
     *   draw_chessboard_corners!(pattern_size, corners, pattern_was_found) -> self
     *
     * Renders the detected chessboard corners.
     *
     * pattern_size (CvSize) - Number of inner corners per a chessboard row and column.
     * corners (Array<CvPoint2D32f>) - Array of detected corners, the output of CvMat#find_chessboard_corners.
     * pattern_was_found (Boolean)- Parameter indicating whether the complete board was found or not.
     */
    VALUE
    rb_draw_chessboard_corners_bang(VALUE self, VALUE pattern_size, VALUE corners, VALUE pattern_was_found)
    {
      Check_Type(corners, T_ARRAY);
      int count = RARRAY_LEN(corners);
      CvPoint2D32f* corners_buff = ALLOCA_N(CvPoint2D32f, count);
      VALUE* corners_ptr = RARRAY_PTR(corners);
      for (int i = 0; i < count; i++) {
	corners_buff[i] = *(CVPOINT2D32F(corners_ptr[i]));
      }

      try {
	int found = (pattern_was_found == Qtrue);
	cvDrawChessboardCorners(CVARR(self), VALUE_TO_CVSIZE(pattern_size), corners_buff, count, found);
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }

      return self;
    }

    /*
     * call-seq:
     *   pyr_mean_shift_filtering(sp, sr[,max_level = 1][termcrit = CvTermCriteria.new(5,1)]) -> cvmat
     *
     * Does meanshift image segmentation.
     *
     *   sp - The spatial window radius.
     *   sr - The color window radius.
     *   max_level - Maximum level of the pyramid for the segmentation.
     *   termcrit - Termination criteria: when to stop meanshift iterations.
     *
     * This method is implements the filtering stage of meanshift segmentation,
     * that is, the output of the function is the filtered "posterized" image with color gradients and fine-grain texture flattened.
     * At every pixel (X,Y) of the input image (or down-sized input image, see below)
     * the function executes meanshift iterations, that is, the pixel (X,Y) neighborhood in the joint space-color hyperspace is considered:
     *   {(x,y): X-sp≤x≤X+sp && Y-sp≤y≤Y+sp && ||(R,G,B)-(r,g,b)|| ≤ sr},
     * where (R,G,B) and (r,g,b) are the vectors of color components at (X,Y) and (x,y),
     * respectively (though, the algorithm does not depend on the color space used,
     * so any 3-component color space can be used instead).
     * Over the neighborhood the average spatial value (X',Y')
     * and average color vector (R',G',B') are found and they act as the neighborhood center on the next iteration:
     *   (X,Y)~(X',Y'), (R,G,B)~(R',G',B').
     * After the iterations over, the color components of the initial pixel (that is, the pixel from where the iterations started)
     * are set to the final value (average color at the last iteration):
     *   I(X,Y) <- (R*,G*,B*).
     * Then max_level > 0, the gaussian pyramid of max_level+1 levels is built,
     * and the above procedure is run on the smallest layer.
     * After that, the results are propagated to the larger layer and the iterations are run again
     * only on those pixels where the layer colors differ much (>sr) from the lower-resolution layer,
     * that is, the boundaries of the color regions are clarified.
     *
     * Note, that the results will be actually different from the ones obtained by running the meanshift procedure on the whole original image (i.e. when max_level==0).
     */
    VALUE
    rb_pyr_mean_shift_filtering(int argc, VALUE *argv, VALUE self)
    {
      VALUE spatial_window_radius, color_window_radius, max_level, termcrit;
      rb_scan_args(argc, argv, "22", &spatial_window_radius, &color_window_radius, &max_level, &termcrit);
      CvArr* self_ptr = CVARR(self);
      VALUE dest = Qnil;
      try {
	dest = new_mat_kind_object(cvGetSize(self_ptr), self);
	cvPyrMeanShiftFiltering(self_ptr, CVARR(dest),
				NUM2DBL(spatial_window_radius),
				NUM2DBL(color_window_radius),
				IF_INT(max_level, 1),
				NIL_P(termcrit) ? cvTermCriteria(CV_TERMCRIT_ITER + CV_TERMCRIT_EPS, 5, 1)
				: VALUE_TO_CVTERMCRITERIA(termcrit));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return dest;
    }

    /*
     * Performs a marker-based image segmentation using the watershed algorithm.
     *
     * @overload watershed(markers)
     *   @param markers [CvMat] Input 32-bit single-channel image of markers. It should have the same size as <tt>self</tt>
     * @return [CvMat] Output image
     * @opencv_func cvWatershed
     */
    VALUE
    rb_watershed(VALUE self, VALUE markers)
    {
      try {
	cvWatershed(CVARR(self), CVARR_WITH_CHECK(markers));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return markers;
    }

    /*
     * call-seq:
     *   moments -> cvmoments
     *
     * Calculates moments.
     */
    VALUE
    rb_moments(int argc, VALUE *argv, VALUE self)
    {
      VALUE is_binary;
      rb_scan_args(argc, argv, "01", &is_binary);
      CvArr *self_ptr = CVARR(self);
      VALUE moments = Qnil;
      try {
	moments = cCvMoments::new_object(self_ptr, TRUE_OR_FALSE(is_binary, 0));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return moments;
    }

    /*
     * Finds lines in binary image using a Hough transform.
     *
     * @overload hough_lines(method, rho, theta, threshold, param1, param2)
     *   @param method [Integer] The Hough transform variant, one of the following:
     *     * CV_HOUGH_STANDARD - classical or standard Hough transform.
     *     * CV_HOUGH_PROBABILISTIC - probabilistic Hough transform (more efficient in case if picture contains a few long linear segments).
     *     * CV_HOUGH_MULTI_SCALE - multi-scale variant of the classical Hough transform. The lines are encoded the same way as CV_HOUGH_STANDARD.
     *   @param rho [Number] Distance resolution in pixel-related units.
     *   @param theta [Number] Angle resolution measured in radians.
     *   @param threshold [Number] Threshold parameter. A line is returned by the function if the corresponding
     *     accumulator value is greater than threshold.
     *   @param param1 [Number] The first method-dependent parameter:
     *     * For the classical Hough transform it is not used (0).
     *     * For the probabilistic Hough transform it is the minimum line length.
     *     * For the multi-scale Hough transform it is the divisor for the distance resolution.
     *       (The coarse distance resolution will be rho and the accurate resolution will be (rho / param1)).
     *   @param param2 [Number] The second method-dependent parameter:
     *     * For the classical Hough transform it is not used (0).
     *     * For the probabilistic Hough transform it is the maximum gap between line segments lying
     *       on the same line to treat them as a single line segment (i.e. to join them).
     *     * For the multi-scale Hough transform it is the divisor for the angle resolution.
     *       (The coarse angle resolution will be theta and the accurate resolution will be (theta / param2).)
     * @return [CvSeq<CvLine, CvTwoPoints>] Output lines. If <tt>method</tt> is <tt>CV_HOUGH_STANDARD</tt> or <tt>CV_HOUGH_MULTI_SCALE</tt>,
     *   the class of elements is <tt>CvLine</tt>, otherwise <tt>CvTwoPoints</tt>.
     * @opencv_func cvHoughLines2
     */
    VALUE
    rb_hough_lines(int argc, VALUE *argv, VALUE self)
    {
      const int INVALID_TYPE = -1;
      VALUE method, rho, theta, threshold, p1, p2;
      rb_scan_args(argc, argv, "42", &method, &rho, &theta, &threshold, &p1, &p2);
      int method_flag = CVMETHOD("HOUGH_TRANSFORM_METHOD", method, INVALID_TYPE);
      if (method_flag == INVALID_TYPE)
	rb_raise(rb_eArgError, "Invalid method: %d", method_flag);
      VALUE storage = cCvMemStorage::new_object();
      CvSeq *seq = NULL;
      try {
	seq = cvHoughLines2(CVARR(copy(self)), CVMEMSTORAGE(storage),
			    method_flag, NUM2DBL(rho), NUM2DBL(theta), NUM2INT(threshold),
			    IF_DBL(p1, 0), IF_DBL(p2, 0));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      switch (method_flag) {
      case CV_HOUGH_STANDARD:
      case CV_HOUGH_MULTI_SCALE:
	return cCvSeq::new_sequence(cCvSeq::rb_class(), seq, cCvLine::rb_class(), storage);
	break;
      case CV_HOUGH_PROBABILISTIC:
	return cCvSeq::new_sequence(cCvSeq::rb_class(), seq, cCvTwoPoints::rb_class(), storage);
	break;
      default:
	break;
      }

      return Qnil;
    }

    /*
     * Finds circles in a grayscale image using the Hough transform.
     *
     * @overload hough_circles(method, dp, min_dist, param1, param2, min_radius = 0, max_radius = 0)
     *   @param method [Integer] Detection method to use. Currently, the only implemented method is <tt>CV_HOUGH_GRADIENT</tt>.
     *   @param dp [Number] Inverse ratio of the accumulator resolution to the image resolution.
     *     For example, if dp=1, the accumulator has the same resolution as the input image.
     *     If dp=2, the accumulator has half as big width and height.
     *   @param min_dist [Number] Minimum distance between the centers of the detected circles.
     *     If the parameter is too small, multiple neighbor circles may be falsely detected
     *     in addition to a true one. If it is too large, some circles may be missed.
     *   @param param1 [Number] First method-specific parameter. In case of <tt>CV_HOUGH_GRADIENT</tt>,
     *     it is the higher threshold of the two passed to the #canny detector (the lower one is twice smaller). 
     *   @param param2 [Number] Second method-specific parameter. In case of <tt>CV_HOUGH_GRADIENT</tt>,
     *     it is the accumulator threshold for the circle centers at the detection stage. The smaller it is,
     *     the more false circles may be detected. Circles, corresponding to the larger accumulator values,
     *     will be returned first.
     * @return [CvSeq<CvCircle32f>] Output circles.
     * @opencv_func cvHoughCircles
     */
    VALUE
    rb_hough_circles(int argc, VALUE *argv, VALUE self)
    {
      const int INVALID_TYPE = -1;
      VALUE method, dp, min_dist, param1, param2, min_radius, max_radius, storage;
      rb_scan_args(argc, argv, "52", &method, &dp, &min_dist, &param1, &param2, 
		   &min_radius, &max_radius);
      storage = cCvMemStorage::new_object();
      int method_flag = CVMETHOD("HOUGH_TRANSFORM_METHOD", method, INVALID_TYPE);
      if (method_flag == INVALID_TYPE)
	rb_raise(rb_eArgError, "Invalid method: %d", method_flag);
      CvSeq *seq = NULL;
      try {
	seq = cvHoughCircles(CVARR(self), CVMEMSTORAGE(storage),
			     method_flag, NUM2DBL(dp), NUM2DBL(min_dist),
			     NUM2DBL(param1), NUM2DBL(param2),
			     IF_INT(min_radius, 0), IF_INT(max_radius, 0));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return cCvSeq::new_sequence(cCvSeq::rb_class(), seq, cCvCircle32f::rb_class(), storage);
    }

    /*
     * call-seq:
     *   inpaint(inpaint_method, mask, radius) -> cvmat
     *
     * Inpaints the selected region in the image
     * The radius of circlular neighborhood of each point inpainted that is considered by the algorithm.
     */
    VALUE
    rb_inpaint(VALUE self, VALUE inpaint_method, VALUE mask, VALUE radius)
    {
      const int INVALID_TYPE = -1;
      VALUE dest = Qnil;
      int method = CVMETHOD("INPAINT_METHOD", inpaint_method, INVALID_TYPE);
      if (method == INVALID_TYPE)
	rb_raise(rb_eArgError, "Invalid method");
      try {
	CvArr* self_ptr = CVARR(self);
	dest = new_mat_kind_object(cvGetSize(self_ptr), self);
	cvInpaint(self_ptr, MASK(mask), CVARR(dest), NUM2DBL(radius), method);
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return dest;
    }

    /*
     * call-seq:
     *   equalize_hist -> cvmat
     *
     * Equalize histgram of grayscale of image.
     *
     * equalizes histogram of the input image using the following algorithm:
     * 1. calculate histogram H for src.
     * 2. normalize histogram, so that the sum of histogram bins is 255.
     * 3. compute integral of the histogram:
     *    H’(i) = sum0≤j≤iH(j)
     * 4. transform the image using H’ as a look-up table: dst(x,y)=H’(src(x,y))
     * The algorithm normalizes brightness and increases contrast of the image.
     *
     * <b>support single-channel 8bit image (grayscale) only.</b>
     */
    VALUE
    rb_equalize_hist(VALUE self)
    {
      VALUE dest = Qnil;
      try {
	CvArr* self_ptr = CVARR(self);
	dest = new_mat_kind_object(cvGetSize(self_ptr), self);
	cvEqualizeHist(self_ptr, CVARR(dest));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return dest;
    }

    /*
     * call-seq:
     *   apply_color_map(colormap) -> cvmat
     *
     * Applies a GNU Octave/MATLAB equivalent colormap on a given image.
     *
     * Parameters:
     *   colormap - The colormap to apply.
     */
    VALUE
    rb_apply_color_map(VALUE self, VALUE colormap)
    {
      VALUE dst;
      try {
	cv::Mat dst_mat;
	cv::Mat self_mat = cv::cvarrToMat(CVMAT(self));

	cv::applyColorMap(self_mat, dst_mat, NUM2INT(colormap));
	CvMat tmp = dst_mat;
	dst = new_object(tmp.rows, tmp.cols, tmp.type);
	cvCopy(&tmp, CVMAT(dst));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }

      return dst;
    }

    /*
     * Compares template against overlapped image regions.
     *
     * @overload match_template(template, method = CV_TM_SQDIFF)
     *   @param template [CvMat] Searched template. It must be not greater than the source image and have the same data type.
     *   @param method [Integer] Parameter specifying the comparison method.
     *     * CV_TM_SQDIFF
     *     * CV_TM_SQDIFF_NORMED
     *     * CV_TM_CCORR
     *     * CV_TM_CCORR_NORMED
     *     * CV_TM_CCOEFF
     *     * CV_TM_CCOEFF_NORMED
     * @opencv_func cvMatchTemplate
     *
     * After the match_template finishes comparison, the best matches can be found as global
     * minimums (<tt>CV_TM_SQDIFF</tt>) or maximums(<tt>CV_TM_CCORR</tt> or <tt>CV_TM_CCOEFF</tt>) using CvMat#min_max_loc.
     * In case of color image and template summation in both numerator and each sum in denominator
     * is done over all the channels (and separate mean values are used for each channel).
     */
    VALUE
    rb_match_template(int argc, VALUE *argv, VALUE self)
    {
      VALUE templ, method;
      int method_flag;
      if (rb_scan_args(argc, argv, "11", &templ, &method) == 1)
	method_flag = CV_TM_SQDIFF;
      else
	method_flag = CVMETHOD("MATCH_TEMPLATE_METHOD", method);

      CvArr* self_ptr = CVARR(self);
      CvArr* templ_ptr = CVARR_WITH_CHECK(templ);
      VALUE result = Qnil;
      try {
	CvSize src_size = cvGetSize(self_ptr);
	CvSize template_size = cvGetSize(templ_ptr);
	result = cCvMat::new_object(src_size.height - template_size.height + 1,
				    src_size.width - template_size.width + 1,
				    CV_32FC1);
	cvMatchTemplate(self_ptr, templ_ptr, CVARR(result), method_flag);
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return result;
    }

    /*
     * call-seq:
     *   match_shapes(object, method) -> float
     *
     * Compares two shapes(self and object). <i>object</i> should be CvMat or CvContour.
     *
     * A - object1, B - object2:
     * * method=CV_CONTOURS_MATCH_I1
     *     I1(A,B)=sumi=1..7abs(1/mAi - 1/mBi)
     * * method=CV_CONTOURS_MATCH_I2
     *     I2(A,B)=sumi=1..7abs(mAi - mBi)
     * * method=CV_CONTOURS_MATCH_I3
     *     I3(A,B)=sumi=1..7abs(mAi - mBi)/abs(mAi)
     */
    VALUE
    rb_match_shapes(int argc, VALUE *argv, VALUE self)
    {
      VALUE object, method, param;
      rb_scan_args(argc, argv, "21", &object, &method, &param);
      int method_flag = CVMETHOD("COMPARISON_METHOD", method);
      if (!(rb_obj_is_kind_of(object, cCvMat::rb_class()) || rb_obj_is_kind_of(object, cCvContour::rb_class())))
	rb_raise(rb_eTypeError, "argument 1 (shape) should be %s or %s",
		 rb_class2name(cCvMat::rb_class()), rb_class2name(cCvContour::rb_class()));
      double result = 0;
      try {
	result = cvMatchShapes(CVARR(self), CVARR(object), method_flag);
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return rb_float_new(result);
    }

    /*
     * call-seq:
     *   mean_shift(window, criteria) -> comp
     *
     * Implements CAMSHIFT object tracking algrorithm.
     * First, it finds an object center using mean_shift and, after that,
     * calculates the object size and orientation.
     */
    VALUE
    rb_mean_shift(VALUE self, VALUE window, VALUE criteria)
    {
      VALUE comp = cCvConnectedComp::new_object();
      try {
	cvMeanShift(CVARR(self), VALUE_TO_CVRECT(window), VALUE_TO_CVTERMCRITERIA(criteria), CVCONNECTEDCOMP(comp));    
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return comp;
    }

    /*
     * call-seq:
     *   cam_shift(window, criteria) -> [comp, box]
     *
     * Implements CAMSHIFT object tracking algrorithm. First, it finds an object center using cvMeanShift and,
     * after that, calculates the object size and orientation. The function returns number of iterations made
     * within cvMeanShift.
     */
    VALUE
    rb_cam_shift(VALUE self, VALUE window, VALUE criteria)
    {
      VALUE comp = cCvConnectedComp::new_object();
      VALUE box = cCvBox2D::new_object();
      try {
	cvCamShift(CVARR(self), VALUE_TO_CVRECT(window), VALUE_TO_CVTERMCRITERIA(criteria),
		   CVCONNECTEDCOMP(comp), CVBOX2D(box));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return rb_ary_new3(2, comp, box);
    }

    /*
     * call-seq:
     *   CvMat.find_fundamental_mat(points1, points2[,options = {}]) -> fundamental_matrix(cvmat) or nil
     *
     * Calculates fundamental matrix from corresponding points.
     * Size of the output fundamental matrix is 3x3 or 9x3 (7-point method may return up to 3 matrices)
     *
     * <i>points1</i> and <i>points2</i> should be 2xN, Nx2, 3xN or Nx3 1-channel, or 1xN or Nx1 multi-channel matrix.
     * <i>method<i> is method for computing the fundamental matrix
     *    - CV_FM_7POINT for a 7-point algorithm. (N = 7)
     *    - CV_FM_8POINT for an 8-point algorithm. (N >= 8)
     *    - CV_FM_RANSAC for the RANSAC algorithm. (N >= 8)
     *    - CV_FM_LMEDS for the LMedS algorithm. (N >= 8)
     * <i>option</i> should be Hash include these keys.
     *   :with_status (true or false)
     *      If set true, return fundamental_matrix and status. [fundamental_matrix, status]
     *      Otherwise return fundamental matrix only(default).
     *   :maximum_distance
     *      The parameter is used for RANSAC.  It is the maximum distance from point to epipolar line in pixels, beyond which the point is considered an outlier and is not used for computing the final fundamental matrix. It can be set to something like 1-3, depending on the accuracy of the point localization, image resolution and the image noise.
     *   :desirable_level
     *      The optional output array of N elements, every element of which is set to 0 for outliers and to 1 for the other points. The array is computed only in RANSAC and LMedS methods. For other methods it is set to all 1's.
     *
     * note: <i>option</i>'s default value is CvMat::FIND_FUNDAMENTAL_MAT_OPTION.
     */
    VALUE
    rb_find_fundamental_mat(int argc, VALUE *argv, VALUE klass)
    {
      VALUE points1, points2, method, option, fundamental_matrix, status;
      int num = 0;
      rb_scan_args(argc, argv, "31", &points1, &points2, &method, &option);
      option = FIND_FUNDAMENTAL_MAT_OPTION(option);
      int fm_method = FIX2INT(method);
      CvMat *points1_ptr = CVMAT_WITH_CHECK(points1);
      if (fm_method == CV_FM_7POINT)
	fundamental_matrix = cCvMat::new_object(9, 3, CV_MAT_DEPTH(points1_ptr->type));
      else
	fundamental_matrix = cCvMat::new_object(3, 3, CV_MAT_DEPTH(points1_ptr->type));

      if (FFM_WITH_STATUS(option)) {
	int status_len = (points1_ptr->rows > points1_ptr->cols) ? points1_ptr->rows : points1_ptr->cols;
	status = cCvMat::new_object(1, status_len, CV_8UC1);
	try {
	  num = cvFindFundamentalMat(points1_ptr, CVMAT_WITH_CHECK(points2), CVMAT(fundamental_matrix), fm_method,
				     FFM_MAXIMUM_DISTANCE(option), FFM_DESIRABLE_LEVEL(option), CVMAT(status));
	}
	catch (cv::Exception& e) {
	  raise_cverror(e);
	}
	return num == 0 ? Qnil : rb_ary_new3(2, fundamental_matrix, status);
      }
      else {
	try {
	  num = cvFindFundamentalMat(points1_ptr, CVMAT_WITH_CHECK(points2), CVMAT(fundamental_matrix), fm_method,
				     FFM_MAXIMUM_DISTANCE(option), FFM_DESIRABLE_LEVEL(option), NULL);
	}
	catch (cv::Exception& e) {
	  raise_cverror(e);
	}
	return num == 0 ? Qnil : fundamental_matrix;
      }
    }


    /*
     * call-seq:
     *   CvMat.compute_correspond_epilines(points, which_image, fundamental_matrix) -> correspondent_lines(cvmat)
     *
     * For points in one image of stereo pair computes the corresponding epilines in the other image.
     * Finds equation of a line that contains the corresponding point (i.e. projection of the same 3D point)
     * in the other image. Each line is encoded by a vector of 3 elements l=[a,b,c]T, so that:
     *   lT*[x, y, 1]T=0,
     * or
     *   a*x + b*y + c = 0
     * From the fundamental matrix definition (see cvFindFundamentalMatrix discussion), line l2 for a point p1
     * in the first image (which_image=1) can be computed as:
     *   l2=F*p1
     * and the line l1 for a point p2 in the second image (which_image=1) can be computed as:
     *   l1=FT*p2
     * Line coefficients are defined up to a scale. They are normalized (a2+b2=1) are stored into correspondent_lines.
     */
    VALUE
    rb_compute_correspond_epilines(VALUE klass, VALUE points, VALUE which_image, VALUE fundamental_matrix)
    {
      VALUE correspondent_lines;
      CvMat* points_ptr = CVMAT_WITH_CHECK(points);
      int n;
      if (points_ptr->cols <= 3 && points_ptr->rows >= 7)
	n = points_ptr->rows;
      else if (points_ptr->rows <= 3 && points_ptr->cols >= 7)
	n = points_ptr->cols;
      else
	rb_raise(rb_eArgError, "input points should 2xN, Nx2 or 3xN, Nx3 matrix(N >= 7).");
  
      correspondent_lines = cCvMat::new_object(n, 3, CV_MAT_DEPTH(points_ptr->type));
      try {
      	cvComputeCorrespondEpilines(points_ptr, NUM2INT(which_image), CVMAT_WITH_CHECK(fundamental_matrix),
      				    CVMAT(correspondent_lines));
      }
      catch (cv::Exception& e) {
      	raise_cverror(e);
      }
      return correspondent_lines;
    }

    /*
     * call-seq:
     *   subspace_project(w, mean) -> cvmat
     */
    VALUE
    rb_subspace_project(VALUE self, VALUE w, VALUE mean)
    {
      VALUE projection;
      try {
	cv::Mat w_mat = cv::cvarrToMat(CVMAT_WITH_CHECK(w));
	cv::Mat mean_mat = cv::cvarrToMat(CVMAT_WITH_CHECK(mean));
	cv::Mat self_mat = cv::cvarrToMat(CVMAT(self));
#ifdef IS_OPENCV2
	cv::Mat pmat = cv::subspaceProject(w_mat, mean_mat, self_mat);
#else
	cv::Mat pmat = cv::LDA::subspaceProject(w_mat, mean_mat, self_mat);
#endif
	projection = new_object(pmat.rows, pmat.cols, pmat.type());
	CvMat tmp = pmat;
	cvCopy(&tmp, CVMAT(projection));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }

      return projection;
    }

    /*
     * call-seq:
     *   subspace_reconstruct(w, mean) -> cvmat
     */
    VALUE
    rb_subspace_reconstruct(VALUE self, VALUE w, VALUE mean)
    {
      VALUE result;
      try {
	cv::Mat w_mat = cv::cvarrToMat(CVMAT_WITH_CHECK(w));
	cv::Mat mean_mat = cv::cvarrToMat(CVMAT_WITH_CHECK(mean));
	cv::Mat self_mat = cv::cvarrToMat(CVMAT(self));
#ifdef IS_OPENCV2
	cv::Mat rmat = cv::subspaceReconstruct(w_mat, mean_mat, self_mat);
#else
	cv::Mat rmat = cv::LDA::subspaceReconstruct(w_mat, mean_mat, self_mat);
#endif
	result = new_object(rmat.rows, rmat.cols, rmat.type());
	CvMat tmp = rmat;
	cvCopy(&tmp, CVMAT(result));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }

      return result;
    }

    VALUE
    new_object(int rows, int cols, int type)
    {
      return OPENCV_OBJECT(rb_klass, rb_cvCreateMat(rows, cols, type));
    }

    VALUE
    new_object(CvSize size, int type)
    {
      return OPENCV_OBJECT(rb_klass, rb_cvCreateMat(size.height, size.width, type));
    }

    VALUE
    new_mat_kind_object(CvSize size, VALUE ref_obj)
    {
      VALUE return_type = CLASS_OF(ref_obj);
      if (rb_obj_is_kind_of(ref_obj, cIplImage::rb_class())) {
	IplImage* img = IPLIMAGE(ref_obj);
	return OPENCV_OBJECT(return_type, rb_cvCreateImage(size, img->depth, img->nChannels));
      }
      else if (rb_obj_is_kind_of(ref_obj, rb_klass)) // CvMat
	return OPENCV_OBJECT(return_type, rb_cvCreateMat(size.height, size.width, cvGetElemType(CVMAT(ref_obj))));
      else
	rb_raise(rb_eNotImpError, "Only CvMat or IplImage are supported");

      return Qnil;
    }

    VALUE
    new_mat_kind_object(CvSize size, VALUE ref_obj, int cvmat_depth, int channel)
    {
      VALUE return_type = CLASS_OF(ref_obj);
      if (rb_obj_is_kind_of(ref_obj, cIplImage::rb_class())) {
	return OPENCV_OBJECT(return_type, rb_cvCreateImage(size, CV2IPL_DEPTH(cvmat_depth), channel));
      }
      else if (rb_obj_is_kind_of(ref_obj, rb_klass)) // CvMat
	return OPENCV_OBJECT(return_type, rb_cvCreateMat(size.height, size.width,
							 CV_MAKETYPE(cvmat_depth, channel)));
      else
	rb_raise(rb_eNotImpError, "Only CvMat or IplImage are supported");

      return Qnil;
    }

    /*
     * Extracts Speeded Up Robust Features from an image
     *
     * @overload extract_surf(params, mask = nil) -> [cvseq(cvsurfpoint), array(float)]
     *   @param params [CvSURFParams] Various algorithm parameters put to the structure CvSURFParams.
     *   @param mask [CvMat] The optional input 8-bit mask. The features are only found
     *     in the areas that contain more than 50% of non-zero mask pixels.
     * @return [Array<CvSeq<CvSURFPoint>, Array<float>>] Output vector of keypoints and descriptors.
     * @opencv_func cvExtractSURF
     */
    VALUE
    rb_extract_surf(int argc, VALUE *argv, VALUE self)
    {
#ifdef IS_OPENCV2
      VALUE _params, _mask;
      rb_scan_args(argc, argv, "11", &_params, &_mask);

      // Prepare arguments
      CvSURFParams params = *CVSURFPARAMS_WITH_CHECK(_params);
      CvMat* mask = MASK(_mask);
      VALUE storage = cCvMemStorage::new_object();
      CvSeq* keypoints = NULL;
      CvSeq* descriptors = NULL;

      // Compute SURF keypoints and descriptors
      try {
	cvExtractSURF(CVARR(self), mask, &keypoints, &descriptors, CVMEMSTORAGE(storage),
		      params, 0);
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      VALUE _keypoints = cCvSeq::new_sequence(cCvSeq::rb_class(), keypoints, cCvSURFPoint::rb_class(), storage);

      // Create descriptor array
      const int DIM_SIZE = (params.extended) ? 128 : 64;
      const int NUM_KEYPOINTS = keypoints->total;
      VALUE _descriptors = rb_ary_new2(NUM_KEYPOINTS);
      for (int m = 0; m < NUM_KEYPOINTS; ++m) {
	VALUE elem = rb_ary_new2(DIM_SIZE);
	float *descriptor = (float*)cvGetSeqElem(descriptors, m);
	for (int n = 0; n < DIM_SIZE; ++n) {
	  rb_ary_store(elem, n, rb_float_new(descriptor[n]));
	}
	rb_ary_store(_descriptors, m, elem);
      }

      return rb_assoc_new(_keypoints, _descriptors);
#else
      raise_opencv3_unsupported();
      return Qnil;
#endif
    }

    /*
     * call-seq:
     *   snake_image(points, alpha, beta, gamma, window, criteria[, calc_gradient = true]) -> array(pointset)
     *
     * Updates snake in order to minimize its total energy that is a sum of internal energy
     * that depends on contour shape (the smoother contour is, the smaller internal energy is)
     * and external energy that depends on the energy field and reaches minimum at the local energy
     * extremums that correspond to the image edges in case of image gradient.

     * The parameter criteria.epsilon is used to define the minimal number of points that must be moved
     * during any iteration to keep the iteration process running.
     *
     * If at some iteration the number of moved points is less than criteria.epsilon or
     * the function performed criteria.max_iter iterations, the function terminates.
     *
     * points
     *   Contour points (snake).
     * alpha
     *   Weight[s] of continuity energy, single float or array of length floats, one per each contour point.
     * beta
     *   Weight[s] of curvature energy, similar to alpha.
     * gamma
     *   Weight[s] of image energy, similar to alpha.
     * window
     *   Size of neighborhood of every point used to search the minimum, both win.width and win.height must be odd.
     * criteria
     *  Termination criteria.
     * calc_gradient
     *  Gradient flag. If not 0, the function calculates gradient magnitude for every image pixel and consideres
     *  it as the energy field, otherwise the input image itself is considered.
     */
    VALUE
    rb_snake_image(int argc, VALUE *argv, VALUE self)
    {
#ifdef IS_OPENCV2
      VALUE points, alpha, beta, gamma, window, criteria, calc_gradient;
      rb_scan_args(argc, argv, "61", &points, &alpha, &beta, &gamma, &window, &criteria, &calc_gradient);
      CvPoint *pointset = 0;
      int length = CVPOINTS_FROM_POINT_SET(points, &pointset);
      int coeff = (TYPE(alpha) == T_ARRAY && TYPE(beta) == T_ARRAY && TYPE(gamma) == T_ARRAY) ? CV_ARRAY : CV_VALUE;
      float *a = 0, *b = 0, *c = 0;
      IplImage stub;
      int i;
      if (coeff == CV_VALUE) {
	float buff_a, buff_b, buff_c;
	buff_a = (float)NUM2DBL(alpha);
	buff_b = (float)NUM2DBL(beta);
	buff_c = (float)NUM2DBL(gamma);
	a = &buff_a;
	b = &buff_b;
	c = &buff_c;
      }
      else { // CV_ARRAY
	if ((RARRAY_LEN(alpha) != length) ||
	    (RARRAY_LEN(beta) != length) ||
	    (RARRAY_LEN(gamma) != length))
	  rb_raise(rb_eArgError, "alpha, beta, gamma should be same size of points");
	a = ALLOCA_N(float, length);
	b = ALLOCA_N(float, length);
	c = ALLOCA_N(float, length);
	for (i = 0; i < length; ++i) {
	  a[i] = (float)NUM2DBL(RARRAY_PTR(alpha)[i]);
	  b[i] = (float)NUM2DBL(RARRAY_PTR(beta)[i]);
	  c[i] = (float)NUM2DBL(RARRAY_PTR(gamma)[i]);
	}
      }
      CvSize win = VALUE_TO_CVSIZE(window);
      CvTermCriteria tc = VALUE_TO_CVTERMCRITERIA(criteria);
      try {
	cvSnakeImage(cvGetImage(CVARR(self), &stub), pointset, length,
		     a, b, c, coeff, win, tc, IF_BOOL(calc_gradient, 1, 0, 1));
      }
      catch (cv::Exception& e) {
	if (pointset != NULL)
	  cvFree(&pointset);
	raise_cverror(e);
      }
      VALUE result = rb_ary_new2(length);
      for (i = 0; i < length; ++i)
	rb_ary_push(result, cCvPoint::new_object(pointset[i]));
      cvFree(&pointset);

      return result;
#else
      raise_opencv3_unsupported();
      return Qnil;
#endif
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

      rb_klass = rb_define_class_under(opencv, "CvMat", rb_cObject);
      rb_define_alloc_func(rb_klass, rb_allocate);

      VALUE drawing_option = rb_hash_new();
      rb_define_const(rb_klass, "DRAWING_OPTION", drawing_option);
      rb_hash_aset(drawing_option, ID2SYM(rb_intern("color")), cCvScalar::new_object(cvScalarAll(0)));
      rb_hash_aset(drawing_option, ID2SYM(rb_intern("thickness")), INT2FIX(1));
      rb_hash_aset(drawing_option, ID2SYM(rb_intern("line_type")), INT2FIX(8));
      rb_hash_aset(drawing_option, ID2SYM(rb_intern("shift")), INT2FIX(0));

      VALUE good_features_to_track_option = rb_hash_new();
      rb_define_const(rb_klass, "GOOD_FEATURES_TO_TRACK_OPTION", good_features_to_track_option);
      rb_hash_aset(good_features_to_track_option, ID2SYM(rb_intern("max")), INT2FIX(0xFF));
      rb_hash_aset(good_features_to_track_option, ID2SYM(rb_intern("mask")), Qnil);
      rb_hash_aset(good_features_to_track_option, ID2SYM(rb_intern("block_size")), INT2FIX(3));
      rb_hash_aset(good_features_to_track_option, ID2SYM(rb_intern("use_harris")), Qfalse);
      rb_hash_aset(good_features_to_track_option, ID2SYM(rb_intern("k")), rb_float_new(0.04));

      VALUE flood_fill_option = rb_hash_new();
      rb_define_const(rb_klass, "FLOOD_FILL_OPTION", flood_fill_option);
      rb_hash_aset(flood_fill_option, ID2SYM(rb_intern("connectivity")), INT2FIX(4));
      rb_hash_aset(flood_fill_option, ID2SYM(rb_intern("fixed_range")), Qfalse);
      rb_hash_aset(flood_fill_option, ID2SYM(rb_intern("mask_only")), Qfalse);

      VALUE find_contours_option = rb_hash_new();
      rb_define_const(rb_klass, "FIND_CONTOURS_OPTION", find_contours_option);
      rb_hash_aset(find_contours_option, ID2SYM(rb_intern("mode")), INT2FIX(CV_RETR_LIST));
      rb_hash_aset(find_contours_option, ID2SYM(rb_intern("method")), INT2FIX(CV_CHAIN_APPROX_SIMPLE));
      rb_hash_aset(find_contours_option, ID2SYM(rb_intern("offset")), cCvPoint::new_object(cvPoint(0,0)));

      VALUE find_fundamental_matrix_option = rb_hash_new();
      rb_define_const(rb_klass, "FIND_FUNDAMENTAL_MAT_OPTION", find_fundamental_matrix_option);
      rb_hash_aset(find_fundamental_matrix_option, ID2SYM(rb_intern("with_status")), Qfalse);
      rb_hash_aset(find_fundamental_matrix_option, ID2SYM(rb_intern("maximum_distance")), rb_float_new(1.0));
      rb_hash_aset(find_fundamental_matrix_option, ID2SYM(rb_intern("desirable_level")), rb_float_new(0.99));

      rb_define_private_method(rb_klass, "initialize", RUBY_METHOD_FUNC(rb_initialize), -1);
      rb_define_singleton_method(rb_klass, "load", RUBY_METHOD_FUNC(rb_load_imageM), -1);
      // Ruby/OpenCV original functions
      rb_define_method(rb_klass, "method_missing", RUBY_METHOD_FUNC(rb_method_missing), -1);
      rb_define_method(rb_klass, "to_s", RUBY_METHOD_FUNC(rb_to_s), 0);
      rb_define_method(rb_klass, "inside?", RUBY_METHOD_FUNC(rb_inside_q), 1);
      rb_define_method(rb_klass, "to_IplConvKernel", RUBY_METHOD_FUNC(rb_to_IplConvKernel), 1);
      rb_define_method(rb_klass, "create_mask", RUBY_METHOD_FUNC(rb_create_mask), 0);

      rb_define_method(rb_klass, "width", RUBY_METHOD_FUNC(rb_width), 0);
      rb_define_alias(rb_klass, "columns", "width");
      rb_define_alias(rb_klass, "cols", "width");
      rb_define_method(rb_klass, "height", RUBY_METHOD_FUNC(rb_height), 0);
      rb_define_alias(rb_klass, "rows", "height");
      rb_define_method(rb_klass, "depth", RUBY_METHOD_FUNC(rb_depth), 0);
      rb_define_method(rb_klass, "channel", RUBY_METHOD_FUNC(rb_channel), 0);
      rb_define_method(rb_klass, "data", RUBY_METHOD_FUNC(rb_data), 0);

      rb_define_method(rb_klass, "clone", RUBY_METHOD_FUNC(rb_clone), 0);
      rb_define_method(rb_klass, "copy", RUBY_METHOD_FUNC(rb_copy), -1);
      rb_define_method(rb_klass, "to_8u", RUBY_METHOD_FUNC(rb_to_8u), 0);
      rb_define_method(rb_klass, "to_8s", RUBY_METHOD_FUNC(rb_to_8s), 0);
      rb_define_method(rb_klass, "to_16u", RUBY_METHOD_FUNC(rb_to_16u), 0);
      rb_define_method(rb_klass, "to_16s", RUBY_METHOD_FUNC(rb_to_16s), 0);
      rb_define_method(rb_klass, "to_32s", RUBY_METHOD_FUNC(rb_to_32s), 0);
      rb_define_method(rb_klass, "to_32f", RUBY_METHOD_FUNC(rb_to_32f), 0);
      rb_define_method(rb_klass, "to_64f", RUBY_METHOD_FUNC(rb_to_64f), 0);
      rb_define_method(rb_klass, "vector?", RUBY_METHOD_FUNC(rb_vector_q), 0);
      rb_define_method(rb_klass, "square?", RUBY_METHOD_FUNC(rb_square_q), 0);

      rb_define_method(rb_klass, "to_CvMat", RUBY_METHOD_FUNC(rb_to_CvMat), 0);
      rb_define_method(rb_klass, "sub_rect", RUBY_METHOD_FUNC(rb_sub_rect), -2);
      rb_define_alias(rb_klass, "subrect", "sub_rect");
      rb_define_method(rb_klass, "get_rows", RUBY_METHOD_FUNC(rb_get_rows), -1);
      rb_define_method(rb_klass, "get_cols", RUBY_METHOD_FUNC(rb_get_cols), 1);
      rb_define_method(rb_klass, "each_row", RUBY_METHOD_FUNC(rb_each_row), 0);
      rb_define_method(rb_klass, "each_col", RUBY_METHOD_FUNC(rb_each_col), 0);
      rb_define_alias(rb_klass, "each_column", "each_col");
      rb_define_method(rb_klass, "diag", RUBY_METHOD_FUNC(rb_diag), -1);
      rb_define_alias(rb_klass, "diagonal", "diag");
      rb_define_method(rb_klass, "size", RUBY_METHOD_FUNC(rb_size), 0);
      rb_define_method(rb_klass, "dims", RUBY_METHOD_FUNC(rb_dims), 0);
      rb_define_method(rb_klass, "dim_size", RUBY_METHOD_FUNC(rb_dim_size), 1);
      rb_define_method(rb_klass, "[]", RUBY_METHOD_FUNC(rb_aref), -2);
      rb_define_alias(rb_klass, "at", "[]");
      rb_define_method(rb_klass, "[]=", RUBY_METHOD_FUNC(rb_aset), -2);
      rb_define_method(rb_klass, "set_data", RUBY_METHOD_FUNC(rb_set_data), 1);
      rb_define_method(rb_klass, "set", RUBY_METHOD_FUNC(rb_set), -1);
      rb_define_alias(rb_klass, "fill", "set");
      rb_define_method(rb_klass, "set!", RUBY_METHOD_FUNC(rb_set_bang), -1);
      rb_define_alias(rb_klass, "fill!", "set!");
      rb_define_method(rb_klass, "set_zero", RUBY_METHOD_FUNC(rb_set_zero), 0);
      rb_define_alias(rb_klass, "clear", "set_zero");
      rb_define_alias(rb_klass, "zero", "set_zero");
      rb_define_method(rb_klass, "set_zero!", RUBY_METHOD_FUNC(rb_set_zero_bang), 0);
      rb_define_alias(rb_klass, "clear!", "set_zero!");
      rb_define_alias(rb_klass, "zero!", "set_zero!");
      rb_define_method(rb_klass, "identity", RUBY_METHOD_FUNC(rb_set_identity), -1);
      rb_define_method(rb_klass, "identity!", RUBY_METHOD_FUNC(rb_set_identity_bang), -1);
      rb_define_method(rb_klass, "range", RUBY_METHOD_FUNC(rb_range), 2);
      rb_define_method(rb_klass, "range!", RUBY_METHOD_FUNC(rb_range_bang), 2);

      rb_define_method(rb_klass, "reshape", RUBY_METHOD_FUNC(rb_reshape), -1);
      rb_define_method(rb_klass, "repeat", RUBY_METHOD_FUNC(rb_repeat), 1);
      rb_define_method(rb_klass, "flip", RUBY_METHOD_FUNC(rb_flip), -1);
      rb_define_method(rb_klass, "flip!", RUBY_METHOD_FUNC(rb_flip_bang), -1);
      rb_define_method(rb_klass, "split", RUBY_METHOD_FUNC(rb_split), 0);
      rb_define_singleton_method(rb_klass, "merge", RUBY_METHOD_FUNC(rb_merge), -2);
      rb_define_method(rb_klass, "rand_shuffle", RUBY_METHOD_FUNC(rb_rand_shuffle), -1);
      rb_define_method(rb_klass, "rand_shuffle!", RUBY_METHOD_FUNC(rb_rand_shuffle_bang), -1);
      rb_define_method(rb_klass, "lut", RUBY_METHOD_FUNC(rb_lut), 1);
      rb_define_method(rb_klass, "convert_scale", RUBY_METHOD_FUNC(rb_convert_scale), 1);
      rb_define_method(rb_klass, "convert_scale_abs", RUBY_METHOD_FUNC(rb_convert_scale_abs), 1);
      rb_define_method(rb_klass, "add", RUBY_METHOD_FUNC(rb_add), -1);
      rb_define_alias(rb_klass, "+", "add");
      rb_define_method(rb_klass, "sub", RUBY_METHOD_FUNC(rb_sub), -1);
      rb_define_alias(rb_klass, "-", "sub");
      rb_define_method(rb_klass, "mul", RUBY_METHOD_FUNC(rb_mul), -1);
      rb_define_method(rb_klass, "mat_mul", RUBY_METHOD_FUNC(rb_mat_mul), -1);
      rb_define_alias(rb_klass, "*", "mat_mul");
      rb_define_method(rb_klass, "div", RUBY_METHOD_FUNC(rb_div), -1);
      rb_define_alias(rb_klass, "/", "div");
      rb_define_singleton_method(rb_klass, "add_weighted", RUBY_METHOD_FUNC(rb_add_weighted), 5);
      rb_define_method(rb_klass, "and", RUBY_METHOD_FUNC(rb_and), -1);
      rb_define_alias(rb_klass, "&", "and");
      rb_define_method(rb_klass, "or", RUBY_METHOD_FUNC(rb_or), -1);
      rb_define_alias(rb_klass, "|", "or");
      rb_define_method(rb_klass, "xor", RUBY_METHOD_FUNC(rb_xor), -1);
      rb_define_alias(rb_klass, "^", "xor");
      rb_define_method(rb_klass, "not", RUBY_METHOD_FUNC(rb_not), 0);
      rb_define_method(rb_klass, "not!", RUBY_METHOD_FUNC(rb_not_bang), 0);
      rb_define_method(rb_klass, "eq", RUBY_METHOD_FUNC(rb_eq), 1);
      rb_define_method(rb_klass, "gt", RUBY_METHOD_FUNC(rb_gt), 1);
      rb_define_method(rb_klass, "ge", RUBY_METHOD_FUNC(rb_ge), 1);
      rb_define_method(rb_klass, "lt", RUBY_METHOD_FUNC(rb_lt), 1);
      rb_define_method(rb_klass, "le", RUBY_METHOD_FUNC(rb_le), 1);
      rb_define_method(rb_klass, "ne", RUBY_METHOD_FUNC(rb_ne), 1);
      rb_define_method(rb_klass, "in_range", RUBY_METHOD_FUNC(rb_in_range), 2);
      rb_define_method(rb_klass, "abs_diff", RUBY_METHOD_FUNC(rb_abs_diff), 1);
      rb_define_method(rb_klass, "normalize", RUBY_METHOD_FUNC(rb_normalize), -1);
      rb_define_method(rb_klass, "count_non_zero", RUBY_METHOD_FUNC(rb_count_non_zero), 0);
      rb_define_method(rb_klass, "sum", RUBY_METHOD_FUNC(rb_sum), 0);
      rb_define_method(rb_klass, "avg", RUBY_METHOD_FUNC(rb_avg), -1);
      rb_define_method(rb_klass, "avg_sdv", RUBY_METHOD_FUNC(rb_avg_sdv), -1);
      rb_define_method(rb_klass, "sdv", RUBY_METHOD_FUNC(rb_sdv), -1);
      rb_define_method(rb_klass, "min_max_loc", RUBY_METHOD_FUNC(rb_min_max_loc), -1);
      rb_define_singleton_method(rb_klass, "norm", RUBY_METHOD_FUNC(rb_norm), -1);
      rb_define_method(rb_klass, "dot_product", RUBY_METHOD_FUNC(rb_dot_product), 1);
      rb_define_method(rb_klass, "cross_product", RUBY_METHOD_FUNC(rb_cross_product), 1);
      rb_define_method(rb_klass, "transform", RUBY_METHOD_FUNC(rb_transform), -1);
      rb_define_method(rb_klass, "perspective_transform", RUBY_METHOD_FUNC(rb_perspective_transform), 1);
      rb_define_method(rb_klass, "mul_transposed", RUBY_METHOD_FUNC(rb_mul_transposed), -1);
      rb_define_method(rb_klass, "trace", RUBY_METHOD_FUNC(rb_trace), 0);
      rb_define_method(rb_klass, "transpose", RUBY_METHOD_FUNC(rb_transpose), 0);
      rb_define_alias(rb_klass, "t", "transpose");
      rb_define_method(rb_klass, "det", RUBY_METHOD_FUNC(rb_det), 0);
      rb_define_alias(rb_klass, "determinant", "det");
      rb_define_method(rb_klass, "invert", RUBY_METHOD_FUNC(rb_invert), -1);
      rb_define_singleton_method(rb_klass, "solve", RUBY_METHOD_FUNC(rb_solve), -1);
      rb_define_method(rb_klass, "svd", RUBY_METHOD_FUNC(rb_svd), -1);
      rb_define_method(rb_klass, "eigenvv", RUBY_METHOD_FUNC(rb_eigenvv), -1);

      /* drawing function */
      rb_define_method(rb_klass, "line", RUBY_METHOD_FUNC(rb_line), -1);
      rb_define_method(rb_klass, "line!", RUBY_METHOD_FUNC(rb_line_bang), -1);
      rb_define_method(rb_klass, "rectangle", RUBY_METHOD_FUNC(rb_rectangle), -1);
      rb_define_method(rb_klass, "rectangle!", RUBY_METHOD_FUNC(rb_rectangle_bang), -1);
      rb_define_method(rb_klass, "circle", RUBY_METHOD_FUNC(rb_circle), -1);
      rb_define_method(rb_klass, "circle!", RUBY_METHOD_FUNC(rb_circle_bang), -1);
      rb_define_method(rb_klass, "ellipse", RUBY_METHOD_FUNC(rb_ellipse), -1);
      rb_define_method(rb_klass, "ellipse!", RUBY_METHOD_FUNC(rb_ellipse_bang), -1);
      rb_define_method(rb_klass, "ellipse_box", RUBY_METHOD_FUNC(rb_ellipse_box), -1);
      rb_define_method(rb_klass, "ellipse_box!", RUBY_METHOD_FUNC(rb_ellipse_box_bang), -1);
      rb_define_method(rb_klass, "fill_poly", RUBY_METHOD_FUNC(rb_fill_poly), -1);
      rb_define_method(rb_klass, "fill_poly!", RUBY_METHOD_FUNC(rb_fill_poly_bang), -1);
      rb_define_method(rb_klass, "fill_convex_poly", RUBY_METHOD_FUNC(rb_fill_convex_poly), -1);
      rb_define_method(rb_klass, "fill_convex_poly!", RUBY_METHOD_FUNC(rb_fill_convex_poly_bang), -1);
      rb_define_method(rb_klass, "poly_line", RUBY_METHOD_FUNC(rb_poly_line), -1);
      rb_define_method(rb_klass, "poly_line!", RUBY_METHOD_FUNC(rb_poly_line_bang), -1);
      rb_define_method(rb_klass, "put_text", RUBY_METHOD_FUNC(rb_put_text), -1);
      rb_define_method(rb_klass, "put_text!", RUBY_METHOD_FUNC(rb_put_text_bang), -1);

      rb_define_method(rb_klass, "dft", RUBY_METHOD_FUNC(rb_dft), -1);
      rb_define_method(rb_klass, "dct", RUBY_METHOD_FUNC(rb_dct), -1);

      rb_define_method(rb_klass, "sobel", RUBY_METHOD_FUNC(rb_sobel), -1);
      rb_define_method(rb_klass, "laplace", RUBY_METHOD_FUNC(rb_laplace), -1);
      rb_define_method(rb_klass, "canny", RUBY_METHOD_FUNC(rb_canny), -1);
      rb_define_method(rb_klass, "pre_corner_detect", RUBY_METHOD_FUNC(rb_pre_corner_detect), -1);
      rb_define_method(rb_klass, "corner_eigenvv", RUBY_METHOD_FUNC(rb_corner_eigenvv), -1);
      rb_define_method(rb_klass, "corner_min_eigen_val", RUBY_METHOD_FUNC(rb_corner_min_eigen_val), -1);
      rb_define_method(rb_klass, "corner_harris", RUBY_METHOD_FUNC(rb_corner_harris), -1);
      rb_define_method(rb_klass, "find_chessboard_corners", RUBY_METHOD_FUNC(rb_find_chessboard_corners), -1);
      rb_define_method(rb_klass, "find_corner_sub_pix", RUBY_METHOD_FUNC(rb_find_corner_sub_pix), 4);
      rb_define_method(rb_klass, "good_features_to_track", RUBY_METHOD_FUNC(rb_good_features_to_track), -1);

      rb_define_method(rb_klass, "rect_sub_pix", RUBY_METHOD_FUNC(rb_rect_sub_pix), -1);
      rb_define_method(rb_klass, "quadrangle_sub_pix", RUBY_METHOD_FUNC(rb_quadrangle_sub_pix), -1);
      rb_define_method(rb_klass, "resize", RUBY_METHOD_FUNC(rb_resize), -1);
      rb_define_method(rb_klass, "warp_affine", RUBY_METHOD_FUNC(rb_warp_affine), -1);
      rb_define_singleton_method(rb_klass, "rotation_matrix2D", RUBY_METHOD_FUNC(rb_rotation_matrix2D), 3);
      rb_define_singleton_method(rb_klass, "get_perspective_transform", RUBY_METHOD_FUNC(rb_get_perspective_transform), 2);
      rb_define_method(rb_klass, "warp_perspective", RUBY_METHOD_FUNC(rb_warp_perspective), -1);
      rb_define_singleton_method(rb_klass, "find_homography", RUBY_METHOD_FUNC(rb_find_homography), -1);
      rb_define_method(rb_klass, "remap", RUBY_METHOD_FUNC(rb_remap), -1);
      rb_define_method(rb_klass, "log_polar", RUBY_METHOD_FUNC(rb_log_polar), -1);

      rb_define_method(rb_klass, "erode", RUBY_METHOD_FUNC(rb_erode), -1);
      rb_define_method(rb_klass, "erode!", RUBY_METHOD_FUNC(rb_erode_bang), -1);
      rb_define_method(rb_klass, "dilate", RUBY_METHOD_FUNC(rb_dilate), -1);
      rb_define_method(rb_klass, "dilate!", RUBY_METHOD_FUNC(rb_dilate_bang), -1);
      rb_define_method(rb_klass, "morphology", RUBY_METHOD_FUNC(rb_morphology), -1);

      rb_define_method(rb_klass, "smooth", RUBY_METHOD_FUNC(rb_smooth), -1);
      rb_define_method(rb_klass, "copy_make_border", RUBY_METHOD_FUNC(rb_copy_make_border), -1);
      rb_define_method(rb_klass, "filter2d", RUBY_METHOD_FUNC(rb_filter2d), -1);
      rb_define_method(rb_klass, "integral", RUBY_METHOD_FUNC(rb_integral), -1);
      rb_define_method(rb_klass, "threshold", RUBY_METHOD_FUNC(rb_threshold), -1);
      rb_define_method(rb_klass, "adaptive_threshold", RUBY_METHOD_FUNC(rb_adaptive_threshold), -1);

      rb_define_method(rb_klass, "pyr_down", RUBY_METHOD_FUNC(rb_pyr_down), -1);
      rb_define_method(rb_klass, "pyr_up", RUBY_METHOD_FUNC(rb_pyr_up), -1);

      rb_define_method(rb_klass, "flood_fill", RUBY_METHOD_FUNC(rb_flood_fill), -1);
      rb_define_method(rb_klass, "flood_fill!", RUBY_METHOD_FUNC(rb_flood_fill_bang), -1);
      rb_define_method(rb_klass, "find_contours", RUBY_METHOD_FUNC(rb_find_contours), -1);
      rb_define_method(rb_klass, "find_contours!", RUBY_METHOD_FUNC(rb_find_contours_bang), -1);
      rb_define_method(rb_klass, "draw_contours", RUBY_METHOD_FUNC(rb_draw_contours), -1);
      rb_define_method(rb_klass, "draw_contours!", RUBY_METHOD_FUNC(rb_draw_contours_bang), -1);
      rb_define_method(rb_klass, "draw_chessboard_corners", RUBY_METHOD_FUNC(rb_draw_chessboard_corners), 3);
      rb_define_method(rb_klass, "draw_chessboard_corners!", RUBY_METHOD_FUNC(rb_draw_chessboard_corners_bang), 3);
      rb_define_method(rb_klass, "pyr_mean_shift_filtering", RUBY_METHOD_FUNC(rb_pyr_mean_shift_filtering), -1);
      rb_define_method(rb_klass, "watershed", RUBY_METHOD_FUNC(rb_watershed), 1);

      rb_define_method(rb_klass, "moments", RUBY_METHOD_FUNC(rb_moments), -1);

      rb_define_method(rb_klass, "hough_lines", RUBY_METHOD_FUNC(rb_hough_lines), -1);
      rb_define_method(rb_klass, "hough_circles", RUBY_METHOD_FUNC(rb_hough_circles), -1);

      rb_define_method(rb_klass, "inpaint", RUBY_METHOD_FUNC(rb_inpaint), 3);

      rb_define_method(rb_klass, "equalize_hist", RUBY_METHOD_FUNC(rb_equalize_hist), 0);
      rb_define_method(rb_klass, "apply_color_map", RUBY_METHOD_FUNC(rb_apply_color_map), 1);
      rb_define_method(rb_klass, "match_template", RUBY_METHOD_FUNC(rb_match_template), -1);
      rb_define_method(rb_klass, "match_shapes", RUBY_METHOD_FUNC(rb_match_shapes), -1);

      rb_define_method(rb_klass, "mean_shift", RUBY_METHOD_FUNC(rb_mean_shift), 2);
      rb_define_method(rb_klass, "cam_shift", RUBY_METHOD_FUNC(rb_cam_shift), 2);
      rb_define_method(rb_klass, "snake_image", RUBY_METHOD_FUNC(rb_snake_image), -1);

      rb_define_singleton_method(rb_klass, "find_fundamental_mat",
				 RUBY_METHOD_FUNC(rb_find_fundamental_mat), -1);
      rb_define_singleton_method(rb_klass, "compute_correspond_epilines",
				 RUBY_METHOD_FUNC(rb_compute_correspond_epilines), 3);

      rb_define_method(rb_klass, "extract_surf", RUBY_METHOD_FUNC(rb_extract_surf), -1);

      rb_define_method(rb_klass, "subspace_project", RUBY_METHOD_FUNC(rb_subspace_project), 2);
      rb_define_method(rb_klass, "subspace_reconstruct", RUBY_METHOD_FUNC(rb_subspace_reconstruct), 2);

      rb_define_method(rb_klass, "save_image", RUBY_METHOD_FUNC(rb_save_image), -1);
      rb_define_alias(rb_klass, "save", "save_image");

      rb_define_method(rb_klass, "encode_image", RUBY_METHOD_FUNC(rb_encode_imageM), -1);
      rb_define_alias(rb_klass, "encode", "encode_image");
      rb_define_singleton_method(rb_klass, "decode_image", RUBY_METHOD_FUNC(rb_decode_imageM), -1);
      rb_define_alias(rb_singleton_class(rb_klass), "decode", "decode_image");
    }
  }
}
