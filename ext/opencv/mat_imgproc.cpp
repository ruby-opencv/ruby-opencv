// -*- mode: c++; coding: utf-8 -*-
#include "opencv2/imgproc.hpp"

#include "mat.hpp"
#include "size.hpp"
#include "point.hpp"
#include "error.hpp"

/*
 * Document-class: Cv::Mat
 */
namespace rubyopencv {
  namespace Mat {
    cv::Mat* rb_sobel_internal(int argc, VALUE *argv, VALUE self, cv::Mat* destptr) {
      VALUE ddepth, dx, dy, ksize, scale, delta, border_type;
      rb_scan_args(argc, argv, "34", &ddepth ,&dx, &dy, &ksize, &scale, &delta, &border_type);
      int ksize_value = NIL_P(ksize) ? 3 : NUM2INT(ksize);
      double scale_value = NIL_P(scale) ? 1.0 : NUM2DBL(scale);
      double delta_value = NIL_P(delta) ? 0.0 : NUM2DBL(delta);
      int border_type_value = NIL_P(border_type) ? cv::BORDER_DEFAULT : NUM2INT(border_type);

      cv::Mat* selfptr = obj2mat(self);
      cv::Sobel(*selfptr, *destptr, NUM2INT(ddepth), NUM2INT(dx), NUM2INT(dy),
		ksize_value, scale_value, delta_value, border_type_value);

      return destptr;
    }

    /*
     * Calculates the first, second, third, or mixed image derivatives using an extended Sobel operator.
     *
     * @overload sobel(ddepth, dx, dy, ksize = 3, scale = 1, delta = 0, border_type = BORDER_DEFAULT)
     *   @param ddepth [Integer] Output image depth
     *   @param dx [Integer] Order of the derivative x.
     *   @param dy [Integer] Order of the derivative y.
     *   @param ksize [Integer] Size of the extended Sobel kernel; it must be 1, 3, 5, or 7.
     *   @param scale [Number] Optional scale factor for the computed derivative values; by default, no scaling is applied.
     *   @param delta [Number] Optional delta value that is added to the results prior to storing them in the output image.
     *   @param border_type [Integer] Pixel extrapolation method.
     * @return [Mat] Output image.
     * @opencv_func cv::Sobel
     */
    VALUE rb_sobel(int argc, VALUE *argv, VALUE self) {
      cv::Mat* destptr = new cv::Mat();
      try {
	rb_sobel_internal(argc, argv, self, destptr);
      }
      catch (cv::Exception& e) {
	delete destptr;
	Error::raise(e);
      }
      return mat2obj(destptr, CLASS_OF(self));
    }

    /*
     * @overload sobel!(ddepth, dx, dy, ksize = 3, scale = 1, delta = 0, border_type = BORDER_DEFAULT)
     * @see #sobel
     */
    VALUE rb_sobel_bang(int argc, VALUE *argv, VALUE self) {
      cv::Mat* destptr = obj2mat(self);
      try {
	rb_sobel_internal(argc, argv, self, destptr);
      }
      catch (cv::Exception& e) {
	Error::raise(e);
      }
      return self;
    }

    cv::Mat* rb_canny_internal(int argc, VALUE *argv, VALUE self, cv::Mat* destptr) {
      VALUE threshold1, threshold2, aperture_size, l2gradient;
      rb_scan_args(argc, argv, "22", &threshold1, &threshold2, &aperture_size, &l2gradient);

      int aperture_size_value = NIL_P(aperture_size) ? 3 : NUM2INT(aperture_size);
      bool l2gradient_value = RTEST(l2gradient) ? true : false;

      cv::Mat* selfptr = obj2mat(self);
      cv::Canny(*selfptr, *destptr, NUM2DBL(threshold1), NUM2DBL(threshold2),
		aperture_size_value, l2gradient_value);

      return destptr;
    }

