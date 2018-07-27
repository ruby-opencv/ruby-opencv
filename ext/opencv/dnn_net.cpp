#include "opencv2/dnn.hpp"

#include "opencv.hpp"
#include "mat.hpp"
#include "error.hpp"
#include "dnn_layer.hpp"

/*
 * Document-class: Cv::Dnn::Net
 */
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

      /*
       * Creates or reads a deep learning network
       *
       * @overload new(model = nil, config = nil, framework = nil)
       *   @param model [String] Binary file contains trained weights
       *   @param config [String] Text file contains network configuration
       *   @param framework [String] Explicit framework name tag to determine a format
       * @return [Net] Network object
       * @opencv_func cv::dnn::Net
       * @example
       *   net1 = Dnn::Net.new
       *   net2 = Dnn::Net.new("bvlc_googlenet.caffemodel", "bvlc_googlenet.prototxt")
       */
      VALUE rb_initialize(int argc, VALUE *argv, VALUE self) {
        VALUE model, config, framework;
        rb_scan_args(argc, argv, "03", &model, &config, &framework);

        if (argc > 0) {
          RTYPEDDATA_DATA(self) = rb_read_net_internal(model, config, framework);
        }

        return self;
      }

      /*
       * Sets the new input value for the network
       *
       * @overload input=(blob)
       *   @param blob [Mat] A blob of CV_32F or CV_8U depth
       * @overload input(blob, name = nil)
       *   @param blob [Mat] A blob of CV_32F or CV_8U depth
       *   @param name [String] Name of an input layer
       * @return [void]
       */
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

      /*
       * Runs forward pass
       *
       * @overload forward(output_name = nil)
       *   @param output_name [String] Name of the layer for which output is needed
       * @return [Mat] Blob for first output
       */
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

      /*
       * Returns whether or not the network is empty
       *
       * @overload empty?
       * @return [Boolean] Whether or not the network is empty
       */
      VALUE rb_empty(VALUE self) {
        cv::dnn::Net* selfptr = obj2net(self);
        return selfptr->empty() ? Qtrue : Qfalse;
      }

      /*
       * Returns an array of layers loaded in this model
       *
       * @overload layers
       * @return [Array<Layer>] Loaded layers
       */
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

      /*
       * Enables or disables layer fusion in the network
       *
       * @overload fusion=(fusion)
       * @param fusion [Boolean] Whether or not fusion should be enabled
       * @return [Net] The current network
       */
      VALUE rb_enable_fusion(VALUE self, VALUE fusion) {
        cv::dnn::Net* selfptr = obj2net(self);
        selfptr->enableFusion(RTEST(fusion) ? true : false);
        return self;
      }

      /*
       * Ask network to use specific computation backend where it supported
       *
       * @overload preferable_backend=(backend_id)
       * @param backend_id [Integer] The preferable backend identifier
       * @return [Net] The current network
       */
      VALUE rb_set_preferable_backend(VALUE self, VALUE backend_id) {
        cv::dnn::Net* selfptr = obj2net(self);
        selfptr->setPreferableBackend(NUM2INT(backend_id));
        return self;
      }

      /*
       * Ask network to make computations on specific target device
       *
       * @overload preferable_target=(target_id)
       * @param target_id [Integer] The preferable target identifier
       * @return [Net] The current network
       */
      VALUE rb_set_preferable_target(VALUE self, VALUE target_id) {
        cv::dnn::Net* selfptr = obj2net(self);
        selfptr->setPreferableTarget(NUM2INT(target_id));
        return self;
      }

      /*
       * Read deep learning network represented in one of the supported formats.
       *
       * @overload read_net(model = nil, config = nil, framework = nil)
       *   @param model [String] Binary file contains trained weights
       *   @param config [String] Text file contains network configuration
       *   @param framework [String] Explicit framework name tag to determine a format
       * @return [Net] Network object
       */
      VALUE rb_read_net(int argc, VALUE *argv, VALUE self) {
        VALUE model, config, framework;
        rb_scan_args(argc, argv, "12", &model, &config, &framework);
        return net2obj(rb_read_net_internal(model, config, framework));
      }

      /*
       * Reads a network model stored in Caffe framework's format
       *
       * @overload read_net_from_caffe(prototxt, caffe_model)
       * @param prototxt [String] Path to the .prototxt file with text description of the network architecture
       * @param caffe_model [String] Path to the .caffemodel file with learned network
       * @return [Net] Network object
       */
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

      /*
       * Reads a network model stored in TensorFlow framework's format
       *
       * @overload read_net_from_tensorflow(model, config)
       * @param model [String] Path to the .pb file with binary protobuf description of the network architecture
       * @param config [String] Path to the .pbtxt file that contains text graph definition in protobuf format
       * @return [Net] Network object
       */
      VALUE rb_read_net_from_tensorflow(VALUE self, VALUE model, VALUE config) {
        cv::dnn::Net *net = NULL;

        try {
          net = new cv::dnn::Net(cv::dnn::readNetFromTensorflow(StringValueCStr(model), StringValueCStr(config)));
        } catch(cv::Exception& e) {
          delete net;
          Error::raise(e);
        }

        return net2obj(net);
      }

      /*
       * Reads a network model stored in Torch7 framework's format
       *
       * @overload read_net_from_torch(model, binary = true)
       *   @param model [String] Path to the file, dumped from Torch by using torch.save() function
       *   @param binary [Boolean] Specifies whether the network was serialized in ascii mode or binary
       * @return [Net] Network object
       */
      VALUE rb_read_net_from_torch(int argc, VALUE *argv, VALUE self) {
        VALUE model, binary;
        rb_scan_args(argc, argv, "11", &model, &binary);
        cv::dnn::Net *net = NULL;

        try {
          net = new cv::dnn::Net(cv::dnn::readNetFromTorch(StringValueCStr(model), RTEST_DEFAULT(binary, true)));
        } catch(cv::Exception& e) {
          delete net;
          Error::raise(e);
        }

        return net2obj(net);
      }

      /*
       * Reads a network model stored in Darknet model files
       *
       * @overload read_net_from_darknet(cfg_file, darknet_model)
       * @param cfg_file [String] Path to the .cfg file with text description of the network architecture
       * @param darknet_model [String] Path to the .weights file with learned network
       * @return [Net] Network object
       */
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

      void init() {
        VALUE opencv = rb_define_module("Cv");
        VALUE dnn = rb_define_module_under(opencv, "Dnn");
        rb_klass = rb_define_class_under(dnn, "Net", rb_cData);
        rb_define_alloc_func(rb_klass, rb_allocate);

        rb_define_method(rb_klass, "initialize", RUBY_METHOD_FUNC(rb_initialize), -1);

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
