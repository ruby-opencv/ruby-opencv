/************************************************************

   mouseevent.h -

   $Author: lsxi $

   Copyright (C) 2005-2006 Masakazu Yonekura

************************************************************/
#ifndef RUBY_OPENCV_GUI_MOUSEEVENT_H
#define RUBY_OPENCV_GUI_MOUSEEVENT_H

#include <ruby.h>
#include "opencv2/core/core_c.h"
#include "opencv2/core/types_c.h"

namespace mOpenCV {
  namespace mGUI {
    namespace cMouseEvent {

      typedef struct MouseEvent {
	CvPoint point;
	int event;
	int flags;
      } MouseEvent;

      VALUE rb_class();
      void init_ruby_class();
      VALUE rb_allocate(VALUE klass);
      VALUE rb_event(VALUE self);
      VALUE rb_left_button_q(VALUE self);
      VALUE rb_right_button_q(VALUE self);
      VALUE rb_middle_button_q(VALUE self);
      VALUE rb_ctrl_key_q(VALUE self);
      VALUE rb_shift_key_q(VALUE self);
      VALUE rb_alt_key_q(VALUE self);
      VALUE new_object(int event, int x, int y, int flag);

      inline MouseEvent *MOUSEEVENT(VALUE object) {
	MouseEvent *ptr;
	Data_Get_Struct(object, MouseEvent, ptr);
	return ptr;
      }
    }
  }
}
#endif // RUBY_OPENCV_GUI_MOUSEEVENT_H

