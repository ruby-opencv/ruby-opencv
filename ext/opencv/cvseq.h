/************************************************************

   cvseq.h -

   $Author: lsxi $

   Copyright (C) 2005-2006 Masakazu Yonekura

************************************************************/
#ifndef RUBY_OPENCV_CVSEQ_H
#define RUBY_OPENCV_CVSEQ_H

#include <ruby.h>
#include "opencv2/imgproc/types_c.h"

#include "cvutils.h"

namespace mOpenCV {
  namespace cCvSeq {
    VALUE rb_class();
    void init_ruby_class();
    VALUE seqblock_class(void *ptr);
    void register_elem_class(CvSeq *seq, VALUE klass);
    void unregister_elem_class(void *ptr);
    CvSeq* create_seq(int seq_flags, size_t header_size, VALUE storage_value);
    VALUE rb_allocate(VALUE klass);
    VALUE rb_initialize(int argc, VALUE *argv, VALUE self);
    VALUE rb_total(VALUE self);
    VALUE rb_empty_q(VALUE self);
    VALUE rb_aref(VALUE self, VALUE index);
    VALUE rb_first(VALUE self);
    VALUE rb_last(VALUE self);
    VALUE rb_h_prev(VALUE self);
    VALUE rb_h_next(VALUE self);
    VALUE rb_v_prev(VALUE self);
    VALUE rb_v_next(VALUE self);
    VALUE rb_push(VALUE self, VALUE args);
    VALUE rb_pop(VALUE self);
    VALUE rb_unshift(VALUE self, VALUE args);
    VALUE rb_shift(VALUE self);
    VALUE rb_each(VALUE self);
    VALUE rb_each_index(VALUE self);
    VALUE rb_insert(VALUE self, VALUE index, VALUE object);
    VALUE rb_remove(VALUE self, VALUE index);
    VALUE rb_clear(VALUE self);
    VALUE new_object(CvSeq *seq, VALUE klass);
    VALUE new_object(CvSeq *seq, VALUE klass, VALUE storage);
    VALUE new_sequence(VALUE klass, CvSeq *seq, VALUE element_klass, VALUE storage);
  }

  inline CvSeq*
    CVSEQ(VALUE object)
  {
    CvSeq *ptr;
    Data_Get_Struct(object, CvSeq, ptr);
    return ptr;
  }

  inline CvSeq*
    CVSEQ_WITH_CHECK(VALUE object)
  {
    if (!rb_obj_is_kind_of(object, cCvSeq::rb_class()))
      raise_typeerror(object, cCvSeq::rb_class());
    return CVSEQ(object);
  }
}
#endif // RUBY_OPENCV_CVSEQ_H
