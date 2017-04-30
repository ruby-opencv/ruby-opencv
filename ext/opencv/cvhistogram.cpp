/************************************************************

   cvhistogram.cpp -

   $Author: lsxi $

   Copyright (C) 2005-2008 Masakazu Yonekura

************************************************************/
#include "cvhistogram.h"
#include "cvmat.h"
#include "iplimage.h"
#include "cvsize.h"
#include "opencv.h"

/*
 * Document-class: OpenCV::CvHistogram
 *
 * Multi-dimensional histogram.
 */
namespace mOpenCV {
  namespace cCvHistogram {
    VALUE rb_klass;

    VALUE
    rb_class()
    {
      return rb_klass;
    }

    void
    release_hist(void* ptr)
    {
      if (ptr) {
	try {
	  cvReleaseHist((CvHistogram**)&ptr);
	}
	catch (cv::Exception& e) {
	  raise_cverror(e);
	}
      }
    }

    VALUE
    rb_allocate(VALUE klass)
    {
      CvHistogram* ptr = NULL;
      return Data_Wrap_Struct(klass, 0, release_hist, ptr);
    }

    float*
    ary2fltptr(VALUE ary, float* buff)
    {
      Check_Type(ary, T_ARRAY);
      int size = RARRAY_LEN(ary);
      VALUE* ary_ptr = RARRAY_PTR(ary);
      for (int i = 0; i < size; ++i) {
	buff[i] = NUM2DBL(ary_ptr[i]);
      }
      return buff;
    }

    int*
    ary2intptr(VALUE ary, int* buff)
    {
      Check_Type(ary, T_ARRAY);
      int size = RARRAY_LEN(ary);
      VALUE* ary_ptr = RARRAY_PTR(ary);
      for (int i = 0; i < size; ++i) {
	buff[i] = NUM2INT(ary_ptr[i]);
      }
      return buff;
    }

    /*
     * Creates a histogram
     * @overload new(dims, sizes, type, ranges=nil, uniform=true)
     * @param dims [Integer] Number of histogram dimensions
     * @param sizes [Array<Integer>] Array of the histogram dimension sizes
     * @param type [Integer]
     *     Histogram representation format. CV_HIST_ARRAY means that the histogram data is represented
     *     as a multi-dimensional dense array CvMatND. CV_HIST_SPARSE means that histogram data is
     *     represented as a multi-dimensional sparse array CvSparseMat.
     * @param ranges [Array<Integer>]
     *     Array of ranges for the histogram bins. Its meaning depends on the uniform parameter value.
     *     The ranges are used when the histogram is calculated or backprojected to determine which
     *     histogram bin corresponds to which value/tuple of values from the input image(s).
     * @param uniform [Boolean] Uniformity flag.
     * @return [CvHistogram] Histogram
     * @opencv_func cvCreateHist
     */
    VALUE
    rb_initialize(int argc, VALUE *argv, VALUE self)
    {
      VALUE _dims, _sizes, _type, _ranges, _uniform;
      int uniform;
      int* sizes;
      float** ranges = NULL;

      rb_scan_args(argc, argv, "32", &_dims, &_sizes, &_type, &_ranges, &_uniform);
      int sizes_len = RARRAY_LEN(_sizes);
      sizes = ALLOCA_N(int, sizes_len);

      if (NIL_P(_ranges)) {
	sizes = ary2intptr(_sizes, sizes);
	ranges = NULL;
      }
      else {
	ranges = ALLOCA_N(float*, sizes_len);
	VALUE* range_ptr = RARRAY_PTR(_ranges);
	int i;
	for (i = 0; i < sizes_len; i++) {
	  sizes[i] = NUM2INT(RARRAY_PTR(_sizes)[i]);
	  ranges[i] = ary2fltptr(range_ptr[i], ALLOCA_N(float, 2));
	}
      }
      uniform = TRUE_OR_FALSE(_uniform, 1);

      try {
	DATA_PTR(self) = cvCreateHist(NUM2INT(_dims), sizes, NUM2INT(_type), ranges, uniform);
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }

      return self;
    }

    /*
     * Returns <tt>self</tt> is uniform histogram or not
     * @overload is_uniform?
     * @return [Boolean] Uniform or not
     * @opencv_func CV_IS_UNIFORM_HIST
     */
    VALUE
    rb_is_uniform(VALUE self)
    {
      return CV_IS_UNIFORM_HIST(CVHISTOGRAM(self)) ? Qtrue : Qfalse;
    }

