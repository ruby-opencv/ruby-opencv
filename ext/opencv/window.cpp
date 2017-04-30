/************************************************************

   window.cpp -

   $Author: lsxi $

   Copyright (C) 2005-2006 Masakazu Yonekura

************************************************************/
#include "opencv2/highgui/highgui_c.h"

#include "window.h"
#include "cvsize.h"
#include "cvpoint.h"
#include "trackbar.h"
#include "mouseevent.h"
#include "cvmat.h"

/*
 * Document-class: OpenCV::GUI::Window
 *
 * Simple Window wedget to show images(CvMat/IplImage).
 *
 * Sample:
 *   image = OpenCV::IplImage::load("opencv.bmp")     #=> load image
 *   window = OpenCV::GUI::Window.new("simple viewer")#=> create new window named "simaple viewer"
 *   window.show(image)                               #=> show image
 */
namespace mOpenCV {
  namespace mGUI {
    namespace cWindow {
      int num_windows = 0;
      VALUE rb_klass;

      VALUE
      rb_class()
      {
	return rb_klass;
      }

      VALUE
      rb_allocate(VALUE klass)
      {
	Window *ptr;
	return Data_Make_Struct(klass, Window, window_mark, window_free, ptr);
      }

      void
      window_mark(void *ptr)
      {
	Window* window_ptr = (Window*)ptr;
	rb_gc_mark(window_ptr->name);
	rb_gc_mark(window_ptr->image);
	rb_gc_mark(window_ptr->trackbars);
	rb_gc_mark(window_ptr->blocks);
      }

      void
      window_free(void *ptr)
      {
	free(ptr);
      }

      /*
       * Creates a window.
       *
       * @overload new(name, flags = CV_WINDOW_AUTOSIZE)
       *   @param name [String] Name of the window in the window caption that may be used as a window identifier.
       *   @param flags [Integer] Flags of the window. The supported flags are:
       *     * CV_WINDOW_AUTOSIZE - If this is set, the window size is automatically adjusted
       *       to fit the displayed image, and you cannot change the window size manually.
       *     * CV_WINDOW_NORMAL - If this is set, the user can resize the window (no constraint).
       *     * CV_WINDOW_OPENGL - If this is set, the window will be created with OpenGL support.
       * @opencv_func cvNamedWindow
       */
      VALUE
      rb_initialize(int argc, VALUE *argv, VALUE self)
      {
	VALUE name, flags;
	rb_scan_args(argc, argv, "11", &name, &flags);
	Check_Type(name, T_STRING);
	char* name_str = StringValueCStr(name);
	if (cvGetWindowHandle(name_str) != NULL) {
	  rb_raise(rb_eStandardError, "window name should be unique.");
	}      
  
	int mode = CV_WINDOW_AUTOSIZE;
	if (argc == 2) {
	  Check_Type(flags, T_FIXNUM);
	  mode = FIX2INT(flags);
	}

	Window* self_ptr = WINDOW(self);
	self_ptr->name = name;
	self_ptr->trackbars = rb_ary_new();
	self_ptr->blocks = rb_ary_new();
	try {
	  cvNamedWindow(name_str, mode);
	}
	catch (cv::Exception& e) {
	  raise_cverror(e);
	}
	num_windows++;
	return self;
      }

      /*
       * Return alive status of window. Return true if alive, otherwise return false.
       */
      VALUE
      rb_alive_q(VALUE self)
      {
	const char* name_str = GET_WINDOW_NAME(self);
	return (cvGetWindowHandle(name_str) == NULL) ? Qfalse : Qtrue;
      }

      /*
       * Destroys a window. alive status of window be false.
       */
      VALUE
      rb_destroy(VALUE self)
      {
	const char* name_str = GET_WINDOW_NAME(self);
	try {
	  cvDestroyWindow(name_str);
	}
	catch (cv::Exception& e) {
	  raise_cverror(e);
	}
	num_windows--;
	return self;
      }

      /*
       * Destorys all the windows.
       */
      VALUE
      rb_destroy_all(VALUE klass)
      {
	if (num_windows > 0) {
	  try {
	    cvDestroyAllWindows();
	  }
	  catch (cv::Exception& e) {
	    raise_cverror(e);
	  }
	  num_windows = 0;
	}
	return Qnil;
      }

