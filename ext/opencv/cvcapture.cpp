/************************************************************

   cvcapture.cpp -

   $Author: lsxi $

   Copyright (C) 2005-2006 Masakazu Yonekura

************************************************************/
#include "cvcapture.h"
#include "iplimage.h"
#include "cvsize.h"

/*
 * Document-class: OpenCV::CvCapture
 *
 * Class for video capturing from video files or cameras
 */
namespace mOpenCV {
  namespace cCvCapture {
    VALUE rb_klass;

    VALUE
    rb_class()
    {
      return rb_klass;
    }

    void
    cvcapture_free(void *ptr)
    { 
      if (ptr) {
	sCvCapture* scap = (sCvCapture*)ptr;
	if (scap->opened)
	  cvReleaseCapture(&scap->ptr);
	delete scap;
      }
    }

    /*
     * Open video file or a capturing device for video capturing
     * @scope class
     * @overload open(dev = nil)
     *   @param dev [String,Integer,Simbol,nil] Video capturing device
     *     * If dev is a string (i.e "stream.avi"), reads video stream from a file.
     *     * If dev is a number or symbol (included in CvCapture::INTERFACE), reads video stream from a device.
     *     * If dev is a nil, same as CvCapture.open(:any)
     * @return [CvCapture] Opened CvCapture instance
     * @opencv_func cvCaptureFromCAM
     * @opencv_func cvCaptureFromFile
     */
    VALUE
    rb_open(int argc, VALUE *argv, VALUE self)
    {
      VALUE device;
      rb_scan_args(argc, argv, "01", &device);
      CvCapture *capture = 0;
      sCvCapture *scap = new sCvCapture();
      try {
	switch (TYPE(device)) {
	case T_STRING:
	  capture = cvCaptureFromFile(StringValueCStr(device));
	  break;
	case T_FIXNUM:
	  capture = cvCaptureFromCAM(FIX2INT(device));
	  break;
	case T_SYMBOL: {
	  VALUE cap_index = rb_hash_lookup(rb_const_get(rb_class(), rb_intern("INTERFACE")), device);
	  if (NIL_P(cap_index))
	    rb_raise(rb_eArgError, "undefined interface.");
	  capture = cvCaptureFromCAM(NUM2INT(cap_index));
	  break;
	}
	case T_NIL:
	  capture = cvCaptureFromCAM(CV_CAP_ANY);
	  break;
	}
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      if (!capture)
	rb_raise(rb_eStandardError, "Invalid capture format.");
      scap->ptr = capture;
      scap->opened = true;
      return Data_Wrap_Struct(rb_klass, 0, cvcapture_free, scap);
    }

    /*
     * Releases an opened video file or a capturing device
     * @return [boolean] False if the device was already closed
     */

    VALUE
    rb_close(VALUE self)
    {
      sCvCapture *scap;
      Data_Get_Struct(self, sCvCapture, scap);
      if (scap->opened) {
	scap->opened = false;
	cvReleaseCapture(&scap->ptr);
	return true;
      } else
	return false;
    }

    /*
     * Grabs the next frame from video file or capturing device.
     * @overload grab
     * @return [Boolean] If grabbing a frame successed, returns true, otherwise returns false.
     * @opencv_func cvGrabFrame
     */
    VALUE
    rb_grab(VALUE self)
    {
      int grab = 0;
      try {
	grab = cvGrabFrame(CVCAPTURE(self));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return grab ? Qtrue : Qfalse;
    }

    /*
     * Decodes and returns the grabbed video frame.
     * @overload retrieve
     * @return [IplImage] Grabbed video frame
     * @return [nil] Failed to grabbing a frame
     * @opencv_func cvRetrieveFrame
     */
    VALUE
    rb_retrieve(VALUE self)
    {
      VALUE image = Qnil;
      IplImage *frame = NULL;
      try {
	if (!(frame = cvRetrieveFrame(CVCAPTURE(self)))) {
	  return Qnil;
	}
	image = cIplImage::new_object(frame->width, frame->height,
				      CV_MAKETYPE(IPL2CV_DEPTH(frame->depth), frame->nChannels));
	if (frame->origin == IPL_ORIGIN_TL) {
	  cvCopy(frame, CVARR(image));
	}
	else {
	  cvFlip(frame, CVARR(image));
	}
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return image;

    }

    /*
     * Grabs, decodes and returns the next video frame.
     * @overload query
     * @return [IplImage] Next video frame
     * @return [nil] Failed to read next video frame
     * @opencv_func cvQueryFrame
     */
    VALUE
    rb_query(VALUE self)
    {
      VALUE image = Qnil;
      IplImage *frame = NULL;
      try {
	if (!(frame = cvQueryFrame(CVCAPTURE(self)))) {
	  return Qnil;
	}
	image = cIplImage::new_object(frame->width, frame->height,
				      CV_MAKETYPE(IPL2CV_DEPTH(frame->depth), frame->nChannels));
	if (frame->origin == IPL_ORIGIN_TL) {
	  cvCopy(frame, CVARR(image));
	}
	else {
	  cvFlip(frame, CVARR(image));
	}
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return image;
    }

    VALUE
    rb_get_capture_property(VALUE self, int id)
    {
      double result = 0;
      try {
	result = cvGetCaptureProperty(CVCAPTURE(self), id);
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return rb_float_new(result);
    }

    VALUE
    rb_set_capture_property(VALUE self, int id, VALUE value)
    {
      double result = 0;
      try {
	result = cvSetCaptureProperty(CVCAPTURE(self), id, NUM2DBL(value));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return rb_float_new(result);
    }

    /*
     * Get film current position in milliseconds or video capture timestamp.
     * @overload millisecond
     * @return [Number] Current position of the video file in milliseconds or video capture timestamp
     * @opencv_func cvGetCaptureProperty (propId=CV_CAP_PROP_POS_MSEC)
     */
    VALUE
    rb_get_millisecond(VALUE self)
    {
      return rb_get_capture_property(self, CV_CAP_PROP_POS_MSEC);
    }

    /*
     * Set film current position in milliseconds or video capture timestamp.
     * @overload millisecond=value
     *   @param value [Number] Position in milliseconds or video capture timestamp.
     * @return [Number]
     * @opencv_func cvSetCaptureProperty (propId=CV_CAP_PROP_POS_MSEC)
     */
    VALUE
    rb_set_millisecond(VALUE self, VALUE value)
    {
      return rb_set_capture_property(self, CV_CAP_PROP_POS_MSEC, value);
    }

    /*
     * Get 0-based index of the frame to be decoded/captured next
     * @overload frames
     * @return [Number] 0-based index of the frame to be decoded/captured next
     * @opencv_func cvGetCaptureProperty (propId=CV_CAP_PROP_POS_FRAMES)
     */
    VALUE
    rb_get_frames(VALUE self)
    {
      return rb_get_capture_property(self, CV_CAP_PROP_POS_FRAMES);
    }

    /*
     * Set 0-based index of the frame to be decoded/captured next
     * @overload frames=value
     *   @param value [Number] 0-based index of the frame to be decoded/captured next
     * @return [Number]
     * @opencv_func cvSetCaptureProperty (propId=CV_CAP_PROP_POS_FRAMES)
     */
    VALUE
    rb_set_frames(VALUE self, VALUE value)
    {
      return rb_set_capture_property(self, CV_CAP_PROP_POS_FRAMES, value);
    }
    /*
     * Get relative position of video file
     * @overload avi_ratio
     * @return [Number] Relative position of video file (0: Start of the film, 1: End of the film)
     * @opencv_func cvGetCaptureProperty (propId=CV_CAP_PROP_POS_AVI_RATIO)
     */
    VALUE
    rb_get_avi_ratio(VALUE self)
    {
      return rb_get_capture_property(self, CV_CAP_PROP_POS_AVI_RATIO);
    }
    /*
     * Set relative position of video file
     * @overload avi_ratio=value
     *   @param value [Number] Relative position of video file (0: Start of the film, 1: End of the film)
     * @return [Number]
     * @opencv_func cvSetCaptureProperty (propId=CV_CAP_PROP_POS_AVI_RATIO)
     */
    VALUE
    rb_set_avi_ratio(VALUE self, VALUE value)
    {
      return rb_set_capture_property(self, CV_CAP_PROP_POS_AVI_RATIO, value);
    }

    /*
     * Get size of frames in the video stream.
     * @overload size
     * @return [Size] Size of frames in the video stream.
     * @opencv_func cvGetCaptureProperty (propId=CV_CAP_PROP_FRAME_WIDTH,CV_CAP_PROP_FRAME_HEIGHT)
     */
    VALUE
    rb_get_size(VALUE self)
    {
      CvSize size;
      try {
	CvCapture* self_ptr = CVCAPTURE(self);
	size = cvSize((int)cvGetCaptureProperty(self_ptr, CV_CAP_PROP_FRAME_WIDTH),
		      (int)cvGetCaptureProperty(self_ptr, CV_CAP_PROP_FRAME_HEIGHT));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return cCvSize::new_object(size);
    }

    /*
     * Set size of frames in the video stream.
     * @overload size=value
     *   @param value [CvSize] Size of frames
     * @return [Number]
     * @opencv_func cvSetCaptureProperty (propId=CV_CAP_PROP_FRAME_WIDTH,CV_CAP_PROP_FRAME_HEIGHT)
     */
    VALUE
    rb_set_size(VALUE self, VALUE value)
    {
      double result = 0;
      CvSize size = VALUE_TO_CVSIZE(value);
      try {
	CvCapture* self_ptr = CVCAPTURE(self);
	cvSetCaptureProperty(self_ptr, CV_CAP_PROP_FRAME_WIDTH, size.width);
	result = cvSetCaptureProperty(self_ptr, CV_CAP_PROP_FRAME_HEIGHT, size.height);
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return DBL2NUM(result);
    }

    /*
     * Get width of frames in the video stream.
     * @overload width
     * @return [Number] Width of frames in the video stream.
     * @opencv_func cvGetCaptureProperty (propId=CV_CAP_FRAME_WIDTH)
     */
    VALUE
    rb_get_width(VALUE self)
    {
      return rb_get_capture_property(self, CV_CAP_PROP_FRAME_WIDTH);
    }

    /*
     * Set width of frames in the video stream.
     * @overload width=value
     *   @param value [Number] Width of frames
     * @return [Number]
     * @opencv_func cvSetCaptureProperty (propId=CV_CAP_PROP_FRAME_WIDTH)
     */
    VALUE
    rb_set_width(VALUE self, VALUE value)
    {
      return rb_set_capture_property(self, CV_CAP_PROP_FRAME_WIDTH, value);
    }

    /*
     * Get height of frames in the video stream.
     * @overload height
     * @return [Number] Height of frames in the video stream.
     * @opencv_func cvGetCaptureProperty (propId=CV_CAP_PROP_FRAME_HEIGHT)
     */
    VALUE
    rb_get_height(VALUE self)
    {
      return rb_get_capture_property(self, CV_CAP_PROP_FRAME_HEIGHT);
    }

    /*
     * Set height of frames in the video stream.
     * @overload height=value
     *   @param value [Number] Height of frames
     * @return [Number]
     * @opencv_func cvSetCaptureProperty (propId=CV_CAP_PROP_FRAME_HEIGHT)
     */
    VALUE
    rb_set_height(VALUE self, VALUE value)
    {
      return rb_set_capture_property(self, CV_CAP_PROP_FRAME_HEIGHT, value);
    }

    /*
     * Get frame rate
     * @overload fps
     * @return [Number] Frame rate
     * @opencv_func cvGetCaptureProperty (propId=CV_CAP_PROP_FPS)
     */
    VALUE
    rb_get_fps(VALUE self)
    {
      return rb_get_capture_property(self, CV_CAP_PROP_FPS);
    }

    /*
     * Set frame rate
     * @overload fps=value
     *   @param value [Number] Frame rate
     * @return [Number]
     * @opencv_func cvSetCaptureProperty (propId=CV_CAP_PROP_FPS)
     */
    VALUE
    rb_set_fps(VALUE self, VALUE value)
    {
      return rb_set_capture_property(self, CV_CAP_PROP_FPS, value);
    }

    /*
     * Get 4 character code of codec. see http://www.fourcc.org/
     * @overload fourcc
     * @return [Number] Codec code
     * @opencv_func cvGetCaptureProperty (propId=CV_CAP_PROP_FOURCC)
     */
    VALUE
    rb_get_fourcc(VALUE self)
    {
      char str[4];
      double fourcc = cvGetCaptureProperty(CVCAPTURE(self), CV_CAP_PROP_FOURCC);
      sprintf(str, "%s", (char*)&fourcc);
      return rb_str_new2(str);
    }

    /*
     * Get number of frames in video file.
     * @overload frame_count
     * @return [Number] Number of frames
     * @opencv_func cvGetCaptureProperty (propId=CV_CAP_PROP_FRAME_COUNT)
     */
    VALUE
    rb_get_frame_count(VALUE self)
    {
      return rb_get_capture_property(self, CV_CAP_PROP_FRAME_COUNT);
    }

    /*
     * Get format of images returned by CvCapture#retrieve
     * @overload format
     * @return [Number] format
     * @opencv_func cvGetCaptureProperty (propId=CV_CAP_PROP_FORMAT)
     */
    VALUE
    rb_get_format(VALUE self)
    {
      return rb_get_capture_property(self, CV_CAP_PROP_FORMAT);
    }

    /*
     * Get a backend-specific value indicating the current capture mode
     * @overload mode
     * @return [Number] Backend-specific value indicating the current capture mode
     * @opencv_func cvGetCaptureProperty (propId=CV_CAP_PROP_MODE)
     */
    VALUE
    rb_get_mode(VALUE self)
    {
      return rb_get_capture_property(self, CV_CAP_PROP_MODE);
    }

    /*
     * Get brightness of the image (only for cameras)
     * @overload brightness
     * @return [Number] Brightness
     * @opencv_func cvGetCaptureProperty (propId=CV_CAP_PROP_BRIGHTNESS)
     */
    VALUE
    rb_get_brightness(VALUE self)
    {
      return rb_get_capture_property(self, CV_CAP_PROP_BRIGHTNESS);
    }

    /*
     * Get contrast of the image (only for cameras)
     * @overload contrast
     * @return [Number] Contrast
     * @opencv_func cvGetCaptureProperty (propId=CV_CAP_PROP_CONTRAST)
     */
    VALUE
    rb_get_contrast(VALUE self)
    {
      return rb_get_capture_property(self, CV_CAP_PROP_CONTRAST);
    }

    /*
     * Get saturation of the image (only for cameras)
     * @overload saturation
     * @return [Number] Saturation
     * @opencv_func cvGetCaptureProperty (propId=CV_CAP_PROP_SATURATION)
     */
    VALUE
    rb_get_saturation(VALUE self)
    {
      return rb_get_capture_property(self, CV_CAP_PROP_SATURATION);
    }
    /*
     * Get hue of the image (only for cameras)
     * @overload hue
     * @return [Number] Hue
     * @opencv_func cvGetCaptureProperty (propId=CV_CAP_PROP_HUE)
     */
    VALUE
    rb_get_hue(VALUE self)
    {
      return rb_get_capture_property(self, CV_CAP_PROP_HUE);
    }

    /*
     * Get gain of the image (only for cameras)
     * @overload gain
     * @return [Number] Gain
     * @opencv_func cvGetCaptureProperty (propId=CV_CAP_PROP_GAIN)
     */
    VALUE
    rb_get_gain(VALUE self)
    {
      return rb_get_capture_property(self, CV_CAP_PROP_GAIN);
    }

    /*
     * Get exposure (only for cameras)
     * @overload exposure
     * @return [Number] Exposure
     * @opencv_func cvGetCaptureProperty (propId=CV_CAP_PROP_EXPOSURE)
     */
    VALUE
    rb_get_exposure(VALUE self)
    {
      return rb_get_capture_property(self, CV_CAP_PROP_EXPOSURE);
    }

    /*
     * Get boolean flags indicating whether images should be converted to RGB
     * @overload convert_rgb
     * @return [Boolean] Whether images should be converted to RGB
     * @opencv_func cvGetCaptureProperty (propId=CV_CAP_PROP_CONVERT_RGB)
     */
    VALUE
    rb_get_convert_rgb(VALUE self)
    {
      int flag = 0;
      try {
	flag = (int)cvGetCaptureProperty(CVCAPTURE(self), CV_CAP_PROP_CONVERT_RGB);
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return flag ? Qtrue : Qfalse;
    }

    /*
     * Get rectification flag for stereo cameras (note: only supported by DC1394 v 2.x backend currently)
     * @overload rectification
     * @return [Number] Rectification flag
     * @opencv_func cvGetCaptureProperty (propId=CV_CAP_PROP_RECTIFICATION)
     */
    VALUE
    rb_get_rectification(VALUE self)
    {
      return rb_get_capture_property(self, CV_CAP_PROP_RECTIFICATION);
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

      VALUE opencv = rb_module_opencv();
  
      rb_klass = rb_define_class_under(opencv, "CvCapture", rb_cData);
  
      VALUE video_interface = rb_hash_new();
      /*
       * :any, :mil, :vfw, :v4l, :v4l2, :fireware, :ieee1394, :dc1394, :cmu1394,
       * :stereo, :tyzx, :tyzx_left, :tyzx_right, :tyzx_color, :tyzx_z, :qt, :qtuicktime
       */
      rb_define_const(rb_klass, "INTERFACE", video_interface);
      rb_hash_aset(video_interface, ID2SYM(rb_intern("any")), INT2FIX(CV_CAP_ANY));
      rb_hash_aset(video_interface, ID2SYM(rb_intern("mil")), INT2FIX(CV_CAP_MIL));
      rb_hash_aset(video_interface, ID2SYM(rb_intern("vfw")), INT2FIX(CV_CAP_VFW));
      rb_hash_aset(video_interface, ID2SYM(rb_intern("v4l")), INT2FIX(CV_CAP_V4L));
      rb_hash_aset(video_interface, ID2SYM(rb_intern("v4l2")), INT2FIX(CV_CAP_V4L2));
      rb_hash_aset(video_interface, ID2SYM(rb_intern("fireware")), INT2FIX(CV_CAP_FIREWARE));
      rb_hash_aset(video_interface, ID2SYM(rb_intern("ieee1394")), INT2FIX(CV_CAP_IEEE1394));
      rb_hash_aset(video_interface, ID2SYM(rb_intern("dc1394")), INT2FIX(CV_CAP_DC1394));
      rb_hash_aset(video_interface, ID2SYM(rb_intern("cmu1394")), INT2FIX(CV_CAP_CMU1394));
      rb_hash_aset(video_interface, ID2SYM(rb_intern("stereo")), INT2FIX(CV_CAP_STEREO));
      rb_hash_aset(video_interface, ID2SYM(rb_intern("tyzx")), INT2FIX(CV_CAP_TYZX));
      rb_hash_aset(video_interface, ID2SYM(rb_intern("tyzx_left")), INT2FIX(CV_TYZX_LEFT));
      rb_hash_aset(video_interface, ID2SYM(rb_intern("tyzx_right")), INT2FIX(CV_TYZX_RIGHT));
      rb_hash_aset(video_interface, ID2SYM(rb_intern("tyzx_color")), INT2FIX(CV_TYZX_COLOR));
      rb_hash_aset(video_interface, ID2SYM(rb_intern("tyzx_z")), INT2FIX(CV_TYZX_Z));
      rb_hash_aset(video_interface, ID2SYM(rb_intern("qt")), INT2FIX(CV_CAP_QT));
      rb_hash_aset(video_interface, ID2SYM(rb_intern("quicktime")), INT2FIX(CV_CAP_QT));
  
      rb_define_singleton_method(rb_klass, "open", RUBY_METHOD_FUNC(rb_open), -1);
      rb_define_method(rb_klass, "close", RUBY_METHOD_FUNC(rb_close), 0);
  
      rb_define_method(rb_klass, "grab", RUBY_METHOD_FUNC(rb_grab), 0);
      rb_define_method(rb_klass, "retrieve", RUBY_METHOD_FUNC(rb_retrieve), 0);
      rb_define_method(rb_klass, "query", RUBY_METHOD_FUNC(rb_query), 0);
      rb_define_method(rb_klass, "millisecond", RUBY_METHOD_FUNC(rb_get_millisecond), 0);
      rb_define_method(rb_klass, "millisecond=", RUBY_METHOD_FUNC(rb_set_millisecond), 1);
      rb_define_method(rb_klass, "frames", RUBY_METHOD_FUNC(rb_get_frames), 0);
      rb_define_method(rb_klass, "frames=", RUBY_METHOD_FUNC(rb_set_frames), 1);
      rb_define_method(rb_klass, "avi_ratio", RUBY_METHOD_FUNC(rb_get_avi_ratio), 0);
      rb_define_method(rb_klass, "avi_ratio=", RUBY_METHOD_FUNC(rb_set_avi_ratio), 1);
      rb_define_method(rb_klass, "size", RUBY_METHOD_FUNC(rb_get_size), 0);
      rb_define_method(rb_klass, "size=", RUBY_METHOD_FUNC(rb_set_size), 1);
      rb_define_method(rb_klass, "width", RUBY_METHOD_FUNC(rb_get_width), 0);
      rb_define_method(rb_klass, "width=", RUBY_METHOD_FUNC(rb_set_width), 1);
      rb_define_method(rb_klass, "height", RUBY_METHOD_FUNC(rb_get_height), 0);
      rb_define_method(rb_klass, "height=", RUBY_METHOD_FUNC(rb_set_height), 1);
      rb_define_method(rb_klass, "fps", RUBY_METHOD_FUNC(rb_get_fps), 0);
      rb_define_method(rb_klass, "fps=", RUBY_METHOD_FUNC(rb_set_fps), 1);
      rb_define_method(rb_klass, "fourcc", RUBY_METHOD_FUNC(rb_get_fourcc), 0);
      rb_define_method(rb_klass, "frame_count", RUBY_METHOD_FUNC(rb_get_frame_count), 0);
      rb_define_method(rb_klass, "format", RUBY_METHOD_FUNC(rb_get_format), 0);
      rb_define_method(rb_klass, "mode", RUBY_METHOD_FUNC(rb_get_mode), 0);
      rb_define_method(rb_klass, "brightness", RUBY_METHOD_FUNC(rb_get_brightness), 0);
      rb_define_method(rb_klass, "contrast", RUBY_METHOD_FUNC(rb_get_contrast), 0);
      rb_define_method(rb_klass, "saturation", RUBY_METHOD_FUNC(rb_get_saturation), 0);
      rb_define_method(rb_klass, "hue", RUBY_METHOD_FUNC(rb_get_hue), 0);
      rb_define_method(rb_klass, "gain", RUBY_METHOD_FUNC(rb_get_gain), 0);
      rb_define_method(rb_klass, "exposure", RUBY_METHOD_FUNC(rb_get_exposure), 0);
      rb_define_method(rb_klass, "convert_rgb", RUBY_METHOD_FUNC(rb_get_convert_rgb), 0);
      rb_define_method(rb_klass, "rectification", RUBY_METHOD_FUNC(rb_get_rectification), 0);
    }
  }
}
