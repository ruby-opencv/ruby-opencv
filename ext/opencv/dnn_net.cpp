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

      cv::dnn::Net* rb_read_net_internal(VALUE model, VALUE config, VALUE framework) {
        cv::dnn::Net* dataptr = NULL;

        try {
          cv::dnn::Net net = cv::dnn::readNet(StringValueCStr(model), CSTR_DEFAULT(config, ""), CSTR_DEFAULT(framework, ""));
          dataptr = new cv::dnn::Net(net);
        } catch(cv::Exception& e) {
          delete dataptr;
          Error::raise(e);
        }

        return dataptr;
      }

      VALUE rb_initialize(int argc, VALUE *argv, VALUE self) {
        VALUE model, config, framework;
        rb_scan_args(argc, argv, "03", &model, &config, &framework);

        if (argc > 0) {
          RTYPEDDATA_DATA(self) = rb_read_net_internal(model, config, framework);
        }

        return self;
      }

      VALUE rb_read_net(int argc, VALUE *argv, VALUE self) {
        VALUE model, config, framework;
        rb_scan_args(argc, argv, "12", &model, &config, &framework);
        return net2obj(rb_read_net_internal(model, config, framework));
      }

      // void setInput(const Mat &blob, const String& name = "")
      VALUE rb_set_input(int argc, VALUE *argv, VALUE self) {
        VALUE blob, name;
        rb_scan_args(argc, argv, "11", &blob, &name);

        cv::dnn::Net* selfptr = obj2net(self);

        try {
          selfptr->setInput(*Mat::obj2mat(blob), CSTR_DEFAULT(name, ""));
        } catch(cv::Exception& e) {
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

        std::vector<cv::String> layer_names = selfptr->getLayerNames();
        const long size = layer_names.size();

        VALUE layers = rb_ary_new_capa(size);
        for (long i = 0; i < size; i++) {
          VALUE layer = Dnn::Layer::layer2obj(selfptr->getLayer(layer_names[i]));
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

      // Net readNetFromCaffe(const String &prototxt, const String &caffeModel = String());
      VALUE rb_read_net_from_caffe(VALUE self, VALUE prototxt, VALUE caffe_model) {
        cv::dnn::Net *net = NULL;

        try {
          net = new cv::dnn::Net(cv::dnn::readNetFromCaffe(StringValueCStr(prototxt), StringValueCStr(caffe_model)));
        } catch(cv::Exception& e) {
          delete net;
          Error::raise(e);
        }

        return net2obj(net);
      }

      // Net readNetFromTorch(const String &model, bool isBinary)
      VALUE rb_read_net_from_tensorflow(VALUE self, VALUE model) {
        cv::dnn::Net *net = NULL;

        try {
          net = new cv::dnn::Net(cv::dnn::readNetFromTensorflow(StringValueCStr(model)));
        } catch(cv::Exception& e) {
          delete net;
          Error::raise(e);
        }

        return net2obj(net);
      }

      // Net readNetFromTorch(const String &model, bool isBinary)
      VALUE rb_read_net_from_torch(VALUE self, VALUE model) {
        cv::dnn::Net *net = NULL;

        try {
          net = new cv::dnn::Net(cv::dnn::readNetFromTorch(StringValueCStr(model)));
        } catch(cv::Exception& e) {
          delete net;
          Error::raise(e);
        }

        return net2obj(net);
      }

      // Net readNetFromDarknet(const String &cfgFile, const String &darknetModel /*= String()*/)
      VALUE rb_read_net_from_darknet(VALUE self, VALUE cfg_file, VALUE darknet_model) {
        cv::dnn::Net *net = NULL;

        try {
          net = new cv::dnn::Net(cv::dnn::readNetFromDarknet(StringValueCStr(cfg_file), StringValueCStr(darknet_model)));
        } catch(cv::Exception& e) {
          delete net;
          Error::raise(e);
        }

        return net2obj(net);
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