    /**
     * Finds edges in an image using the [Canny86] algorithm.
     *
     * @overload canny(threshold1, threshold2, aperture_size = 3, l2gradient = false)
     *   @param threshold1 [Number] First threshold for the hysteresis procedure.
     *   @param threshold2 [Number] Second threshold for the hysteresis procedure.
     *   @param aperture_size [Integer] Aperture size for the Sobel operator.
     *   @param l2gradient [Boolean] a flag, indicating whether a more accurate L_2 = \sqrt{ (dI/dx)^2 + (dI/dy)^2 } norm
     *     should be used to calculate the image gradient magnitude (l2gradient=true),
     *     or whether the default L_1 norm = |dI/dx|+|dI/dy| is enough (l2gradient=false).
     * @return [Mat] Output image
     * @opencv_func cv::Canny
     */
    VALUE rb_canny(int argc, VALUE *argv, VALUE self) {
      cv::Mat* destptr = new cv::Mat();
      try {
	rb_canny_internal(argc, argv, self, destptr);
      }
      catch (cv::Exception& e) {
	delete destptr;
	Error::raise(e);
      }

      return mat2obj(destptr, CLASS_OF(self));
    }

    /**
     * @overload canny!(threshold1, threshold2, aperture_size = 3, l2gradient = false)
     * @see #canny
     */
    VALUE rb_canny_bang(int argc, VALUE *argv, VALUE self) {
      cv::Mat* destptr = obj2mat(self);
      try {
	rb_canny_internal(argc, argv, self, destptr);
      }
      catch (cv::Exception& e) {
	Error::raise(e);
      }

      return self;
    }

    cv::Mat* rb_laplacian_internal(int argc, VALUE *argv, VALUE self, cv::Mat* destptr) {
      VALUE ddepth, ksize, scale, delta, border_type;
      rb_scan_args(argc, argv, "14", &ddepth, &ksize, &scale, &delta, &border_type);
      int ksize_value = NIL_P(ksize) ? 3 : NUM2INT(ksize);
      double scale_value = NIL_P(scale) ? 1.0 : NUM2DBL(scale);
      double delta_value = NIL_P(delta) ? 0.0 : NUM2DBL(delta);
      int border_type_value = NIL_P(border_type) ? cv::BORDER_DEFAULT : NUM2INT(border_type);

      cv::Mat* selfptr = obj2mat(self);
      cv::Laplacian(*selfptr, *destptr, NUM2INT(ddepth), ksize_value, scale_value,
		    delta_value, border_type_value);

      return destptr;
    }

    /*
     * Calculates the Laplacian of an image.
     *
     * @overload laplacian(ddepth, ksize = 1, scale = 1, delta = 0, border_type = BORDER_DEFAULT)
     *   @param ddepth [Integer] Desired depth of the destination image.
     *   @param ksize [Integer] Aperture size used to compute the second-derivative filters.
     *     The size must be positive and odd.
     *   @param scale [Number] Optional scale factor for the computed Laplacian values. By default, no scaling is applied.
     *   @param delta [Number] Optional delta value that is added to the results prior to storing them in the output image.
     *   @param border_type [Integer] Pixel extrapolation method.
     * @return [Mat] Output image.
     * @opencv_func cv::Laplacian
     */
    VALUE rb_laplacian(int argc, VALUE *argv, VALUE self) {
      cv::Mat* destptr = new cv::Mat();
      try {
	rb_laplacian_internal(argc, argv, self, destptr);
      }
      catch (cv::Exception& e) {
	delete destptr;
	Error::raise(e);
      }

      return mat2obj(destptr, CLASS_OF(self));
    }

    /*
     * @overload laplacian!(ddepth, ksize = 1, scale = 1, delta = 0, border_type = BORDER_DEFAULT)
     * @see #laplacian
     */
    VALUE rb_laplacian_bang(int argc, VALUE *argv, VALUE self) {
      cv::Mat* destptr = obj2mat(self);
      try {
	rb_laplacian_internal(argc, argv, self, destptr);
      }
      catch (cv::Exception& e) {
	Error::raise(e);
      }

      return self;
    }

