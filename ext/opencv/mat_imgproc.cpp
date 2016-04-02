// -*- mode: c++; coding: utf-8 -*-
#include "opencv2/imgproc.hpp"

#include "mat.hpp"
#include "size.hpp"
#include "error.hpp"

/*
 * Document-class: OpenCV::Mat
 */
namespace rubyopencv {
  namespace Mat {
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
     * @opencv_func cv::Sovel
     */
    VALUE rb_sobel(int argc, VALUE *argv, VALUE self) {
      VALUE ddepth, dx, dy, ksize, scale, delta, border_type;
      rb_scan_args(argc, argv, "34", &ddepth ,&dx, &dy, &ksize, &scale, &delta, &border_type);
      int ksize_value = NIL_P(ksize) ? 3 : NUM2INT(ksize);
      double scale_value = NIL_P(scale) ? 1.0 : NUM2DBL(scale);
      double delta_value = NIL_P(delta) ? 0.0 : NUM2DBL(delta);
      int border_type_value = NIL_P(border_type) ? cv::BORDER_DEFAULT : NUM2INT(border_type);

      cv::Mat* selfptr = obj2mat(self);
      cv::Mat* destptr = empty_mat();
      try {
	cv::Sobel(*selfptr, *destptr, NUM2INT(ddepth), NUM2INT(dx), NUM2INT(dy),
		  ksize_value, scale_value, delta_value, border_type_value);
      }
      catch (cv::Exception& e) {
	delete destptr;
	Error::raise(e);
      }

      return mat2obj(destptr, CLASS_OF(self));
    }

    /**
     * Finds edges in an image using the [Canny86] algorithm.
     *
     * @overload canny(threshold1, threshold2, aperture_size = 3, l2gradient = false)
     *   @param threshold1 [Number] First threshold for the hysteresis procedure.
     *   @param threshold2 [Number] Second threshold for the hysteresis procedure.
     *   @param aperture_size [Integer] Aperture size for the Sobel operator.
     *   @param l2gradient [Boolean] a flag, indicating whether a more accurate L_2 =\sqrt{ (dI/dx)^2 + (dI/dy)^2 } norm
     *     should be used to calculate the image gradient magnitude (l2gradient=true),
     *     or whether the default L_1 norm =|dI/dx|+|dI/dy| is enough (l2gradient=false).
     * @opencv_func cv::Canny
     */
    VALUE rb_canny(int argc, VALUE *argv, VALUE self) {
      VALUE threshold1, threshold2, aperture_size, l2gradient;
      rb_scan_args(argc, argv, "22", &threshold1, &threshold2, &aperture_size, &l2gradient);

      int aperture_size_value = NIL_P(aperture_size) ? 3 : NUM2INT(aperture_size);
      bool l2gradient_value = RTEST(l2gradient) ? true : false;

      cv::Mat* selfptr = obj2mat(self);
      cv::Mat* destptr = empty_mat();
      try {
	cv::Canny(*selfptr, *destptr, NUM2DBL(threshold1), NUM2DBL(threshold2),
		  aperture_size_value, l2gradient_value);
      }
      catch (cv::Exception& e) {
	delete destptr;
	Error::raise(e);
      }

      return mat2obj(destptr, CLASS_OF(self));
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
      VALUE ddepth, ksize, scale, delta, border_type;
      rb_scan_args(argc, argv, "14", &ddepth, &ksize, &scale, &delta, &border_type);
      int ksize_value = NIL_P(ksize) ? 3 : NUM2INT(ksize);
      double scale_value = NIL_P(scale) ? 1.0 : NUM2DBL(scale);
      double delta_value = NIL_P(delta) ? 0.0 : NUM2DBL(delta);
      int border_type_value = NIL_P(border_type) ? cv::BORDER_DEFAULT : NUM2INT(border_type);

      cv::Mat* selfptr = obj2mat(self);
      cv::Mat* destptr = empty_mat();
      try {
	cv::Laplacian(*selfptr, *destptr, NUM2INT(ddepth), ksize_value, scale_value,
		      delta_value, border_type_value);
      }
      catch (cv::Exception& e) {
	delete destptr;
	Error::raise(e);
      }

      return mat2obj(destptr, CLASS_OF(self));
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
      VALUE code, dcn;
      rb_scan_args(argc, argv, "11", &code, &dcn);
      int dcn_value = NIL_P(dcn) ? 0 : NUM2INT(dcn);

      cv::Mat* destptr = empty_mat();
      cv::Mat* selfptr = obj2mat(self);
      try {
	cv::cvtColor(*selfptr, *destptr, NUM2INT(code), dcn_value);
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
    VALUE rb_resize(int argc, VALUE *argv, VALUE self) {
      VALUE size, inv_scale_x, inv_scale_y, interpolation;
      rb_scan_args(argc, argv, "13", &size, &inv_scale_x, &inv_scale_y, &interpolation);
      cv::Size* sizeptr = Size::obj2size(size);
      cv::Mat* selfptr = obj2mat(self);
      cv::Mat* destptr = empty_mat();
      double sx = NIL_P(inv_scale_x) ? 0 : NUM2DBL(inv_scale_x);
      double sy = NIL_P(inv_scale_y) ? 0 : NUM2DBL(inv_scale_y);
      int method = NIL_P(interpolation) ? CV_INTER_LINEAR : NUM2INT(interpolation);

      try {
	cv::resize(*selfptr, *destptr, *sizeptr, sx, sy, method);
      }
      catch (cv::Exception& e) {
	Error::raise(e);
      }

      return mat2obj(destptr, CLASS_OF(self));
    }
  }
}
