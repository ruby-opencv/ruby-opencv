#include "opencv2/dnn.hpp"

#include "opencv.hpp"
#include "error.hpp"

/*
 * Document-class: Cv::Dnn::Layer
 */
namespace rubyopencv {
  namespace Dnn {
    namespace Layer {
      VALUE rb_klass = Qnil;

      rb_data_type_t opencv_layer_type = {
        "Dnn::Layer", { 0, 0, 0, }, 0, 0, 0
      };

      VALUE layer2obj(cv::dnn::Layer* ptr) {
        return TypedData_Wrap_Struct(rb_klass, &opencv_layer_type, ptr);
      }

      cv::dnn::Layer* obj2layer(VALUE obj) {
        cv::dnn::Layer* ptr = NULL;
        TypedData_Get_Struct(obj, cv::dnn::Layer, &opencv_layer_type, ptr);
        return ptr;
      }

      VALUE rb_allocate(VALUE klass) {
        cv::dnn::Layer* ptr = NULL;
        return TypedData_Wrap_Struct(klass, &opencv_layer_type, ptr);
      }

      VALUE rb_initialize(VALUE self) {
        return self;
      }

      /*
       * Returns the layer name
       * @overload name
       * @return [String] Layer name
       */
      VALUE rb_name(VALUE self) {
        cv::dnn::Layer* selfptr = obj2layer(self);
        return rb_str_new_cstr(selfptr->name.c_str());
      }

      /*
       * Returns the layer type
       *
       * @overload type
       * @return [String] Layer type
       */
      VALUE rb_type(VALUE self) {
        cv::dnn::Layer* selfptr = obj2layer(self);
        return rb_str_new_cstr(selfptr->type.c_str());
      }

      void init() {
        VALUE opencv = rb_define_module("Cv");
        VALUE dnn = rb_define_module_under(opencv, "Dnn");
        rb_klass = rb_define_class_under(dnn, "Layer", rb_cData);
        rb_define_alloc_func(rb_klass, rb_allocate);

        rb_define_private_method(rb_klass, "initialize", RUBY_METHOD_FUNC(rb_initialize), 0);

        rb_define_method(rb_klass, "name", RUBY_METHOD_FUNC(rb_name), 0);
        rb_define_method(rb_klass, "type", RUBY_METHOD_FUNC(rb_type), 0);

        #if 0
        rb_define_attr(rb_klass, "name", 1, 0);
        rb_define_attr(rb_klass, "type", 1, 0);
        #endif
      }
    }
  }
}