    cv::Mat* rb_cvt_color_internal(int argc, VALUE *argv, VALUE self, cv::Mat* destptr) {
      VALUE code, dcn;
      rb_scan_args(argc, argv, "11", &code, &dcn);
      int dcn_value = NIL_P(dcn) ? 0 : NUM2INT(dcn);

      cv::Mat* selfptr = obj2mat(self);
      cv::cvtColor(*selfptr, *destptr, NUM2INT(code), dcn_value);

      return destptr;
    }

    /**
     * Converts an image from one color space to another.
     *
     * @overload cvt_color(code, dcn = 0)
     *   @param code [Integer] Color space conversion code
     *   @param dcn [Integer] Number of channels in the destination image; if the parameter is 0,
     *     the number of the channels is derived automatically from src and code
     * @return [Mat] Output image
     * @opencv_func cv::cvtColor
     */
    VALUE rb_cvt_color(int argc, VALUE *argv, VALUE self) {
      cv::Mat* destptr = new cv::Mat();
      try {
	rb_cvt_color_internal(argc, argv, self, destptr);
      }
      catch (cv::Exception& e) {
	delete destptr;
	Error::raise(e);
      }

      return mat2obj(destptr, CLASS_OF(self));
    }

    /**
     * @overload cvt_color!(code, dcn = 0)
     * @see #cvt_color
     */
    VALUE rb_cvt_color_bang(int argc, VALUE *argv, VALUE self) {
      cv::Mat* destptr = obj2mat(self);
      try {
	rb_cvt_color_internal(argc, argv, self, destptr);
      }
      catch (cv::Exception& e) {
	Error::raise(e);
      }

      return self;
    }

    cv::Mat* rb_resize_internal(int argc, VALUE *argv, VALUE self, cv::Mat* destptr) {
      VALUE size, inv_scale_x, inv_scale_y, interpolation;
      rb_scan_args(argc, argv, "13", &size, &inv_scale_x, &inv_scale_y, &interpolation);
      cv::Size* sizeptr = Size::obj2size(size);
      cv::Mat* selfptr = obj2mat(self);
      double sx = NIL_P(inv_scale_x) ? 0 : NUM2DBL(inv_scale_x);
      double sy = NIL_P(inv_scale_y) ? 0 : NUM2DBL(inv_scale_y);
      int method = NIL_P(interpolation) ? CV_INTER_LINEAR : NUM2INT(interpolation);

      cv::resize(*selfptr, *destptr, *sizeptr, sx, sy, method);

      return destptr;
    }

    /*
     * Resizes an image.
     *
     * @overload resize(size, interpolation = INTER_LINEAR)
     *   @param size [Size] Output image size.
     *   @param interpolation [Integer] Interpolation method:
     *     * <tt>INTER_NEAREST</tt> - A nearest-neighbor interpolation
     *	   * <tt>INTER_LINEAR</tt> - A bilinear interpolation (used by default)
     *	   * <tt>INTER_AREA</tt> - Resampling using pixel area relation. It may be a preferred method for
     *       image decimation, as it gives moire'-free results. But when the image is zoomed,
     *       it is similar to the <tt>INTER_NEAREST</tt> method.
     *     * <tt>INTER_CUBIC</tt> - A bicubic interpolation over 4x4 pixel neighborhood
     *	   * <tt>INTER_LANCZOS4</tt> - A Lanczos interpolation over 8x8 pixel neighborhood
     * @return [Mat] Output image.
     * @opencv_func cv::Resize
     */
    VALUE rb_resize(int argc, VALUE *argv, VALUE self) {
      cv::Mat* destptr = new cv::Mat();
      try {
	rb_resize_internal(argc, argv, self, destptr);
      }
      catch (cv::Exception& e) {
	delete destptr;
	Error::raise(e);
      }

      return mat2obj(destptr, CLASS_OF(self));
    }

