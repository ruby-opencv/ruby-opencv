#ifndef RUBY_OPENCV_DNN_NET_H
#define RUBY_OPENCV_DNN_NET_H

/*
 * Document-class: Cv::Dnn::Net
 */
namespace rubyopencv {
  namespace Dnn {
    namespace Net {
      void init(VALUE rb_module);
      VALUE net2obj(cv::dnn::Net* ptr);
      VALUE rb_read_net(int argc, VALUE *argv, VALUE self);
      VALUE rb_read_net_from_caffe(VALUE self, VALUE prototxt, VALUE caffe_model);
      VALUE rb_read_net_from_tensorflow(VALUE self, VALUE model);
      VALUE rb_read_net_from_torch(VALUE self, VALUE model);
      VALUE rb_read_net_from_darknet(VALUE self, VALUE cfg_file, VALUE darknet_model);
    }
  }
}

#endif // RUBY_OPENCV_DNN_NET_H
