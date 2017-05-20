/************************************************************

   facerecognizer.cpp -

   $Author: ser1zw $

   Copyright (C) 2013 ser1zw

************************************************************/
#include "ruby.h"
#include "algorithm.h"
#include "cvmat.h"
#include "facerecognizer.h"
#include "opencv.h"

/*
 * Document-class: OpenCV::FaceRecognizer
 *
 */
namespace mOpenCV {
  namespace cFaceRecognizer {

    VALUE rb_klass;

    VALUE
    rb_class()
    {
      return rb_klass;
    }

    std::map<void*, cv::Ptr<cv::FaceRecognizer> > ptr_guard_map;

    void
    guard_facerecognizer(void* data_ptr, cv::Ptr<cv::FaceRecognizer> ptr)
    {
      ptr_guard_map[data_ptr] = ptr;
    }

    void
    release_facerecognizer(void *data_ptr)
    {
      ptr_guard_map[data_ptr].release();
      ptr_guard_map.erase(data_ptr);
    }

    VALUE
    allocate_facerecognizer(VALUE klass)
    {
      return Data_Wrap_Struct(klass, 0, release_facerecognizer, NULL);
    }

#if IS_OPENCV2

    /*
     * call-seq:
     *   train(src, labels)
     *
     * Trains a FaceRecognizer with given data and associated labels.
     */
    VALUE
    rb_train(VALUE self, VALUE src, VALUE labels)
    {
      Check_Type(src, T_ARRAY);
      Check_Type(labels, T_ARRAY);

      VALUE *src_ptr = RARRAY_PTR(src);
      int src_size = RARRAY_LEN(src);
      std::vector<cv::Mat> images;
      for (int i = 0; i < src_size; i++) {
	images.push_back(cv::Mat(CVMAT_WITH_CHECK(src_ptr[i])));
      }

      VALUE *labels_ptr = RARRAY_PTR(labels);
      int labels_size = RARRAY_LEN(labels);
      std::vector<int> local_labels;
      for (int i = 0; i < labels_size; i++) {
	local_labels.push_back(NUM2INT(labels_ptr[i]));
      }

      cv::FaceRecognizer *self_ptr = FACERECOGNIZER(self);
      try {
	self_ptr->train(images, local_labels);
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }

      return Qnil;
    }

    /*
     * call-seq:
     *   udpate(src, labels)
     *
     * Updates a FaceRecognizer with given data and associated labels. Only valid on LBPH models.
     */
    VALUE
    rb_update(VALUE self, VALUE src, VALUE labels)
    {
      Check_Type(src, T_ARRAY);
      Check_Type(labels, T_ARRAY);

      VALUE *src_ptr = RARRAY_PTR(src);
      int src_size = RARRAY_LEN(src);
      std::vector<cv::Mat> images;
      for (int i = 0; i < src_size; i++) {
	images.push_back(cv::Mat(CVMAT_WITH_CHECK(src_ptr[i])));
      }

      VALUE *labels_ptr = RARRAY_PTR(labels);
      int labels_size = RARRAY_LEN(labels);
      std::vector<int> local_labels;
      for (int i = 0; i < labels_size; i++) {
	local_labels.push_back(NUM2INT(labels_ptr[i]));
      }

      cv::FaceRecognizer *self_ptr = FACERECOGNIZER(self);
      try {
	self_ptr->update(images, local_labels);
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }

      return Qnil;
    }

    /*
     * call-seq:
     *   predict(src)
     * 
     * Predicts a label and associated confidence (e.g. distance) for a given input image.
     */
    VALUE
    rb_predict(VALUE self, VALUE src)
    {
      cv::Mat mat = cv::Mat(CVMAT_WITH_CHECK(src));
      cv::FaceRecognizer *self_ptr = FACERECOGNIZER(self);
      int label;
      double confidence;
      try {
	self_ptr->predict(mat, label, confidence);
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }

      return rb_ary_new3(2, INT2NUM(label), DBL2NUM(confidence));
    }


    /*
     * call-seq:
     *   save(filename)
     *
     * Saves this model to a given filename, either as XML or YAML.
     */
    VALUE
    rb_save(VALUE self, VALUE filename)
    {
      Check_Type(filename, T_STRING);
      cv::FaceRecognizer *self_ptr = FACERECOGNIZER(self);
      try {
	char* s = StringValueCStr(filename);
	self_ptr->save(std::string(s));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }

      return Qnil;
    }

    /*
     * call-seq:
     *   load(filename)
     *
     * Loads a FaceRecognizer and its model state.
     */
    VALUE
    rb_load(VALUE self, VALUE filename)
    {
      Check_Type(filename, T_STRING);
      cv::FaceRecognizer *self_ptr = FACERECOGNIZER(self);
      try {
	char* s = StringValueCStr(filename);
	self_ptr->load(std::string(s));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }

      return Qnil;
    }
#else
#  define rb_train raise_opencv3_unsupported_n
#  define rb_update raise_opencv3_unsupported_n
#  define rb_predict raise_opencv3_unsupported_n
#  define rb_save raise_opencv3_unsupported_n
#  define rb_load raise_opencv3_unsupported_n
#endif

    void
    init_ruby_class()
    {
#if 0
      // For documentation using YARD
      VALUE opencv = rb_define_module("OpenCV");
      VALUE alghorithm = rb_define_class_under(opencv, "Algorithm", rb_cObject);
#endif

      if (rb_klass)
	return;
      /* 
       * opencv = rb_define_module("OpenCV");
       * 
       * note: this comment is used by rdoc.
       */
      VALUE opencv = rb_module_opencv();
      VALUE alghorithm = cAlgorithm::rb_class();
      rb_klass = rb_define_class_under(opencv, "FaceRecognizer", alghorithm);

      rb_define_method(rb_klass, "train", RUBY_METHOD_FUNC(rb_train), 2);
      rb_define_method(rb_klass, "update", RUBY_METHOD_FUNC(rb_update), 2);
      rb_define_method(rb_klass, "predict", RUBY_METHOD_FUNC(rb_predict), 1);
      rb_define_method(rb_klass, "save", RUBY_METHOD_FUNC(rb_save), 1);
      rb_define_method(rb_klass, "load", RUBY_METHOD_FUNC(rb_load), 1);
    }
  }
}
