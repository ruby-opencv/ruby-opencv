#!/usr/bin/env ruby
# -*- mode: ruby; coding: utf-8 -*-
require 'test/unit'
require 'opencv'
require File.expand_path(File.dirname(__FILE__)) + '/helper'

include OpenCV

class TestOpenCV < OpenCVTestCase
  def test_constants
    # OpenCV version
    assert(CV_VERSION =~ /\A\d+\.\d+\.\d+/)
    assert(CV_MAJOR_VERSION.is_a? Integer)
    assert(CV_MINOR_VERSION.is_a? Integer)
    assert(CV_SUBMINOR_VERSION.is_a? Integer)

    assert(CV_VERSION_MAJOR.is_a? Integer)
    assert(CV_VERSION_MINOR.is_a? Integer)
    assert(CV_VERSION_REVISION.is_a? Integer)

    # Depths
    assert_equal(0, CV_8U)
    assert_equal(1, CV_8S)
    assert_equal(2, CV_16U)
    assert_equal(3, CV_16S)
    assert_equal(4, CV_32S)
    assert_equal(5, CV_32F)
    assert_equal(6, CV_64F)

    assert_equal(0, CV_8UC1)
    assert_equal(8, CV_8UC2)
    assert_equal(16, CV_8UC3)
    assert_equal(24, CV_8UC4)
    assert_equal(1, CV_8SC1)
    assert_equal(9, CV_8SC2)
    assert_equal(17, CV_8SC3)
    assert_equal(25, CV_8SC4)
    assert_equal(2, CV_16UC1)
    assert_equal(10, CV_16UC2)
    assert_equal(18, CV_16UC3)
    assert_equal(26, CV_16UC4)
    assert_equal(3, CV_16SC1)
    assert_equal(11, CV_16SC2)
    assert_equal(19, CV_16SC3)
    assert_equal(27, CV_16SC4)
    assert_equal(4, CV_32SC1)
    assert_equal(12, CV_32SC2)
    assert_equal(20, CV_32SC3)
    assert_equal(28, CV_32SC4)
    assert_equal(5, CV_32FC1)
    assert_equal(13, CV_32FC2)
    assert_equal(21, CV_32FC3)
    assert_equal(29, CV_32FC4)
    assert_equal(6, CV_64FC1)
    assert_equal(14, CV_64FC2)
    assert_equal(22, CV_64FC3)
    assert_equal(30, CV_64FC4)

    # Load image flags
    assert_equal(-1, CV_LOAD_IMAGE_UNCHANGED)
    assert_equal(0, CV_LOAD_IMAGE_GRAYSCALE)
    assert_equal(1, CV_LOAD_IMAGE_COLOR)
    assert_equal(2, CV_LOAD_IMAGE_ANYDEPTH)
    assert_equal(4, CV_LOAD_IMAGE_ANYCOLOR)

    # Structuring element shapes    
    assert_equal(0, CV_SHAPE_RECT)
    assert_equal(1, CV_SHAPE_CROSS)
    assert_equal(2, CV_SHAPE_ELLIPSE)
    assert_equal(100, CV_SHAPE_CUSTOM)

    # Types of morphological operations
    assert_equal(2, CV_MOP_OPEN)
    assert_equal(3, CV_MOP_CLOSE)
    assert_equal(4, CV_MOP_GRADIENT)
    assert_equal(5, CV_MOP_TOPHAT)
    assert_equal(6, CV_MOP_BLACKHAT)

    # Types of the smoothing
    assert_equal(0, CV_BLUR_NO_SCALE)
    assert_equal(1, CV_BLUR)
    assert_equal(2, CV_GAUSSIAN)
    assert_equal(3, CV_MEDIAN)
    assert_equal(4, CV_BILATERAL)

    # Border types
    assert_equal(0, IPL_BORDER_CONSTANT)
    assert_equal(1, IPL_BORDER_REPLICATE)
    
    # Thresholding types
    assert_equal(0, CV_THRESH_BINARY)
    assert_equal(1, CV_THRESH_BINARY_INV)
    assert_equal(2, CV_THRESH_TRUNC)
    assert_equal(3, CV_THRESH_TOZERO)
    assert_equal(4, CV_THRESH_TOZERO_INV)
    assert_equal(8, CV_THRESH_OTSU)

    # Adaptive methods
    assert_equal(0, CV_ADAPTIVE_THRESH_MEAN_C)
    assert_equal(1, CV_ADAPTIVE_THRESH_GAUSSIAN_C)

    # Retrieval mode
    assert_equal(0, CV_RETR_EXTERNAL)
    assert_equal(1, CV_RETR_LIST)
    assert_equal(2, CV_RETR_CCOMP)
    assert_equal(3, CV_RETR_TREE)

    # Approximation method
    assert_equal(0, CV_CHAIN_CODE)
    assert_equal(1, CV_CHAIN_APPROX_NONE)
    assert_equal(2, CV_CHAIN_APPROX_SIMPLE)
    assert_equal(3, CV_CHAIN_APPROX_TC89_L1)
    assert_equal(4, CV_CHAIN_APPROX_TC89_KCOS)
    assert_equal(5, CV_LINK_RUNS)

    # Termination criteria for iterative algorithms
    assert_equal(1, CV_TERMCRIT_ITER)
    assert_equal(1, CV_TERMCRIT_NUMBER)
    assert_equal(2, CV_TERMCRIT_EPS)

    # Hough transform methods
    assert_equal(0, CV_HOUGH_STANDARD)
    assert_equal(1, CV_HOUGH_PROBABILISTIC)
    assert_equal(2, CV_HOUGH_MULTI_SCALE)
    assert_equal(3, CV_HOUGH_GRADIENT)

    # Inpaint method
    assert_equal(0, CV_INPAINT_NS)
    assert_equal(1, CV_INPAINT_TELEA)

    # Match tempalte method
    assert_equal(0, CV_TM_SQDIFF)
    assert_equal(1, CV_TM_SQDIFF_NORMED)
    assert_equal(2, CV_TM_CCORR)
    assert_equal(3, CV_TM_CCORR_NORMED)
    assert_equal(4, CV_TM_CCOEFF)
    assert_equal(5, CV_TM_CCOEFF_NORMED)
    
    # Comparison method
    assert_equal(1, CV_CONTOURS_MATCH_I1)
    assert_equal(2, CV_CONTOURS_MATCH_I2)
    assert_equal(3, CV_CONTOURS_MATCH_I3)

    # Fundamental matrix computing methods
    assert_equal(1, CV_FM_7POINT)
    assert_equal(2, CV_FM_8POINT)
    assert_equal(8, CV_FM_RANSAC)
    assert_equal(4, CV_FM_LMEDS)

    # Flags of window
    assert_equal(0, CV_WINDOW_NORMAL)
    assert_equal(1, CV_WINDOW_AUTOSIZE)
    assert_equal(4096, CV_WINDOW_OPENGL)

    # Object detection mode
    assert_equal(1, CV_HAAR_DO_CANNY_PRUNING)
    
    # Interpolation methods
    assert_equal(0, CV_INTER_NN)
    assert_equal(1, CV_INTER_LINEAR)
    assert_equal(2, CV_INTER_CUBIC)
    assert_equal(3, CV_INTER_AREA)
    
    # Warp affine optional flags
    assert_equal(8, CV_WARP_FILL_OUTLIERS)
    assert_equal(16, CV_WARP_INVERSE_MAP)

    # SVD operation flags
    assert_equal(1, CV_SVD_MODIFY_A)
    assert_equal(2, CV_SVD_U_T)
    assert_equal(4, CV_SVD_V_T)

    # Histogram representation format
    assert_equal(0, CV_HIST_ARRAY)
    assert_equal(1, CV_HIST_SPARSE)
    assert_equal(1, CV_HIST_TREE)
    assert_equal(1, CV_HIST_UNIFORM)

    # Histogram comparison method
    assert_equal(0, CV_COMP_CORREL)
    assert_equal(1, CV_COMP_CHISQR)
    assert_equal(2, CV_COMP_INTERSECT)
    assert_equal(3, CV_COMP_BHATTACHARYYA)
  end

  def test_symbols
    # Depths
    assert_equal(0, DEPTH[:cv8u])
    assert_equal(1, DEPTH[:cv8s])
    assert_equal(2, DEPTH[:cv16u])
    assert_equal(3, DEPTH[:cv16s])
    assert_equal(4, DEPTH[:cv32s])
    assert_equal(5, DEPTH[:cv32f])
    assert_equal(6, DEPTH[:cv64f])

    # Inversion methods
    assert_equal(0, INVERSION_METHOD[:lu])
    assert_equal(1, INVERSION_METHOD[:svd])
    assert_equal(2, INVERSION_METHOD[:svd_sym])
    assert_equal(2, INVERSION_METHOD[:svd_symmetric])

    # Homography calculation methods
    assert_equal(0, HOMOGRAPHY_CALC_METHOD[:all])
    assert_equal(4, HOMOGRAPHY_CALC_METHOD[:lmeds])
    assert_equal(8, HOMOGRAPHY_CALC_METHOD[:ransac])

    # Anti aliasing flags
    assert_equal(16, CONNECTIVITY[:aa])
    assert_equal(16, CONNECTIVITY[:anti_alias])

    # Retrieval modes
    assert_equal(0, RETRIEVAL_MODE[:external])
    assert_equal(1, RETRIEVAL_MODE[:list])
    assert_equal(2, RETRIEVAL_MODE[:ccomp])
    assert_equal(3, RETRIEVAL_MODE[:tree])

    # Approximation methods
    assert_equal(0, APPROX_CHAIN_METHOD[:code])
    assert_equal(1, APPROX_CHAIN_METHOD[:approx_none])
    assert_equal(2, APPROX_CHAIN_METHOD[:approx_simple])
    assert_equal(3, APPROX_CHAIN_METHOD[:approx_tc89_l1])
    assert_equal(4, APPROX_CHAIN_METHOD[:approx_tc89_kcos])

    # Approximation methods (polygon)
    assert_equal(0, APPROX_POLY_METHOD[:dp])

    # Match template methods
    assert_equal(0, MATCH_TEMPLATE_METHOD[:sqdiff])
    assert_equal(1, MATCH_TEMPLATE_METHOD[:sqdiff_normed])
    assert_equal(2, MATCH_TEMPLATE_METHOD[:ccorr])
    assert_equal(3, MATCH_TEMPLATE_METHOD[:ccorr_normed])
    assert_equal(4, MATCH_TEMPLATE_METHOD[:ccoeff])
    assert_equal(5, MATCH_TEMPLATE_METHOD[:ccoeff_normed])

    # Structuring element shapes
    assert_equal(0, STRUCTURING_ELEMENT_SHAPE[:rect])
    assert_equal(1, STRUCTURING_ELEMENT_SHAPE[:cross])
    assert_equal(2, STRUCTURING_ELEMENT_SHAPE[:ellipse])
    assert_equal(100, STRUCTURING_ELEMENT_SHAPE[:custom])

    # Types of morphological operations
    assert_equal(2, MORPHOLOGICAL_OPERATION[:open])
    assert_equal(3, MORPHOLOGICAL_OPERATION[:close])
    assert_equal(4, MORPHOLOGICAL_OPERATION[:gradient])
    assert_equal(5, MORPHOLOGICAL_OPERATION[:tophat])
    assert_equal(6, MORPHOLOGICAL_OPERATION[:blackhat])

    # Types of the smoothing
    assert_equal(0, SMOOTHING_TYPE[:blur_no_scale])
    assert_equal(1, SMOOTHING_TYPE[:blur])
    assert_equal(2, SMOOTHING_TYPE[:gaussian])
    assert_equal(3, SMOOTHING_TYPE[:median])
    assert_equal(4, SMOOTHING_TYPE[:bilateral])

    # Thresholding types
    assert_equal(0, THRESHOLD_TYPE[:binary])
    assert_equal(1, THRESHOLD_TYPE[:binary_inv])
    assert_equal(2, THRESHOLD_TYPE[:trunc])
    assert_equal(3, THRESHOLD_TYPE[:tozero])
    assert_equal(4, THRESHOLD_TYPE[:tozero_inv])
    assert_equal(8, THRESHOLD_TYPE[:otsu])

    # Hough transform methods
    assert_equal(0, HOUGH_TRANSFORM_METHOD[:standard])
    assert_equal(1, HOUGH_TRANSFORM_METHOD[:probabilistic])
    assert_equal(2, HOUGH_TRANSFORM_METHOD[:multi_scale])
    assert_equal(3, HOUGH_TRANSFORM_METHOD[:gradient])

    # Inpaint method
    assert_equal(0, INPAINT_METHOD[:ns])
    assert_equal(1, INPAINT_METHOD[:telea])

    # Comparison method
    assert_equal(1, COMPARISON_METHOD[:i1])
    assert_equal(2, COMPARISON_METHOD[:i2])
    assert_equal(3, COMPARISON_METHOD[:i3])
  end

  def test_cvt_color_funcs
    mat_1ch = CvMat.new(1, 1, :cv8u, 1)
    mat_1ch[0] = CvScalar.new(10)

    mat_3ch = CvMat.new(1, 1, :cv8u, 3)
    mat_3ch[0] = CvScalar.new(10, 20, 30)

    mat_4ch = CvMat.new(1, 1, :cv8u, 4)
    mat_4ch[0] = CvScalar.new(10, 20, 30, 40)

    gray_rgb = (0.299 * mat_3ch[0][0] + 0.587 * mat_3ch[0][1] + 0.114 * mat_3ch[0][2]).round
    gray_bgr = (0.299 * mat_3ch[0][2] + 0.587 * mat_3ch[0][1] + 0.114 * mat_3ch[0][0]).round

    # RGB(A) <=> RGB(A)
    [mat_3ch.BGR2BGRA, mat_3ch.RGB2RGBA].each { |m|
      assert_equal(CvMat, m.class)
      assert_equal(4, m.channel)
      assert_cvscalar_equal(CvScalar.new(10, 20, 30, 255), m[0])
    }
    [mat_3ch.BGR2RGBA, mat_3ch.RGB2BGRA].each { |m|
      assert_equal(CvMat, m.class)
      assert_equal(4, m.channel)
      assert_cvscalar_equal(CvScalar.new(30, 20, 10, 255), m[0])
    }
    [mat_4ch.BGRA2BGR, mat_4ch.RGBA2RGB].each { |m|
      assert_equal(CvMat, m.class)
      assert_equal(3, m.channel)
      assert_cvscalar_equal(CvScalar.new(10, 20, 30, 0), m[0])
    }
    [mat_4ch.RGBA2BGR, mat_4ch.BGRA2RGB].each { |m|
      assert_equal(CvMat, m.class)
      assert_equal(3, m.channel)
      assert_cvscalar_equal(CvScalar.new(30, 20, 10, 0), m[0])
    }
    [mat_3ch.BGR2RGB, mat_3ch.RGB2BGR].each { |m|
      assert_equal(CvMat, m.class)
      assert_equal(3, m.channel)
      assert_cvscalar_equal(CvScalar.new(30, 20, 10, 0), m[0])
    }
    [mat_4ch.BGRA2RGBA, mat_4ch.RGBA2BGRA].each { |m|
      assert_equal(CvMat, m.class)
      assert_equal(4, m.channel)
      assert_cvscalar_equal(CvScalar.new(30, 20, 10, 40), m[0])
    }

    # RGB <=> GRAY
    [mat_3ch.BGR2GRAY, mat_4ch.BGRA2GRAY].each { |m|
      assert_equal(CvMat, m.class)
      assert_equal(1, m.channel)
      assert_cvscalar_equal(CvScalar.new(gray_bgr, 0, 0, 0), m[0])
    }
    [mat_3ch.RGB2GRAY, mat_4ch.RGBA2GRAY].each { |m|
      assert_equal(CvMat, m.class)
      assert_equal(1, m.channel)
      assert_cvscalar_equal(CvScalar.new(gray_rgb, 0, 0, 0), m[0])
    }
    [mat_1ch.GRAY2BGR, mat_1ch.GRAY2RGB].each { |m|
      assert_equal(CvMat, m.class)
      assert_equal(3, m.channel)
      assert_cvscalar_equal(CvScalar.new(10, 10, 10, 0), m[0])
    }
    [mat_1ch.GRAY2BGRA, mat_1ch.GRAY2RGBA].each { |m|
      assert_equal(CvMat, m.class)
      assert_equal(4, m.channel)
      assert_cvscalar_equal(CvScalar.new(10, 10, 10, 255), m[0])
    }

    img_3ch = IplImage.new(1, 1, :cv8u, 3)
    assert_equal(IplImage, img_3ch.BGR2GRAY.class)
  end

  def test_build_information
    s = build_information
    assert_equal(String, s.class)
    assert(s =~ /^\s+General configuration for OpenCV #{CV_VERSION}/)
  end
end


