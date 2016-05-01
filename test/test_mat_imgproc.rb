#!/usr/bin/env ruby
# -*- mode: ruby; coding: utf-8 -*-
require 'opencv'
require File.expand_path(File.dirname(__FILE__)) + '/helper'

include Cv

class TestCvMat < OpenCVTestCase
  def test_sobel
    m0 = Cv::imread(FILENAME_LENA256x256, 0)

    sobel = []
    sobel << m0.sobel(CV_32F, 1, 1)
    sobel << m0.sobel(CV_32F, 1, 1, 5, 0.5, 32, BORDER_CONSTANT)
    sobel.each { |m|
      assert_equal(m0.rows, m.rows)
      assert_equal(m0.cols, m.cols)
      assert_equal(CV_32F, m.depth)
      assert_equal(m0.dims, m.dims)
      assert_equal(m0.channels, m.channels)
    }

    assert_raise(TypeError) {
      m0.sobel(DUMMY_OBJ, 1, 1, 5, 0.5, 32, BORDER_CONSTANT)
    }
    assert_raise(TypeError) {
      m0.sobel(CV_32F, DUMMY_OBJ, 1, 5, 0.5, 32, BORDER_CONSTANT)
    }
    assert_raise(TypeError) {
      m0.sobel(CV_32F, 1, DUMMY_OBJ, 5, 0.5, 32, BORDER_CONSTANT)
    }
    assert_raise(TypeError) {
      m0.sobel(CV_32F, 1, 1, DUMMY_OBJ, 0.5, 32, BORDER_CONSTANT)
    }
    assert_raise(TypeError) {
      m0.sobel(CV_32F, 1, 1, 5, DUMMY_OBJ, 32, BORDER_CONSTANT)
    }
    assert_raise(TypeError) {
      m0.sobel(CV_32F, 1, 1, 5, 0.5, DUMMY_OBJ, BORDER_CONSTANT)
    }
    assert_raise(TypeError) {
      m0.sobel(CV_32F, 1, 1, 5, 0.5, 32, DUMMY_OBJ)
    }

    # w = Window.new('Sobel')
    # w.show(m0.sobel(CV_32F, 1, 1))
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

  def test_laplacian
    m0 = Cv::imread(FILENAME_LENA256x256, 0)

    laplacian = []
    laplacian << m0.laplacian(CV_32F)
    laplacian << m0.laplacian(CV_32F, 5, 0.5, 32, BORDER_CONSTANT)
    laplacian.each { |m|
      assert_equal(m0.rows, m.rows)
      assert_equal(m0.cols, m.cols)
      assert_equal(CV_32F, m.depth)
      assert_equal(m0.dims, m.dims)
      assert_equal(m0.channels, m.channels)
    }

    assert_raise(TypeError) {
      m0.laplacian(DUMMY_OBJ, 5, 0.5, 32, BORDER_CONSTANT)
    }
    assert_raise(TypeError) {
      m0.laplacian(CV_32F, DUMMY_OBJ, 0.5, 32, BORDER_CONSTANT)
    }
    assert_raise(TypeError) {
      m0.laplacian(CV_32F, 5, DUMMY_OBJ, 32, BORDER_CONSTANT)
    }
    assert_raise(TypeError) {
      m0.laplacian(CV_32F, 5, 0.5, DUMMY_OBJ, BORDER_CONSTANT)
    }
    assert_raise(TypeError) {
      m0.laplacian(CV_32F, 5, 0.5, 32, DUMMY_OBJ)
    }

    # w = Window.new('Laplacian')
    # w.show(m0.laplacian(CV_32F))
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
end