      /*
       * Resizes window to the specified size.
       *
       * @overload resize(size)
       *   @param size [CvSize] The new window size.
       * @overload resize(width, height)
       *   @param width [Integer] The new window width.
       *   @param height [Integer] The new window height.
       * @opencv_func cvResizeWindow
       */
      VALUE
      rb_resize(int argc, VALUE *argv, VALUE self)
      {
	int width = 0;
	int height = 0;
	switch (argc) {
	case 1: {
	  CvSize size = VALUE_TO_CVSIZE(argv[0]);
	  width = size.width;
	  height = size.height;
	  break;
	}
	case 2:
	  width = NUM2INT(argv[0]);
	  height = NUM2INT(argv[1]);
	  break;
	default:
	  rb_raise(rb_eArgError, "wrong number of arguments (1 or 2)");
	  break;
	}
	try {
	  cvResizeWindow(GET_WINDOW_NAME(self), width, height);
	}
	catch (cv::Exception& e) {
	  raise_cverror(e);
	}
	return self;
      }

      /*
       * Moves window to the specified position.
       *
       * @overload move(point)
       *   @param point [CvPoint] The new coordinate of the window.
       * @overload move(x, y)
       *   @param x [Integer] The new x-coordinate of the window.
       *   @param y [Integer] The new y-coordinate of the window.
       * @opencv_func cvMoveWindow
       */
      VALUE
      rb_move(int argc, VALUE *argv, VALUE self)
      {
	int x = 0;
	int y = 0;
	switch (argc) {
	case 1: {
	  CvPoint point = VALUE_TO_CVPOINT(argv[0]);
	  x = point.x;
	  y = point.y;
	  break;
	}
	case 2:
	  x = NUM2INT(argv[0]);
	  y = NUM2INT(argv[1]);
	  break;
	default:
	  rb_raise(rb_eArgError, "wrong number of arguments (1 or 2)");
	  break;
	}
	try {
	  cvMoveWindow(GET_WINDOW_NAME(self), x, y);
	}
	catch (cv::Exception& e) {
	  raise_cverror(e);
	}
	return self;
      }
      
      /*
       * Displays an image in the specified window.
       *
       * @overload show_image(image)
       *   @param image [CvMat] Image to be shown.
       * @opencv_func cvShowImage
       */
      VALUE
      rb_show_image(VALUE self, VALUE img)
      {
	CvArr* image = CVARR_WITH_CHECK(img);
	WINDOW(self)->image = img;
	try {
	  cvShowImage(GET_WINDOW_NAME(self), image);
	}
	catch (cv::Exception& e) {
	  raise_cverror(e);
	}
	return self;
      }

      void
      trackbar_callback(int value, void* block)
      {
	rb_funcall((VALUE)block, rb_intern("call"), 1, INT2NUM(value));
      }

      /*
       * Creates or sets a trackbar and attaches it to the specified window.
       *
       * @overload set_trackbar(trackbar)
       *   @param trackbar [TrackBar] The trackbar to set.
       *
       * @overload set_trackbar(name, count, value = nil) { |value| ... }
       *   @param name [String] Name of the created trackbar.
       *   @param count [Integer] Maximal position of the slider. The minimal position is always 0.
       *   @param value [Integer] Optional value to an integer variable whose value reflects the position of the slider.
       *     Upon creation, the slider position is defined by this variable.
       *   @yield [value] Function to be called every time the slider changes position.
       *   @yieldparam value [Integer] The trackbar position.
       * @opencv_func cv::createTrackbar
       */
      VALUE
      rb_set_trackbar(int argc, VALUE *argv, VALUE self)
      {
	VALUE trackbar;
	if (argc == 1) {
	  trackbar = argv[0];
	}
	else {
	  trackbar = cTrackbar::rb_initialize(argc, argv, cTrackbar::rb_allocate(cTrackbar::rb_class()));
	}
	Trackbar *trackbar_ptr = TRACKBAR_WITH_CHECK(trackbar);
	try {
	  cv::createTrackbar(trackbar_ptr->name, GET_WINDOW_NAME(self), &(trackbar_ptr->val), trackbar_ptr->maxval,
			     (cv::TrackbarCallback)trackbar_callback, (void*)(trackbar_ptr->block));
	}
	catch (cv::Exception& e) {
	  raise_cverror(e);
	}
	rb_ary_push(WINDOW(self)->trackbars, trackbar);
  
	return trackbar;
      }

