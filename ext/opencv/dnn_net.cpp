#include "opencv2/dnn.hpp"

#include "opencv.hpp"
#include "mat.hpp"
#include "error.hpp"
#include "dnn_layer.hpp"

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

      VALUE rb_initialize(int argc, VALUE *argv, VALUE self) {
        VALUE model, config, framework;
        rb_scan_args(argc, argv, "03", &model, &config, &framework);

        if (!NIL_P(model)) {
          cv::dnn::Net* dataptr = NULL;

          try {
            cv::dnn::Net net = cv::dnn::readNet(StringValueCStr(model), CSTR_DEFAULT(config, ""), CSTR_DEFAULT(framework, ""));
            cv::dnn::Net* dataptr = new cv::dnn::Net(net);
            RTYPEDDATA_DATA(self) = dataptr;
          } catch(cv::Exception& e) {
            delete dataptr;
            Error::raise(e);
          }
        }

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
          m = new cv::Mat(selfptr->forward(CSTR_DEFAULT(output_name, "")));
        } catch(cv::Exception& e) {
          delete m;
          Error::raise(e);
        }

        return Mat::rb_clone(Mat::mat2obj(m));
      }

      // bool empty() const
      VALUE rb_empty(VALUE self) {
        cv::dnn::Net* selfptr = obj2net(self);
        return selfptr->empty() ? Qtrue : Qfalse;
      }

      VALUE rb_get_layers(VALUE self) {
        cv::dnn::Net* selfptr = obj2net(self);

        std::vector<cv::String> v = selfptr->getLayerNames();
        const long size = v.size();

        VALUE layers = rb_ary_new_capa(size);
        for (long i = 0; i < size; i++) {
          VALUE layer = Dnn::Layer::layer2obj(selfptr->getLayer(v[i]));
          rb_ary_store(layers, i, layer);
        }

        return layers;
      }

      VALUE rb_enable_fusion(VALUE self, VALUE fusion) {
        cv::dnn::Net* selfptr = obj2net(self);
        selfptr->enableFusion(RTEST(fusion) ? true : false);
        return self;
      }

      VALUE rb_set_preferable_backend(VALUE self, VALUE backend_id) {
        cv::dnn::Net* selfptr = obj2net(self);
        selfptr->setPreferableBackend(NUM2INT(backend_id));
        return self;
      }

      VALUE rb_set_preferable_target(VALUE self, VALUE target_id) {
        cv::dnn::Net* selfptr = obj2net(self);
        selfptr->setPreferableTarget(NUM2INT(target_id));
        return self;
      }

      void init(VALUE rb_module) {
        rb_klass = rb_define_class_under(rb_module, "Net", rb_cData);
        rb_define_alloc_func(rb_klass, rb_allocate);

        rb_define_private_method(rb_klass, "initialize", RUBY_METHOD_FUNC(rb_initialize), -1);

        rb_define_method(rb_klass, "input=", RUBY_METHOD_FUNC(rb_set_input), -1);
        rb_define_alias(rb_klass, "input", "input=");
        rb_define_method(rb_klass, "fusion=", RUBY_METHOD_FUNC(rb_enable_fusion), 1);
        rb_define_method(rb_klass, "preferable_backend=", RUBY_METHOD_FUNC(rb_set_preferable_backend), 1);
        rb_define_method(rb_klass, "preferable_target=", RUBY_METHOD_FUNC(rb_set_preferable_target), 1);

        rb_define_method(rb_klass, "forward", RUBY_METHOD_FUNC(rb_forward), -1);
        rb_define_method(rb_klass, "empty?", RUBY_METHOD_FUNC(rb_empty), 0);
        rb_define_method(rb_klass, "layers", RUBY_METHOD_FUNC(rb_get_layers), 0);
      }
    }
  }
}
