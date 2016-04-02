#!/usr/bin/env ruby
# -*- mode: ruby; coding: utf-8 -*-
require 'test/unit'
require 'opencv'
require File.expand_path(File.dirname(__FILE__)) + '/helper'

include OpenCV

# Tests for OpenCV::Rect
class TestRect < OpenCVTestCase
  def test_x
    rect = Rect.new
    rect.x = 100
    assert_equal(100, rect.x)
    rect.x = 200
    assert_equal(200, rect.x)
  end

  def test_y
    rect = Rect.new
    rect.y = 100
    assert_equal(100, rect.y)
    rect.y = 200
    assert_equal(200, rect.y)
  end

  def test_width
    rect = Rect.new
    rect.width = 100
    assert_equal(100, rect.width)
    rect.width = 200
    assert_equal(200, rect.width)
  end

  def test_height
    rect = Rect.new
    rect.height = 100
    assert_equal(100, rect.height)
    rect.height = 200
    assert_equal(200, rect.height)
  end

  def test_initialize
    rect = Rect.new
    assert_equal(0, rect.x)
    assert_equal(0, rect.y)
    assert_equal(0, rect.width)
    assert_equal(0, rect.height)

    rect = Rect.new(10, 20, 30, 40)
    assert_equal(10, rect.x)
    assert_equal(20, rect.y)
    assert_equal(30, rect.width)
    assert_equal(40, rect.height)

    assert_raise(TypeError) {
      Rect.new(DUMMY_OBJ, 1, 1, 1)
    }
    assert_raise(TypeError) {
      Rect.new(1, DUMMY_OBJ, 1, 1)
    }
    assert_raise(TypeError) {
      Rect.new(1, 1, DUMMY_OBJ, 1)
    }
    assert_raise(TypeError) {
      Rect.new(1, 1, 1, DUMMY_OBJ)
    }
    assert_raise(ArgumentError) {
      Rect.new(1)
    }
    assert_raise(ArgumentError) {
      Rect.new(1, 2)
    }
    assert_raise(ArgumentError) {
      Rect.new(1, 2, 3)
    }
    assert_raise(ArgumentError) {
      Rect.new(1, 2, 3, 4, 5)
    }
  end

  def test_to_s
    rect = Rect.new(1, 2, 3, 4)
    assert_equal('#<OpenCV::Rect:[3 x 4 from (1, 2)]>', rect.to_s)
  end
end