    /*
     * Returns <tt>self</tt> is sparse histogram or not
     * @overload is_sparse?
     * @return [Boolean] Sparse or not
     * @opencv_func CV_IS_SPARSE_HIST
     */
    VALUE
    rb_is_sparse(VALUE self)
    {
      return CV_IS_SPARSE_HIST(CVHISTOGRAM(self)) ? Qtrue : Qfalse;
    }

    /*
     * Returns <tt>self</tt> has range or not
     * @overload has_range?
     * @return [Boolean] Has range or not
     * @opencv_func CV_HIST_HAS_RANGES
     */
    VALUE
    rb_has_range(VALUE self)
    {
      return CV_HIST_HAS_RANGES(CVHISTOGRAM(self)) ? Qtrue : Qfalse;
    }

    /*
     * Calculates a histogram of a set of arrays.
     * @overload calc_hist(images, accumulate=nil, mask=nil)
     * @param images [Array<IplImage>]
     *    Source arrays. They all should have the same depth, CV_8U or CV_32F, and the same size.
     *    Each of them can have an arbitrary number of channels.
     * @param accumulate [Boolean]
     *    Accumulation flag. If it is set, the histogram is not cleared in the beginning when it is allocated.
     *    This feature enables you to compute a single histogram from several sets of arrays,
     *    or to update the histogram in time.
     * @param mask [CvMat]
     *    Optional mask. If the matrix is not empty, it must be an 8-bit array of the same size as images[i].
     *    The non-zero mask elements mark the array elements counted in the histogram.
     * @return [CvHistogram] Histogram of a set of arrays
     * @opencv_func cvCalcHist
     */
    VALUE
    rb_calc_hist(int argc, VALUE* argv, VALUE self)
    {
      return rb_calc_hist_bang(argc, argv, rb_copy_hist(self));
    }

