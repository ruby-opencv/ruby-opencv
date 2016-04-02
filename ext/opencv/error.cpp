// -*- mode: c++; coding: utf-8 -*-
#include <map>
#include "error.hpp"

namespace rubyopencv {
  namespace Error {
    VALUE rb_klass;
    std::map<int, VALUE> error_code_map;

    inline void REGISTER_CVERROR(const char* object_name, int error_code) {
      VALUE klass = rb_define_class_under(rb_klass, object_name, rb_klass);
      error_code_map[error_code] = klass;
    }

    VALUE rb_class() {
      return rb_klass;
    }

    VALUE find_error_by_code(int error_code) {
      VALUE klass = Qnil;
      if (error_code_map.find(error_code) != error_code_map.end()) {
	klass = error_code_map[error_code];
      }
      return NIL_P(klass) ? rb_eStandardError : klass;
    }

    void raise(cv::Exception e) {
      rb_raise(find_error_by_code(e.code), "%s", e.what());
    }

    void init() {
      VALUE opencv = rb_define_module("OpenCV");
      rb_klass = rb_define_class_under(opencv, "Error", rb_eStandardError);

      REGISTER_CVERROR("StsOk", cv::Error::StsOk);
      REGISTER_CVERROR("StsBackTrace", cv::Error::StsBackTrace);
      REGISTER_CVERROR("StsError", cv::Error::StsError);
      REGISTER_CVERROR("StsInternal", cv::Error::StsInternal);
      REGISTER_CVERROR("StsNoMem", cv::Error::StsNoMem);
      REGISTER_CVERROR("StsBadArg", cv::Error::StsBadArg);
      REGISTER_CVERROR("StsBadFunc", cv::Error::StsBadFunc);
      REGISTER_CVERROR("StsNoConv", cv::Error::StsNoConv);
      REGISTER_CVERROR("StsAutoTrace", cv::Error::StsAutoTrace);
      REGISTER_CVERROR("HeaderIsNull", cv::Error::HeaderIsNull);
      REGISTER_CVERROR("BadImageSize", cv::Error::BadImageSize);
      REGISTER_CVERROR("BadOffset", cv::Error::BadOffset);
      REGISTER_CVERROR("BadDataPtr", cv::Error::BadDataPtr);
      REGISTER_CVERROR("BadStep", cv::Error::BadStep);
      REGISTER_CVERROR("BadModelOrChSeq", cv::Error::BadModelOrChSeq);
      REGISTER_CVERROR("BadNumChannels", cv::Error::BadNumChannels);
      REGISTER_CVERROR("BadNumChannel1U", cv::Error::BadNumChannel1U);
      REGISTER_CVERROR("BadDepth", cv::Error::BadDepth);
      REGISTER_CVERROR("BadAlphaChannel", cv::Error::BadAlphaChannel);
      REGISTER_CVERROR("BadOrder", cv::Error::BadOrder);
      REGISTER_CVERROR("BadOrigin", cv::Error::BadOrigin);
      REGISTER_CVERROR("BadAlign", cv::Error::BadAlign);
      REGISTER_CVERROR("BadCallBack", cv::Error::BadCallBack);
      REGISTER_CVERROR("BadTileSize", cv::Error::BadTileSize);
      REGISTER_CVERROR("BadCOI", cv::Error::BadCOI);
      REGISTER_CVERROR("BadROISize", cv::Error::BadROISize);
      REGISTER_CVERROR("MaskIsTiled", cv::Error::MaskIsTiled);
      REGISTER_CVERROR("StsNullPtr", cv::Error::StsNullPtr);
      REGISTER_CVERROR("StsVecLengthErr", cv::Error::StsVecLengthErr);
      REGISTER_CVERROR("StsFilterStructContentErr", cv::Error::StsFilterStructContentErr);
      REGISTER_CVERROR("StsKernelStructContentErr", cv::Error::StsKernelStructContentErr);
      REGISTER_CVERROR("StsFilterOffsetErr", cv::Error::StsFilterOffsetErr);
      REGISTER_CVERROR("StsBadSize", cv::Error::StsBadSize);
      REGISTER_CVERROR("StsDivByZero", cv::Error::StsDivByZero);
      REGISTER_CVERROR("StsInplaceNotSupported", cv::Error::StsInplaceNotSupported);
      REGISTER_CVERROR("StsObjectNotFound", cv::Error::StsObjectNotFound);
      REGISTER_CVERROR("StsUnmatchedFormats", cv::Error::StsUnmatchedFormats);
      REGISTER_CVERROR("StsBadFlag", cv::Error::StsBadFlag);
      REGISTER_CVERROR("StsBadPoint", cv::Error::StsBadPoint);
      REGISTER_CVERROR("StsBadMask", cv::Error::StsBadMask);
      REGISTER_CVERROR("StsUnmatchedSizes", cv::Error::StsUnmatchedSizes);
      REGISTER_CVERROR("StsUnsupportedFormat", cv::Error::StsUnsupportedFormat);
      REGISTER_CVERROR("StsOutOfRange", cv::Error::StsOutOfRange);
      REGISTER_CVERROR("StsParseError", cv::Error::StsParseError);
      REGISTER_CVERROR("StsNotImplemented", cv::Error::StsNotImplemented);
      REGISTER_CVERROR("StsBadMemBlock", cv::Error::StsBadMemBlock);
      REGISTER_CVERROR("StsAssert", cv::Error::StsAssert);
      REGISTER_CVERROR("GpuNotSupported", cv::Error::GpuNotSupported);
      REGISTER_CVERROR("GpuApiCallError", cv::Error::GpuApiCallError);
      REGISTER_CVERROR("OpenGlNotSupported", cv::Error::OpenGlNotSupported);
      REGISTER_CVERROR("OpenGlApiCallError", cv::Error::OpenGlApiCallError);
      REGISTER_CVERROR("OpenCLApiCallError", cv::Error::OpenCLApiCallError);
      REGISTER_CVERROR("OpenCLDoubleNotSupported", cv::Error::OpenCLDoubleNotSupported);
      REGISTER_CVERROR("OpenCLInitError", cv::Error::OpenCLInitError);
      REGISTER_CVERROR("OpenCLNoAMDBlasFft", cv::Error::OpenCLNoAMDBlasFft);
    }
  }
}
