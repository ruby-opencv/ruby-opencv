#ifndef RUBY_OPENCV_DNN_NET_H
#define RUBY_OPENCV_DNN_NET_H

/*
 * Document-class: Cv::Dnn::Net
 */
namespace rubyopencv {
  namespace Dnn {
    namespace Net {
      void init(VALUE rb_module);
      VALUE net2obj(cv::dnn::experimental_dnn_v1::Net* ptr);
    }
  }
}

#endif // RUBY_OPENCV_DNN_NET_H
