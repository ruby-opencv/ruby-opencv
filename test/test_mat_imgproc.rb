#!/usr/bin/env ruby
# -*- mode: ruby; coding: utf-8 -*-
require 'opencv'
require File.expand_path(File.dirname(__FILE__)) + '/helper'

include Cv

class TestCvMat < OpenCVTestCase
  def test_resize
    m0 = Mat.ones(200, 300, CV_8U)
    s = Size.new(150, 100)

    m = m0.resize(s)
    assert_equal(s.height, m.rows)
    assert_equal(s.width, m.cols)
    assert_equal(m0.depth, m.depth)
    assert_equal(m0.dims, m.dims)
    assert_equal(m0.channels, m.channels)

    [INTER_NEAREST, INTER_LINEAR, INTER_AREA,
     INTER_CUBIC, INTER_LANCZOS4].each { |interpolation|
      m = m0.resize(s, interpolation)
      assert_equal(s.height, m.rows)
      assert_equal(s.width, m.cols)
      assert_equal(m0.depth, m.depth)
      assert_equal(m0.dims, m.dims)
      assert_equal(m0.channels, m.channels)
    }

    assert_raise(TypeError) {
      m.resize(DUMMY_OBJ)
    }
  end

  def test_cvt_color
    m = Mat.new(1, 1, CV_32FC3)
    m[0, 0] = Scalar.new(1.0, 2.0, 3.0)
    delta = 0.01

    m2 = m.cvt_color(COLOR_BGR2GRAY)
    assert_in_delta(2.1849999, m2[0, 0][0], delta)

    m2 = m.cvt_color(COLOR_BGR2HSV)
    [30, 0.66666669, 3].each_with_index { |expected, i|
      assert_in_delta(expected, m2[0, 0][i], delta, "Failed at m2[0, 0][#{i}]")
    }

    assert_raise(TypeError) {
      m.cvt_color(DUMMY_OBJ)
    }
  end

  def test_cvt_color_bang
    m0 = Mat.new(1, 1, CV_32FC3)
    m0[0, 0] = Scalar.new(1.0, 2.0, 3.0)
    delta = 0.01

    m = m0.clone
    m.cvt_color!(COLOR_BGR2GRAY)
    assert_in_delta(2.1849999, m[0, 0][0], delta)

    m = m0.clone
    m.cvt_color!(COLOR_BGR2HSV)
    [30, 0.66666669, 3].each_with_index { |expected, i|
      assert_in_delta(expected, m[0, 0][i], delta, "Failed at m[0, 0][#{i}]")
    }

    assert_raise(TypeError) {
      m.cvt_color!(DUMMY_OBJ)
    }
  end

  def test_resize_bang
    m0 = Mat.ones(200, 300, CV_8U)
    m = m0.clone
    s = Size.new(150, 100)

    m.resize!(s)
    assert_equal(s.height, m.rows)
    assert_equal(s.width, m.cols)
    assert_equal(m0.depth, m.depth)
    assert_equal(m0.dims, m.dims)
    assert_equal(m0.channels, m.channels)

    [INTER_NEAREST, INTER_LINEAR, INTER_AREA,
     INTER_CUBIC, INTER_LANCZOS4].each { |interpolation|
      m = m0.clone
      m.resize!(s, interpolation)
      assert_equal(s.height, m.rows)
      assert_equal(s.width, m.cols)
      assert_equal(m0.depth, m.depth)
      assert_equal(m0.dims, m.dims)
      assert_equal(m0.channels, m.channels)
    }

    assert_raise(TypeError) {
      m.resize!(DUMMY_OBJ)
    }
  end

  def test_sobel
    m0 = Cv::imread(FILENAME_LENA256x256, 0)

    sobel = []
    sobel << m0.sobel(CV_8U, 1, 1)
    sobel << m0.sobel(CV_8U, 1, 1, 5, 0.5, 32, BORDER_CONSTANT)
    sobel.each { |m|
      assert_equal(m0.rows, m.rows)
      assert_equal(m0.cols, m.cols)
      assert_equal(CV_8U, m.depth)
      assert_equal(m0.dims, m.dims)
      assert_equal(m0.channels, m.channels)
    }

    assert_raise(TypeError) {
      m0.sobel(DUMMY_OBJ, 1, 1, 5, 0.5, 32, BORDER_CONSTANT)
    }
    assert_raise(TypeError) {
      m0.sobel(CV_8U, DUMMY_OBJ, 1, 5, 0.5, 32, BORDER_CONSTANT)
    }
    assert_raise(TypeError) {
      m0.sobel(CV_8U, 1, DUMMY_OBJ, 5, 0.5, 32, BORDER_CONSTANT)
    }
    assert_raise(TypeError) {
      m0.sobel(CV_8U, 1, 1, DUMMY_OBJ, 0.5, 32, BORDER_CONSTANT)
    }
    assert_raise(TypeError) {
      m0.sobel(CV_8U, 1, 1, 5, DUMMY_OBJ, 32, BORDER_CONSTANT)
    }
    assert_raise(TypeError) {
      m0.sobel(CV_8U, 1, 1, 5, 0.5, DUMMY_OBJ, BORDER_CONSTANT)
    }
    assert_raise(TypeError) {
      m0.sobel(CV_8U, 1, 1, 5, 0.5, 32, DUMMY_OBJ)
    }

    # w = Window.new('Sobel')
    # w.show(m0.sobel(CV_8U, 1, 1))
    # Cv::wait_key
  end

  def test_sobel_bang
    m0 = Cv::imread(FILENAME_LENA256x256, 0)
    m1 = m0.clone

    m0.sobel!(CV_8U, 1, 1)
    m1.sobel!(CV_8U, 1, 1, 5, 0.5, 32, BORDER_CONSTANT)
    sobel = [m0, m1]
    sobel.each { |m|
      assert_equal(m0.rows, m.rows)
      assert_equal(m0.cols, m.cols)
      assert_equal(CV_8U, m.depth)
      assert_equal(m0.dims, m.dims)
      assert_equal(m0.channels, m.channels)
    }

    assert_raise(TypeError) {
      m0.sobel!(DUMMY_OBJ, 1, 1, 5, 0.5, 32, BORDER_CONSTANT)
    }
    assert_raise(TypeError) {
      m0.sobel!(CV_8U, DUMMY_OBJ, 1, 5, 0.5, 32, BORDER_CONSTANT)
    }
    assert_raise(TypeError) {
      m0.sobel!(CV_8U, 1, DUMMY_OBJ, 5, 0.5, 32, BORDER_CONSTANT)
    }
    assert_raise(TypeError) {
      m0.sobel!(CV_8U, 1, 1, DUMMY_OBJ, 0.5, 32, BORDER_CONSTANT)
    }
    assert_raise(TypeError) {
      m0.sobel!(CV_8U, 1, 1, 5, DUMMY_OBJ, 32, BORDER_CONSTANT)
    }
    assert_raise(TypeError) {
      m0.sobel!(CV_8U, 1, 1, 5, 0.5, DUMMY_OBJ, BORDER_CONSTANT)
    }
    assert_raise(TypeError) {
      m0.sobel!(CV_8U, 1, 1, 5, 0.5, 32, DUMMY_OBJ)
    }

    # w = Window.new('Sobel')
    # m0 = Cv::imread(FILENAME_LENA256x256, 0)
    # m0.sobel!(CV_8U, 1, 1)
    # w.show(m0)
    # Cv::wait_key
  end

  def test_canny
    m0 = Cv::imread(FILENAME_LENA256x256, 0)

    canny = []
    canny << m0.canny(50, 200)
    canny << m0.canny(50, 200, 5, true)
    canny.each { |m|
      assert_equal(m0.rows, m.rows)
      assert_equal(m0.cols, m.cols)
      assert_equal(m0.depth, m.depth)
      assert_equal(m0.dims, m.dims)
      assert_equal(m0.channels, m.channels)
    }

    assert_raise(TypeError) {
      m0.canny(DUMMY_OBJ, 200, 5, true)
    }
    assert_raise(TypeError) {
      m0.canny(50, DUMMY_OBJ, 5, true)
    }
    assert_raise(TypeError) {
      m0.canny(50, 200, DUMMY_OBJ, true)
    }
    assert_nothing_raised {
      m0.canny(50, 200, 5, DUMMY_OBJ)
    }

    # w = Window.new('Canny')
    # w.show(m0.canny(50, 200))
    # Cv::wait_key
  end

  def test_canny_bang
    m0 = Cv::imread(FILENAME_LENA256x256, 0)
    m1 = m0.clone
    m0.canny!(50, 200)
    m1.canny!(50, 200, 5, true)

    canny = [m0, m1]
    canny.each { |m|
      assert_equal(m0.rows, m.rows)
      assert_equal(m0.cols, m.cols)
      assert_equal(m0.depth, m.depth)
      assert_equal(m0.dims, m.dims)
      assert_equal(m0.channels, m.channels)
    }

    assert_raise(TypeError) {
      m0.canny!(DUMMY_OBJ, 200, 5, true)
    }
    assert_raise(TypeError) {
      m0.canny!(50, DUMMY_OBJ, 5, true)
    }
    assert_raise(TypeError) {
      m0.canny!(50, 200, DUMMY_OBJ, true)
    }
    assert_nothing_raised {
      m0.canny!(50, 200, 5, DUMMY_OBJ)
    }

    # w = Window.new('Canny')
    # m0 = Cv::imread(FILENAME_LENA256x256, 0)
    # m0.canny!(50, 200)
    # w.show(m0)
    # Cv::wait_key
  end

  def test_laplacian
    m0 = Cv::imread(FILENAME_LENA256x256, 0)

    laplacian = []
    laplacian << m0.laplacian(CV_8U)
    laplacian << m0.laplacian(CV_8U, 5, 0.5, 32, BORDER_CONSTANT)
    laplacian.each { |m|
      assert_equal(m0.rows, m.rows)
      assert_equal(m0.cols, m.cols)
      assert_equal(CV_8U, m.depth)
      assert_equal(m0.dims, m.dims)
      assert_equal(m0.channels, m.channels)
    }

    assert_raise(TypeError) {
      m0.laplacian(DUMMY_OBJ, 5, 0.5, 32, BORDER_CONSTANT)
    }
    assert_raise(TypeError) {
      m0.laplacian(CV_8U, DUMMY_OBJ, 0.5, 32, BORDER_CONSTANT)
    }
    assert_raise(TypeError) {
      m0.laplacian(CV_8U, 5, DUMMY_OBJ, 32, BORDER_CONSTANT)
    }
    assert_raise(TypeError) {
      m0.laplacian(CV_8U, 5, 0.5, DUMMY_OBJ, BORDER_CONSTANT)
    }
    assert_raise(TypeError) {
      m0.laplacian(CV_8U, 5, 0.5, 32, DUMMY_OBJ)
    }

    # w = Window.new('Laplacian')
    # w.show(m0.laplacian(CV_8U))
    # Cv::wait_key
  end

  def test_laplacian_bang
    m0 = Cv::imread(FILENAME_LENA256x256, 0)
    m1 = m0.clone
    m0.laplacian!(CV_8U)
    m1.laplacian!(CV_8U, 5, 0.5, 32, BORDER_CONSTANT)

    laplacian = [m0, m1]
    laplacian.each { |m|
      assert_equal(m0.rows, m.rows)
      assert_equal(m0.cols, m.cols)
      assert_equal(CV_8U, m.depth)
      assert_equal(m0.dims, m.dims)
      assert_equal(m0.channels, m.channels)
    }

    assert_raise(TypeError) {
      m0.laplacian!(DUMMY_OBJ, 5, 0.5, 32, BORDER_CONSTANT)
    }
    assert_raise(TypeError) {
      m0.laplacian!(CV_8U, DUMMY_OBJ, 0.5, 32, BORDER_CONSTANT)
    }
    assert_raise(TypeError) {
      m0.laplacian!(CV_8U, 5, DUMMY_OBJ, 32, BORDER_CONSTANT)
    }
    assert_raise(TypeError) {
      m0.laplacian!(CV_8U, 5, 0.5, DUMMY_OBJ, BORDER_CONSTANT)
    }
    assert_raise(TypeError) {
      m0.laplacian!(CV_8U, 5, 0.5, 32, DUMMY_OBJ)
    }

    # w = Window.new('Laplacian')
    # m0 = Cv::imread(FILENAME_LENA256x256, 0)
    # m0.laplacian!(CV_8U)
    # w.show(m0)
    # Cv::wait_key
  end

  def test_add_weighted
    m0 = Mat.ones(3, 3, CV_32F) * 32
    m1 = Mat.ones(3, 3, CV_32F) * 64

    results = []
    results << Cv::add_weighted(m0, 0.5, m1, 0.5, 0)
    results << Cv::add_weighted(m0, 0.5, m1, 0.5, 32, CV_32F)
    results.each { |m|
      assert_equal(m0.rows, m.rows)
      assert_equal(m0.cols, m.cols)
      assert_equal(CV_32F, m.depth)
      assert_equal(m0.dims, m.dims)
      assert_equal(m0.channels, m.channels)
    }

    assert_raise(TypeError) {
      Cv::add_weighted(DUMMY_OBJ, 0.5, m1, 0.5, 32, CV_32F)
    }
    assert_raise(TypeError) {
      Cv::add_weighted(m0, DUMMY_OBJ, m1, 0.5, 32, CV_32F)
    }
    assert_raise(TypeError) {
      Cv::add_weighted(m0, 0.5, DUMMY_OBJ, 0.5, 32, CV_32F)
    }
    assert_raise(TypeError) {
      Cv::add_weighted(m0, 0.5, m1, DUMMY_OBJ, 32, CV_32F)
    }
    assert_raise(TypeError) {
      Cv::add_weighted(m0, 0.5, m1, 0.5, DUMMY_OBJ, CV_32F)
    }
    assert_raise(TypeError) {
      Cv::add_weighted(m0, 0.5, m1, 0.5, 32, DUMMY_OBJ)
    }
  end

  def test_blur
    m0 = Cv::imread(FILENAME_LENA256x256, -1)

    ksize = Cv::Size.new(3, 3)
    anchor = Cv::Point.new(1, 1)
    blurs = []
    blurs << m0.blur(ksize)
    blurs << m0.blur(ksize, anchor)
    blurs << m0.blur(ksize, anchor, Cv::BORDER_REPLICATE)
    blurs.each { |m|
      assert_equal(m0.rows, m.rows)
      assert_equal(m0.cols, m.cols)
      assert_equal(m0.depth, m.depth)
      assert_equal(m0.dims, m.dims)
      assert_equal(m0.channels, m.channels)
    }

    assert_raise(TypeError) {
      m0.blur(DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      m0.blur(ksize, DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      m0.blur(ksize, anchor, DUMMY_OBJ)
    }

    # w = Window.new('Blur')
    # w.show(m0.blur(ksize, anchor, Cv::BORDER_REPLICATE))
    # Cv::wait_key
  end

  def test_blur_bang
    m0 = Cv::imread(FILENAME_LENA256x256, -1)
    m1 = m0.clone
    m2 = m0.clone
    m3 = m0.clone

    ksize = Cv::Size.new(3, 3)
    anchor = Cv::Point.new(1, 1)
    m0.blur!(ksize)
    m0.blur!(ksize, anchor)
    m0.blur!(ksize, anchor, Cv::BORDER_REPLICATE)

    blurs = [m1, m2, m3]
    blurs.each { |m|
      assert_equal(m0.rows, m.rows)
      assert_equal(m0.cols, m.cols)
      assert_equal(m0.depth, m.depth)
      assert_equal(m0.dims, m.dims)
      assert_equal(m0.channels, m.channels)
    }

    assert_raise(TypeError) {
      m0.blur!(DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      m0.blur!(ksize, DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      m0.blur!(ksize, anchor, DUMMY_OBJ)
    }

    # w = Window.new('Blur')
    # m0 = Cv::imread(FILENAME_LENA256x256, -1)
    # m0.blur!(ksize, anchor, Cv::BORDER_REPLICATE)
    # w.show(m0)
    # Cv::wait_key
  end

  def test_gaussian_blur
    m0 = Cv::imread(FILENAME_LENA256x256, -1)

    ksize = Cv::Size.new(3, 3)
    gaussian_blurs = []
    gaussian_blurs << m0.gaussian_blur(ksize, 3)
    gaussian_blurs << m0.gaussian_blur(ksize, 3, 5)
    gaussian_blurs << m0.gaussian_blur(ksize, 3, 5, Cv::BORDER_REPLICATE)
    gaussian_blurs.each { |m|
      assert_equal(m0.rows, m.rows)
      assert_equal(m0.cols, m.cols)
      assert_equal(m0.depth, m.depth)
      assert_equal(m0.dims, m.dims)
      assert_equal(m0.channels, m.channels)
    }

    assert_raise(TypeError) {
      m0.gaussian_blur(ksize, DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      m0.gaussian_blur(ksize, 3, DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      m0.gaussian_blur(ksize, 3, 5, DUMMY_OBJ)
    }

    # w = Window.new('Gaussian blur')
    # w.show(m0.gaussian_blur(ksize, 3, 5, Cv::BORDER_REPLICATE))
    # Cv::wait_key
  end

  def test_median_blur
    m0 = Cv::Mat.ones(3, 3, Cv::CV_8U)
    m0[1, 1] = Cv::Scalar.new(0)
    m1 = m0.median_blur(3)

    assert_equal(m0.class, m1.class)
    assert_equal(m0.rows, m1.rows)
    assert_equal(m0.cols, m1.cols)
    assert_equal(m0.depth, m1.depth)
    assert_equal(m0.dims, m1.dims)
    assert_equal(m0.channels, m1.channels)
    m1.rows.times { |r|
      m1.cols.times { |c|
        assert_equal(1, m1[r, c][0].to_i)
      }
    }

    assert_raise(TypeError) {
      m0.median_blur(DUMMY_OBJ)
    }

    # m = Cv::imread(FILENAME_LENA256x256, -1)
    # m2 = m.median_blur(9)
    # snap(['Original', m], ['Median blur', m2])
  end

  def test_threshold
    m0 = Cv::Mat.zeros(2, 2, Cv::CV_8U)
    m0[0, 0] = Cv::Scalar.new(10)
    m0[0, 1] = Cv::Scalar.new(20)
    m0[1, 0] = Cv::Scalar.new(30)
    m0[1, 1] = Cv::Scalar.new(40)

    m = m0.threshold(25, 255, Cv::THRESH_BINARY)
    expected = "<Cv::Mat:2x2,depth=0,channels=1,\n[  0,   0;\n 255, 255]>"
    assert_equal(expected, m.to_s)

    m = m0.threshold(25, 255, Cv::THRESH_BINARY_INV)
    expected = "<Cv::Mat:2x2,depth=0,channels=1,\n[255, 255;\n   0,   0]>"
    assert_equal(expected, m.to_s)

    m = m0.threshold(25, 255, Cv::THRESH_TRUNC)
    expected = "<Cv::Mat:2x2,depth=0,channels=1,\n[ 10,  20;\n  25,  25]>"
    assert_equal(expected, m.to_s)

    m = m0.threshold(25, 255, Cv::THRESH_TOZERO)
    expected = "<Cv::Mat:2x2,depth=0,channels=1,\n[  0,   0;\n  30,  40]>"
    assert_equal(expected, m.to_s)

    m = m0.threshold(25, 255, Cv::THRESH_TOZERO_INV)
    expected = "<Cv::Mat:2x2,depth=0,channels=1,\n[ 10,  20;\n   0,   0]>"
    assert_equal(expected, m.to_s)

    m, optimal_threshold = m0.threshold(25, 255, Cv::THRESH_BINARY | Cv::THRESH_OTSU)
    expected = "<Cv::Mat:2x2,depth=0,channels=1,\n[  0,   0;\n 255, 255]>"
    assert_equal(expected, m.to_s)
    assert_in_delta(20, optimal_threshold, 0.1)

    m, optimal_threshold = m0.threshold(25, 255, Cv::THRESH_BINARY | Cv::THRESH_TRIANGLE)
    expected = "<Cv::Mat:2x2,depth=0,channels=1,\n[  0, 255;\n 255, 255]>"
    assert_equal(expected, m.to_s)
    assert_in_delta(12, optimal_threshold, 0.1)

    assert_raise(TypeError) {
      m0.threshold(DUMMY_OBJ, 255, Cv::THRESH_BINARY)
    }
    assert_raise(TypeError) {
      m0.threshold(25, DUMMY_OBJ, Cv::THRESH_BINARY)
    }
    assert_raise(TypeError) {
      m0.threshold(25, 255, DUMMY_OBJ)
    }

    # m0 = Cv::imread(FILENAME_LENA256x256, 0)
    # m = m0.threshold(127, 255, Cv::THRESH_BINARY)
    # w = Window.new('Original | Binary')
    # w.show(Cv::hconcat([m0, m]))
    # Cv::wait_key
  end

  def test_adaptive_threshold
    m0 = Cv::Mat.new(2, 2, Cv::CV_8U)
    m0[0, 0] = Cv::Scalar.new(10)
    m0[0, 1] = Cv::Scalar.new(20)
    m0[1, 0] = Cv::Scalar.new(30)
    m0[1, 1] = Cv::Scalar.new(40)

    expected = "<Cv::Mat:2x2,depth=0,channels=1,\n[  0,   0;\n 255, 255]>"
    m = m0.adaptive_threshold(255, Cv::ADAPTIVE_THRESH_MEAN_C, Cv::THRESH_BINARY, 3, 0)
    assert_equal(expected, m.to_s)

    m = m0.adaptive_threshold(255, Cv::ADAPTIVE_THRESH_GAUSSIAN_C, Cv::THRESH_BINARY, 3, 0)
    assert_equal(expected, m.to_s)

    assert_raise(TypeError) {
      m0.adaptive_threshold(DUMMY_OBJ, Cv::ADAPTIVE_THRESH_MEAN_C, Cv::THRESH_BINARY, 3, 0)
    }
    assert_raise(TypeError) {
      m0.adaptive_threshold(255, DUMMY_OBJ, Cv::THRESH_BINARY, 3, 0)
    }
    assert_raise(TypeError) {
      m0.adaptive_threshold(255, Cv::ADAPTIVE_THRESH_MEAN_C, DUMMY_OBJ, 3, 0)
    }
    assert_raise(TypeError) {
      m0.adaptive_threshold(DUMMY_OBJ, Cv::ADAPTIVE_THRESH_MEAN_C, Cv::THRESH_BINARY, DUMMY_OBJ, 0)
    }
    assert_raise(TypeError) {
      m0.adaptive_threshold(DUMMY_OBJ, Cv::ADAPTIVE_THRESH_MEAN_C, Cv::THRESH_BINARY, 3, DUMMY_OBJ)
    }

    # m0 = Cv::imread(FILENAME_LENA256x256, 0)
    # m = m0.adaptive_threshold(255, Cv::ADAPTIVE_THRESH_MEAN_C, Cv::THRESH_BINARY, 25, 0)
    # w = Window.new('Original | Binary')
    # w.show(Cv::hconcat([m0, m]))
    # Cv::wait_key
  end
end
