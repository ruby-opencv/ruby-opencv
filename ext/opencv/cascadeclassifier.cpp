#include "opencv2/objdetect.hpp"

#include "opencv.hpp"
#include "mat.hpp"
#include "size.hpp"
#include "rect.hpp"
#include "error.hpp"

/*
 * Document-class: Cv::CascadeClassifier
 */
namespace rubyopencv {
  namespace CascadeClassifier {
    void free_cascadeclassifier(void* ptr);
    size_t memsize_cascadeclassifier(const void* ptr);

    VALUE rb_klass = Qnil;
    rb_data_type_t opencv_cascadeclassifier_type = {
      "CascadeClassifier",
      { 0, free_cascadeclassifier, memsize_cascadeclassifier, },
      0,
      0,
      0
    };

    void free_cascadeclassifier(void* ptr) {
      delete (cv::CascadeClassifier*)ptr;
    }

    size_t memsize_cascadeclassifier(const void* ptr) {
      return sizeof(cv::CascadeClassifier);
    }

    cv::CascadeClassifier* obj2cascadeclassifier(VALUE obj) {
      cv::CascadeClassifier* ptr = NULL;
      TypedData_Get_Struct(obj, cv::CascadeClassifier, &opencv_cascadeclassifier_type, ptr);
      return ptr;
    }

    VALUE rb_allocate(VALUE klass) {
      cv::CascadeClassifier* ptr = new cv::CascadeClassifier();
      return TypedData_Wrap_Struct(klass, &opencv_cascadeclassifier_type, ptr);
    }

    VALUE rb_initialize(int argc, VALUE *argv, VALUE self) {
      VALUE filename;
      rb_scan_args(argc, argv, "01", &filename);

      cv::CascadeClassifier* selfptr = obj2cascadeclassifier(self);
      if (!NIL_P(filename)) {
	selfptr->load(StringValueCStr(filename));
      }

      return self;
    }

    VALUE rb_load(VALUE self, VALUE filename) {
      cv::CascadeClassifier* selfptr = obj2cascadeclassifier(self);
      bool ret = selfptr->load(StringValueCStr(filename));

      return ret ? Qtrue : Qfalse;
    }

    /*
     * Detects objects of different sizes in the input image. The detected objects are returned as a list of rectangles.
     *
     * @overload detect_multi_scale(image, options = nil)
     *   @param image [Mat] Matrix of the type CV_8U containing an image where objects are detected.
     *   @param options [Hash] Options
     *   @option options [Number] :scale_factor
     *     Parameter specifying how much the image size is reduced at each image scale.
     *   @option options [Integer] :min_neighbors
     *      Parameter specifying how many neighbors each candidate rectangle should have to retain it.
     *   @option options [Size] :min_size
     *      Minimum possible object size. Objects smaller than that are ignored.
     *   @option options [Size] :max_size
     *      Maximum possible object size. Objects larger than that are ignored.
     *   @return [Array<Rect>] Detected objects as a list of rectangles.
     * @opencv_func cv::CascadeClassifier::detectMultiScale
     */
    VALUE rb_detect_multi_scale(int argc, VALUE *argv, VALUE self) {
      VALUE image, options;

      rb_scan_args(argc, argv, "11", &image, &options);

      cv::CascadeClassifier* selfptr = obj2cascadeclassifier(self);
      std::vector<cv::Rect> objects;
      try {
	cv::Mat* m = Mat::obj2mat(image);
	if (NIL_P(options)) {
	  selfptr->detectMultiScale(*m, objects);
	}
	else {
	  Check_Type(options, T_HASH);
	  double scale_factor = NUM2DBL_DEFAULT(HASH_LOOKUP(options, "scale_factor"), 1.1);
	  int min_neighbors = NUM2INT_DEFAULT(HASH_LOOKUP(options, "min_neighbors"), 3);
	  cv::Size min_size;
	  cv::Size max_size;

	  VALUE tmp = Qnil;
	  tmp = rb_hash_lookup(options, ID2SYM(rb_intern("min_size")));
	  if (!NIL_P(tmp)) {
	    min_size = *(Size::obj2size(tmp));
	  }
	  tmp = rb_hash_lookup(options, ID2SYM(rb_intern("max_size")));
	  if (!NIL_P(tmp)) {
	    max_size = *(Size::obj2size(tmp));
	  }
	  selfptr->detectMultiScale(*m, objects, scale_factor, min_neighbors, 0, min_size, max_size);
	}
      }
      catch (cv::Exception& e) {
	Error::raise(e);
      }

      const long size = objects.size();
      VALUE detected_objects = rb_ary_new_capa(size);
      for (long i = 0; i < size; i++) {
	VALUE v = Rect::rect2obj(objects[i]);
	rb_ary_store(detected_objects, i, v);
      }

      return detected_objects;
    }

    void init() {
      VALUE opencv = rb_define_module("Cv");

      rb_klass = rb_define_class_under(opencv, "CascadeClassifier", rb_cData);
      rb_define_alloc_func(rb_klass, rb_allocate);

      rb_define_private_method(rb_klass, "initialize", RUBY_METHOD_FUNC(rb_initialize), -1);

      rb_define_method(rb_klass, "load", RUBY_METHOD_FUNC(rb_load), 1);
      rb_define_method(rb_klass, "detect_multi_scale", RUBY_METHOD_FUNC(rb_detect_multi_scale), -1);
    }
  }
}
