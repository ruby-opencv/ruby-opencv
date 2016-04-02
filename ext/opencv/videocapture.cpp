// -*- mode: c++; coding: utf-8 -*-
#include "ruby.h"
#include "opencv2/core.hpp"
#include "opencv2/videoio.hpp"

#include "videocapture.hpp"
#include "mat.hpp"
#include "error.hpp"

/*
 * Document-class: Cv::VideoCapture
 */
namespace rubyopencv {
  namespace VideoCapture {
    void free_videocapture(void* ptr);
    size_t memsize_videocapture(const void* ptr);

    VALUE rb_klass = Qnil;
    rb_data_type_t opencv_videocapture_type = {
      "VideoCapture",
      { 0, free_videocapture, memsize_videocapture, 0 },
      0,
      0,
      0
    };

    void free_videocapture(void* ptr) {
      delete (cv::VideoCapture*)ptr;
    }

    size_t memsize_videocapture(const void* ptr) {
      return sizeof(cv::VideoCapture);
    }

    cv::VideoCapture* obj2videocapture(VALUE obj) {
      cv::VideoCapture* ptr = NULL;
      TypedData_Get_Struct(obj, cv::VideoCapture, &opencv_videocapture_type, ptr);
      return ptr;
    }

    VALUE rb_allocate(VALUE klass) {
      cv::VideoCapture* ptr = new cv::VideoCapture();
      return TypedData_Wrap_Struct(klass, &opencv_videocapture_type, ptr);
    }

    /*
     * Open video file or a capturing device for video capturing
     *
     * @scope class
     * @overload new(device = 0)
     *   @param device [String, Fixnum, nil] Video capturing device
     *     * If dev is a string (i.e "stream.avi"), reads video stream from a file.
     *     * If dev is a number, reads video stream from a device.
     * @return [VideoCapture] Opened capture instance
     * @opencv_func cv::VideoCapture::open
     */
    VALUE rb_initialize(int argc, VALUE *argv, VALUE self) {
      VALUE value;
      rb_scan_args(argc, argv, "01", &value);

      cv::VideoCapture* selfptr = obj2videocapture(self);
      try {
	if (TYPE(value) == T_STRING) {
	  char* filename = StringValueCStr(value);
	  selfptr->open(filename);
	}
	else {
	  int device = NIL_P(value) ? 0 : NUM2INT(value);
	  selfptr->open(device);
	}
      }
      catch (cv::Exception& e) {
	Error::raise(e);
      }

      return self;
    }

    /*
     * Grabs, decodes and returns the next video frame.
     * @overload query
     * @return [Mat] Next video frame
     * @opencv_func cv::VideoCapture::operator>>
     */
    VALUE rb_read(VALUE self) {
      cv::VideoCapture* selfptr = obj2videocapture(self);
      cv::Mat* m = Mat::empty_mat();

      try {
	(*selfptr) >> (*m);
      }
      catch (cv::Exception& e) {
	delete m;
	Error::raise(e);
      }

      return Mat::mat2obj(m);
    }

    /*
     * Returns true if video capturing has been initialized already.
     *
     * @overload opened?
     * @return [Boolean] The video capturing has been initialized already or not.
     * @opencv_func cv::VideoCapture::isOpened
     */
    VALUE rb_is_opened(VALUE self) {
      cv::VideoCapture* selfptr = obj2videocapture(self);
      bool is_opened = false;
      try {
	is_opened = selfptr->isOpened();
      }
      catch (cv::Exception& e) {
	Error::raise(e);
      }

      return is_opened ? Qtrue : Qfalse;
    }

    /*
     * Returns the specified VideoCapture property.
     *
     * @overload get(prop_id)
     * @param prop_id [Integer] Property identifier. It can be one of the following:
     *   - CAP_PROP_POS_MSEC - Current position of the video file in milliseconds.
     *   - CAP_PROP_POS_FRAMES - 0-based index of the frame to be decoded/captured next.
     *   - CAP_PROP_POS_AVI_RATIO - Relative position of the video file: 0 - start of the film, 1 - end of the film.
     *   - CAP_PROP_FRAME_WIDTH - Width of the frames in the video stream.
     *   - CAP_PROP_FRAME_HEIGHT - Height of the frames in the video stream.
     *   - CAP_PROP_FPS - Frame rate.
     *   - CAP_PROP_FOURCC - 4-character code of codec.
     *   - CAP_PROP_FRAME_COUNT - Number of frames in the video file.
     *   - CAP_PROP_FORMAT - Format of the Mat objects returned by retrieve() .
     *   - CAP_PROP_MODE - Backend-specific value indicating the current capture mode.
     *   - CAP_PROP_BRIGHTNESS - Brightness of the image (only for cameras).
     *   - CAP_PROP_CONTRAST - Contrast of the image (only for cameras).
     *   - CAP_PROP_SATURATION - Saturation of the image (only for cameras).
     *   - CAP_PROP_HUE - Hue of the image (only for cameras).
     *   - CAP_PROP_GAIN - Gain of the image (only for cameras).
     *   - CAP_PROP_EXPOSURE - Exposure (only for cameras).
     *   - CAP_PROP_CONVERT_RGB - Boolean flags indicating whether images should be converted to RGB.
     *   - CAP_PROP_WHITE_BALANCE - Currently unsupported
     *   - CAP_PROP_RECTIFICATION - Rectification flag for stereo cameras (note: only supported by DC1394 v 2.x backend currently)
     * @return [Number] VideoCapture property
     * @opencv_func cv::VideoCapture::get
     */
    VALUE rb_get(VALUE self, VALUE prop_id) {
      cv::VideoCapture* selfptr = obj2videocapture(self);
      double ret = 0;
      try {
	ret = selfptr->get(NUM2INT(prop_id));
      }
      catch (cv::Exception& e) {
	Error::raise(e);
      }

      return DBL2NUM(ret);
    }

