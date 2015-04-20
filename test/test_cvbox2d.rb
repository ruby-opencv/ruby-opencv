#!/usr/bin/env ruby
# -*- mode: ruby; coding: utf-8 -*-
require 'test-unit'
require 'opencv'
require File.expand_path(File.dirname(__FILE__)) + '/helper'

include OpenCV

# Tests for OpenCV::CvBox2D
class TestCvBox2D < OpenCVTestCase
  class MyPoint; end
  def test_initialize
    box = CvBox2D.new
    assert_in_delta(0, box.center.x, 0.001)
    assert_in_delta(0, box.center.y, 0.001)
    assert_in_delta(0, box.size.width, 0.001)
    assert_in_delta(0, box.size.height, 0.001)
    assert_in_delta(0, box.angle, 0.001)

    box = CvBox2D.new(CvPoint2D32f.new(1.1, 2.2), CvSize2D32f.new(3.3, 4.4), 5.5)
    assert_in_delta(1.1, box.center.x, 0.001)
    assert_in_delta(2.2, box.center.y, 0.001)
    assert_in_delta(3.3, box.size.width, 0.001)
    assert_in_delta(4.4, box.size.height, 0.001)
    assert_in_delta(5.5, box.angle, 0.001)
  end

  def test_center
    box = CvBox2D.new
    box.center = CvPoint2D32f.new(1.1, 2.2)
    assert_in_delta(1.1, box.center.x, 0.001)
    assert_in_delta(2.2, box.center.y, 0.001)

    box.center.x = 3.3
    box.center.y = 4.4
    assert_in_delta(3.3, box.center.x, 0.001)
    assert_in_delta(4.4, box.center.y, 0.001)
  end
  
  def test_size
    box = CvBox2D.new
    box.size = CvSize2D32f.new(1.1, 2.2)
    assert_in_delta(1.1, box.size.width, 0.001)
    assert_in_delta(2.2, box.size.height, 0.001)

    box.size.width = 3.3
    box.size.height = 4.4
    assert_in_delta(3.3, box.size.width, 0.001)
    assert_in_delta(4.4, box.size.height, 0.001)
  end

  def test_angle
    box = CvBox2D.new
    box.angle = 1.1
    assert_in_delta(1.1, box.angle, 0.001)
  end

  def test_points
    box = CvBox2D.new
    box.center = CvPoint2D32f.new(10, 20)
    box.size = CvSize2D32f.new(5, 7)
    pt = box.points

    assert_equal(4, pt.size)
    assert_in_delta(7.5, pt[0].x, 0.001)
    assert_in_delta(23.5, pt[0].y, 0.001)
    assert_in_delta(7.5, pt[1].x, 0.001)
    assert_in_delta(16.5, pt[1].y, 0.001)
    assert_in_delta(12.5, pt[2].x, 0.001)
    assert_in_delta(16.5, pt[2].y, 0.001)
    assert_in_delta(12.5, pt[3].x, 0.001)
    assert_in_delta(23.5, pt[3].y, 0.001)
  end
end


