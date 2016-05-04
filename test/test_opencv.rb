#!/usr/bin/env ruby
# -*- mode: ruby; coding: utf-8 -*-
require 'opencv'
require File.expand_path(File.dirname(__FILE__)) + '/helper'

include Cv

class TestOpenCV < OpenCVTestCase
  def test_imread
    mat = Cv::imread(FILENAME_CAT, IMREAD_GRAYSCALE)
    assert_equal(Mat, mat.class)
    assert_equal(375, mat.cols)
    assert_equal(500, mat.rows)
    assert_equal(CV_8U, mat.depth)
    assert_equal(1, mat.channels)

    mat = Cv::imread(FILENAME_CAT, IMREAD_ANYDEPTH | IMREAD_ANYCOLOR)
    assert_equal(Mat, mat.class)
    assert_equal(375, mat.cols)
    assert_equal(500, mat.rows)
    assert_equal(CV_8U, mat.depth)
    assert_equal(3, mat.channels)

    assert_raise(ArgumentError) {
      Cv::imread
    }
    assert_raise(ArgumentError) {
      Cv::imread(FILENAME_CAT)
    }
    assert_raise(TypeError) {
      Cv::imread(FILENAME_CAT, DUMMY_OBJ)
    }
    assert_raise(StandardError) {
      Cv::imread('file/does/not/exist', 0)
    }
  end

  def test_imdecode
    src = Cv::imread(FILENAME_LENA32x32, IMREAD_ANYDEPTH | IMREAD_ANYCOLOR)
    buf = src.imencode('.jpg')

    m = Cv::imdecode(buf, IMREAD_ANYDEPTH | IMREAD_ANYCOLOR)
    assert_equal(src.class, m.class)
    assert_equal(src.rows, m.rows)
    assert_equal(src.cols, m.cols)
    assert_equal(src.depth, m.depth)
    assert_equal(src.channels, m.channels)

    m = Cv::imdecode(buf, IMREAD_GRAYSCALE)
    assert_equal(src.class, m.class)
    assert_equal(src.rows, m.rows)
    assert_equal(src.cols, m.cols)
    assert_equal(CV_8U, m.depth)
    assert_equal(1, m.channels)

    assert_raise(TypeError) {
      Cv::imdecode(DUMMY_OBJ, IMREAD_GRAYSCALE)
    }
    assert_raise(TypeError) {
      Cv::imdecode(buf, DUMMY_OBJ)
    }

    # w = Window.new('Decoded')
    # w.show(m)
    # w.wait_key
  end

  def test_hconcat
    m1 = Mat::ones(2, 2, CV_8U) + 1
    m2 = Mat::ones(2, 2, CV_8U) + 2

    m = Cv::hconcat([m1, m2]);
    assert_equal(m1.class, m1.class)
    assert_equal(m1.rows, m.rows)
    assert_equal(m1.cols + m2.cols, m.cols)
    assert_equal(m1.depth, m.depth)
    assert_equal(m1.channels, m.channels)

    m1.rows.times { |r|
      (m1.cols + m2.cols).times { |c|
        expected = (c < m1.cols) ? m1[r, c][0] : m2[r, c - m1.cols][0]
        assert_equal(expected, m[r, c][0])
      }
    }

    assert_raise(TypeError) {
      Cv::hconcat(DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      Cv::hconcat([DUMMY_OBJ])
    }

    # img = Cv::imread(FILENAME_LENA256x256, -1)
    # dst = Cv::hconcat([img, img])
    # Cv::Window.new('hconcat').show(dst)
    # Cv::wait_key
  end

  def test_vconcat
    m1 = Mat::ones(2, 2, CV_8U) + 1
    m2 = Mat::ones(2, 2, CV_8U) + 2

    m = Cv::vconcat([m1, m2]);
    assert_equal(m1.class, m1.class)
    assert_equal(m1.rows + m2.rows, m.rows)
    assert_equal(m1.cols, m.cols)
    assert_equal(m1.depth, m.depth)
    assert_equal(m1.channels, m.channels)

    (m1.rows + m2.rows).times { |r|
      m1.cols.times { |c|
        expected = (r < m1.rows) ? m1[r, c][0] : m2[r - m1.rows, c][0]
        assert_equal(expected, m[r, c][0])
      }
    }

    assert_raise(TypeError) {
      Cv::vconcat(DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      Cv::vconcat([DUMMY_OBJ])
    }

    # img = Cv::imread(FILENAME_LENA256x256, -1)
    # dst = Cv::vconcat([img, img])
    # Cv::Window.new('vconcat').show(dst)
    # Cv::wait_key
  end
end
