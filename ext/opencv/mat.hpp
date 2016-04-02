// -*- mode: c++; coding: utf-8 -*-
#ifndef RUBY_OPENCV_MAT_H
#define RUBY_OPENCV_MAT_H
// #include <iostream>

#include "ruby.h"
#include "opencv2/core.hpp"

namespace rubyopencv {
  namespace Mat {
    void init();
    VALUE rb_imread(VALUE self, VALUE filename, VALUE flags);
    VALUE rb_imdecode(VALUE self, VALUE buf, VALUE flags);
    VALUE rb_imwrite_internal(VALUE filename, VALUE img, VALUE params);
    VALUE rb_clone(VALUE self);
    VALUE rb_add_weighted(int argc, VALUE *argv, VALUE self);
    cv::Mat* obj2mat(VALUE obj);
    VALUE mat2obj(cv::Mat* ptr);
    VALUE mat2obj(cv::Mat* ptr, VALUE klass);
    cv::Mat* empty_mat();
    
    class RubyMatAllocator: public cv::MatAllocator {
    public:
      RubyMatAllocator() {}
      virtual ~RubyMatAllocator() {}

      cv::UMatData* allocate(int dims, const int* sizes, int type, void* data,
			     size_t* step, int flags, cv::UMatUsageFlags usageFlags) const {
	size_t total = CV_ELEM_SIZE(type);
	for (int i = dims - 1; i >= 0; i--) {
	  if (step) {
	    if (data && step[i] != CV_AUTOSTEP) {
	      total = step[i];
	    }
	    else {
	      step[i] = total;
	    }
	  }
	  total *= sizes[i];
	}

	cv::UMatData* u = new cv::UMatData(this);
	u->data = u->origdata = data ? (uchar*)data : (uchar*)xmalloc(total);
	u->size = total;
	if (data) {
	  u->flags |= cv::UMatData::USER_ALLOCATED;
	}

	return u;
      }

      bool allocate(cv::UMatData* u, int accessFlags, cv::UMatUsageFlags usageFlags) const {
	return (u) ? true : false;
      }

      void deallocate(cv::UMatData* u) const {
	// std::cout << "[DEBUG] deallocate" << std::endl;
	if (u == NULL) {
	  return;
	}

	if (u->refcount == 0) {
	  // std::cout << "[DEBUG] delete" << std::endl;
	  xfree(u->origdata);
	  delete u;
	}
      }
    };
  }
}

#endif // RUBY_OPENCV_MAT_H
