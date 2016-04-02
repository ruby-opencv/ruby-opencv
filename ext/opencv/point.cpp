#include <sstream>
#include "ruby.h"
#include "opencv2/core.hpp"

#include "point.hpp"

namespace rubyopencv {
  namespace Point {
    void free_point(void* ptr);
    size_t memsize_point(const void* ptr);

    VALUE rb_klass = Qnil;
    rb_data_type_t opencv_point_type = {
      "Point",
      { 0, free_point, memsize_point, 0 },
      0,
      0,
      0
    };

    size_t memsize_point(const void* ptr) {
      return sizeof(cv::Point);
    }

    cv::Point* obj2point(VALUE obj) {
      cv::Point* ptr = NULL;
      TypedData_Get_Struct(obj, cv::Point, &opencv_point_type, ptr);
      return ptr;
    }

    VALUE point2obj(cv::Point* ptr) {
      return TypedData_Wrap_Struct(rb_klass, &opencv_point_type, (void*)ptr);
    }

    cv::Point conpatible_obj2point(VALUE obj) {
      if (rb_respond_to(obj, rb_intern("x")) && rb_respond_to(obj, rb_intern("y"))) {
	return cv::Point(NUM2INT(rb_funcall(obj, rb_intern("x"), 0)),
			 NUM2INT(rb_funcall(obj, rb_intern("y"), 0)));
      }
      rb_raise(rb_eTypeError, "wrong argument type %s (expected %s or compatible object)",
	       rb_obj_classname(obj), rb_class2name(rb_klass));
      throw;
    }

    void free_point(void* ptr) {
      xfree(ptr);
    }

    VALUE rb_allocate(VALUE klass) {
      cv::Point* ptr = NULL;
      return TypedData_Make_Struct(klass, cv::Point, &opencv_point_type, ptr);
    }

    /*
     * call-seq:
     *   new -> OpenCV::Point.new(0, 0)
     *   new(obj) -> OpenCV::Point.new(obj.x.to_i, obj.y.to_i)
     *   new(x, y)
     *
     * Create new 2D-coordinate, (x, y). It is dropped below the decimal point.
     *
     * new() is same as new(0, 0)
     *
     * new(obj) is same as new(obj.x.to_i, obj.y.to_i)
     */
    VALUE rb_initialize(int argc, VALUE *argv, VALUE self) {
      VALUE v1, v2;
      rb_scan_args(argc, argv, "02", &v1, &v2);
      cv::Point* selfptr = obj2point(self);
      switch (argc) {
      case 0:
	selfptr->x = 0;
	selfptr->y = 0;
	break;
      case 1: {
	cv::Point point = conpatible_obj2point(v1);
	selfptr->x = point.x;
	selfptr->y = point.y;
	break;
      }
      case 2:
	selfptr->x = NUM2INT(v1);
	selfptr->y = NUM2INT(v2);
	break;
      default:
	rb_raise(rb_eArgError, "wrong number of arguments (%d for 0..2)", argc);
	break;
      }

      return self;
    }

    /*
     * Return parameter on x-axis.
     */
    VALUE rb_x(VALUE self) {
      return INT2NUM(obj2point(self)->x);
    }

    /*
     * call-seq:
     *   x = val
     */
    VALUE rb_set_x(VALUE self, VALUE x) {
      obj2point(self)->x = NUM2INT(x);
      return self;
    }

    /*
     * Return parameter on y-axis.
     */
    VALUE rb_y(VALUE self) {
      return INT2NUM(obj2point(self)->y);
    }

    /*
     * call-seq:
     *   y = val
     */
    VALUE rb_set_y(VALUE self, VALUE y) {
      obj2point(self)->y = NUM2INT(y);
      return self;
    }

    /*
     * Return x and y as an +String+.
     *
     * @overload to_s
     * @return [String] String representation of the point
     */
    VALUE rb_to_s(VALUE self) {
      std::stringstream s;
      cv::Point* selfptr = obj2point(self);
      s << *selfptr;

      VALUE param[3];
      param[0] = rb_str_new2("#<%s:%s>");
      param[1] = rb_str_new2(rb_class2name(CLASS_OF(self)));
      param[2] = rb_str_new2(s.str().c_str());

      int n = sizeof(param) / sizeof(param[0]);
      return rb_f_sprintf(n, param);
    }

    /*
     * Return x and y as an +Array+.
     *
     * @overload to_a
     * @return [Array<Integer>] Array representation of the point
     */
    VALUE rb_to_a(VALUE self) {
      cv::Point* selfptr = obj2point(self);
      return rb_ary_new3(2, INT2NUM(selfptr->x), INT2NUM(selfptr->y));
    }

    void init() {
      VALUE opencv = rb_define_module("OpenCV");

      rb_klass = rb_define_class_under(opencv, "Point", rb_cData);
      rb_define_alloc_func(rb_klass, rb_allocate);
      rb_define_method(rb_klass, "initialize", RUBY_METHOD_FUNC(rb_initialize), -1);
      rb_define_method(rb_klass, "x", RUBY_METHOD_FUNC(rb_x), 0);
      rb_define_method(rb_klass, "x=", RUBY_METHOD_FUNC(rb_set_x), 1);
      rb_define_method(rb_klass, "y", RUBY_METHOD_FUNC(rb_y), 0);
      rb_define_method(rb_klass, "y=", RUBY_METHOD_FUNC(rb_set_y), 1);

      rb_define_method(rb_klass, "to_s", RUBY_METHOD_FUNC(rb_to_s), 0);
      rb_define_method(rb_klass, "to_a", RUBY_METHOD_FUNC(rb_to_a), 0);
    }
  }
}

