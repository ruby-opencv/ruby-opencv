/************************************************************

   mouseevent.cpp -

   $Author: lsxi $

   Copyright (C) 2005-2007 Masakazu Yonekura

************************************************************/
#include "mouseevent.h"
#include "gui.h"
#include "cvpoint.h"

/*
 * Document-module: OpenCV::GUI::MouseEvent
 *
 * MouseEvent object.
 */
namespace mOpenCV {
  namespace mGUI {
    namespace cMouseEvent {
      VALUE rb_klass;

      VALUE
      rb_class()
      {
	return rb_klass;
      }

      VALUE
      rb_allocate(VALUE klass)
      {
	MouseEvent *ptr;
	return Data_Make_Struct(klass, MouseEvent, 0, 0, ptr);
      }

      /*
       * call-seq:
       *   event -> symbol
       *
       * Return Symbol about mouse event.
       *
       * Currently, return these symbol:
       *   :move
       *     When mouse move.
       *   :right_button_down
       *     When mouse right button down.
       *   :left_button_down
       *     When mosue left button down.
       *   :middle_button_down
       *     When mosue middle button down.
       *   :left_button_up
       *     When mouse left button down.
       *   :right_button_up
       *     When mouse right button down.
       *   :middle_button_up
       *     When mouse middle button down.
       *
       * note: original OpenCV define "double-click" event(e.g. CV_EVENT_LBUTTONDBLCLK).
       * But never call these event. Is it bug?
       */
      VALUE rb_event(VALUE self)
      {
	switch(MOUSEEVENT(self)->event) {
	case CV_EVENT_MOUSEMOVE:
	  return ID2SYM(rb_intern("move"));
	case CV_EVENT_LBUTTONDOWN:
	  return ID2SYM(rb_intern("left_button_down"));
	case CV_EVENT_RBUTTONDOWN:
	  return ID2SYM(rb_intern("right_button_down"));
	case CV_EVENT_MBUTTONDOWN:
	  return ID2SYM(rb_intern("middle_button_down"));
	case CV_EVENT_LBUTTONUP:
	  return ID2SYM(rb_intern("left_button_up"));
	case CV_EVENT_RBUTTONUP:
	  return ID2SYM(rb_intern("right_button_up"));
	case CV_EVENT_MBUTTONUP:
	  return ID2SYM(rb_intern("middle_button_up"));
	case CV_EVENT_LBUTTONDBLCLK:
	  return ID2SYM(rb_intern("left_button_double_click"));
	case CV_EVENT_RBUTTONDBLCLK:
	  return ID2SYM(rb_intern("right_button_double_click"));
	case CV_EVENT_MBUTTONDBLCLK:
	  return ID2SYM(rb_intern("middle_button_double_click"));
	}
	return Qnil;
      }

      /*
       * Return true when mouse left button is pushed. Otherwise return false.
       */
      VALUE
      rb_left_button_q(VALUE self)
      {
	return MOUSEEVENT(self)->flags & CV_EVENT_FLAG_LBUTTON ? Qtrue : Qfalse;
      }

      /*
       * Return true when mouse right button is pushed. Otherwise return false.
       */
      VALUE
      rb_right_button_q(VALUE self)
      {
	return MOUSEEVENT(self)->flags & CV_EVENT_FLAG_RBUTTON ? Qtrue : Qfalse;
      }

      /*
       * Return true when mouse middle button is pushed. Otherwise return false.
       */
      VALUE
      rb_middle_button_q(VALUE self)
      {
	return MOUSEEVENT(self)->flags & CV_EVENT_FLAG_MBUTTON ? Qtrue : Qfalse;
      }

      /*
       * Return true when CTRL key is pushed. Otherwise return false.
       */
      VALUE
      rb_ctrl_key_q(VALUE self)
      {
	return MOUSEEVENT(self)->flags & CV_EVENT_FLAG_CTRLKEY ? Qtrue : Qfalse;
      }

      /*
       * Return true when shift key is pushed. Otherwise return false.
       */
      VALUE
      rb_shift_key_q(VALUE self)
      {
	return MOUSEEVENT(self)->flags & CV_EVENT_FLAG_SHIFTKEY ? Qtrue : Qfalse;
      }

      /*
       * Return true when ALT key is pushed. Otherwise return false.
       */
      VALUE
      rb_alt_key_q(VALUE self)
      {
	return MOUSEEVENT(self)->flags & CV_EVENT_FLAG_ALTKEY ? Qtrue : Qfalse;
      }

      VALUE
      new_object(int event, int x, int y, int flags)
      {
	VALUE object = rb_allocate(rb_class());
	MouseEvent *mouseevent = MOUSEEVENT(object);
	mouseevent->point.x = x;
	mouseevent->point.y = y;
	mouseevent->event = event;
	mouseevent->flags = flags;
	return object;
      }

      void
      init_ruby_class()
      {
#if 0
	// For documentation using YARD
	VALUE opencv = rb_define_module("OpenCV");
	VALUE GUI = rb_define_module_under(opencv, "GUI");
	VALUE cvpoint = rb_define_class_under(opencv, "CvPoint", rb_cObject);
#endif

	if (rb_klass)
	  return;
	/* 
	 * opencv = rb_define_module("OpenCV");
	 * GUI = rb_define_module_under(opencv, "GUI");
	 * cvpoint = rb_define_class_under(opencv, "CvPoint", rb_cObject);
	 */
	VALUE GUI = rb_module_GUI();
	VALUE cvpoint = cCvPoint::rb_class();
	rb_klass = rb_define_class_under(GUI, "MouseEvent", cvpoint);
	rb_define_alloc_func(rb_klass, rb_allocate);
	rb_define_method(rb_klass, "event", RUBY_METHOD_FUNC(rb_event), 0);
	rb_define_method(rb_klass, "left_button?", RUBY_METHOD_FUNC(rb_left_button_q), 0);
	rb_define_method(rb_klass, "right_button?", RUBY_METHOD_FUNC(rb_right_button_q), 0);
	rb_define_method(rb_klass, "middle_button?", RUBY_METHOD_FUNC(rb_middle_button_q), 0);
	rb_define_method(rb_klass, "ctrl_key?", RUBY_METHOD_FUNC(rb_ctrl_key_q), 0);
	rb_define_method(rb_klass, "shift_key?", RUBY_METHOD_FUNC(rb_shift_key_q), 0);
	rb_define_method(rb_klass, "alt_key?", RUBY_METHOD_FUNC(rb_alt_key_q), 0);
      }
    }
  }
}
