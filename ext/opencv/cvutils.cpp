/************************************************************

  cvutils.cpp -

  $Author: ser1zw $

  Copyright (C) 2011 ser1zw

************************************************************/
#include "opencv2/imgproc/imgproc_c.h"

#include "cvutils.h"

// OpenCV private function
#define  CV_MALLOC_ALIGN    16
static inline void* cvAlignPtr( const void* ptr, int align = 32 )
{
  CV_DbgAssert ( (align & (align-1)) == 0 );
  return (void*)( ((size_t)ptr + align - 1) & ~(size_t)(align-1) );
}

void
raise_typeerror(VALUE object, VALUE expected_class)
{
  raise_typeerror(object, rb_class2name(expected_class));
}

void
raise_typeerror(VALUE object, const char* expected_class_name)
{
  rb_raise(rb_eTypeError, "wrong argument type %s (expected %s)",
  	   rb_obj_classname(object), expected_class_name);
}

void
raise_compatible_typeerror(VALUE object, VALUE expected_class)
{
  raise_compatible_typeerror(object, rb_class2name(expected_class));
}

void
raise_compatible_typeerror(VALUE object, const char* expected_class_name)
{
  rb_raise(rb_eTypeError, "wrong argument type %s (expected %s or compatible object)",
  	   rb_obj_classname(object), expected_class_name);
}

/*
 * Allocates a memory buffer
 * see cv::fastMalloc()
 */
void*
rbFastMalloc(size_t size)
{
  uchar* udata = (uchar*)xmalloc(size + sizeof(void*) + CV_MALLOC_ALIGN);
  if(!udata) {
    rb_raise(rb_eNoMemError, "Failed to allocate memory");
  }
  uchar** adata = cv::alignPtr((uchar**)udata + 1, CV_MALLOC_ALIGN);
  adata[-1] = udata;
  return adata;
}

/*
 * Allocates a memory buffer
 * When memory allocation is failed, run GC and retry it
 */
void*
rb_cvAlloc(size_t size)
{
  return rbFastMalloc(size);
}

/*
 * Creates CvMat and underlying data
 * When memory allocation is failed, run GC and retry it
 */
CvMat*
rb_cvCreateMat(int rows, int cols, int type)
{
  CvMat* mat = NULL;
  try {
    mat = cvCreateMatHeader(rows, cols, type);
    if (mat) {
      // see OpenCV's cvCreateData()
      size_t step = mat->step;
      size_t total_size = step * mat->rows + sizeof(int) + CV_MALLOC_ALIGN;

      mat->refcount = (int*)rbFastMalloc(total_size);
      mat->data.ptr = (uchar*)cvAlignPtr(mat->refcount + 1, CV_MALLOC_ALIGN);
      *mat->refcount = 1;
    }
    else {
      rb_raise(rb_eRuntimeError, "Failed to create mat header");
    }
  }
  catch(cv::Exception& e) {
    if (mat) {
      cvReleaseMat(&mat);
    }
    rb_raise(rb_eRuntimeError, "%s", e.what());
  }
  return mat;
}

/*
 * Create IplImage header and allocate underlying data
 * When memory allocation is failed, run GC and retry it
 */
IplImage*
rb_cvCreateImage(CvSize size, int depth, int channels)
{
  IplImage* ptr = NULL;
  try {
    ptr = cvCreateImageHeader(size, depth, channels);
    if (ptr) {
      // see OpenCV's cvCreateData()
      ptr->imageData = ptr->imageDataOrigin = (char*)rbFastMalloc((size_t)ptr->imageSize);
    }
    else {
      rb_raise(rb_eRuntimeError, "Failed to create image header");
    }
  }
  catch(cv::Exception& e) {
    if (ptr) {
      cvReleaseImage(&ptr);
    }
    rb_raise(rb_eRuntimeError, "%s", e.what());
  }
  return ptr;
}

/*
 * Creates a structuring element
 * When memory allocation is failed, run GC and retry it
 */
IplConvKernel*
rb_cvCreateStructuringElementEx(int cols, int rows,
				int anchorX, int anchorY,
				int shape, int *values)
{
  IplConvKernel* ptr = NULL;
  try {
    ptr = cvCreateStructuringElementEx(cols, rows, anchorX, anchorY, shape, values);
  }
  catch(cv::Exception& e) {
    if (e.code != CV_StsNoMem)
      rb_raise(rb_eRuntimeError, "%s", e.what());

    rb_gc_start();
    try {
      ptr = cvCreateStructuringElementEx(cols, rows, anchorX, anchorY, shape, values);
    }
    catch (cv::Exception& e) {
      if (e.code == CV_StsNoMem)
	rb_raise(rb_eNoMemError, "%s", e.what());
      else
	rb_raise(rb_eRuntimeError, "%s", e.what());
    }
  }
  return ptr;
}

/*
 * Creates memory storage
 * When memory allocation is failed, run GC and retry it
 */
CvMemStorage*
rb_cvCreateMemStorage(int block_size)
{
  CvMemStorage* ptr = NULL;
  try {
    ptr = cvCreateMemStorage(block_size);
  }
  catch(cv::Exception& e) {
    if (e.code != CV_StsNoMem)
      rb_raise(rb_eRuntimeError, "%s", e.what());

    rb_gc_start();
    try {
      ptr = cvCreateMemStorage(block_size);
    }
    catch (cv::Exception& e) {
      if (e.code == CV_StsNoMem)
	rb_raise(rb_eNoMemError, "%s", e.what());
      else
	rb_raise(rb_eRuntimeError, "%s", e.what());
    }
  }
  return ptr;
}

VALUE
rb_get_option_table(VALUE klass, const char* table_name, VALUE option)
{
  VALUE table = rb_const_get(klass, rb_intern(table_name));
  if (NIL_P(option))
    return table;
  else
    return rb_funcall(table, rb_intern("merge"), 1, option);
}

