#include "opencv2/dnn.hpp"

#include "opencv.hpp"
#include "mat.hpp"
#include "size.hpp"
#include "scalar.hpp"
#include "dnn_net.hpp"
#include "dnn_layer.hpp"
#include "error.hpp"

namespace rubyopencv {
  namespace Dnn {
    VALUE rb_module = Qnil;

    // Mat blobFromImage(const Mat& image, double scalefactor=1.0, const Size& size = Size(), const Scalar& mean = Scalar(), bool swapRB=true)
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

    void init(VALUE opencv) {
      rb_module = rb_define_module_under(opencv, "Dnn");

      rb_define_singleton_method(rb_module, "blob_from_image", RUBY_METHOD_FUNC(rb_blob_from_image), -1);

      rb_define_singleton_method(rb_module, "read_net", RUBY_METHOD_FUNC(Dnn::Net::rb_read_net), -1);
      rb_define_singleton_method(rb_module, "read_net_from_caffe", RUBY_METHOD_FUNC(Dnn::Net::rb_read_net_from_caffe), 2);
      rb_define_singleton_method(rb_module, "read_net_from_tensorflow", RUBY_METHOD_FUNC(Dnn::Net::rb_read_net_from_tensorflow), 1);
      rb_define_singleton_method(rb_module, "read_net_from_torch", RUBY_METHOD_FUNC(Dnn::Net::rb_read_net_from_torch), 1);
      rb_define_singleton_method(rb_module, "read_net_from_darknet", RUBY_METHOD_FUNC(Dnn::Net::rb_read_net_from_darknet), 2);

      Dnn::Net::init(rb_module);
      Dnn::Layer::init(rb_module);
    }
  }
}
