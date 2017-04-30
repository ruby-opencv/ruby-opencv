/************************************************************

    cverror.cpp -

    $Author: lsxi $

    Copyright (C) 2005-2006 Masakazu Yonekura

************************************************************/
#include "cverror.h"
#include "opencv.h"
/*
 * Document-class: OpenCV::CvError
 * 
 * OpenCV errors
 */
namespace mOpenCV {
  namespace cCvError {
    st_table *cv_error = st_init_numtable();
    
    VALUE rb_klass;

    void
    REGISTER_CVERROR(const char* object_name, int error_code)
    {
      st_insert(cv_error, (st_data_t)error_code,
		(st_data_t)rb_define_class_under(rb_module_opencv(), object_name, rb_klass));
    }

    VALUE
    rb_class()
    {
      return rb_klass;
    }

    VALUE
    by_code(int error_code)
    {
      VALUE klass = 0;
      st_lookup(cv_error, (st_data_t)error_code, (st_data_t*)&klass);
      return klass ? klass : rb_eStandardError;
    }

    void
    raise(cv::Exception e)
    {
      rb_raise(by_code(e.code), "%s", e.what());
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

      VALUE opencv = rb_module_opencv();
      rb_klass = rb_define_class_under(opencv, "CvError", rb_eStandardError);
      REGISTER_CVERROR("CvStsBackTrace", CV_StsBackTrace);
      REGISTER_CVERROR("CvStsError", CV_StsError);
      REGISTER_CVERROR("CvStsInternal", CV_StsInternal);
      REGISTER_CVERROR("CvStsNoMem", CV_StsNoMem);
      REGISTER_CVERROR("CvStsBadArg", CV_StsBadArg);
      REGISTER_CVERROR("CvStsBadFunc", CV_StsBadFunc);
      REGISTER_CVERROR("CvStsNoConv", CV_StsNoConv);
      REGISTER_CVERROR("CvStsAutoTrace", CV_StsAutoTrace);
      REGISTER_CVERROR("CvHeaderIsNull", CV_HeaderIsNull);
      REGISTER_CVERROR("CvBadImageSize", CV_BadImageSize);
      REGISTER_CVERROR("CvBadOffset", CV_BadOffset);
      REGISTER_CVERROR("CvBadDataPtr", CV_BadDataPtr);
      REGISTER_CVERROR("CvBadStep", CV_BadStep);
      REGISTER_CVERROR("CvBadModelOrChSeq", CV_BadModelOrChSeq);
      REGISTER_CVERROR("CvBadNumChannels", CV_BadNumChannels);
      REGISTER_CVERROR("CvBadNumChannel1U", CV_BadNumChannel1U);
      REGISTER_CVERROR("CvBadDepth", CV_BadDepth);
      REGISTER_CVERROR("CvBadAlphaChannel", CV_BadAlphaChannel);
      REGISTER_CVERROR("CvBadOrder", CV_BadOrder);
      REGISTER_CVERROR("CvBadOrigin", CV_BadOrigin);
      REGISTER_CVERROR("CvBadAlign", CV_BadAlign);
      REGISTER_CVERROR("CvBadCallBack", CV_BadCallBack);
      REGISTER_CVERROR("CvBadTileSize", CV_BadTileSize);
      REGISTER_CVERROR("CvBadCOI", CV_BadCOI);
      REGISTER_CVERROR("CvBadROISize", CV_BadROISize);
      REGISTER_CVERROR("CvMaskIsTiled", CV_MaskIsTiled);
      REGISTER_CVERROR("CvStsNullPtr", CV_StsNullPtr);
      REGISTER_CVERROR("CvStsVecLengthErr", CV_StsVecLengthErr);
      REGISTER_CVERROR("CvStsFilterStructContentErr", CV_StsFilterStructContentErr);
      REGISTER_CVERROR("CvStsKernelStructContentErr", CV_StsKernelStructContentErr);
      REGISTER_CVERROR("CvStsFilterOffsetErr", CV_StsFilterOffsetErr);
      REGISTER_CVERROR("CvStsBadSize", CV_StsBadSize);
      REGISTER_CVERROR("CvStsDivByZero", CV_StsDivByZero);
      REGISTER_CVERROR("CvStsInplaceNotSupported", CV_StsInplaceNotSupported);
      REGISTER_CVERROR("CvStsObjectNotFound", CV_StsObjectNotFound);
      REGISTER_CVERROR("CvStsUnmatchedFormats", CV_StsUnmatchedFormats);
      REGISTER_CVERROR("CvStsBadFlag", CV_StsBadFlag);
      REGISTER_CVERROR("CvStsBadPoint", CV_StsBadPoint);
      REGISTER_CVERROR("CvStsBadMask", CV_StsBadMask);
      REGISTER_CVERROR("CvStsUnmatchedSizes", CV_StsUnmatchedSizes);
      REGISTER_CVERROR("CvStsUnsupportedFormat", CV_StsUnsupportedFormat);
      REGISTER_CVERROR("CvStsOutOfRange", CV_StsOutOfRange);
      REGISTER_CVERROR("CvStsParseError", CV_StsParseError);
      REGISTER_CVERROR("CvStsNotImplemented", CV_StsNotImplemented);
      REGISTER_CVERROR("CvStsBadMemBlock", CV_StsBadMemBlock);
      REGISTER_CVERROR("CvStsAssert", CV_StsAssert);
      REGISTER_CVERROR("CvGpuNotSupported", CV_GpuNotSupported);
      REGISTER_CVERROR("CvGpuApiCallError", CV_GpuApiCallError);
    }
  }
}
