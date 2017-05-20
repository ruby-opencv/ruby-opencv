#!/usr/bin/env ruby
# -*- mode: ruby; coding: utf-8 -*-
require 'test/unit'
require 'opencv'
require File.expand_path(File.dirname(__FILE__)) + '/helper'

include OpenCV

# Tests for OpenCV::CvSURFPoint
class TestCvSURFPoint < OpenCVTestCase
  def setup
    omit_unless(IS_OPENCV2, 'CvSURFPoint is not supported in OpenCV 3 or later')
    @surf_point1 = CvSURFPoint.new(CvPoint2D32f.new(0, 0), 0, 0, 0, 0)
  end

  def test_initialize
    sp1 = CvSURFPoint.new(CvPoint2D32f.new(1.1, 2.2), 1, 10, 12.3, 45.6)
    assert_in_delta(1.1, sp1.pt.x, 0.001)
    assert_in_delta(2.2, sp1.pt.y, 0.001)
    assert_equal(1, sp1.laplacian)
    assert_equal(10, sp1.size)
    assert_in_delta(12.3, sp1.dir, 0.001)
    assert_in_delta(45.6, sp1.hessian, 0.001)
  end

  def test_pt
    assert_in_delta(0, @surf_point1.pt.x, 0.001)
    assert_in_delta(0, @surf_point1.pt.y, 0.001)

    @surf_point1.pt = CvPoint2D32f.new(12.3, 45.6)
    assert_in_delta(12.3, @surf_point1.pt.x, 0.001)
    assert_in_delta(45.6, @surf_point1.pt.y, 0.001)

    assert_raise(TypeError) {
      @surf_point1.pt = DUMMY_OBJ
    }
  end

  def test_laplacian
    assert_equal(0, @surf_point1.laplacian)

    @surf_point1.laplacian = -1
    assert_equal(-1, @surf_point1.laplacian)
  end

  def test_size
    assert_equal(0, @surf_point1.size)

    @surf_point1.size = 10
    assert_equal(10, @surf_point1.size)
  end

  def test_dir
    assert_in_delta(0, @surf_point1.dir, 0.001)

    @surf_point1.dir = 23.4
    assert_in_delta(23.4, @surf_point1.dir, 0.001)
  end

  def test_hessian
    assert_in_delta(0, @surf_point1.hessian, 0.001)

    @surf_point1.hessian = 2.1
    assert_in_delta(2.1, @surf_point1.hessian, 0.001)
  end
end

