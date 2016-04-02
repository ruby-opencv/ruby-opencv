#include <sstream>
#include "ruby.h"
#include "opencv2/core.hpp"

#include "error.hpp"

/*
 * Document-class: OpenCV::Rect
 */
namespace rubyopencv {
  namespace Rect {
    void free_rect(void* ptr);
    size_t memsize_rect(const void* ptr);

    VALUE rb_klass = Qnil;
    rb_data_type_t opencv_rect_type = {
      "Rect",
      { 0, free_rect, memsize_rect, 0 },
      0,
      0,
      0
    };

    void free_rect(void* ptr) {
      delete (cv::Rect*)ptr;
    }

    size_t memsize_rect(const void* ptr) {
      return sizeof(cv::Rect);
    }

    cv::Rect* obj2rect(VALUE obj) {
      cv::Rect* ptr = NULL;
      TypedData_Get_Struct(obj, cv::Rect, &opencv_rect_type, ptr);
      return ptr;
    }

    VALUE rect2obj (cv::Rect rect) {
      cv::Rect* ptr = new cv::Rect(rect);
      return TypedData_Wrap_Struct(rb_klass, &opencv_rect_type, ptr);
    }

    VALUE rb_allocate(VALUE klass) {
      cv::Rect* ptr = new cv::Rect();
      return TypedData_Wrap_Struct(klass, &opencv_rect_type, ptr);
    }

    /*
     * Create a rectangle
     *
     * @overload new()
     *   @return [Rect] new rectangle
     * @overload new(x, y, width, height)
     *   @param x [Integer] x coordinate
     *   @param y [Integer] y coordinate
     *   @param width [Integer] Width
     *   @param height [Integer] Height
     *   @return [Rect] new rectangle
     */
    VALUE rb_initialize(int argc, VALUE *argv, VALUE self) {
      const int SIZE = 4;
      VALUE values[SIZE];
      rb_scan_args(argc, argv, "04", &values[0], &values[1], &values[2], &values[3]);

      cv::Rect* selfptr = obj2rect(self);
      switch (argc) {
      case 0:
	selfptr->x = 0;
	selfptr->y = 0;
	selfptr->width = 0;
	selfptr->height = 0;
	break;
      case 4:
	selfptr->x = NUM2INT(values[0]);
	selfptr->y = NUM2INT(values[1]);
	selfptr->width = NUM2INT(values[2]);
	selfptr->height = NUM2INT(values[3]);
	break;
      default:
	rb_raise(rb_eArgError, "wrong number of arguments (%d for 0 or 4)", argc);
	break;
      }

      return self;
    }

    /*
     * Return parameter on x-axis.
     */
    VALUE rb_x(VALUE self) {
      return INT2NUM(obj2rect(self)->x);
    }

    /*
     * call-seq:
     *   x = val
     */
    VALUE rb_set_x(VALUE self, VALUE x) {
      obj2rect(self)->x = NUM2INT(x);
      return self;
    }

    /*
     * Return parameter on y-axis.
     */
    VALUE rb_y(VALUE self) {
      return INT2NUM(obj2rect(self)->y);
    }

    /*
     * call-seq:
     *   y = val
     */
    VALUE rb_set_y(VALUE self, VALUE y) {
      obj2rect(self)->y = NUM2INT(y);
      return self;
    }

    /*
     * Return width
     *
     * @overload width
     * @return [Integer] Width
     */
    VALUE rb_width(VALUE self) {
      return INT2NUM(obj2rect(self)->width);
    }

    /*
     * Set width
     *
     * @overload width=(value)
     * @param value [Integer] Width
     * @return [Rect] +self+
     */
    VALUE rb_set_width(VALUE self, VALUE width) {
      obj2rect(self)->width = NUM2INT(width);
      return self;
    }

    /*
     * Return height
     *
     * @overload height
     * @return [Integer] Height
     */
    VALUE rb_height(VALUE self) {
      return INT2NUM(obj2rect(self)->height);
    }

    /*
     * Set height
     *
     * @overload height=(value)
     * @param value [Integer] Height
     * @return [Rect] +self+
     */
    VALUE rb_set_height(VALUE self, VALUE height) {
      obj2rect(self)->height = NUM2INT(height);
      return self;
    }

    /*
     * @overload to_s
     * @return [String] String representation of the rectangle
     */
    VALUE rb_to_s(VALUE self) {
      std::stringstream s;
      cv::Rect* selfptr = obj2rect(self);
      s << *selfptr;

      VALUE param[3];
      param[0] = rb_str_new2("#<%s:%s>");
      param[1] = rb_str_new2(rb_class2name(CLASS_OF(self)));
      param[2] = rb_str_new2(s.str().c_str());

      int n = sizeof(param) / sizeof(param[0]);
      return rb_f_sprintf(n, param);
    }

    void init() {
      VALUE opencv = rb_define_module("OpenCV");

      rb_klass = rb_define_class_under(opencv, "Rect", rb_cData);
      rb_define_alloc_func(rb_klass, rb_allocate);

      rb_define_private_method(rb_klass, "initialize", RUBY_METHOD_FUNC(rb_initialize), -1);
      rb_define_method(rb_klass, "to_s", RUBY_METHOD_FUNC(rb_to_s), 0);
      rb_define_method(rb_klass, "x", RUBY_METHOD_FUNC(rb_x), 0);
      rb_define_method(rb_klass, "x=", RUBY_METHOD_FUNC(rb_set_x), 1);
      rb_define_method(rb_klass, "y", RUBY_METHOD_FUNC(rb_y), 0);
      rb_define_method(rb_klass, "y=", RUBY_METHOD_FUNC(rb_set_y), 1);

      rb_define_method(rb_klass, "width", RUBY_METHOD_FUNC(rb_width), 0);
      rb_define_method(rb_klass, "width=", RUBY_METHOD_FUNC(rb_set_width), 1);
      rb_define_method(rb_klass, "height", RUBY_METHOD_FUNC(rb_height), 0);
      rb_define_method(rb_klass, "height=", RUBY_METHOD_FUNC(rb_set_height), 1);
    }
  }
}
