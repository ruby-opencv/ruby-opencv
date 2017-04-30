/************************************************************

   cvseq.cpp -

   $Author: lsxi $

   Copyright (C) 2005-2006 Masakazu Yonekura

************************************************************/
#include "cvseq.h"
#include "cvpoint.h"
#include "cvpoint2d32f.h"
#include "cvpoint3d32f.h"
#include "cvmemstorage.h"
#include "opencv.h"
/*
 * Document-class: OpenCV::CvSeq
 */
namespace mOpenCV {
  namespace cCvSeq {

    VALUE rb_allocate(VALUE klass);
    void cvseq_free(void *ptr);

    VALUE rb_klass;
    // contain sequence-block class
    st_table *seqblock_klass_table = st_init_numtable();

    VALUE
    rb_class()
    {
      return rb_klass;
    }

    int
    eltype2class(int eltype, VALUE* ret) {
      int found = 1;

      switch (eltype) {
      case CV_SEQ_ELTYPE_POINT:
	*ret = cCvPoint::rb_class();
	break;
      case CV_32FC2:
	*ret = cCvPoint2D32f::rb_class();
	break;
      case CV_SEQ_ELTYPE_POINT3D:
	*ret = cCvPoint3D32f::rb_class();
	break;
      case CV_SEQ_ELTYPE_CODE:
      case CV_SEQ_ELTYPE_INDEX:
	*ret = rb_cInteger;
	break;
      case CV_SEQ_ELTYPE_PPOINT: // or CV_SEQ_ELTYPE_PTR:
	// Not supported
	rb_raise(rb_eArgError, "seq_flags %d is not supported.", eltype);
	break;
      default:
	found = 0;
	*ret = cCvPoint::rb_class();
	break;
      }

      return found;
    }

    VALUE
    seqblock_class(void *ptr)
    {
      VALUE klass = Qnil;
      if (st_lookup(seqblock_klass_table, (st_data_t)ptr, (st_data_t*)&klass)) {
	return klass;
      }

      int eltype = CV_SEQ_ELTYPE((CvSeq*)ptr);
      eltype2class(eltype, &klass);

      return klass;
    }

    void
    register_elem_class(CvSeq *seq, VALUE klass)
    {
      st_insert(seqblock_klass_table, (st_data_t)seq, (st_data_t)klass);
    }

    void
    unregister_elem_class(void *ptr)
    {
      if (ptr) {
	st_delete(seqblock_klass_table, (st_data_t*)&ptr, NULL);
	unregister_object(ptr);
      }
    }

    VALUE
    rb_allocate(VALUE klass)
    {
      CvSeq *ptr = ALLOC(CvSeq);
      return Data_Wrap_Struct(klass, mark_root_object, unregister_elem_class, ptr);
    }

