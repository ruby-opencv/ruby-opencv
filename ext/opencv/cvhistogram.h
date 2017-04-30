/************************************************************

   cvhistogram.h -

   $Author: lsxi $

   Copyright (C) 2005-2008 Masakazu Yonekura

************************************************************/
#ifndef RUBY_OPENCV_CVHISTOGRAM_H
#define RUBY_OPENCV_CVHISTOGRAM_H

#include <ruby.h>
#include "opencv2/core/core_c.h"
#include "opencv2/core/types_c.h"

#include "cvutils.h"

namespace mOpenCV {
  namespace cCvHistogram {
    VALUE rb_class();
    VALUE rb_allocate(VALUE klass);
    void init_ruby_class();

    VALUE rb_initialize(int argc, VALUE *argv, VALUE self);
    VALUE rb_is_uniform(VALUE self);
    VALUE rb_is_sparse(VALUE self);
    VALUE rb_has_range(VALUE self);
    VALUE rb_calc_hist(int argc, VALUE* argv, VALUE self);
    VALUE rb_calc_hist_bang(int argc, VALUE* argv, VALUE self);
    VALUE rb_aref(VALUE self, VALUE args);
    VALUE rb_min_max_value(VALUE self);
    VALUE rb_dims(VALUE self);
    VALUE rb_copy_hist(VALUE self);

    VALUE rb_clear_hist(VALUE self);
    VALUE rb_clear_hist_bang(VALUE self);

    VALUE rb_normalize_hist(VALUE self, VALUE factor);
    VALUE rb_normalize_hist_bang(VALUE self, VALUE factor);
    VALUE rb_thresh_hist(VALUE self, VALUE threshold);
    VALUE rb_thresh_hist_bang(VALUE self, VALUE threshold);

    VALUE rb_set_hist_bin_ranges(int argc, VALUE* argv, VALUE self);
    VALUE rb_set_hist_bin_ranges_bang(int argc, VALUE* argv, VALUE self);

    VALUE rb_calc_back_project(VALUE self, VALUE image);
    VALUE rb_calc_back_project_patch(VALUE self, VALUE image, VALUE patch_size, VALUE method, VALUE factor);

    VALUE rb_compare_hist(VALUE self, VALUE hist1, VALUE hist2, VALUE method);
    VALUE rb_calc_prob_density(int argc, VALUE* argv, VALUE self);

  }

  inline CvHistogram*
    CVHISTOGRAM(VALUE object)
  {
    CvHistogram* ptr;
    Data_Get_Struct(object, CvHistogram, ptr);
    return ptr;
  }

  inline CvHistogram*
    CVHISTOGRAM_WITH_CHECK(VALUE object)
  {
    if (!rb_obj_is_kind_of(object, cCvHistogram::rb_class()))
      raise_typeerror(object, cCvHistogram::rb_class());
    return CVHISTOGRAM(object);
  }
}

#endif // RUBY_OPENCV_CVHISTOGRAM_H