    /*
     * Sets a property in the VideoCapture.
     *
     * @overload set(prop_id, value)
     * @param prop_id [Integer] Property identifier. It can be one of the following:
     *   - CAP_PROP_POS_MSEC - Current position of the video file in milliseconds.
     *   - CAP_PROP_POS_FRAMES - 0-based index of the frame to be decoded/captured next.
     *   - CAP_PROP_POS_AVI_RATIO - Relative position of the video file: 0 - start of the film, 1 - end of the film.
     *   - CAP_PROP_FRAME_WIDTH - Width of the frames in the video stream.
     *   - CAP_PROP_FRAME_HEIGHT - Height of the frames in the video stream.
     *   - CAP_PROP_FPS - Frame rate.
     *   - CAP_PROP_FOURCC - 4-character code of codec.
     *   - CAP_PROP_FRAME_COUNT - Number of frames in the video file.
     *   - CAP_PROP_FORMAT - Format of the Mat objects returned by retrieve() .
     *   - CAP_PROP_MODE - Backend-specific value indicating the current capture mode.
     *   - CAP_PROP_BRIGHTNESS - Brightness of the image (only for cameras).
     *   - CAP_PROP_CONTRAST - Contrast of the image (only for cameras).
     *   - CAP_PROP_SATURATION - Saturation of the image (only for cameras).
     *   - CAP_PROP_HUE - Hue of the image (only for cameras).
     *   - CAP_PROP_GAIN - Gain of the image (only for cameras).
     *   - CAP_PROP_EXPOSURE - Exposure (only for cameras).
     *   - CAP_PROP_CONVERT_RGB - Boolean flags indicating whether images should be converted to RGB.
     *   - CAP_PROP_WHITE_BALANCE - Currently unsupported
     *   - CAP_PROP_RECTIFICATION - Rectification flag for stereo cameras (note: only supported by DC1394 v 2.x backend currently)
     * @param value [Number] Value of the property
     * @return [Boolean] Result
     * @opencv_func cv::VideoCapture::set
     */
    VALUE rb_set(VALUE self, VALUE prop_id, VALUE value) {
      cv::VideoCapture* selfptr = obj2videocapture(self);
      double ret = 0;
      try {
	ret = selfptr->set(NUM2INT(prop_id), NUM2DBL(value));
      }
      catch (cv::Exception& e) {
	Error::raise(e);
      }

      return ret ? Qtrue : Qfalse;
    }

    /*
     * Grabs the next frame from video file or capturing device.
     *
     * @overload grab
     * @return [Boolean] If grabbing a frame successed, returns true, otherwise returns false.
     * @opencv_func cv::VideCapture.grab
     */
    VALUE rb_grab(VALUE self) {
      cv::VideoCapture* selfptr = obj2videocapture(self);
      bool ret = false;
      try {
	ret = selfptr->grab();
      }
      catch (cv::Exception& e) {
	Error::raise(e);
      }

      return ret ? Qtrue : Qfalse;
    }

    /*
     * Decodes and returns the grabbed video frame.
     *
     * @overload retrieve
     * @return [Mat] Grabbed video frame
     * @return [nil] Failed to grabbing a frame
     * @opencv_func cv::VideCapture::retrieve
     */
    VALUE rb_retrieve(int argc, VALUE *argv, VALUE self) {
      VALUE flag;
      rb_scan_args(argc, argv, "01", &flag);
      int flag_value = NIL_P(flag) ? 0 : NUM2INT(flag);

      cv::VideoCapture* selfptr = obj2videocapture(self);
      bool ret = false;
      cv::Mat* dstptr = Mat::empty_mat();
      try {
	ret = selfptr->retrieve(*dstptr, flag_value);
	if (!ret) {
	  delete dstptr;
	}
      }
      catch (cv::Exception& e) {
	delete dstptr;
	Error::raise(e);
      }

      return (ret) ? Mat::mat2obj(dstptr) : Qnil;
    }

    void init() {
      VALUE opencv = rb_define_module("Cv");

      rb_klass = rb_define_class_under(opencv, "VideoCapture", rb_cData);
      rb_define_alloc_func(rb_klass, rb_allocate);

      rb_define_private_method(rb_klass, "initialize", RUBY_METHOD_FUNC(rb_initialize), -1);
      rb_define_method(rb_klass, "read", RUBY_METHOD_FUNC(rb_read), 0);
      rb_define_method(rb_klass, "opened?", RUBY_METHOD_FUNC(rb_is_opened), 0);
      rb_define_method(rb_klass, "get", RUBY_METHOD_FUNC(rb_get), 1);
      rb_define_method(rb_klass, "set", RUBY_METHOD_FUNC(rb_set), 2);
      rb_define_method(rb_klass, "grab", RUBY_METHOD_FUNC(rb_grab), 0);
      rb_define_method(rb_klass, "retrieve", RUBY_METHOD_FUNC(rb_retrieve), -1);
    }
  }
}
