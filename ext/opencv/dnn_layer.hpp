#ifndef RUBY_OPENCV_DNN_LAYER_H
#define RUBY_OPENCV_DNN_LAYER_H

/*
 * Document-class: Cv::Dnn::Layer
 */
namespace rubyopencv {
  namespace Dnn {
    namespace Layer {
      void init(VALUE rb_module);
      VALUE layer2obj(cv::dnn::Layer* ptr);
    }
  }
}

#endif // RUBY_OPENCV_DNN_LAYER_H