      void
      on_mouse(int event, int x, int y, int flags, void* param)
      {
	VALUE block = (VALUE)param;
	if (rb_obj_is_kind_of(block, rb_cProc)) {
	  rb_funcall(block, rb_intern("call"), 1, cMouseEvent::new_object(event, x, y, flags));
	}
      }

      /*
       * Sets mouse handler for the specified window.
       *
       * @overload set_mouse_callback { |mouse_event| ... }
       *   @yield [mouse_event] Mouse callback.
       *   @yieldparam mouse_event [MouseEvent] Mouse event
       *
       * @example display mouse event on console
       *   window = OpenCV::GUI::Window.new "sample window"
       *   image = OpenCV::IplImage::load "sample.png"
       *   window.show(image)
       *   window.set_mouse_callback {|mouse|  
       *     e = "#{mouse.x}, #{mouse.y} : #{mouse.event} : "
       *     e << "<L>" if mouse.left_button?
       *     e << "<R>" if mouse.right_button?
       *     e << "<M>" if mouse.middle_button?
       *     e << "[CTRL]" if mouse.ctrl_key?
       *     e << "[SHIFT]" if mouse.shift_key?
       *     e << "[ALT]" if mouse.alt_key?
       *     puts e
       *   }
       *   OpenCV::GUI::wait_key
       */
      VALUE
      rb_set_mouse_callback(int argc, VALUE* argv, VALUE self)
      {
	if (!rb_block_given_p()) {
	  rb_raise(rb_eArgError, "block not given.");
	}

	VALUE block = Qnil;
	rb_scan_args(argc, argv, "0&", &block);
	try {
	  cvSetMouseCallback(GET_WINDOW_NAME(self), on_mouse, (void*)block);
	}
	catch (cv::Exception& e) {
	  raise_cverror(e);
	}

	rb_ary_push(WINDOW(self)->blocks, block);
	return block;
      }

      void
      init_ruby_class()
      {
#if 0
	// For documentation using YARD
	VALUE opencv = rb_define_module("OpenCV");
	VALUE GUI = rb_define_module_under(opencv, "GUI");
#endif

	if (rb_klass)
	  return;
	/* 
	 * opencv = rb_define_module("OpenCV");
	 * GUI = rb_define_module_under(opencv, "GUI");
	 *
	 * note: this comment is used by rdoc.
	 */
	VALUE GUI = rb_module_GUI();
	rb_klass = rb_define_class_under(GUI, "Window", rb_cObject);
	rb_define_alloc_func(rb_klass, rb_allocate);
	rb_define_method(rb_klass, "initialize", RUBY_METHOD_FUNC(rb_initialize), -1);
	rb_define_method(rb_klass, "alive?", RUBY_METHOD_FUNC(rb_alive_q), 0);
	rb_define_method(rb_klass, "destroy", RUBY_METHOD_FUNC(rb_destroy), 0);
	rb_define_singleton_method(rb_klass, "destroy_all", RUBY_METHOD_FUNC(rb_destroy_all), 0);
	rb_define_method(rb_klass, "resize", RUBY_METHOD_FUNC(rb_resize), -1);
	rb_define_method(rb_klass, "move", RUBY_METHOD_FUNC(rb_move), -1);
	rb_define_method(rb_klass, "show_image", RUBY_METHOD_FUNC(rb_show_image), 1);
	rb_define_alias(rb_klass, "show", "show_image");
	rb_define_method(rb_klass, "set_trackbar", RUBY_METHOD_FUNC(rb_set_trackbar), -1);
	rb_define_method(rb_klass, "set_mouse_callback", RUBY_METHOD_FUNC(rb_set_mouse_callback), -1);
	rb_define_alias(rb_klass, "on_mouse", "set_mouse_callback");
      }
    }
  }
}
