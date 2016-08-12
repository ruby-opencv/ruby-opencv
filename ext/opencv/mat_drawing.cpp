#include "ruby.h"

#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"

#include "mat.hpp"
#include "mat_drawing.hpp"
#include "scalar.hpp"
#include "point.hpp"
#include "error.hpp"

/*
 * Document-class: Cv::Mat
 */
namespace rubyopencv {
  namespace Mat {
    typedef struct _drawing_option {
      int thickness;
      int line_type;
      int shift;
    } drawing_option_t;
    const drawing_option_t DEFAULT_DRAWING_OPTION = { 1, 8, 0 };

    drawing_option_t drawing_option(VALUE option) {
      drawing_option_t opt = DEFAULT_DRAWING_OPTION;

      if (!NIL_P(option)) {
	Check_Type(option, T_HASH);

	VALUE tmp = Qnil;
	tmp = rb_hash_lookup(option, ID2SYM(rb_intern("thickness")));
	if (!NIL_P(tmp)) {
	  opt.thickness = NUM2INT(tmp);
	}
	tmp = rb_hash_lookup(option, ID2SYM(rb_intern("line_type")));
	if (!NIL_P(tmp)) {
	  opt.line_type = NUM2INT(tmp);
	}
	tmp = rb_hash_lookup(option, ID2SYM(rb_intern("shift")));
	if (!NIL_P(tmp)) {
	  opt.shift = NUM2INT(tmp);
	}
      }

      return opt;
    }

    /*
     * Draws a line segment connecting two points.
     *
     * @overload line!(p1, p2, color, options = nil)
     *   @param p1 [Point] First point of the line segment.
     *   @param p2 [Point] Second point of the line segment.
     *   @param color [Scalar] Line color.
     *   @param options [Hash] Drawing options
     *   @option options [Integer] :thickness Line thickness.
     *   @option options [Integer] :line_type Type of the line.
     *     * 8 - 8-connected line.
     *     * 4 - 4-connected line.
     *     * <tt>CV_AA</tt> - Antialiased line.
     *   @option options [Integer] :shift Number of fractional bits in the point coordinates.
     * @return [Mat] <tt>self</tt>
     * @opencv_func (see #line)
     */
    VALUE rb_line_bang(int argc, VALUE *argv, VALUE self) {
      VALUE p1, p2, color, option;
      rb_scan_args(argc, argv, "31", &p1, &p2, &color, &option);

      drawing_option_t opt = drawing_option(option);
      try {
	cv::Point pt1 = Point::conpatible_obj2point(p1);
	cv::Point pt2 = Point::conpatible_obj2point(p2);
      	cv::line(*(obj2mat(self)), pt1, pt2, *(Scalar::obj2scalar(color)), opt.thickness, opt.line_type, opt.shift);
      }
      catch (cv::Exception& e) {
	Error::raise(e);
      }
      return self;
    }

    /*
     * Returns a line segment connecting two points.
     *
     * @overload line!(p1, p2, color, options = nil)
     *   @param p1 [Point] First point of the line segment.
     *   @param p2 [Point] Second point of the line segment.
     *   @param color [Scalar] Line color.
     *   @param options [Hash] Drawing options
     *   @option options [Integer] :thickness Line thickness.
     *   @option options [Integer] :line_type Type of the line.
     *     * 8 - 8-connected line.
     *     * 4 - 4-connected line.
     *     * <tt>CV_AA</tt> - Antialiased line.
     *   @option options [Integer] :shift Number of fractional bits in the point coordinates.
     * @return [Mat] Image
     * @opencv_func cv::line
     */
    VALUE rb_line(int argc, VALUE *argv, VALUE self) {
      VALUE dst = rb_clone(self);
      return rb_line_bang(argc, argv, dst);
    }

    /*
     * Returns a simple, thick, or filled up-right rectangle.
     *
     * @overload rectangle(p1, p2, color, options = nil)
     *   @param p1 [Point] Vertex of the rectangle.
     *   @param p2 [Point] Vertex of the rectangle opposite to <tt>p1</tt>.
     *   @param color [Scalar] Line color.
     *   @param options [Hash] Drawing options
     *   @option options [Integer] :thickness Line thickness.
     *   @option options [Integer] :line_type Type of the line.
     *     * 8 - 8-connected line.
     *     * 4 - 4-connected line.
     *     * <tt>CV_AA</tt> - Antialiased line.
     *   @option options [Integer] :shift Number of fractional bits in the point coordinates.
     * @return [Mat] <tt>self</tt>
     * @opencv_func cv::rectangle
     */
    VALUE rb_rectangle(int argc, VALUE *argv, VALUE self) {
      VALUE dst = rb_clone(self);
      return rb_rectangle_bang(argc, argv, dst);
    }

