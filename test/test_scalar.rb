#!/usr/bin/env ruby
# -*- mode: ruby; coding: utf-8 -*-
require 'test/unit'
require 'opencv'
require File.expand_path(File.dirname(__FILE__)) + '/helper'

include OpenCV

# Tests for OpenCV::Scalar
class TestScalar < OpenCVTestCase
  def test_initialize
    s = Scalar.new
    assert_in_delta([0, 0, 0, 0], s, 0.01)

    s = Scalar.new(1.1)
    assert_in_delta([1.1, 0, 0, 0], s, 0.01)

    s = Scalar.new(1.1, 2.2)
    assert_in_delta([1.1, 2.2, 0, 0], s, 0.01)

    s = Scalar.new(1.1, 2.2, 3.3)
    assert_in_delta([1.1, 2.2, 3.3, 0], s, 0.01)

    s = Scalar.new(1.1, 2.2, 3.3, 4.4)
    assert_in_delta([1.1, 2.2, 3.3, 4.4], s, 0.01)
  end
  
  def test_aref
    assert_in_delta([0, 0, 0, 0], Scalar.new, 0.01)
    assert_in_delta([10, 20, 30, 40], Scalar.new(10, 20, 30, 40), 0.01)
    assert_in_delta([0.1, 0.2, 0.3, 0.4], Scalar.new(0.1, 0.2, 0.3, 0.4), 0.01)
  end

  def test_aset
    s = Scalar.new
    [10, 20, 30, 40].each_with_index { |x, i|
      s[i] = x
    }
    assert_in_delta([10, 20, 30, 40], s, 0.01)

    s = Scalar.new
    [0.1, 0.2, 0.3, 0.4].each_with_index { |x, i|
      s[i] = x
    }
    assert_in_delta([0.1, 0.2, 0.3, 0.4], s, 0.01)
  end

  def test_to_s
    assert_equal('#<OpenCV::Scalar:[10, 20, 30, 40]>', Scalar.new(10, 20, 30, 40).to_s)
    assert_equal('#<OpenCV::Scalar:[0.1, 0.2, 0.3, 0.4]>', Scalar.new(0.1, 0.2, 0.3, 0.4).to_s)
  end

  def test_to_ary
    [[10, 20, 30, 40], [0.1, 0.2, 0.3, 0.4]].each { |a|
      s = Scalar.new(*a)
      x = s.to_a # Alias
      assert_equal(Array, x.class)
      assert_in_delta(a, x, 0.01)
    }
  end
end
