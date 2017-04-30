/************************************************************

   iplimage.cpp -

   $Author: lsxi $

   Copyright (C) 2005-2006 Masakazu Yonekura

************************************************************/
#include "iplimage.h"
#include "cvmat.h"
#include "cvrect.h"
#include "cvseq.h"
#include "cvmemstorage.h"
#include "cvconnectedcomp.h"
#include "opencv.h"
/*
 * Document-class: OpenCV::IplImage
 *
 * IPL(Intel Image Processing Library) Image class. 
 *
 * IplImage is subclass of CvMat. IplImage support ROI(region of interest) and COI(color of interest).
 * Most of CvMat method support ROI, and some of CvMat method support COI.
 */
namespace mOpenCV {
  namespace cIplImage {
    VALUE rb_klass;

    VALUE
    rb_class()
    {
      return rb_klass;
    }

    VALUE
    rb_allocate(VALUE klass)
    {
      return OPENCV_OBJECT(rb_klass, 0);
    }

    /*
     * call-seq:
     *   new(width, height[, depth = CV_8U][, channel = 3])
     *
     * Create width * height image. Each element-value set 0.
     *
     * Each element possigle range is set by <i>depth</i>. Default is unsigned 8bit.
     *
     * Number of channel is set by <i>channel</i>. <i>channel</i> should be 1..4.
     *
     * note: width = col, height = row, on CvMat. It is noted not to make a mistake
     * because the order of argument is differenct to CvMat.
     */
    VALUE
    rb_initialize(int argc, VALUE *argv, VALUE self)
    {
      VALUE width, height, depth, channel;
      rb_scan_args(argc, argv, "22", &width, &height, &depth, &channel);
      int _depth = CVMETHOD("DEPTH", depth, CV_8U);
      int _channel = argc < 4 ? 3 : NUM2INT(channel);
      DATA_PTR(self) = rb_cvCreateImage(cvSize(NUM2INT(width), NUM2INT(height)), cvIplDepth(_depth), _channel);
      return self;
    }

    /*
     * call-seq:
     *   IplImage::load(filename[,iscolor = CV_LOAD_IMAGE_COLOR])
     *
     * Load an image from file.
     *  iscolor = CV_LOAD_IMAGE_COLOR, the loaded image is forced to be a 3-channel color image
     *  iscolor = CV_LOAD_IMAGE_GRAYSCALE, the loaded image is forced to be grayscale
     *  iscolor = CV_LOAD_IMAGE_UNCHANGED, the loaded image will be loaded as is.
     * Currently the following file format are supported.
     * * Windows bitmaps - BMP,DIB
     * * JPEG files - JPEG,JPG,JPE
     * * Portable Network Graphics - PNG
     * * Portable image format - PBM,PGM,PPM
     * * Sun rasters - SR,RAS     
     * * TIFF files - TIFF,TIF
     */
    VALUE
    rb_load_image(int argc, VALUE *argv, VALUE self)
    {
      VALUE filename, iscolor;
      rb_scan_args(argc, argv, "11", &filename, &iscolor);
      Check_Type(filename, T_STRING);

      int _iscolor;
      if (TYPE(iscolor) == T_NIL) {
	_iscolor = CV_LOAD_IMAGE_COLOR;
      }
      else {
	Check_Type(iscolor, T_FIXNUM);
	_iscolor = FIX2INT(iscolor);
      }
  
      IplImage *image;
      if ((image = cvLoadImage(StringValueCStr(filename), _iscolor)) == NULL) {
	rb_raise(rb_eStandardError, "file does not exist or invalid format image.");
      }
      return OPENCV_OBJECT(rb_klass, image);
    }

    /*
     * call-seq:
     *   decode_image(buf[, iscolor=CV_LOAD_IMAGE_COLOR]) -> IplImage
     *
     * Reads an image from a buffer in memory.
     *
     * Parameters:
     *   buf <CvMat, Array, String> - Input array
     *   iscolor <Integer> - Flags specifying the color type of a decoded image (the same flags as CvMat#load)
     */
    VALUE
    rb_decode_image(int argc, VALUE *argv, VALUE self)
    {
      int iscolor, need_release;
      CvMat* buff = cCvMat::prepare_decoding(argc, argv, &iscolor, &need_release);
      IplImage* img_ptr = NULL;
      try {
	img_ptr = cvDecodeImage(buff, iscolor);
	if (need_release) {
	  cvReleaseMat(&buff);
	}
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }

      return OPENCV_OBJECT(rb_klass, img_ptr);
    }

