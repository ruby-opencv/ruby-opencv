/************************************************************

   cvvideowriter.cpp -

   $Author: lsxi $

   Copyright (C) 2005-2006 Masakazu Yonekura

************************************************************/
#include "cvvideowriter.h"
#include "iplimage.h"
#include "cvsize.h"
/*
 * Document-class: OpenCV::CvVideoWriter
 *
 * Create video stream from images.
 *
 * C structure is "black box".
 */
namespace mOpenCV {
  namespace cCvVideoWriter {
    VALUE rb_klass;

    VALUE
    rb_class()
    {
      return rb_klass;
    }

    /*
     * call-seq:
     *   CvVideoWriter.new(filname, fourcc, fps, size[, is_color]) -> cvvideowriter
     *   CvVideoWriter.new(filname, fourcc, fps, size[, is_color]){|vw| ... } -> nil
     *
     * Open new video writer. If block given, writer is closed automatically when end of block.
     * 
     * note: if <i>fourcc</i> is nil, popup codec select dialog (Windows only).
     */
    VALUE
    rb_initialize(int argc, VALUE *argv, VALUE self)
    {
      VALUE filename, fourcc, fps, size, is_color_val;
      rb_scan_args(argc, argv, "41", &filename, &fourcc, &fps, &size, &is_color_val);
      char codec[4] = {' ', ' ', ' ', ' '};
      int codec_number;
      Check_Type(filename, T_STRING);
      if (RSTRING_LEN(filename) == 0)
	rb_raise(rb_eArgError, "argument 1 (file name) dose not given");
      if (NIL_P(fourcc))
	codec_number = -1;
      else {
	Check_Type(fourcc, T_STRING);
	if (RSTRING_LEN(fourcc) > 4)
	  rb_raise(rb_eStandardError, "argument 2 (fourcc) should be specific 4-character. (i.e \"PIM1\",\"MJPG\")");
	else {
	  int len = RSTRING_LEN(fourcc);
	  for (int i = 0; i < len; ++i)
	    codec[i] = RSTRING_PTR(fourcc)[i];
	  codec_number = CV_FOURCC(codec[0], codec[1], codec[2], codec[3]);
	}
      }
      int is_color;
      if (NIL_P(is_color_val))
	is_color = 1;
      else
	is_color = (is_color_val == Qtrue) ? 1 : 0;
      try {
	DATA_PTR(self) = cvCreateVideoWriter(StringValueCStr(filename), codec_number,
					     NUM2DBL(fps), VALUE_TO_CVSIZE(size), is_color);
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      if (rb_block_given_p()) {
	rb_yield(self);
	rb_close(self);
	return Qnil;
      }
      else
	return self;
    }

    /*
     * call-seq:
     *   write(frame)
     *
     * Write image as frame of video stream.
     * <i>frame</i> should be IplImage
     */
    VALUE
    rb_write(VALUE self, VALUE frame)
    {
      try {
	cvWriteFrame(CVVIDEOWRITER(self), IPLIMAGE_WITH_CHECK(frame));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return self;
    }

    /*
     * Close vidoe writer.
     */
    VALUE
    rb_close(VALUE self)
    {
      CvVideoWriter *writer = CVVIDEOWRITER(self);
      try {
	if (writer)
	  cvReleaseVideoWriter(&writer);
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return Qnil;
    }

    void
    init_ruby_class()
    {
#if 0
      // For documentation using YARD
      VALUE opencv = rb_define_module("OpenCV");
#endif

      if (rb_klass)
	return;
      /* 
       * opencv = rb_define_module("OpenCV");
       * 
       * note: this comment is used by rdoc.
       */
      VALUE opencv = rb_module_opencv();
      rb_klass = rb_define_class_under(opencv, "CvVideoWriter", rb_cObject);
      rb_define_method(rb_klass, "initialize", RUBY_METHOD_FUNC(rb_initialize), -1);
      rb_define_method(rb_klass, "write", RUBY_METHOD_FUNC(rb_write), 1);
      rb_define_method(rb_klass, "close", RUBY_METHOD_FUNC(rb_close), 0);
    }
  }
}