    /*
     * Draws a simple, thick, or filled up-right rectangle.
     *
     * @overload rectangle!(p1, p2, color, options = nil)
     *   @param p1 [Point] Vertex of the rectangle.
     *   @param p2 [Point] Vertex of the rectangle opposite to <tt>p1</tt>.
     *   @param color [Scalar] Line color.
     *   @param options [Hash] Drawing options
     *   @option options [Integer] :thickness Line thickness.
     *   @option options [Integer] :line_type Type of the line.
     *     * 8 - 8-connected line.
     *     * 4 - 4-connected line.
     *     * <tt>CV_AA</tt> - Antialiased line.
     *   @option options [Integer] :shift Number of fractional bits in the point coordinates.
     * @return [Mat] <tt>self</tt>
     * @opencv_func cv::rectangle
     */
    VALUE rb_rectangle_bang(int argc, VALUE *argv, VALUE self) {
      VALUE p1, p2, color, option;
      rb_scan_args(argc, argv, "31", &p1, &p2, &color, &option);

      drawing_option_t opt = drawing_option(option);
      try {
	cv::Point pt1 = Point::conpatible_obj2point(p1);
	cv::Point pt2 = Point::conpatible_obj2point(p2);
      	cv::rectangle(*(obj2mat(self)), pt1, pt2, *(Scalar::obj2scalar(color)), opt.thickness, opt.line_type, opt.shift);
      }
      catch (cv::Exception& e) {
	Error::raise(e);
      }
      return self;
    }

    /*
     * Draws a circle
     *
     * @overload circle(center, radius, options = nil)
     *   @param center [Point] Center of the circle.
     *   @param radius [Integer] Radius of the circle.
     *   @param color [Scalar] Circle color.
     *   @param options [Hash] Drawing options
     *   @option options [Integer] :thickness Line thickness.
     *   @option options [Integer] :line_type Type of the line.
     *     * 8 - 8-connected line.
     *     * 4 - 4-connected line.
     *     * <tt>CV_AA</tt> - Antialiased line.
     *   @option options [Integer] :shift Number of fractional bits in the coordinates of
     *     the center and in the radius value.
     * @return [Mat] Output array
     * @opencv_func cv::circle
     */
    VALUE rb_circle(int argc, VALUE *argv, VALUE self) {
      VALUE dst = rb_clone(self);
      return rb_circle_bang(argc, argv, dst);
    }

    /*
     * Draws a circle
     *
     * @overload circle!(center, radius, options = nil)
     *   @param center [Point] Center of the circle.
     *   @param radius [Integer] Radius of the circle.
     *   @param color [Scalar] Circle color.
     *   @param options [Hash] Drawing options
     *   @option options [Integer] :thickness Line thickness.
     *   @option options [Integer] :line_type Type of the line.
     *     * 8 - 8-connected line.
     *     * 4 - 4-connected line.
     *     * <tt>CV_AA</tt> - Antialiased line.
     *   @option options [Integer] :shift Number of fractional bits in the coordinates of
     *     the center and in the radius value.
     * @return [Mat] Output array
     * @opencv_func cv::circle
     */
    VALUE rb_circle_bang(int argc, VALUE *argv, VALUE self) {
      VALUE center, radius, color, option;
      rb_scan_args(argc, argv, "31", &center, &radius, &color, &option);

      cv::Mat* selfptr = obj2mat(self);
      drawing_option_t opt = drawing_option(option);
      try {
	cv::Point center_value = Point::conpatible_obj2point(center);
	cv::Scalar color_value = *(Scalar::obj2scalar(color));
	cv::circle(*selfptr, center_value, NUM2INT(radius), color_value, opt.thickness, opt.line_type, opt.shift);
      }
      catch (cv::Exception& e) {
	Error::raise(e);
      }

      return self;
    }
  }
}
