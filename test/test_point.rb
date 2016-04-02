#!/usr/bin/env ruby
# -*- mode: ruby; coding: utf-8 -*-
require 'test/unit'
require 'opencv'
require File.expand_path(File.dirname(__FILE__)) + '/helper'

include OpenCV

# Tests for OpenCV::Point
class TestPoint < OpenCVTestCase
  def test_x
    point = Point.new
    point.x = 100
    assert_equal(100, point.x)
    point.x = 200
    assert_equal(200, point.x)

    assert_raise(TypeError) {
      point.x = DUMMY_OBJ
    }
  end

  def test_y
    point = Point.new
    point.y = 100
    assert_equal(100, point.y)
    point.y = 200
    assert_equal(200, point.y)

    assert_raise(TypeError) {
      point.y = DUMMY_OBJ
    }
  end
  
  def test_initialize
    point = Point.new
    assert_equal(0, point.x)
    assert_equal(0, point.y)

    point = Point.new(10, 20)
    assert_equal(10, point.x)
    assert_equal(20, point.y)

    point = Point.new(Point.new(10, 20))
    assert_equal(10, point.x)
    assert_equal(20, point.y)

    assert_raise(TypeError) {
      Point.new(DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      Point.new(DUMMY_OBJ, 1)
    }
    assert_raise(TypeError) {
      Point.new(1, DUMMY_OBJ)
    }
    assert_raise(ArgumentError) {
      Point.new(1, 2, 3)
    }
  end

  def test_to_s
    point = Point.new(10, 20)
    assert_equal('#<OpenCV::Point:[10, 20]>', point.to_s)
  end

  def test_to_a
    a = Point.new(10, 20).to_a
    assert_equal(10, a[0])
    assert_equal(20, a[1])
  end
end