    /*
     * Calculates a histogram of a set of arrays.
     * @overload calc_hist!(images, accumulate=nil, mask=nil)
     * @see #calc_hist
     * @opencv_func cvCalcHist
     */
    VALUE
    rb_calc_hist_bang(int argc, VALUE* argv, VALUE self)
    {
      VALUE images, accumulate, mask;
      rb_scan_args(argc, argv, "12", &images, &accumulate, &mask);
      Check_Type(images, T_ARRAY);
      int num_images = RARRAY_LEN(images);
      if (num_images == 0) {
	rb_raise(rb_eArgError, "One or more arrays are required.");
      }
      IplImage** img = ALLOCA_N(IplImage*, num_images);
      VALUE* images_ptr = RARRAY_PTR(images);
      for (int i = 0; i < num_images; i++) {
	img[i] = IPLIMAGE_WITH_CHECK(images_ptr[i]);
      }
      CvMat* m = NIL_P(mask) ? NULL : CVMAT_WITH_CHECK(mask);
      try {
	cvCalcHist(img, CVHISTOGRAM(self), TRUE_OR_FALSE(accumulate, 0), m);
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return self;
    }

    /*
     * Queries the value of the histogram bin.
     * @overload [](idx0)
     * @overload [](idx0, idx1)
     * @overload [](idx0, idx1, idx2)
     * @overload [](idx0, idx1, idx2, idx3, ...)
     * @param idx* [Integer] *-th index
     * @return [Number] The value of the specified bin of the 1D, 2D, 3D, or N-D histogram.
     * @opencv_func cvQueryHistValue_1D
     * @opencv_func cvQueryHistValue_2D
     * @opencv_func cvQueryHistValue_3D
     * @opencv_func cvQueryHistValue_nD
     */
    VALUE
    rb_aref(VALUE self, VALUE args)
    {
      int num_idx = RARRAY_LEN(args);
      int* idx = ALLOCA_N(int, num_idx);
      VALUE* args_ptr = RARRAY_PTR(args);
      for (int i = 0; i < num_idx; i++) {
	idx[i] = NUM2INT(args_ptr[i]);
      }
  
      float value = 0.0;
      CvHistogram* self_ptr = CVHISTOGRAM(self);
      try {
	switch (num_idx) {
	case 1:
	  value = (float)cvGetReal1D(self_ptr, idx[0]);
	  break;
	case 2:
	  value = (float)cvGetReal2D(self_ptr, idx[0], idx[1]);
	  break;
	case 3:
	  value = (float)cvGetReal3D(self_ptr, idx[0], idx[1], idx[2]);
	  break;
	default:
	  value = (float)cvGetRealND(self_ptr, idx);
	  break;
	}
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
  
      return rb_float_new((double)value);
    }

    /*
     * Finds the minimum and maximum histogram bins.
     * @overload min_max_value
     * @return [Array]
     *    [min_value, max_value, min_idx, max_idx]: Array of the minimum / maximum value of the histogram
     *    and their coordinates.
     *    - min_value: The minimum value of the histogram.
     *    - max_value: The maximum value of the histogram.
     *    - min_idx: The array of coordinates for the minimum.
     *    - max_idx: The array of coordinates for the maximum.
     * @opencv_func cvGetMinMaxHistValue
     */
    VALUE
    rb_min_max_value(VALUE self)
    {
      CvHistogram* self_ptr = CVHISTOGRAM(self);
      int dims = 0;
      float min_value = 0.0, max_value = 0.0;
      int *min_idx = NULL;
      int *max_idx = NULL;
      try {
	dims = cvGetDims(self_ptr->bins, NULL);
	min_idx = ALLOCA_N(int, dims);
	max_idx = ALLOCA_N(int, dims);
	cvGetMinMaxHistValue(CVHISTOGRAM(self), &min_value, &max_value, min_idx, max_idx);
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }

      VALUE _min_idx = rb_ary_new2(dims);
      VALUE _max_idx = rb_ary_new2(dims);
      for (int i = 0; i < dims; i++) {
	rb_ary_store(_min_idx, i, INT2NUM(min_idx[i]));
	rb_ary_store(_max_idx, i, INT2NUM(max_idx[i]));
      }

      return rb_ary_new3(4, rb_float_new((double)min_value), rb_float_new((double)max_value),
			 _min_idx, _max_idx);
    }

    /*
     * Returns number of array dimensions
     * @overload [](idx0, idx1, ...)
     * @param idx* [Integer] *-th index
     * @return [Array<Integer, Array<Integer>>]
     *     [dims, sizes]: Number of array dimensions and its sizes.
     *     - dims (Integer): Number of array dimensions
     *     - sizes (Array<Integer>): Vector of the array dimension sizes.
     *       For 2D arrays the number of rows (height) goes first,
     *       number of columns (width) next.
     * @opencv_func cvGetDims
     */
    VALUE
    rb_dims(VALUE self)
    {
      VALUE _sizes = Qnil;
      int size[CV_MAX_DIM];
      int dims = 0;
      try {
	dims = cvGetDims(CVHISTOGRAM(self)->bins, size);  
	_sizes = rb_ary_new2(dims);
	for (int i = 0; i < dims; ++i) {
	  rb_ary_store(_sizes, i, INT2NUM(size[i]));
	}
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return rb_assoc_new(INT2NUM(dims), _sizes);
    }

    /*
     * Clones histogram
     * @overload copy_hist
     * @return [CvHistogram] Copy of the histogram
     * @opencv_func cvCopyHist
     */
    VALUE
    rb_copy_hist(VALUE self)
    {
      CvHistogram* hist = NULL;
      try {
	cvCopyHist(CVHISTOGRAM(self), &hist);
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return Data_Wrap_Struct(rb_klass, 0, release_hist, hist);
    }

    /*
     * Sets all histogram bins to 0 in case of dense histogram
     * and removes all histogram bins in case of sparse array.
     * @overload clear_hist
     * @return [CvHistogram] Cleared histogram
     * @opencv_func cvClearHist
     */
    VALUE
    rb_clear_hist(VALUE self)
    {
      return rb_clear_hist_bang(rb_copy_hist(self));
    }

    /*
     * Sets all histogram bins to 0 in case of dense histogram
     * and removes all histogram bins in case of sparse array.
     * This method changes <tt>self</tt>.
     * @overload clear_hist!
     * @see #clear_hist
     * @return [CvHistogram] Cleared histogram
     * @opencv_func cvClearHist
     */
    VALUE
    rb_clear_hist_bang(VALUE self)
    {
      try {
	cvClearHist(CVHISTOGRAM(self));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return self;
    }

    /*
     * Returns normalized the histogram bins by scaling them,
     * such that the sum of the bins becomes equal to <tt>factor</tt>.
     * @overload normalize(factor)
     * @param factor [Number] Normalization factor. The sum of the bins becomes equal to this value.
     * @return [CvHistogram] Normalized histogram
     * @opencv_func cvNormalizeHist
     */
    VALUE
    rb_normalize_hist(VALUE self, VALUE factor)
    {
      return rb_normalize_hist_bang(rb_copy_hist(self), factor);
    }

    /*
     * Returns normalized the histogram bins by scaling them,
     * such that the sum of the bins becomes equal to <tt>factor</tt>.
     * This method changes <tt>self</tt>.
     * @overload normalize!(factor)
     * @param factor [Number] Normalization factor. The sum of the bins becomes equal to this value.
     * @return [CvHistogram] Normalized histogram
     * @see #normalize
     * @opencv_func cvNormalizeHist
     */
    VALUE
    rb_normalize_hist_bang(VALUE self, VALUE factor)
    {
      try {
	cvNormalizeHist(CVHISTOGRAM(self), NUM2DBL(factor));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return self;
    }

    /*
     * Returns cleared histogram bins that are below the specified threshold.
     * @overload thresh_hist(threshold)
     * @param threshold [Number] Threshold value
     * @return [CvHistogram] Cleared histogram
     * @opencv_func cvThreshHist
     */
    VALUE
    rb_thresh_hist(VALUE self, VALUE threshold)
    {
      return rb_thresh_hist_bang(rb_copy_hist(self), threshold);
    }

    /*
     * Cleares histogram bins that are below the specified threshold.
     * This method changes <tt>self</tt>.
     * @overload thresh_hist!(threshold)
     * @param threshold [Number] Threshold value
     * @return [CvHistogram] Cleared histogram
     * @see #thresh_hist
     * @opencv_func cvThreshHist
     */
    VALUE
    rb_thresh_hist_bang(VALUE self, VALUE threshold)
    {
      try {
	cvThreshHist(CVHISTOGRAM(self), NUM2DBL(threshold));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return self;
    }

    /*
     * Sets the bounds of the histogram bins.
     * @overload set_hist_bin_ranges(ranges, uniform=true)
     * @param ranges [Array<Number>]
     *     Array of ranges for the histogram bins. Its meaning depends on the uniform parameter value.
     *     The ranges are used when the histogram is calculated or backprojected to determine
     *     which histogram bin corresponds to which value/tuple of values from the input image(s).
     * @param uniform [Boolean]
     *     Uniformity flag.
     * @return [CvHistogram]
     *     Histogram
     * @opencv_func cvSetHistBinRanges
     */
    VALUE
    rb_set_hist_bin_ranges(int argc, VALUE* argv, VALUE self)
    {
      return rb_set_hist_bin_ranges_bang(argc, argv, rb_copy_hist(self));
    }

    /*
     * Sets the bounds of the histogram bins. This method changes <tt>self</tt>.
     * @overload set_hist_bin_ranges!(ranges, uniform=true)
     * @param ranges [Array<Number>]
     *     Array of ranges for the histogram bins. Its meaning depends on the uniform parameter value.
     *     The ranges are used when the histogram is calculated or backprojected to determine
     *     which histogram bin corresponds to which value/tuple of values from the input image(s).
     * @param uniform [Boolean]
     *     Uniformity flag.
     * @return [CvHistogram]
     *     Histogram
     * @see #set_hist_bin_ranges
     * @opencv_func cvSetHistBinRanges
     */
    VALUE
    rb_set_hist_bin_ranges_bang(int argc, VALUE* argv, VALUE self)
    {
      VALUE _ranges, _uniform;
      rb_scan_args(argc, argv, "11", &_ranges, &_uniform);
      Check_Type(_ranges, T_ARRAY);

      int ranges_size = RARRAY_LEN(_ranges);
      float** ranges = ALLOCA_N(float*, ranges_size);
      VALUE* range_ptr = RARRAY_PTR(_ranges);
      for (int i = 0; i < ranges_size; ++i) {
	ranges[i] = ary2fltptr(range_ptr[i], ALLOCA_N(float, 2));
      }
      int uniform = TRUE_OR_FALSE(_uniform, 1);

      try {
	cvSetHistBinRanges(CVHISTOGRAM(self), ranges, uniform);
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }

      return self;
    }

    /*
     * Calculates the back projection of a histogram.
     * @overload calc_back_project(images)
     * @param images [Array<IplImage>]
     *     Source arrays. They all should have the same depth, CV_8U or CV_32F, and the same size.
     *     Each of them can have an arbitrary number of channels.
     * @return [CvMat,IplImage]
     *     Destination back projection array that is a single-channel array of the same size and depth
     *     as the first element of <tt>images</tt>
     * @opencv_func cvCalcBackProject
     */
    VALUE
    rb_calc_back_project(VALUE self, VALUE image)
    {
      Check_Type(image, T_ARRAY);
      int num_images = RARRAY_LEN(image);
      if (num_images == 0) {
	return Qnil;
      }
  
      IplImage** img = ALLOCA_N(IplImage*, num_images);
      VALUE* image_ptr = RARRAY_PTR(image);
      for (int i = 0; i < num_images; ++i) {
	img[i] = IPLIMAGE_WITH_CHECK(image_ptr[i]);
      }
  
      CvSize size;
      size.width = img[0]->width;
      size.height = img[0]->height;
      VALUE back_project = cCvMat::new_mat_kind_object(size, image_ptr[0]);
      try {
	cvCalcBackProject(img, CVARR(back_project), CVHISTOGRAM(self));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
  
      return back_project;
    }

    /*
     * Locates a template within an image by using a histogram comparison.
     * @overload calc_back_project_patch(images, patch_size, method, factor)
     * @param images [Array<IplImage>] Source arrays.
     * @param pach_size [CvSize] Size of the patch slid though the source image.
     * @param method [Integer]
     *     Comparison method that could be one of the following:
     *     - <tt>CV_COMP_CORREL</tt>: Correlation
     *     - <tt>CV_COMP_CHISQR</tt>: Chi-Square
     *     - <tt>CV_COMP_INTERSECT</tt>: Intersection
     *     - <tt>CV_COMP_BHATTACHARYYA</tt>: Bhattacharyya distance
     *     - <tt>CV_COMP_HELLINGER</tt>: Synonym for <tt>CV_COMP_BHATTACHARYYA</tt>
     * @param factor [Number]
     *     Normalization factor for histograms that affects the normalization scale
     *     of the destination image. Pass 1 if not sure.
     * @return [CvMat,IplImage] Destination image.
     * @opencv_func cvCalcBackProject
     */
    VALUE
    rb_calc_back_project_patch(VALUE self, VALUE image, VALUE patch_size, VALUE method, VALUE factor)
    {
      Check_Type(image, T_ARRAY);
      int num_images = RARRAY_LEN(image);
      if (num_images == 0) {
	return Qnil;
      }
  
      IplImage** img = ALLOCA_N(IplImage*, num_images);
      VALUE* image_ptr = RARRAY_PTR(image);
      for (int i = 0; i < num_images; ++i) {
	img[i] = IPLIMAGE_WITH_CHECK(image_ptr[i]);
      }

      CvSize patchsize = VALUE_TO_CVSIZE(patch_size);
      CvSize dst_size;
      dst_size.width = img[0]->width - patchsize.width + 1;
      dst_size.height = img[0]->height - patchsize.height + 1;

      VALUE dst = cCvMat::new_mat_kind_object(dst_size, image_ptr[0], CV_32F, 1);
      try {
	cvCalcBackProjectPatch(img, CVARR(dst), patchsize, CVHISTOGRAM(self),
			       NUM2INT(method), NUM2DBL(factor));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
  
      return dst;
    }

    /*
     * Compares two histograms.
     * @overload compare_hist(hist1, hist2, method)
     * @param hist1 [CvHistogram] First compared histogram.
     * @param hist2 [CvHistogram] Second compared histogram of the same size as <tt>hist1</tt>.
     * @param method [Integer]
     *     Comparison method that could be one of the following:
     *     - <tt>CV_COMP_CORREL</tt>: Correlation
     *     - <tt>CV_COMP_CHISQR</tt>: Chi-Square
     *     - <tt>CV_COMP_INTERSECT</tt>: Intersection
     *     - <tt>CV_COMP_BHATTACHARYYA</tt>: Bhattacharyya distance
     *     - <tt>CV_COMP_HELLINGER</tt>: Synonym for <tt>CV_COMP_BHATTACHARYYA</tt>
     * @return [Number] Distance of the two histograms.
     * @scope class
     * @opencv_func cvCompareHist
     */
    VALUE
    rb_compare_hist(VALUE self, VALUE hist1, VALUE hist2, VALUE method)
    {
      double result = 0;
      try {
	result = cvCompareHist(CVHISTOGRAM_WITH_CHECK(hist1), CVHISTOGRAM_WITH_CHECK(hist2),
			       NUM2INT(method));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }

      return rb_float_new(result);
    }

    /*
     * Divides one histogram by another.
     * @overload calc_prob_density(hist1, hist2, scale=255)
     * @param hist1 [CvHistogram] First histogram (the divisor).
     * @param hist2 [CvHistogram] Second histogram.
     * @param scale [Number] Scale factor for the destination histogram.
     * @return [CvHistogram] Destination histogram.
     * @opencv_func cvCalcProbDensity
     */
    VALUE
    rb_calc_prob_density(int argc, VALUE* argv, VALUE self)
    {
      VALUE hist1, hist2, scale;
      rb_scan_args(argc, argv, "21", &hist1, &hist2, &scale);
      double s = NIL_P(scale) ? 255 : NUM2DBL(scale);

      CvHistogram* hist1_ptr = CVHISTOGRAM_WITH_CHECK(hist1);
      VALUE dst_hist = rb_allocate(rb_klass);
      try {
	cvCopyHist(hist1_ptr, (CvHistogram**)&(DATA_PTR(dst_hist)));
	cvCalcProbDensity(hist1_ptr, CVHISTOGRAM_WITH_CHECK(hist2), CVHISTOGRAM(dst_hist), s);
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }

      return dst_hist;
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
      rb_klass = rb_define_class_under(opencv, "CvHistogram", rb_cObject);
      rb_define_alloc_func(rb_klass, rb_allocate);
      rb_define_method(rb_klass, "initialize", RUBY_METHOD_FUNC(rb_initialize), -1);
      rb_define_method(rb_klass, "is_uniform?", RUBY_METHOD_FUNC(rb_is_uniform), 0);
      rb_define_method(rb_klass, "is_sparse?", RUBY_METHOD_FUNC(rb_is_sparse), 0);
      rb_define_method(rb_klass, "has_range?", RUBY_METHOD_FUNC(rb_has_range), 0);
      rb_define_method(rb_klass, "dims", RUBY_METHOD_FUNC(rb_dims), 0);
      rb_define_method(rb_klass, "calc_hist", RUBY_METHOD_FUNC(rb_calc_hist), -1);
      rb_define_method(rb_klass, "calc_hist!", RUBY_METHOD_FUNC(rb_calc_hist_bang), -1);
      rb_define_method(rb_klass, "[]", RUBY_METHOD_FUNC(rb_aref), -2);
      rb_define_alias(rb_klass, "query_hist_value", "[]");
      rb_define_method(rb_klass, "min_max_value", RUBY_METHOD_FUNC(rb_min_max_value), 0);
      rb_define_method(rb_klass, "copy_hist", RUBY_METHOD_FUNC(rb_copy_hist), 0);

      rb_define_method(rb_klass, "clear_hist", RUBY_METHOD_FUNC(rb_clear_hist), 0);
      rb_define_alias(rb_klass, "clear", "clear_hist");
      rb_define_method(rb_klass, "clear_hist!", RUBY_METHOD_FUNC(rb_clear_hist_bang), 0);
      rb_define_alias(rb_klass, "clear!", "clear_hist!");

      rb_define_method(rb_klass, "normalize_hist", RUBY_METHOD_FUNC(rb_normalize_hist), 1);
      rb_define_alias(rb_klass, "normalize", "normalize_hist");
      rb_define_method(rb_klass, "normalize_hist!", RUBY_METHOD_FUNC(rb_normalize_hist_bang), 1);
      rb_define_alias(rb_klass, "normalize!", "normalize_hist!");

      rb_define_method(rb_klass, "thresh_hist", RUBY_METHOD_FUNC(rb_thresh_hist), 1);
      rb_define_alias(rb_klass, "thresh", "thresh_hist");
      rb_define_method(rb_klass, "thresh_hist!", RUBY_METHOD_FUNC(rb_thresh_hist_bang), 1);
      rb_define_alias(rb_klass, "thresh!", "thresh_hist!");

      rb_define_method(rb_klass, "set_hist_bin_ranges", RUBY_METHOD_FUNC(rb_set_hist_bin_ranges), -1);
      rb_define_method(rb_klass, "set_hist_bin_ranges!", RUBY_METHOD_FUNC(rb_set_hist_bin_ranges_bang), -1);

      rb_define_method(rb_klass, "calc_back_project", RUBY_METHOD_FUNC(rb_calc_back_project), 1);
      rb_define_method(rb_klass, "calc_back_project_patch", RUBY_METHOD_FUNC(rb_calc_back_project_patch), 4);

      rb_define_singleton_method(rb_klass, "calc_prob_density", RUBY_METHOD_FUNC(rb_calc_prob_density), -1);
      rb_define_singleton_method(rb_klass, "compare_hist", RUBY_METHOD_FUNC(rb_compare_hist), 3);
    }
  }
}
