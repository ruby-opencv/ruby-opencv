#include "opencv2/dnn.hpp"

#include "opencv.hpp"
#include "mat.hpp"
#include "size.hpp"
#include "scalar.hpp"
#include "dnn_net.hpp"
#include "dnn_layer.hpp"
#include "error.hpp"

/*
 * Document-class: Cv::Dnn
 */
namespace rubyopencv {
  namespace Dnn {
    VALUE rb_module = Qnil;

    /*
     * Creates 4-dimensional blob from image. Optionally resizes and crops image from center, subtract mean values, scales values by scalefactor, swap Blue and Red channels.
     *
     * @overload blob_from_image(image, options = {})
     *   @param image [Mat] Input image (with 1-, 3- or 4-channels)
     *   @param options [Hash] Options
     *   @option options [Number] :scale_factor (1.0) Multiplier for image values
     *   @option options [Mat] :size Spatial size for output image
     *   @option options [Scalar] :mean Scalar with mean values which are subtracted from channels – values are intended to be in (mean-R, mean-G, mean-B) order if image has BGR ordering and swap_rb is true
     *   @option options [Boolean] :swap_rb (true) Indicates that swap first and last channels in 3-channel image is necessary
     *   @option options [Boolean] :crop (true) Indicates whether image will be cropped after resize or not
     * @return [Mat] 4-dimensional Mat with NCHW dimensions order
     */
    VALUE rb_blob_from_image(int argc, VALUE *argv, VALUE self) {
      VALUE image, options;
      rb_scan_args(argc, argv, "11", &image, &options);

      cv::Mat *b = NULL;

      try {
        double scale_factor = 1.0;
        cv::Size size;
        cv::Scalar mean;
        bool swap_rb = true;
        bool crop = true;

        if (!NIL_P(options)) {
          Check_Type(options, T_HASH);

          scale_factor = NUM2DBL_DEFAULT(HASH_LOOKUP(options, "scale_factor"), scale_factor);
          swap_rb = RTEST_DEFAULT(HASH_LOOKUP(options, "swap_rb"), (bool)swap_rb);
          crop = RTEST_DEFAULT(HASH_LOOKUP(options, "crop"), (bool)crop);

      	  VALUE tmp = Qnil;
      	  tmp = HASH_LOOKUP(options, "size");
      	  if (!NIL_P(tmp)) {
      	    size = *(Size::obj2size(tmp));
      	  }

      	  tmp = HASH_LOOKUP(options, "mean");
      	  if (!NIL_P(tmp)) {
      	    mean = *(Scalar::obj2scalar(tmp));
      	  }
        }

        b = new cv::Mat(cv::dnn::blobFromImage(*Mat::obj2mat(image), scale_factor, size, mean, swap_rb, crop));
      } catch(cv::Exception& e) {
        delete b;
        Error::raise(e);
      }

      return Mat::mat2obj(b);
    }

    void init() {
      VALUE opencv = rb_define_module("Cv");
      rb_module = rb_define_module_under(opencv, "Dnn");

      rb_define_singleton_method(rb_module, "blob_from_image", RUBY_METHOD_FUNC(rb_blob_from_image), -1);

      rb_define_singleton_method(rb_module, "read_net", RUBY_METHOD_FUNC(Dnn::Net::rb_read_net), -1); // in ext/opencv/dnn_net.cpp
      rb_define_singleton_method(rb_module, "read_net_from_caffe", RUBY_METHOD_FUNC(Dnn::Net::rb_read_net_from_caffe), 2); // in ext/opencv/dnn_net.cpp
      rb_define_singleton_method(rb_module, "read_net_from_tensorflow", RUBY_METHOD_FUNC(Dnn::Net::rb_read_net_from_tensorflow), 2); // in ext/opencv/dnn_net.cpp
      rb_define_singleton_method(rb_module, "read_net_from_torch", RUBY_METHOD_FUNC(Dnn::Net::rb_read_net_from_torch), -1); // in ext/opencv/dnn_net.cpp
      rb_define_singleton_method(rb_module, "read_net_from_darknet", RUBY_METHOD_FUNC(Dnn::Net::rb_read_net_from_darknet), 2); // in ext/opencv/dnn_net.cpp

      Dnn::Net::init();
      Dnn::Layer::init();
    }
  }
}