    /*
     * Resizes an image.
     *
     * @overload resize(size, interpolation = INTER_LINEAR)
     *   @param size [Size] Output image size.
     *   @param interpolation [Integer] Interpolation method:
     *     * <tt>INTER_NEAREST</tt> - A nearest-neighbor interpolation
     *	   * <tt>INTER_LINEAR</tt> - A bilinear interpolation (used by default)
     *	   * <tt>INTER_AREA</tt> - Resampling using pixel area relation. It may be a preferred method for
     *       image decimation, as it gives moire'-free results. But when the image is zoomed,
     *       it is similar to the <tt>INTER_NEAREST</tt> method.
     *     * <tt>INTER_CUBIC</tt> - A bicubic interpolation over 4x4 pixel neighborhood
     *	   * <tt>INTER_LANCZOS4</tt> - A Lanczos interpolation over 8x8 pixel neighborhood
     * @return [Mat] Output image.
     * @opencv_func cv::Resize
     */
    VALUE rb_resize_bang(int argc, VALUE *argv, VALUE self) {
      cv::Mat* destptr = obj2mat(self);
      try {
	rb_resize_internal(argc, argv, self, destptr);
      }
      catch (cv::Exception& e) {
	Error::raise(e);
      }

      return self;
    }

    cv::Mat* rb_blur_internal(int argc, VALUE *argv, VALUE self, cv::Mat* destptr) {
      VALUE ksize, anchor, border_type;
      rb_scan_args(argc, argv, "12", &ksize, &anchor, &border_type);
      cv::Point anchor_value = NIL_P(anchor) ? cv::Point(-1, -1) : *(Point::obj2point(anchor));
      int border_type_value = NIL_P(border_type) ? cv::BORDER_DEFAULT : NUM2INT(border_type);

      cv::Mat* selfptr = obj2mat(self);
      cv::blur(*selfptr, *destptr, *(Size::obj2size(ksize)), anchor_value, border_type_value);

      return destptr;
    }

    /*
     * Blurs an image using the normalized box filter.
     *
     * @overload blur(ksize, anchor = Point.new(-1, -1), border_type = BORDER_DEFAULT)
     *   @param ksize [Size] Blurring kernel size.
     *   @param border_type [Integer] Border mode used to extrapolate pixels outside of the image.
     *   @return [Mat] Output array
     *   @opencv_func cv::blur
     */
    VALUE rb_blur(int argc, VALUE *argv, VALUE self) {
      cv::Mat* destptr = new cv::Mat();
      try {
	rb_blur_internal(argc, argv, self, destptr);
      }
      catch (cv::Exception& e) {
	delete destptr;
	Error::raise(e);
      }

      return mat2obj(destptr, CLASS_OF(self));
    }

    /*
     * @overload blur!(ksize, anchor = Point.new(-1, -1), border_type = BORDER_DEFAULT)
     * @see #blue
     */
    VALUE rb_blur_bang(int argc, VALUE *argv, VALUE self) {
      cv::Mat* destptr = obj2mat(self);
      try {
	rb_blur_internal(argc, argv, self, destptr);
      }
      catch (cv::Exception& e) {
	Error::raise(e);
      }

      return self;
    }

    cv::Mat* rb_gaussian_blur_internal(int argc, VALUE *argv, VALUE self, cv::Mat* destptr) {
      VALUE ksize, sigma_x, sigma_y, border_type;
      rb_scan_args(argc, argv, "22", &ksize, &sigma_x, &sigma_y, &border_type);
      double sigma_y_value = NIL_P(sigma_y) ? 0 : NUM2DBL(sigma_y);
      int border_type_value = NIL_P(border_type) ? cv::BORDER_DEFAULT : NUM2INT(border_type);

      cv::Mat* selfptr = obj2mat(self);
      cv::GaussianBlur(*selfptr, *destptr, *(Size::obj2size(ksize)), NUM2DBL(sigma_x),
		       sigma_y_value, border_type_value);

      return destptr;
    }

