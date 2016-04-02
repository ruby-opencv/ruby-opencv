#!/usr/bin/env ruby
# -*- mode: ruby; coding: utf-8 -*-
require 'opencv'
require File.expand_path(File.dirname(__FILE__)) + '/helper'

include OpenCV

class TestOpenCV < OpenCVTestCase
  def test_imread
    mat = OpenCV::imread(FILENAME_CAT, IMREAD_GRAYSCALE)
    assert_equal(Mat, mat.class)
    assert_equal(375, mat.cols)
    assert_equal(500, mat.rows)
    assert_equal(CV_8U, mat.depth)
    assert_equal(1, mat.channels)

    mat = OpenCV::imread(FILENAME_CAT, IMREAD_ANYDEPTH | IMREAD_ANYCOLOR)
    assert_equal(Mat, mat.class)
    assert_equal(375, mat.cols)
    assert_equal(500, mat.rows)
    assert_equal(CV_8U, mat.depth)
    assert_equal(3, mat.channels)

    assert_raise(ArgumentError) {
      OpenCV::imread
    }
    assert_raise(ArgumentError) {
      OpenCV::imread(FILENAME_CAT)
    }
    assert_raise(TypeError) {
      OpenCV::imread(FILENAME_CAT, DUMMY_OBJ)
    }
    assert_raise(StandardError) {
      OpenCV::imread('file/does/not/exist', 0)
    }
  end

  def test_imdecode
    src = OpenCV::imread(FILENAME_LENA32x32, IMREAD_ANYDEPTH | IMREAD_ANYCOLOR)
    buf = src.imencode('.jpg')

    m = OpenCV::imdecode(buf, IMREAD_ANYDEPTH | IMREAD_ANYCOLOR)
    assert_equal(src.class, m.class)
    assert_equal(src.rows, m.rows)
    assert_equal(src.cols, m.cols)
    assert_equal(src.depth, m.depth)
    assert_equal(src.channels, m.channels)

    m = OpenCV::imdecode(buf, IMREAD_GRAYSCALE)
    assert_equal(src.class, m.class)
    assert_equal(src.rows, m.rows)
    assert_equal(src.cols, m.cols)
    assert_equal(CV_8U, m.depth)
    assert_equal(1, m.channels)

    assert_raise(TypeError) {
      OpenCV::imdecode(DUMMY_OBJ, IMREAD_GRAYSCALE)
    }
    assert_raise(TypeError) {
      OpenCV::imdecode(buf, DUMMY_OBJ)
    }

    # w = Window.new('Decoded')
    # w.show(m)
    # w.wait_key
  end
end