    /*
     * Get ROI as CvRect.
     */
    VALUE
    rb_get_roi(VALUE self)
    {
      CvRect rect;
      try {
	rect = cvGetImageROI(IPLIMAGE(self));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return cCvRect::new_object(rect);
    }

    /*
     * call-seq:
     *   set_roi(rect)
     *   set_roi(rect){|image| ...}
     *
     * Set ROI. <i>rect</i> should be CvRect or compatible object.
     * Return self.
     */
    VALUE
    rb_set_roi(VALUE self, VALUE roi)
    {
      VALUE block = rb_block_given_p() ? rb_block_proc() : 0;
      try {
	if (block) {
	  CvRect prev_roi = cvGetImageROI(IPLIMAGE(self));
	  cvSetImageROI(IPLIMAGE(self), VALUE_TO_CVRECT(roi));
	  rb_yield_values(1, self);
	  cvSetImageROI(IPLIMAGE(self), prev_roi);
	}
	else {
	  cvSetImageROI(IPLIMAGE(self), VALUE_TO_CVRECT(roi));
	}
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return self;
    }


    /*
     * Reset ROI setting. Same as IplImage#roi = nil. Return self.
     */
    VALUE
    rb_reset_roi(VALUE self)
    {
      try {
	cvResetImageROI(IPLIMAGE(self));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return self;
    }

    /*
     * Return COI as Integer.
     */
    VALUE
    rb_get_coi(VALUE self)
    {
      int coi = 0;
      try {
	coi = cvGetImageCOI(IPLIMAGE(self));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return INT2FIX(coi);
    }

    /*
     * call-seq:
     *   set_coi(coi)
     *   set_coi(coi){|image| ...}
     *
     * Set COI. <i>coi</i> should be Integer.
     * Return self.
     */
    VALUE
    rb_set_coi(VALUE self, VALUE coi)
    {
      VALUE block = rb_block_given_p() ? rb_block_proc() : 0;
      try {
	if (block) {
	  int prev_coi = cvGetImageCOI(IPLIMAGE(self));
	  cvSetImageCOI(IPLIMAGE(self), NUM2INT(coi));
	  rb_yield_values(1, self);
	  cvSetImageCOI(IPLIMAGE(self), prev_coi);
	}
	else {
	  cvSetImageCOI(IPLIMAGE(self), NUM2INT(coi));
	}
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return self;
    }

    /*
     * Reset COI setting. Same as IplImage#coi = 0. Return self.
     */
    VALUE
    rb_reset_coi(VALUE self)
    {
      try {
	cvSetImageCOI(IPLIMAGE(self), 0);
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return self;
    }

    /*
     * call-seq:
     *   IplImage.smoothness(lowFreqRatio, blankDensity, messyDensity, highFreqRatio) -> [ symbol, float, float ]
     *
     * Determines if the image's smoothness is either, :smooth, :messy, or :blank.
     *
     * Original Author: yuhanz@gmail.com
     */
    VALUE
    rb_smoothness(int argc, VALUE *argv, VALUE self)
    {
      VALUE lowFreqRatio, blankDensity, messyDensity, highFreqRatio;
      rb_scan_args(argc, argv, "04", &lowFreqRatio, &blankDensity, &messyDensity, &highFreqRatio);

      double f_lowFreqRatio, f_blankDensity, f_messyDensity, f_highFreqRatio;
      double outLowDensity, outHighDensity;
      if (TYPE(lowFreqRatio) == T_NIL) {
	f_lowFreqRatio = 10 / 128.0f;
      }
      else {
	Check_Type(lowFreqRatio, T_FLOAT);
	f_lowFreqRatio = NUM2DBL(lowFreqRatio);
      }
      if (TYPE(blankDensity) == T_NIL) {
	f_blankDensity = 1.2f;
      }
      else {
	Check_Type(blankDensity, T_FLOAT);
	f_blankDensity = NUM2DBL(blankDensity);
      }
      if (TYPE(messyDensity) == T_NIL) {
	f_messyDensity = 0.151f;
      }
      else {
	Check_Type(messyDensity, T_FLOAT);
	f_messyDensity = NUM2DBL(messyDensity);
      }
      if (TYPE(highFreqRatio) == T_NIL) {
	f_highFreqRatio = 5 / 128.0f;
      }
      else {
	Check_Type(highFreqRatio, T_FLOAT);
	f_highFreqRatio = NUM2DBL(highFreqRatio);
      }

      IplImage *pFourierImage;
      IplImage *p64DepthImage;

      // the image is required to be in depth of 64
      if (IPLIMAGE(self)->depth == 64) {
	p64DepthImage = NULL;
	pFourierImage = create_fourier_image(IPLIMAGE(self));
      }
      else {
	p64DepthImage = rb_cvCreateImage(cvGetSize(IPLIMAGE(self)), IPL_DEPTH_64F, 1);
	cvConvertScale(CVARR(self), p64DepthImage, 1.0, 0.0);
	pFourierImage = create_fourier_image(p64DepthImage);
      }

      Smoothness result = compute_smoothness(pFourierImage, f_lowFreqRatio, f_blankDensity, f_messyDensity,
					     f_highFreqRatio, outLowDensity, outHighDensity);

      cvReleaseImage(&pFourierImage);
      if (p64DepthImage != NULL)
	cvReleaseImage(&p64DepthImage);

      switch(result) {
      case SMOOTH:
	return rb_ary_new3(3, ID2SYM(rb_intern("smooth")), rb_float_new(outLowDensity), rb_float_new(outHighDensity));
      case MESSY:
	return rb_ary_new3(3, ID2SYM(rb_intern("messy")), rb_float_new(outLowDensity), rb_float_new(outHighDensity));
      case BLANK:
	return rb_ary_new3(3, ID2SYM(rb_intern("blank")), rb_float_new(outLowDensity), rb_float_new(outHighDensity));
      default:
	return rb_ary_new3(3, NULL, rb_float_new(outLowDensity), rb_float_new(outHighDensity));
      }
    }

    /**
     * Note: if lowDensity < blankDensityThreshold -> blank;
     * 		else if highDensity > messyDensityThreshold -> messy;
     * 		else -> good;
     */
    Smoothness
    compute_smoothness(const IplImage *pFourierImage, const double lowFreqRatio, const double blankDensity,
		       const double messyDensity, const double highFreqRatio, double &outLowDensity,
		       double &outHighDensity)
    {
      int low, high;
      IplImage *filteredFourierImage;
      int totalIntensity;
      double den, totalArea;
      CvScalar scalar;

      if (!(pFourierImage->nChannels == 1 && pFourierImage->depth == 64) ) {
	cvError(CV_StsUnmatchedSizes, "compute_smoothness", "input image must contain only 1 channel and a depth of 64",
		__FILE__, __LINE__ );
      }
  
      high_pass_range(pFourierImage, lowFreqRatio, low, high );
      totalArea = M_PI * (high * high - low * low);

      filteredFourierImage = create_frequency_filtered_image(pFourierImage, low, high);
      scalar = cvSum(filteredFourierImage);
      totalIntensity = (int)scalar.val[0];
      cvReleaseImage(&filteredFourierImage);
      outLowDensity = den = totalIntensity / totalArea;

      if (den <= blankDensity) {
	return BLANK;
      }

      low = (int)(high * (1.0 - highFreqRatio));

      filteredFourierImage = create_frequency_filtered_image(pFourierImage, low, high);
      scalar = cvSum(filteredFourierImage);
      totalIntensity = (int)scalar.val[0];
      cvReleaseImage(&filteredFourierImage);
      outHighDensity = den = totalIntensity / totalArea;

      if (den >= messyDensity) {
	return MESSY;
      }

      return SMOOTH;
    }

    // Rearrange the quadrants of Fourier image so that the origin is at
    // the image center
    // src & dst arrays of equal size & type
    void
    cvShiftDFT(CvArr *src_arr, CvArr *dst_arr )
    {
      CvMat *tmp = NULL;
      CvMat q1stub, q2stub;
      CvMat q3stub, q4stub;
      CvMat d1stub, d2stub;
      CvMat d3stub, d4stub;
      CvMat *q1, *q2, *q3, *q4;
      CvMat *d1, *d2, *d3, *d4;

      CvSize size = cvGetSize(src_arr);
      CvSize dst_size = cvGetSize(dst_arr);
      int cx, cy;

      if (dst_size.width != size.width ||
	  dst_size.height != size.height) {
	cvError( CV_StsUnmatchedSizes, "cvShiftDFT", "Source and Destination arrays must have equal sizes",
		 __FILE__, __LINE__ );
      }

      if (src_arr == dst_arr) {
	tmp = rb_cvCreateMat(size.height / 2, size.width / 2, cvGetElemType(src_arr));
      }
  
      cx = size.width / 2;
      cy = size.height / 2; // image center

      q1 = cvGetSubRect(src_arr, &q1stub, cvRect(0,0,cx, cy));
      q2 = cvGetSubRect(src_arr, &q2stub, cvRect(cx,0,cx,cy));
      q3 = cvGetSubRect(src_arr, &q3stub, cvRect(cx,cy,cx,cy));
      q4 = cvGetSubRect(src_arr, &q4stub, cvRect(0,cy,cx,cy));
      d1 = cvGetSubRect(src_arr, &d1stub, cvRect(0,0,cx,cy));
      d2 = cvGetSubRect(src_arr, &d2stub, cvRect(cx,0,cx,cy));
      d3 = cvGetSubRect(src_arr, &d3stub, cvRect(cx,cy,cx,cy));
      d4 = cvGetSubRect(src_arr, &d4stub, cvRect(0,cy,cx,cy));

      if (src_arr != dst_arr) {
	if (!CV_ARE_TYPES_EQ(q1, d1)) {
	  cvError(CV_StsUnmatchedFormats, "cvShiftDFT", "Source and Destination arrays must have the same format",
		  __FILE__, __LINE__ );
	}
	cvCopy(q3, d1, 0);
	cvCopy(q4, d2, 0);
	cvCopy(q1, d3, 0);
	cvCopy(q2, d4, 0);
      }
      else {
	cvCopy(q3, tmp, 0);
	cvCopy(q1, q3, 0);
	cvCopy(tmp, q1, 0);
	cvCopy(q4, tmp, 0);
	cvCopy(q2, q4, 0);
	cvCopy(tmp, q2, 0);
      }

      if (tmp != NULL) {
	cvReleaseMat(&tmp);
      }
    }

    IplImage*
    create_fourier_image(const IplImage *im)
    {
      IplImage *realInput;
      IplImage *imaginaryInput;
      IplImage *complexInput;
      int dft_M, dft_N;
      CvMat *dft_A, tmp;
      IplImage *image_Re;
      IplImage *image_Im;

      realInput = rb_cvCreateImage(cvGetSize(im), IPL_DEPTH_64F, 1);
      imaginaryInput = rb_cvCreateImage(cvGetSize(im), IPL_DEPTH_64F, 1);
      complexInput = rb_cvCreateImage(cvGetSize(im), IPL_DEPTH_64F, 2);

      cvScale(im, realInput, 1.0, 0.0);
      cvZero(imaginaryInput);
      cvMerge(realInput, imaginaryInput, NULL, NULL, complexInput);

      dft_M = cvGetOptimalDFTSize(im->height - 1);
      dft_N = cvGetOptimalDFTSize(im->width - 1);

      dft_A = rb_cvCreateMat(dft_M, dft_N, CV_64FC2);
      image_Re = rb_cvCreateImage(cvSize(dft_N, dft_M), IPL_DEPTH_64F, 1);
      image_Im = rb_cvCreateImage(cvSize(dft_N, dft_M), IPL_DEPTH_64F, 1);

      // copy A to dft_A and pad dft_A with zeros
      cvGetSubRect(dft_A, &tmp, cvRect(0,0, im->width, im->height));
      cvCopy(complexInput, &tmp, NULL);
      if (dft_A->cols > im->width) {
	cvGetSubRect(dft_A, &tmp, cvRect(im->width,0, dft_A->cols - im->width, im->height));
	cvZero(&tmp);
      }

      // no need to pad bottom part of dft_A with zeros because of
      // use nonzero_rows parameter in cvDFT() call below

      cvDFT(dft_A, dft_A, CV_DXT_FORWARD, complexInput->height);

      // Split Fourier in real and imaginary parts
      cvSplit(dft_A, image_Re, image_Im, 0, 0);

      // Compute the magnitude of the spectrum Mag = sqrt(Re^2 + Im^2)
      cvPow(image_Re, image_Re, 2.0);
      cvPow(image_Im, image_Im, 2.0);
      cvAdd(image_Re, image_Im, image_Re, NULL);
      cvPow(image_Re, image_Re, 0.5);

      // Compute log(1 + Mag)
      cvAddS(image_Re, cvScalarAll(1.0), image_Re, NULL); // 1 + Mag
      cvLog(image_Re, image_Re); // log(1 + Mag)

      // Rearrange the quadrants of Fourier image so that the origin is at
      // the image center
      cvShiftDFT(image_Re, image_Re);

      cvReleaseImage(&realInput);
      cvReleaseImage(&imaginaryInput);
      cvReleaseImage(&complexInput);
      cvReleaseImage(&image_Im);

      cvReleaseMat(&dft_A);

      return image_Re;
    }

    IplImage*
    create_frequency_filtered_image(const IplImage *pImage, int low, int high)
    {

      CvPoint2D32f  center;
      center.x = (float)(pImage->width / 2);
      center.y = (float)(pImage->height / 2);
      CvBox2D box;
      box.center = center;

      box.size.width = (float)high;
      box.size.height = (float)high;

      IplImage *pFilterMask = rb_cvCreateImage(cvGetSize(pImage), IPL_DEPTH_64F, 1);
      IplImage *pFiltered = rb_cvCreateImage(cvGetSize(pImage), IPL_DEPTH_64F, 1);

      cvZero(pFilterMask);
      cvZero(pFiltered);

      if (high > 0)
	cvEllipseBox(pFilterMask, box, cvScalar(255, 255, 255, 255), CV_FILLED, 8, 0);

      box.size.width = (float)low;
      box.size.height = (float)low;
      if (low > 0)
	cvEllipseBox(pFilterMask, box, cvScalar(0, 0, 0, 0), CV_FILLED, 8, 0);

      cvAnd(pImage, pFilterMask, pFiltered, NULL);

      cvReleaseImage(&pFilterMask);

      return pFiltered;
    }

    void
    high_pass_range(const IplImage *pImage, float lostPercentage, int &outLow, int &outHigh)
    {
      if (lostPercentage > 1.0f) {
	lostPercentage = 1;
      }
      else if (lostPercentage < 0.0f) {
	lostPercentage = 0;
      }

      outHigh = (int)MIN(pImage->width, pImage->height);
      outLow = (int)(lostPercentage * outHigh);
    }

    VALUE
    new_object(int width, int height, int type)
    {
      return OPENCV_OBJECT(rb_klass, rb_cvCreateImage(cvSize(width, height), cvIplDepth(type), CV_MAT_CN(type)));
    }

    VALUE
    new_object(CvSize size, int type)
    {
      return OPENCV_OBJECT(rb_klass, rb_cvCreateImage(size, cvIplDepth(type), CV_MAT_CN(type)));
    }

    void
    init_ruby_class()
    {
#if 0
      // For documentation using YARD
      VALUE opencv = rb_define_module("OpenCV");
      VALUE cvmat = rb_define_class_under(opencv, "CvMat", rb_cObject);
#endif

      if (rb_klass)
	return;
      /* 
       * opencv = rb_define_module("OpenCV");
       * cvmat = rb_define_class_under(opencv, "CvMat", rb_cObject);
       *
       * note: this comment is used by rdoc.
       */
      VALUE opencv = rb_module_opencv();
      VALUE cvmat = cCvMat::rb_class();
      rb_klass = rb_define_class_under(opencv, "IplImage", cvmat);
      rb_define_alloc_func(rb_klass, rb_allocate);
      rb_define_singleton_method(rb_klass, "load", RUBY_METHOD_FUNC(rb_load_image), -1);
      rb_define_method(rb_klass, "initialize", RUBY_METHOD_FUNC(rb_initialize), -1); 
      rb_define_method(rb_klass, "get_roi", RUBY_METHOD_FUNC(rb_get_roi), 0);
      rb_define_alias(rb_klass, "roi", "get_roi");
      rb_define_method(rb_klass, "set_roi", RUBY_METHOD_FUNC(rb_set_roi), 1);
      rb_define_alias(rb_klass, "roi=", "set_roi");
      rb_define_method(rb_klass, "reset_roi", RUBY_METHOD_FUNC(rb_reset_roi), 0);
      rb_define_method(rb_klass, "get_coi", RUBY_METHOD_FUNC(rb_get_coi), 0);
      rb_define_alias(rb_klass, "coi", "get_coi");
      rb_define_method(rb_klass, "set_coi", RUBY_METHOD_FUNC(rb_set_coi), 1);
      rb_define_alias(rb_klass, "coi=", "set_coi");
      rb_define_method(rb_klass, "reset_coi", RUBY_METHOD_FUNC(rb_reset_coi), 0); 
      rb_define_method(rb_klass, "smoothness", RUBY_METHOD_FUNC(rb_smoothness), -1);

      rb_define_singleton_method(rb_klass, "decode_image", RUBY_METHOD_FUNC(rb_decode_image), -1);
      rb_define_alias(rb_singleton_class(rb_klass), "decode", "decode_image");
    }
  }
}