    /*
     * Blurs an image using a Gaussian filter.
     *
     * @overload gaussian_blur(ksize, sigma_x, sigma_y = 0, border_type = BORDER_DEFAULT)
     *   @param ksize [Size] Gaussian kernel size. ksize.width and ksize.height can differ
     *     but they both must be positive and odd. Or, they can be zero's and then they are computed from sigma.
     *   @param sigma_x [Number] Gaussian kernel standard deviation in X direction.
     *   @param sigma_y [Number] Gaussian kernel standard deviation in Y direction; if sigmaY is zero,
     *     it is set to be equal to sigmaX, if both sigmas are zeros, they are computed from ksize.width
     *     and ksize.height, respectively.
     *   @param border_type [Integer] Pixel extrapolation method.
     *   @return [Mat] Output array
     *   @opencv_func cv::GaussianBlur
     */
    VALUE rb_gaussian_blur(int argc, VALUE *argv, VALUE self) {
      cv::Mat* destptr = new cv::Mat();
      try {
	rb_gaussian_blur_internal(argc, argv, self, destptr);
      }
      catch (cv::Exception& e) {
	delete destptr;
	Error::raise(e);
      }

      return mat2obj(destptr, CLASS_OF(self));
    }

    /*
     * @overload gaussian_blur!(ksize, sigma_x, sigma_y = 0, border_type = BORDER_DEFAULT)
     * @see #gaussian_blur
     */
    VALUE rb_gaussian_blur_bang(int argc, VALUE *argv, VALUE self) {
      cv::Mat* destptr = obj2mat(self);
      try {
	rb_gaussian_blur_internal(argc, argv, self, destptr);
      }
      catch (cv::Exception& e) {
	Error::raise(e);
      }

      return self;
    }

    cv::Mat* rb_median_blur_internal(VALUE self, VALUE ksize, cv::Mat* destptr) {
      cv::Mat* selfptr = obj2mat(self);
      cv::medianBlur(*selfptr, *destptr, NUM2INT(ksize));

      return destptr;
    }

    /*
     * Blurs an image using the median filter.
     *
     * @overload median_blur(ksize)
     *   @param ksize [Integer] Aperture linear size; it must be odd and greater than 1,
     *     for example: 3, 5, 7 ...
     *   @return [Mat] Output array
     *   @opencv_func cv::medianBlur
     */
    VALUE rb_median_blur(VALUE self, VALUE ksize) {
      cv::Mat* destptr = new cv::Mat();
      try {
	rb_median_blur_internal(self, ksize, destptr);
      }
      catch (cv::Exception& e) {
	delete destptr;
	Error::raise(e);
      }

      return mat2obj(destptr, CLASS_OF(self));
    }

    /*
     * @overload median_blur!(ksize)
     * @see #median_blur
     */
    VALUE rb_median_blur_bang(VALUE self, VALUE ksize) {
      cv::Mat* destptr = obj2mat(self);
      try {
	rb_median_blur_internal(self, ksize, destptr);
      }
      catch (cv::Exception& e) {
	Error::raise(e);
      }

      return self;
    }

    VALUE rb_threshold_internal(VALUE self, VALUE threshold, VALUE max_value, VALUE threshold_type, VALUE dest) {
      cv::Mat* selfptr = obj2mat(self);
      cv::Mat* destptr = obj2mat(dest);
      int threshold_type_value = NUM2INT(threshold_type);
      double optimal_threshold = cv::threshold(*selfptr, *destptr, NUM2DBL(threshold), NUM2DBL(max_value), threshold_type_value);

      VALUE ret = Qnil;
      if ((threshold_type_value & cv::THRESH_OTSU) || (threshold_type_value & cv::THRESH_TRIANGLE)) {
	ret = rb_assoc_new(dest, DBL2NUM(optimal_threshold));
      }
      else {
	ret = dest;
      }
      return ret;
    }

