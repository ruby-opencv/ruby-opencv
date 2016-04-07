/************************************************************

    opencv.h -

    $Author: lsxi $

    Copyright (C) 2005-2007 Masakazu Yonekura

************************************************************/
#ifndef RUBY_OPENCV_H
#define RUBY_OPENCV_H

#define __NAMESPACE_BEGIN_OPENCV namespace mOpenCV {
#define __NAMESPACE_END_OPENCV }

/* include headers */
#include <ruby.h>
#ifdef HAVE_RUBY_VERSION_H
#include <ruby/version.h>
#else
#include <version.h>
#endif

// Workaround for https://bugs.ruby-lang.org/issues/11962
#undef RB_OBJ_WB_UNPROTECT_FOR
#define RB_OBJ_WB_UNPROTECT_FOR(type, obj)		\
  (RGENGC_WB_PROTECTED_##type ?				\
   OBJ_WB_UNPROTECT((VALUE)(obj)) : ((VALUE)(obj)))

#ifdef RUBY_WIN32_H
#ifdef write
#undef write
#endif // write
#endif // RUBY_WIN32_H

#ifndef ANYARGS
#define ANYARGS ()
#endif

extern "C" {
#ifdef HAVE_RUBY_ST_H
#include <ruby/st.h>
#else
#include <st.h>
#endif

#ifdef HAVE_STDARG_H
#include <stdarg.h>
#define va_init_list(a,b) va_start(a,b)
#else
#include <varargs.h>
#define va_init_list(a,b) va_start(a)
#endif
}

// standard c headers
#define _USE_MATH_DEFINES // for VC++
#include <math.h>
#include <limits.h>
#include <float.h>
#include <assert.h>

// OpenCV headers
#include "opencv2/core/core_c.h"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc_c.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/video/tracking.hpp"
#include "opencv2/video/background_segm.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/flann/flann.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/legacy/compat.hpp"
#include "opencv2/legacy/legacy.hpp"
#include "opencv2/legacy/blobtrack.hpp"
#include "opencv2/contrib/contrib.hpp"
#include "opencv2/highgui/highgui_c.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/internal.hpp"
#include "opencv2/photo/photo.hpp"

#ifdef HAVE_ML_H
#include "opencv2/ml/ml.hpp"
#endif

#ifdef HAVE_OPENCV2_NONFREE_NONFREE_HPP
#include "opencv2/nonfree/nonfree.hpp"
#endif

// Ruby/OpenCV headers
#include "cvutils.h"
#include "cverror.h"
#include "cvpoint.h"
#include "cvpoint2d32f.h"
#include "cvsize.h"
#include "cvsize2d32f.h"
#include "cvrect.h"
#include "cvscalar.h"
#include "cvslice.h"
#include "cvtermcriteria.h"
#include "cvbox2d.h"
#include "cvfont.h"
#include "iplconvkernel.h"
#include "cvmoments.h"
#include "cvhumoments.h"
#include "cvconvexitydefect.h"
#include "cvpoint3d32f.h"

#include "cvmemstorage.h"

#include "cvseq.h"
#include "curve.h"
#include "pointset.h"
#include "cvchain.h"
#include "cvcontour.h"
#include "cvcontourtree.h"

#include "cvmat.h"
#include "iplimage.h"
#include "cvhistogram.h"
#include "cvcapture.h"
#include "cvvideowriter.h"

#include "cvline.h"
#include "cvtwopoints.h"
#include "cvcircle32f.h"

#include "cvconnectedcomp.h"
#include "cvavgcomp.h"
#include "cvhaarclassifiercascade.h"

#include "cvsurfpoint.h"
#include "cvsurfparams.h"

#include "cvfeaturetree.h"

#include "algorithm.h"
#include "facerecognizer.h"
#include "eigenfaces.h"
#include "fisherfaces.h"
#include "lbph.h"

// GUI
#include "gui.h"
#include "window.h"
#include "trackbar.h"
#include "mouseevent.h"

// memory management wrapper
#define RB_CVALLOC(type) (type*)rb_cvAlloc(sizeof(type))

// useful macros
#define IF_INT(val, ifnone) NIL_P(val) ? ifnone : NUM2INT(val)
#define IF_DBL(val, ifnone) NIL_P(val) ? ifnone : NUM2DBL(val)
#define IF_STRING(str) NIL_P(str) ? NULL : TYPE(str) == T_STRING ? rb
#define IF_BOOL(val, t, f, ifnone) val == Qtrue ? t : val == Qfalse ? f : ifnone

#define IF_DEPTH(val, ifnone) NIL_P(val) ? ifnone : NUM2INT(val)

#define REGISTER_HASH(hash, str, value) rb_hash_aset(hash, ID2SYM(rb_intern(str)), INT2FIX(value))
#define LOOKUP_HASH(hash, key_as_cstr) (rb_hash_lookup(hash, ID2SYM(rb_intern(key_as_cstr))))

#define maxint(a,b) ({int _a = (a), _b = (b); _a > _b ? _a : _b; })

