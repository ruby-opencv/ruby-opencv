#include "opencv2/highgui.hpp"
#include "window.hpp"
#include "trackbar.hpp"
#include "error.hpp"
#include "mat.hpp"

namespace rubyopencv {
  namespace Window {
    void mark_window(void *ptr);
    void free_window(void *ptr);

    VALUE rb_klass;
    rb_data_type_t opencv_window_type = {
      "Window",
      { mark_window, free_window, 0, 0 },
      0,
      0,
      0
    };

    void mark_window(void *ptr) {
      window_t* window_ptr = (window_t*)ptr;
      rb_gc_mark(window_ptr->name);
      rb_gc_mark(window_ptr->trackbars);
    }

    void free_window(void *ptr) {
      xfree(ptr);
    }

    VALUE rb_allocate(VALUE klass) {
      window_t* ptr = NULL;
      return TypedData_Make_Struct(klass, window_t, &opencv_window_type, ptr);
    }

    window_t* obj2window(VALUE obj) {
      window_t* ptr = NULL;
      TypedData_Get_Struct(obj, window_t, &opencv_window_type, ptr);
      return ptr;
    }

    inline char* GET_WINDOW_NAME(VALUE obj) {
      window_t* w = obj2window(obj);
      return StringValueCStr(w->name);
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
     * @opencv_func cv::namedWindow
     */
    VALUE rb_initialize(int argc, VALUE *argv, VALUE self) {
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

      window_t* self_ptr = obj2window(self);
      self_ptr->name = name;
      self_ptr->trackbars = rb_ary_new();

      try {
	cv::namedWindow(name_str, mode);
      }
      catch (cv::Exception& e) {
	Error::raise(e);
      }

      return self;
    }

    /*
     * Displays an image in the specified window.
     *
     * @overload show(image)
     *   @param image [Mat] Image to be shown.
     * @opencv_func cv::imshow
     */
    VALUE rb_show(VALUE self, VALUE img) {
      try {
	cv::Mat* m = Mat::obj2mat(img);
	cv::imshow(GET_WINDOW_NAME(self), *m);
      }
      catch (cv::Exception& e) {
	Error::raise(e);
      }

      return self;
    }

    VALUE rb_wait_key(int argc, VALUE* argv, VALUE self) {
      VALUE delay;
      rb_scan_args(argc, argv, "01", &delay);

      int ret = 0;
      int delay_value = NIL_P(delay) ? 0 : NUM2INT(delay);
      try {
	ret = cv::waitKey(delay_value);
      }
      catch (cv::Exception& e) {
	Error::raise(e);
      }

      return INT2NUM(ret);
    }

    /*
     * Destroys a window. alive status of window be false.
     */
    VALUE
    rb_destroy(VALUE self) {
      try {
	cv::destroyWindow(GET_WINDOW_NAME(self));
      }
      catch (cv::Exception& e) {
	Error::raise(e);
      }

      return self;
    }

    /*
     * Destorys all the windows.
     */
    VALUE rb_destroy_all(VALUE klass) {
      try {
	cv::destroyAllWindows();
      }
      catch (cv::Exception& e) {
	Error::raise(e);
      }

      return Qnil;
    }

    /*
     * Resizes window to the specified size.
     *
     * @overload resize(width, height)
     *   @param width [Integer] The new window width.
     *   @param height [Integer] The new window height.
     * @opencv_func cv::resizeWindow
     */
    VALUE rb_resize(VALUE self, VALUE width, VALUE height) {
      try {
	cv::resizeWindow(GET_WINDOW_NAME(self), NUM2INT(width), NUM2INT(height));
      }
      catch (cv::Exception& e) {
	Error::raise(e);
      }
      return self;
    }

    /*
     * Moves window to the specified position.
     *
     * @overload move(x, y)
     *   @param x [Integer] The new x-coordinate of the window.
     *   @param y [Integer] The new y-coordinate of the window.
     * @opencv_func cvMoveWindow
     */
    VALUE rb_move(VALUE self, VALUE x, VALUE y) {
      try {
	cv::moveWindow(GET_WINDOW_NAME(self), NUM2INT(x), NUM2INT(y));
      }
      catch (cv::Exception& e) {
	Error::raise(e);
      }
      return self;
    }

    void trackbar_callback(int value, void* block) {
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
    VALUE rb_set_trackbar(int argc, VALUE *argv, VALUE self) {
      VALUE trackbar;
      if (argc == 1) {
      	trackbar = argv[0];
      }
      else {
      	trackbar = Trackbar::newobj(argc, argv);
      }
      Trackbar::trackbar_t *trackbar_ptr = Trackbar::obj2trackbar(trackbar);
      try {
	cv::TrackbarCallback callback = NULL;
	void* block_ptr = NULL;
	if (!NIL_P(trackbar_ptr->block)) {
	  block_ptr = (void*)(trackbar_ptr->block);
	  callback = (cv::TrackbarCallback)trackbar_callback;
	}
      	cv::createTrackbar(trackbar_ptr->name, GET_WINDOW_NAME(self), &(trackbar_ptr->value),
      			   trackbar_ptr->maxval, callback, block_ptr);
      }
      catch (cv::Exception& e) {
      	Error::raise(e);
      }
      rb_ary_push(obj2window(self)->trackbars, trackbar);

      return trackbar;
    }

    void onMouse(int event, int x, int y, int flags, void* param) {
      rb_funcall((VALUE)param, rb_intern("call"), 4, INT2FIX(event), INT2NUM(x), INT2NUM(y), INT2FIX(flags));
    }

    /*
     * Sets mouse handler for the specified window.
     *
     * @overload set_mouse_callback { |event, x, y, flags| ... }
     */
    VALUE rb_set_mouse_callback(int argc, VALUE* argv, VALUE self) {
      window_t* selfptr = obj2window(self);
      VALUE block = Qnil;
      rb_scan_args(argc, argv, "0&", &block);
      try {
	if (rb_respond_to(block, rb_intern("call"))) {
	  cv::setMouseCallback(StringValueCStr(selfptr->name), onMouse, (void*)block);
	}
      }
      catch (cv::Exception& e) {
      	Error::raise(e);
      }

      return block;
    }

    void init() {
      VALUE opencv = rb_define_module("OpenCV");

      rb_klass = rb_define_class_under(opencv, "Window", rb_cData);
      rb_define_alloc_func(rb_klass, rb_allocate);
      rb_define_method(rb_klass, "initialize", RUBY_METHOD_FUNC(rb_initialize), -1);
      rb_define_method(rb_klass, "show", RUBY_METHOD_FUNC(rb_show), 1);
      rb_define_method(rb_klass, "destroy", RUBY_METHOD_FUNC(rb_destroy), 0);
      rb_define_singleton_method(rb_klass, "destroy_all", RUBY_METHOD_FUNC(rb_destroy_all), 0);
      rb_define_method(rb_klass, "resize", RUBY_METHOD_FUNC(rb_resize), 2);
      rb_define_method(rb_klass, "move", RUBY_METHOD_FUNC(rb_move), 2);
      rb_define_method(rb_klass, "set_trackbar", RUBY_METHOD_FUNC(rb_set_trackbar), -1);
      rb_define_method(rb_klass, "set_mouse_callback", RUBY_METHOD_FUNC(rb_set_mouse_callback), -1);
      rb_define_alias(rb_klass, "on_mouse", "set_mouse_callback");
    }
  }
}