    CvSeq*
    create_seq(int seq_flags, size_t header_size, VALUE storage_value)
    {
      VALUE klass = Qnil;
      int eltype = seq_flags & CV_SEQ_ELTYPE_MASK;
      storage_value = CHECK_CVMEMSTORAGE(storage_value);

      if (!eltype2class(eltype, &klass)) {
	seq_flags = CV_SEQ_ELTYPE_POINT | CV_SEQ_KIND_GENERIC;
      }

      int mat_type = CV_MAT_TYPE(seq_flags);
      size_t elem_size = (size_t)(CV_ELEM_SIZE(mat_type));
      CvSeq* seq = NULL;
      try {
	seq = cvCreateSeq(seq_flags, header_size, elem_size, CVMEMSTORAGE(storage_value));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      register_elem_class(seq, klass);
      register_root_object(seq, storage_value);
  
      return seq;
    }

    VALUE
    class2seq_flags_value(VALUE klass) {
      int seq_flags;
      if (klass == cCvPoint::rb_class()) {
	seq_flags = CV_SEQ_ELTYPE_POINT;
      }
      else if (klass == cCvPoint2D32f::rb_class()) {
	seq_flags = CV_32FC2;
      }
      else if (klass == cCvPoint3D32f::rb_class()) {
	seq_flags = CV_SEQ_ELTYPE_POINT3D;
      }
      else if (klass == rb_cInteger) {
	seq_flags = CV_SEQ_ELTYPE_INDEX;
      }
      else {
	rb_raise(rb_eTypeError, "unexpected type: %s", rb_class2name(klass));
      }

      return INT2NUM(seq_flags | CV_SEQ_KIND_GENERIC);
    }

    /*
     * Constructor
     *
     * @overload new(seq_flags, storage = nil)
     *   @param [Integer] seq_flags Flags of the created sequence, which are combinations of
     *     the element types and sequence types.
     *     - Element type:
     *       - <tt>CV_SEQ_ELTYPE_POINT</tt>: {CvPoint}
     *       - <tt>CV_32FC2</tt>: {CvPoint2D32f}
     *       - <tt>CV_SEQ_ELTYPE_POINT3D</tt>: {CvPoint3D32f}
     *       - <tt>CV_SEQ_ELTYPE_INDEX</tt>: Integer
     *       - <tt>CV_SEQ_ELTYPE_CODE</tt>: Integer (Freeman code)
     *     - Sequence type:
     *       - <tt>CV_SEQ_KIND_GENERIC</tt>: Generic sequence
     *       - <tt>CV_SEQ_KIND_CURVE</tt>: Curve
     *   @param [CvMemStorage] storage Sequence location
     * @return [CvSeq] self
     * @opencv_func cvCreateSeq
     * @example
     *   seq1 = CvSeq.new(CV_SEQ_ELTYPE_INDEX)
     *   seq1 << 1
     *   seq1 << CvPoint.new(1, 2) #=> TypeError
     *
     *   seq2 = CvSeq.new(CV_SEQ_ELTYPE_POINT | CV_SEQ_KIND_CURVE)
     *   seq2 << CvPoint.new(1, 2)
     *   seq2 << 3 #=> TypeError
     */
    VALUE
    rb_initialize(int argc, VALUE *argv, VALUE self)
    {
      VALUE seq_flags_value, storage_value;
      rb_scan_args(argc, argv, "11", &seq_flags_value, &storage_value);
      int seq_flags = 0;

      if (TYPE(seq_flags_value) == T_CLASS) { // To maintain backward compatibility
	seq_flags_value = class2seq_flags_value(seq_flags_value);
      }
      Check_Type(seq_flags_value, T_FIXNUM);
      seq_flags = FIX2INT(seq_flags_value);

      DATA_PTR(self) = create_seq(seq_flags, sizeof(CvSeq), storage_value);

      return self;
    }
    
    /*
     * call-seq:
     *   total -> int
     *
     * Return total number of sequence-block.
     */
    VALUE
    rb_total(VALUE self)
    {
      return INT2NUM(CVSEQ(self)->total);
    }

    /*
     * call-seq:
     *   empty? -> true or false.
     *
     * Return <tt>true</tt> if contain no object, otherwize return <tt>false</tt>.
     */
    VALUE
    rb_empty_q(VALUE self)
    {
      return CVSEQ(self)->total == 0 ? Qtrue : Qfalse;
    }

    /*
     * call-seq:
     *   [<i>index</i>] -> obj or nil
     *
     * Return sequence-block at <i>index</i>.
     */
    VALUE
    rb_aref(VALUE self, VALUE index)
    {
      CvSeq *seq = CVSEQ(self);
      int idx = NUM2INT(index);
      if (seq->total == 0) {
	return Qnil;
      }
      if (idx >= seq->total) {
	rb_raise(rb_eIndexError, "index %d out of sequence", idx);
      }

      VALUE result = Qnil;
      try {
	VALUE klass = seqblock_class(seq);
	if (RTEST(rb_class_inherited_p(klass, rb_cInteger))) {
	  result = INT2NUM(*CV_GET_SEQ_ELEM(int, seq, idx));
	}
	else {
	  result = REFER_OBJECT(klass, cvGetSeqElem(seq, idx), self);
	}
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return result;
    }

    /*
     * call-seq:
     *   first -> obj or nil
     *
     * Return first sequence-block.
     */
    VALUE
    rb_first(VALUE self)
    {
      return rb_aref(self, INT2FIX(0));
    }

    /*
     * call-seq:
     *   last -> obj or nil
     *
     * Return last sequence-block.
     */
    VALUE
    rb_last(VALUE self)
    {
      return rb_aref(self, INT2FIX(-1));
    }
    
    /*
     * call-seq:
     *   h_prev -> seq or nil
     *
     * Return the sequence horizontally located in previous.
     * Return <tt>nil</tt> if not existing.
     */
    VALUE
    rb_h_prev(VALUE self)
    {
      CvSeq *seq = CVSEQ(self);
      if (seq->h_prev)
	return new_sequence(CLASS_OF(self), seq->h_prev, seqblock_class(seq), lookup_root_object(seq));
      else
	return Qnil;
    }

    /*
     * call-seq:
     *   h_next -> seq or nil
     *
     * Return the sequence horizontally located in next.
     * Return <tt>nil</tt> if not existing.
     */
    VALUE
    rb_h_next(VALUE self)
    {
      CvSeq *seq = CVSEQ(self);
      if (seq->h_next)
	return new_sequence(CLASS_OF(self), seq->h_next, seqblock_class(seq), lookup_root_object(seq));
      else
	return Qnil;
    }

    /*
     * call-seq:
     *   v_prev -> seq or nil
     *
     * Return the sequence vertically located in previous.
     * Return <tt>nil</tt> if not existing.
     */
    VALUE
    rb_v_prev(VALUE self)
    {
      CvSeq *seq = CVSEQ(self);
      if (seq->v_prev)
	return new_sequence(CLASS_OF(self), seq->v_prev, seqblock_class(seq), lookup_root_object(seq));
      else
	return Qnil;
    }

    /*
     * call-seq:
     *   v_next -> seq or nil
     *
     * Return the sequence vertically located in next.
     * Return <tt>nil</tt> if not existing.
     */
    VALUE
    rb_v_next(VALUE self)
    {
      CvSeq *seq = CVSEQ(self);
      if (seq->v_next)
	return new_sequence(CLASS_OF(self), seq->v_next, seqblock_class(seq), lookup_root_object(seq));
      else
	return Qnil;
    }

    VALUE
    rb_seq_push(VALUE self, VALUE args, int flag)
    {
      CvSeq *seq = CVSEQ(self);
      VALUE klass = seqblock_class(seq);
      volatile void *elem = NULL;
      int len = RARRAY_LEN(args);
      for (int i = 0; i < len; i++) {
	VALUE object = RARRAY_PTR(args)[i];
	if (rb_obj_is_kind_of(object, klass)) {
	  if (rb_obj_is_kind_of(object, rb_cInteger)) {
	    volatile int int_elem = NUM2INT(object);
	    elem = &int_elem;
	  }
	  else if (rb_obj_is_kind_of(object, rb_cNumeric)) {
	    volatile double double_elem = NUM2DBL(object);
	    elem = &double_elem;
	  }
	  else {
	    elem = (void*)DATA_PTR(object);
	  }
	  try {
	    if (flag == CV_FRONT)
	      cvSeqPushFront(seq, (const void*)elem);
	    else
	      cvSeqPush(seq, (const void*)elem);
	  }
	  catch (cv::Exception& e) {
	    raise_cverror(e);
	  }
	}
	else if ((rb_obj_is_kind_of(object, rb_klass) == Qtrue) &&
		 RTEST(rb_class_inherited_p(seqblock_class(CVSEQ(object)), klass))) { // object is CvSeq
	  void *buffer = NULL;
	  try {
	    buffer = cvCvtSeqToArray(CVSEQ(object), rb_cvAlloc(CVSEQ(object)->total * CVSEQ(object)->elem_size));
	    cvSeqPushMulti(seq, buffer, CVSEQ(object)->total, flag);
	    cvFree(&buffer);
	  }
	  catch (cv::Exception& e) {
	    if (buffer != NULL)
	      cvFree(&buffer);
	    raise_cverror(e);
	  }
	}
	else {
	  rb_raise(rb_eTypeError, "arguments should be %s or %s which includes %s.",
		   rb_class2name(klass), rb_class2name(rb_klass), rb_class2name(klass));
	}
      }
  
      return self;
    }

    /*
     * call-seq:
     *   push(obj, ...) -> self
     *     
     * Append - Pushes the given object(s) on the end of this sequence. This expression return the sequence itself,
     * so several append may be chained together.
     */
    VALUE
    rb_push(VALUE self, VALUE args)
    {
      return rb_seq_push(self, args, CV_BACK);
    }
    
    /*
     * call-seq:
     *   pop -> obj or nil
     *
     * Remove the last sequence-block from <i>self</i> and return it,
     * or <tt>nil</tt> if the sequence is empty.
     */
    VALUE
    rb_pop(VALUE self)
    {
      CvSeq *seq = CVSEQ(self);
      if (seq->total == 0)
	return Qnil;
  
      VALUE object = Qnil;
      VALUE klass = seqblock_class(seq);
      try {
	if (klass == rb_cInteger) {
	  int n = 0;
	  cvSeqPop(seq, &n);
	  object = INT2FIX(n);
	}
	else {
	  object = GENERIC_OBJECT(klass, malloc(seq->elem_size));
	  cvSeqPop(seq, DATA_PTR(object));
	}
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return object;
    }

    /*
     * call-seq:
     *   clear -> self
     *
     * Clears sequence. Removes all elements from the sequence.
     */
    VALUE
    rb_clear(VALUE self)
    {
      try {
	cvClearSeq(CVSEQ(self));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return self;
    }

    /*
     * call-seq:
     *   unshift -> self
     *
     * Prepends objects to the front of sequence. other elements up one.
     */
    VALUE
    rb_unshift(VALUE self, VALUE args)
    {
      VALUE result = Qnil;
      try {
	result = rb_seq_push(self, args, CV_FRONT);
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return result;
    }

    /*
     * call-seq:
     *   shift -> obj or nil
     *
     * Returns the first element of <i>self</i> and removes it (shifting all other elements down by one). Returns <tt>nil</tt> if the array is empty.
     */
    VALUE
    rb_shift(VALUE self)
    {
      CvSeq *seq = CVSEQ(self);
      if (seq->total == 0)
	return Qnil;

      VALUE object = Qnil;
      try {
	if (seqblock_class(seq) == rb_cInteger) {
	  int n = 0;
	  cvSeqPopFront(seq, &n);
	  object = INT2NUM(n);
	}
	else {
	  object = GENERIC_OBJECT(seqblock_class(seq), malloc(seq->elem_size));
	  cvSeqPopFront(seq, DATA_PTR(object));
	}
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }

      return object;
    }

    /*
     * call-seq:
     *   each {|obj| ... } -> self
     *
     * Calls block once for each sequence-block in <i>self</i>,
     * passing that sequence-block as a parameter.
     *   seq = CvSeq.new(CvIndex)
     *   seq.push(5, 6, 7)
     *   seq.each {|x| print x, " -- " }
     * produces:
     *   5 -- 6 -- 7 --
     */
    VALUE
    rb_each(VALUE self)
    {
      CvSeq *seq = CVSEQ(self);
      if (seq->total > 0) {
	VALUE klass = seqblock_class(seq);
	try {
	  if (klass == rb_cInteger)
	    for (int i = 0; i < seq->total; ++i)
	      rb_yield(INT2NUM(*CV_GET_SEQ_ELEM(int, seq, i)));
	  else
	    for (int i = 0; i < seq->total; ++i)
	      rb_yield(REFER_OBJECT(klass, cvGetSeqElem(seq, i), self));
	}
	catch (cv::Exception& e) {
	  raise_cverror(e);
	}
      }
      return self;
    }

    /*
     * call-seq:
     *    each_index {|index| ... } -> self
     *
     * Same as CvSeq#each, but passes the index of the element instead of the element itself.
     */
    VALUE
    rb_each_index(VALUE self)
    {
      CvSeq *seq = CVSEQ(self);
      for(int i = 0; i < seq->total; ++i)
	rb_yield(INT2NUM(i));
      return self;
    }


    /*
     * call-seq:
     *   insert(index,obj) -> self
     *
     * Inserts the given values before element with the given index (which may be negative).
     */
    VALUE
    rb_insert(VALUE self, VALUE index, VALUE object)
    {
      Check_Type(index, T_FIXNUM);
      CvSeq *seq = CVSEQ(self);
      VALUE klass = seqblock_class(seq);
      if (!rb_obj_is_kind_of(object, klass))
	rb_raise(rb_eTypeError, "arguments should be %s.", rb_class2name(klass));
      try {
	if (klass == rb_cInteger) {
	  int n = NUM2INT(object);
	  cvSeqInsert(seq, NUM2INT(index), &n);
	}
	else
	  cvSeqInsert(seq, NUM2INT(index), DATA_PTR(object));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return self;
    }

    /*
     * call-seq:
     *   remove(index) -> obj or nil
     *
     * Deletes the elements at the specified index.
     */
    VALUE
    rb_remove(VALUE self, VALUE index)
    {
      try {
	cvSeqRemove(CVSEQ(self), NUM2INT(index));
      }
      catch (cv::Exception& e) {
	raise_cverror(e);
      }
      return self;
    }

    VALUE
    new_sequence(VALUE klass, CvSeq *seq, VALUE element_klass, VALUE storage)
    {
      register_root_object(seq, storage);
      if (!NIL_P(element_klass))
	register_elem_class(seq, element_klass);
      return Data_Wrap_Struct(klass, mark_root_object, unregister_elem_class, seq);
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
      rb_klass = rb_define_class_under(opencv, "CvSeq", rb_cObject);
      rb_include_module(rb_klass, rb_mEnumerable);
      rb_define_alloc_func(rb_klass, rb_allocate);
      rb_define_method(rb_klass, "initialize", RUBY_METHOD_FUNC(rb_initialize), -1);
      rb_define_method(rb_klass, "total", RUBY_METHOD_FUNC(rb_total), 0);
      rb_define_alias(rb_klass, "length", "total");
      rb_define_alias(rb_klass, "size", "total");
      rb_define_method(rb_klass, "empty?", RUBY_METHOD_FUNC(rb_empty_q), 0);
      rb_define_method(rb_klass, "[]", RUBY_METHOD_FUNC(rb_aref), 1);
      rb_define_method(rb_klass, "first", RUBY_METHOD_FUNC(rb_first), 0);
      rb_define_method(rb_klass, "last", RUBY_METHOD_FUNC(rb_last), 0);
  
      rb_define_method(rb_klass, "h_prev", RUBY_METHOD_FUNC(rb_h_prev), 0);
      rb_define_method(rb_klass, "h_next", RUBY_METHOD_FUNC(rb_h_next), 0);
      rb_define_method(rb_klass, "v_prev", RUBY_METHOD_FUNC(rb_v_prev), 0);
      rb_define_method(rb_klass, "v_next", RUBY_METHOD_FUNC(rb_v_next), 0);

      rb_define_method(rb_klass, "push", RUBY_METHOD_FUNC(rb_push), -2);
      rb_define_alias(rb_klass, "<<", "push");
      rb_define_method(rb_klass, "pop", RUBY_METHOD_FUNC(rb_pop), 0);
      rb_define_method(rb_klass, "unshift", RUBY_METHOD_FUNC(rb_unshift), -2);
      rb_define_alias(rb_klass, "push_front", "unshift");
      rb_define_method(rb_klass, "shift", RUBY_METHOD_FUNC(rb_shift), 0);
      rb_define_alias(rb_klass, "pop_front", "shift");
      rb_define_method(rb_klass, "each", RUBY_METHOD_FUNC(rb_each), 0);
      rb_define_method(rb_klass, "each_index", RUBY_METHOD_FUNC(rb_each_index), 0);
      rb_define_method(rb_klass, "insert", RUBY_METHOD_FUNC(rb_insert), 2);
      rb_define_method(rb_klass, "remove", RUBY_METHOD_FUNC(rb_remove), 1);
      rb_define_alias(rb_klass, "delete_at", "remove");
      rb_define_method(rb_klass, "clear", RUBY_METHOD_FUNC(rb_clear), 0);
    }
  }
}
