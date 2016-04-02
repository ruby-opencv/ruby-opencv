#!/usr/bin/env ruby
# -*- mode: ruby; coding: utf-8 -*-
require 'test/unit'
require 'opencv'
require File.expand_path(File.dirname(__FILE__)) + '/helper'

include OpenCV

# Tests for OpenCV::Size
class TestSize < OpenCVTestCase
  def test_width
    size = Size.new
    size.width = 100
    assert_equal(100, size.width)
    size.width = 200
    assert_equal(200, size.width)
  end

  def test_height
    size = Size.new
    size.height = 100
    assert_equal(100, size.height)
    size.height = 200
    assert_equal(200, size.height)
  end

  def test_initialize
    size = Size.new
    assert_equal(0, size.width)
    assert_equal(0, size.height)

    size = Size.new(10, 20)
    assert_equal(10, size.width)
    assert_equal(20, size.height)

    assert_raise(TypeError) {
      Size.new(DUMMY_OBJ, 1)
    }
    assert_raise(TypeError) {
      Size.new(1, DUMMY_OBJ)
    }
    assert_raise(ArgumentError) {
      Size.new(1)
    }
    assert_raise(ArgumentError) {
      Size.new(1, 2, 3)
    }
  end

  def test_to_s
    size = Size.new(10, 20)
    assert_equal('#<OpenCV::Size:[10 x 20]>', size.to_s)
  end
end
