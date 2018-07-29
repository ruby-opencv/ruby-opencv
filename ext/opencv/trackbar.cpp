#include "opencv2/highgui.hpp"
#include "ruby.h"

#include "trackbar.hpp"

/*
 * Document-class: Cv::Trackbar
 */
namespace rubyopencv {
  namespace Trackbar {
    void mark_trackbar(void *ptr);
    void free_trackbar(void *ptr);

    VALUE rb_klass = Qnil;
    rb_data_type_t opencv_trackbar_type = {
      "Trackbar",
      { mark_trackbar, free_trackbar, 0, },
      0,
      0,
      0
    };

    void mark_trackbar(void *ptr) {
      trackbar_t* trackbar_ptr = (trackbar_t*)ptr;
      rb_gc_mark(trackbar_ptr->block);
    }

    void free_trackbar(void *ptr) {
      xfree(((trackbar_t*)ptr)->name);
      xfree(ptr);
    }

    VALUE rb_allocate(VALUE klass) {
      trackbar_t* ptr = NULL;
      return TypedData_Make_Struct(klass, trackbar_t, &opencv_trackbar_type, ptr);
    }

    void trackbar_mark(void *ptr) {
      rb_gc_mark(((trackbar_t*)ptr)->block);
    }

    trackbar_t* obj2trackbar(VALUE obj) {
      trackbar_t* ptr = NULL;
      TypedData_Get_Struct(obj, trackbar_t, &opencv_trackbar_type, ptr);
      return ptr;
    }

    /*
     * Creates a new Trackbar.
     *
     * @overload new(name, count, value = 0, callback = nil)
     *   @param name [String] Name of the created trackbar.
     *   @param count [Integer] Maximal position of the slider. The minimal position is always 0.
     *   @param value [Integer] Optional value to an integer variable whose value reflects the position of the slider.
     *     Upon creation, the slider position is defined by this variable.
     *   @param [Proc] Function to be called every time the slider changes position.
     *   @return [Trackbar] Trackbar
     *
     * @overload new(name, count, value = 0) { |value| ... }
     *   @param name [String] Name of the created trackbar.
     *   @param count [Integer] Maximal position of the slider. The minimal position is always 0.
     *   @param value [Integer] Optional value to an integer variable whose value reflects the position of the slider.
     *     Upon creation, the slider position is defined by this variable.
     *   @yield [value] Function to be called every time the slider changes position.
     *   @yieldparam value [Integer] The trackbar position.
     *   @return [Trackbar] Trackbar
     */
    VALUE rb_initialize(int argc, VALUE *argv, VALUE self) {
      VALUE name, maxval, value, block;
      rb_scan_args(argc, argv, "21&", &name, &maxval, &value, &block);
      Check_Type(name, T_STRING);
      trackbar_t* selfptr = obj2trackbar(self);
      selfptr->name = strcpy(ALLOC_N(char, RSTRING_LEN(name) + 1), StringValueCStr(name));
      selfptr->maxval = NUM2INT(maxval);
      selfptr->value = NIL_P(value) ? 0 : NUM2INT(value);
      selfptr->block = block;

      return self;
    }

    VALUE newobj(int argc, VALUE *argv) {
      VALUE obj = rb_allocate(rb_klass);
      return rb_initialize(argc, argv, obj);
    }

    /*
     * Return trackbar name.
     */
    VALUE rb_name(VALUE self) {
      return rb_str_new2(obj2trackbar(self)->name);
    }

    /*
     * Return the maximum value that can be taken this trackbar.
     */
    VALUE rb_max(VALUE self) {
      return INT2NUM(obj2trackbar(self)->maxval);
    }

    /*
     * Return the value of this trackbar.
     */
    VALUE rb_value(VALUE self) {
      return INT2NUM(obj2trackbar(self)->value);
    }

    /*
     * call-seq:
     *   value  = <i>val</i>
     *
     * Set trackbar value.
     */
    VALUE rb_set_value(VALUE self, VALUE value) {
      obj2trackbar(self)->value = NUM2INT(value);
      return self;
    }

    void init() {
      VALUE opencv = rb_define_module("Cv");
      rb_klass = rb_define_class_under(opencv, "Trackbar", rb_cData);
      rb_define_alloc_func(rb_klass, rb_allocate);
      rb_define_method(rb_klass, "initialize", RUBY_METHOD_FUNC(rb_initialize), -1);
      rb_define_method(rb_klass, "name", RUBY_METHOD_FUNC(rb_name), 0);
      rb_define_method(rb_klass, "max", RUBY_METHOD_FUNC(rb_max), 0);
      rb_define_method(rb_klass, "value", RUBY_METHOD_FUNC(rb_value), 0);
      rb_define_method(rb_klass, "value=", RUBY_METHOD_FUNC(rb_set_value), 1);
    }
  }
}
