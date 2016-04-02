#!/usr/bin/env ruby
# -*- mode: ruby; coding: utf-8 -*-
require 'opencv'
require File.expand_path(File.dirname(__FILE__)) + '/helper'

include OpenCV

class TestCvMat < OpenCVTestCase
  def test_sobel
    m0 = OpenCV::imread(FILENAME_LENA256x256, 0)

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
    # OpenCV::wait_key
  end

  def test_canny
    m0 = OpenCV::imread(FILENAME_LENA256x256, 0)

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
    # OpenCV::wait_key
  end

  def test_laplacian
    m0 = OpenCV::imread(FILENAME_LENA256x256, 0)

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
    # OpenCV::wait_key
  end

  def test_add_weighted
    m0 = Mat.ones(3, 3, CV_32F) * 32
    m1 = Mat.ones(3, 3, CV_32F) * 64

    results = []
    results << OpenCV::add_weighted(m0, 0.5, m1, 0.5, 0)
    results << OpenCV::add_weighted(m0, 0.5, m1, 0.5, 32, CV_32F)
    results.each { |m|
      assert_equal(m0.rows, m.rows)
      assert_equal(m0.cols, m.cols)
      assert_equal(CV_32F, m.depth)
      assert_equal(m0.dims, m.dims)
      assert_equal(m0.channels, m.channels)
    }

    assert_raise(TypeError) {
      OpenCV::add_weighted(DUMMY_OBJ, 0.5, m1, 0.5, 32, CV_32F)
    }
    assert_raise(TypeError) {
      OpenCV::add_weighted(m0, DUMMY_OBJ, m1, 0.5, 32, CV_32F)
    }
    assert_raise(TypeError) {
      OpenCV::add_weighted(m0, 0.5, DUMMY_OBJ, 0.5, 32, CV_32F)
    }
    assert_raise(TypeError) {
      OpenCV::add_weighted(m0, 0.5, m1, DUMMY_OBJ, 32, CV_32F)
    }
    assert_raise(TypeError) {
      OpenCV::add_weighted(m0, 0.5, m1, 0.5, DUMMY_OBJ, CV_32F)
    }
    assert_raise(TypeError) {
      OpenCV::add_weighted(m0, 0.5, m1, 0.5, 32, DUMMY_OBJ)
    }
  end
end
