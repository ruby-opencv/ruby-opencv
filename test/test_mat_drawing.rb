#!/usr/bin/env ruby
# -*- mode: ruby; coding: utf-8 -*-
require 'opencv'
require File.expand_path(File.dirname(__FILE__)) + '/helper'

include Cv

class TestMat < OpenCVTestCase
  def test_circle
    m0 = Cv::Mat.zeros(128, 128, Cv::CV_8UC3) + Cv::Scalar.new(255, 255, 255)
    center = Cv::Point.new(m0.cols * 0.5, m0.rows * 0.5)
    radius = 32
    color = Cv::Scalar.new(255, 0, 0)
    option = { thickness: 2, line_type: Cv::CV_AA, shift: 0 }

    m1 = m0.circle(center, radius, color, option)
    assert_equal(m0.class, m1.class)
    assert_equal(m0.rows, m1.rows)
    assert_equal(m0.cols, m1.cols)

    m2 = m0.clone
    m2.circle!(center, radius, color, option)
    assert_equal(m0.class, m2.class)
    assert_equal(m0.rows, m2.rows)
    assert_equal(m0.cols, m2.cols)

    assert_raise(TypeError) {
      m0.circle(DUMMY_OBJ, radius, color, option)
    }
    assert_raise(TypeError) {
      m0.circle(center, DUMMY_OBJ, color, option)
    }
    assert_raise(TypeError) {
      m0.circle(center, radius, DUMMY_OBJ, option)
    }
    assert_raise(TypeError) {
      m0.circle(center, radius, color, DUMMY_OBJ)
    }

    assert_raise(TypeError) {
      m0.circle!(DUMMY_OBJ, radius, color, option)
    }
    assert_raise(TypeError) {
      m0.circle!(center, DUMMY_OBJ, color, option)
    }
    assert_raise(TypeError) {
      m0.circle!(center, radius, DUMMY_OBJ, option)
    }
    assert_raise(TypeError) {
      m0.circle!(center, radius, color, DUMMY_OBJ)
    }

    # w1 = Window.new('Circle 1')
    # w1.show m1
    # w2 = Window.new('Circle 2')
    # w2.show m2
    # Cv::wait_key
  end
end