    /*
     * Applies a fixed-level threshold to each array element.
     *
     * @overload threshold(threshold, max_value, type)
     *   @param threshold [Number] Threshold value
     *   @param max_value [Number] Maximum value to use with the <tt>THRESH_BINARY</tt> and
     *     <tt>THRESH_BINARY_INV</tt> thresholding types.
     *   @param threshold_type [Integer] Thresholding type
     *     * THRESH_BINARY
     *     * THRESH_BINARY_INV
     *     * THRESH_TRUNC
     *     * THRESH_TOZERO
     *     * THRESH_TOZERO_INV
     *     * THRESH_MASK
     *     * THRESH_OTSU
     *     * THRESH_TRIANGLE
     *   @return [Mat] Output array of the same size and type as <tt>self</tt>.
     *   @return [Array<Mat, Number>] Output array and optimal threshold when using
     *     <tt>THRESH_OTSU</tt> or <tt>THRESH_TRIANGLE</tt>.
     * @opencv_func cv::threshold
     */
    VALUE rb_threshold(VALUE self, VALUE threshold, VALUE max_value, VALUE threshold_type) {
      cv::Mat* destptr = new cv::Mat();
      VALUE dest = mat2obj(destptr);
      VALUE ret = Qnil;
      try {
	ret = rb_threshold_internal(self, threshold, max_value, threshold_type, dest);
      }
      catch (cv::Exception& e) {
	Error::raise(e);
      }

      return ret;
    }

    /*
     * @overload threshold!(threshold, max_value, type)
     * @see #threshold
     */
    VALUE rb_threshold_bang(VALUE self, VALUE threshold, VALUE max_value, VALUE threshold_type) {
      VALUE ret = Qnil;
      try {
	ret = rb_threshold_internal(self, threshold, max_value, threshold_type, self);
      }
      catch (cv::Exception& e) {
	Error::raise(e);
      }

      return ret;
    }

    cv::Mat* rb_adaptive_threshold_internal(VALUE self, VALUE max_value, VALUE adaptive_method, VALUE threshold_type,
					    VALUE block_size, VALUE delta, cv::Mat* destptr) {
      cv::Mat* selfptr = obj2mat(self);
      cv::adaptiveThreshold(*selfptr, *destptr, NUM2DBL(max_value), NUM2INT(adaptive_method),
			    NUM2INT(threshold_type), NUM2INT(block_size), NUM2DBL(delta));

      return destptr;
    }

    /*
     * Applies an adaptive threshold to an array.
     *
     * @overload adaptive_threshold(max_value, adaptive_method, threshold_type, block_size, delta)
     *   @param max_value [Number] Non-zero value assigned to the pixels for which the condition is satisfied.
     *   @param adaptive_method [Integer] Adaptive thresholding algorithm to use.
     *   @param threshold_type [Integer] Thresholding type that must be either <tt>THRESH_BINARY</tt>
     *     or <tt>THRESH_BINARY_INV</tt>.
     *   @param block_size [Integer] Size of a pixel neighborhood that is used to calculate a threshold value
     *     for the pixel: 3, 5, 7, and so on.
     *   @param delta [Number] Constant subtracted from the mean or weighted mean.
     *     Normally, it is positive but may be zero or negative as well.
     *   @return [Mat] Destination image of the same size and the same type as <tt>self</tt>.
     * @opencv_func cv::adaptiveThreshold
     */
    VALUE rb_adaptive_threshold(VALUE self, VALUE max_value, VALUE adaptive_method, VALUE threshold_type,
				VALUE block_size, VALUE delta) {
      cv::Mat* destptr = new cv::Mat();
      try {
	rb_adaptive_threshold_internal(self, max_value, adaptive_method, threshold_type,
				       block_size, delta, destptr);
      }
      catch (cv::Exception& e) {
	delete destptr;
	Error::raise(e);
      }

      return mat2obj(destptr, CLASS_OF(self));
    }

    /*
     * @overload adaptive_threshold!(max_value, adaptive_method, threshold_type, block_size, delta)
     * @see #adaptive_threshold
     */
    VALUE rb_adaptive_threshold_bang(VALUE self, VALUE max_value, VALUE adaptive_method, VALUE threshold_type,
				     VALUE block_size, VALUE delta) {
      cv::Mat* destptr = obj2mat(self);
      try {
	rb_adaptive_threshold_internal(self, max_value, adaptive_method, threshold_type,
				       block_size, delta, destptr);
      }
      catch (cv::Exception& e) {
	Error::raise(e);
      }

      return self;
    }
  }
}