#ifndef BOOL2INT
#define BOOL2INT(x)  ((x == Qtrue) ? 1 : 0)
#endif

#ifndef INT2BOOL
#define INT2BOOL(x)  (x ? Qtrue : Qfalse)
#endif

// wrapper for <= 1.8
#ifndef RARRAY_LEN
#define RARRAY_LEN(arg) (RARRAY(arg)->len)
#endif

#ifndef RARRAY_PTR
#define RARRAY_PTR(arg) (RARRAY(arg)->ptr)
#endif

#ifndef RSTRING_LEN
#define RSTRING_LEN(arg) (RSTRING(arg)->len)
#endif

#ifndef RSTRING_PTR
#define RSTRING_PTR(arg) (RSTRING(arg)->ptr)
#endif

#ifndef DBL2NUM
#define DBL2NUM(dbl) (rb_float_new(dbl))
#endif


// OpenCV module
__NAMESPACE_BEGIN_OPENCV

void mark_root_object(void *ptr);
VALUE lookup_root_object(void *ptr);
void register_root_object(void *ptr, VALUE root);
void unregister_object(void *ptr);
void free_object(void *ptr);
void release_object(void *ptr);
void release_iplconvkernel_object(void *ptr);

VALUE rb_module_opencv();
void init_ruby_module();

// Ruby/OpenCV inline functions  
inline CvArr*
CVARR(VALUE object)
{
  CvArr *ptr;
  Data_Get_Struct(object, CvArr, ptr);
  return ptr;
}  

inline CvArr*
CVARR_WITH_CHECK(VALUE object)
{
  Check_Type(object, T_DATA);
  void *ptr = DATA_PTR(object);
  if (CV_IS_IMAGE(ptr) || CV_IS_MAT(ptr) || CV_IS_SEQ(ptr) ||
      CV_IS_MATND(ptr) || CV_IS_SPARSE_MAT(ptr)) {
    return CVARR(object);
  }
  else {
    raise_compatible_typeerror(object, (char*)"CvArr");
  }
  return NULL;
}  

inline VALUE
OPENCV_OBJECT(VALUE klass, void *ptr)
{
  return Data_Wrap_Struct(klass, 0, release_object, ptr);
}

inline VALUE
IPLCONVKERNEL_OBJECT(VALUE klass, void *ptr)
{
  return Data_Wrap_Struct(klass, 0, release_iplconvkernel_object, ptr);
}

inline VALUE
GENERIC_OBJECT(VALUE klass, void *ptr)
{
  return Data_Wrap_Struct(klass, 0, -1, ptr);
}

inline VALUE
DEPEND_OBJECT(VALUE klass, void *ptr, VALUE root)
{
  register_root_object(ptr, root);
  return Data_Wrap_Struct(klass, mark_root_object, free_object, ptr);
}

inline VALUE
REFER_OBJECT(VALUE klass, void *ptr, VALUE root)
{
  register_root_object(ptr, root);
  return Data_Wrap_Struct(klass, mark_root_object, unregister_object, ptr);
}

inline int
CVMETHOD(const char *name, VALUE method, int ifnone = 0)
{
  VALUE value;
  switch (TYPE(method)) {
  case T_NIL:
    return ifnone;
  case T_FIXNUM:
    return FIX2INT(method);
  case T_STRING:
    method = rb_str_intern(method);
  case T_SYMBOL:
    value = rb_hash_lookup(rb_const_get(rb_module_opencv(), rb_intern(name)), method);
    return NIL_P(value) ? ifnone : FIX2INT(value);
  default:
    raise_typeerror(method, rb_cSymbol);
  }
  return ifnone;
}

inline int
TRUE_OR_FALSE(VALUE object, int ifnone = 0)
{
  int value = ifnone;
  switch (TYPE(object)) {
  case T_TRUE:
    value = 1;
    break;
  case T_FALSE:
    value = 0;
    break;
  case T_NIL:
    break;
  default:
    break;
  }
  return value;
}

inline int
CV2IPL_DEPTH(int depth)
{
  switch (depth) {
  case CV_8U:
    return IPL_DEPTH_8U;
    break;
  case CV_8S:
    return IPL_DEPTH_8S;
    break;
  case CV_16U:
    return IPL_DEPTH_16U;
    break;
  case CV_32F:
    return IPL_DEPTH_32F;
    break;
  case CV_32S:
    return IPL_DEPTH_32S;
    break;
  case CV_64F:
    return IPL_DEPTH_64F;
    break;
  default:
    rb_raise(rb_eArgError, "Invalid depth: %d", depth);
    break;
  }
  return 0;
}

