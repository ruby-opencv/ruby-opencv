#include "opencv2/dnn.hpp"

#include "opencv.hpp"
#include "mat.hpp"
#include "error.hpp"

namespace rubyopencv {
  namespace Dnn {
    namespace Net {
      VALUE rb_klass = Qnil;

      void free_net(void* ptr) {
        delete (cv::dnn::Net*)ptr;
      }

      size_t memsize_net(const void* ptr) {
        return sizeof(cv::dnn::Net);
      }

      rb_data_type_t opencv_net_type = {
        "Dnn::Net", { 0, free_net, memsize_net, }, 0, 0, 0
      };

      VALUE net2obj(cv::dnn::Net* ptr) {
        return TypedData_Wrap_Struct(rb_klass, &opencv_net_type, ptr);
      }

      cv::dnn::Net* obj2net(VALUE obj) {
        cv::dnn::Net* ptr = NULL;
        TypedData_Get_Struct(obj, cv::dnn::Net, &opencv_net_type, ptr);
        return ptr;
      }

      VALUE rb_allocate(VALUE klass) {
        cv::dnn::Net* ptr = new cv::dnn::Net();
        return TypedData_Wrap_Struct(klass, &opencv_net_type, ptr);
      }

      VALUE rb_initialize(VALUE self) {
        return self;
      }

      // void setInput(const Mat &blob, const String& name = "")
      VALUE rb_set_input(int argc, VALUE *argv, VALUE self) {
        VALUE blob, name;
        rb_scan_args(argc, argv, "11", &blob, &name);

        cv::dnn::Net* selfptr = obj2net(self);

        cv::Mat *m = Mat::obj2mat(blob);

        try {
          if (NIL_P(name)) {
            selfptr->setInput(*m);
          } else {
            selfptr->setInput(*m, StringValueCStr(name));
          }
        } catch(cv::Exception& e) {
          delete m;
          Error::raise(e);
        }

        return Qnil;
      }

      // Mat forward(const String& outputName = String())
      VALUE rb_forward(int argc, VALUE *argv, VALUE self) {
        VALUE output_name;
        rb_scan_args(argc, argv, "01", &output_name);

        cv::dnn::Net* selfptr = obj2net(self);

        cv::Mat* m = NULL;

        try {
          cv::Mat r;

          if (NIL_P(output_name)) {
            r = selfptr->forward();
          } else {
            r = selfptr->forward(StringValueCStr(output_name));
          }

          m = new cv::Mat(r);
        } catch(cv::Exception& e) {
          delete m;
          Error::raise(e);
        }

        return Mat::mat2obj(m);
      }

      // bool empty() const
      VALUE rb_empty(VALUE self) {
        cv::dnn::Net* selfptr = obj2net(self);
        return selfptr->empty() ? Qtrue : Qfalse;
      }

      void init(VALUE rb_module) {
        rb_klass = rb_define_class_under(rb_module, "Net", rb_cData);
        rb_define_alloc_func(rb_klass, rb_allocate);

        rb_define_private_method(rb_klass, "initialize", RUBY_METHOD_FUNC(rb_initialize), 0);

        rb_define_method(rb_klass, "set_input", RUBY_METHOD_FUNC(rb_set_input), -1);
        rb_define_method(rb_klass, "forward", RUBY_METHOD_FUNC(rb_forward), -1);
        rb_define_method(rb_klass, "empty?", RUBY_METHOD_FUNC(rb_empty), 0);
      }
    }
  }
}