VALUE rb_BGR2BGRA(VALUE klass, VALUE image);
VALUE rb_RGB2RGBA(VALUE klass, VALUE image);
VALUE rb_BGRA2BGR(VALUE klass, VALUE image);
VALUE rb_RGBA2RGB(VALUE klass, VALUE image);
VALUE rb_BGR2RGBA(VALUE klass, VALUE image);
VALUE rb_RGB2BGRA(VALUE klass, VALUE image);
VALUE rb_RGBA2BGR(VALUE klass, VALUE image);
VALUE rb_BGRA2RGB(VALUE klass, VALUE image);
VALUE rb_BGR2RGB(VALUE klass, VALUE image);
VALUE rb_RGB2BGR(VALUE klass, VALUE image);
VALUE rb_BGRA2RGBA(VALUE klass, VALUE image);
VALUE rb_RGBA2BGRA(VALUE klass, VALUE image);
VALUE rb_BGR2GRAY(VALUE klass, VALUE image);
VALUE rb_RGB2GRAY(VALUE klass, VALUE image);
VALUE rb_GRAY2BGR(VALUE klass, VALUE image);
VALUE rb_GRAY2RGB(VALUE klass, VALUE image);
VALUE rb_GRAY2BGRA(VALUE klass, VALUE image);
VALUE rb_GRAY2RGBA(VALUE klass, VALUE image);
VALUE rb_BGRA2GRAY(VALUE klass, VALUE image);
VALUE rb_RGBA2GRAY(VALUE klass, VALUE image);
VALUE rb_BGR2BGR565(VALUE klass, VALUE image);
VALUE rb_RGB2BGR565(VALUE klass, VALUE image);
VALUE rb_BGR5652BGR(VALUE klass, VALUE image);
VALUE rb_BGR5652RGB(VALUE klass, VALUE image);
VALUE rb_BGRA2BGR565(VALUE klass, VALUE image);
VALUE rb_RGBA2BGR565(VALUE klass, VALUE image);
VALUE rb_BGR5652BGRA(VALUE klass, VALUE image);
VALUE rb_BGR5652RGBA(VALUE klass, VALUE image);
VALUE rb_GRAY2BGR565(VALUE klass, VALUE image);
VALUE rb_BGR5652GRAY(VALUE klass, VALUE image);
VALUE rb_BGR2BGR555(VALUE klass, VALUE image);
VALUE rb_RGB2BGR555(VALUE klass, VALUE image);
VALUE rb_BGR5552BGR(VALUE klass, VALUE image);
VALUE rb_BGR5552RGB(VALUE klass, VALUE image);
VALUE rb_BGRA2BGR555(VALUE klass, VALUE image);
VALUE rb_RGBA2BGR555(VALUE klass, VALUE image);
VALUE rb_BGR5552BGRA(VALUE klass, VALUE image);
VALUE rb_BGR5552RGBA(VALUE klass, VALUE image);
VALUE rb_GRAY2BGR555(VALUE klass, VALUE image);
VALUE rb_BGR5552GRAY(VALUE klass, VALUE image);
VALUE rb_BGR2XYZ(VALUE klass, VALUE image);
VALUE rb_RGB2XYZ(VALUE klass, VALUE image);
VALUE rb_XYZ2BGR(VALUE klass, VALUE image);
VALUE rb_XYZ2RGB(VALUE klass, VALUE image);
VALUE rb_BGR2YCrCb(VALUE klass, VALUE image);
VALUE rb_RGB2YCrCb(VALUE klass, VALUE image);
VALUE rb_YCrCb2BGR(VALUE klass, VALUE image);
VALUE rb_YCrCb2RGB(VALUE klass, VALUE image);
VALUE rb_BGR2HSV(VALUE klass, VALUE image);
VALUE rb_RGB2HSV(VALUE klass, VALUE image);
VALUE rb_BGR2Lab(VALUE klass, VALUE image);
VALUE rb_RGB2Lab(VALUE klass, VALUE image);
VALUE rb_BayerBG2BGR(VALUE klass, VALUE image);
VALUE rb_BayerGB2BGR(VALUE klass, VALUE image);
VALUE rb_BayerRG2BGR(VALUE klass, VALUE image);
VALUE rb_BayerGR2BGR(VALUE klass, VALUE image);
VALUE rb_BayerBG2RGB(VALUE klass, VALUE image);
VALUE rb_BayerGB2RGB(VALUE klass, VALUE image);
VALUE rb_BayerRG2RGB(VALUE klass, VALUE image);
VALUE rb_BayerGR2RGB(VALUE klass, VALUE image);
VALUE rb_BGR2Luv(VALUE klass, VALUE image);
VALUE rb_RGB2Luv(VALUE klass, VALUE image);
VALUE rb_BGR2HLS(VALUE klass, VALUE image);
VALUE rb_RGB2HLS(VALUE klass, VALUE image);
VALUE rb_HSV2BGR(VALUE klass, VALUE image);
VALUE rb_HSV2RGB(VALUE klass, VALUE image);
VALUE rb_Lab2BGR(VALUE klass, VALUE image);
VALUE rb_Lab2RGB(VALUE klass, VALUE image);
VALUE rb_Luv2BGR(VALUE klass, VALUE image);
VALUE rb_Luv2RGB(VALUE klass, VALUE image);
VALUE rb_HLS2BGR(VALUE klass, VALUE image);
VALUE rb_HLS2RGB(VALUE klass, VALUE image);

VALUE rb_build_information(VALUE klass);

__NAMESPACE_END_OPENCV

#endif // RUBY_OPENCV_H
