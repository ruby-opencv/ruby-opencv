#!/usr/bin/env ruby
# -*- mode: ruby; coding: utf-8 -*-
require 'opencv'
require File.expand_path(File.dirname(__FILE__)) + '/helper'

include OpenCV

class TestCascadeClassifier < OpenCVTestCase
  def test_initialize
    c = CascadeClassifier.new
    assert_equal(CascadeClassifier, c.class)

    c = CascadeClassifier.new(HAARCASCADE_FRONTALFACE_ALT)
    assert_equal(CascadeClassifier, c.class)

    assert_raise(TypeError) {
      CascadeClassifier.new(DUMMY_OBJ)
    }
  end

  def test_load
    c = CascadeClassifier.new
    ret = c.load(HAARCASCADE_FRONTALFACE_ALT)
    assert(ret)

    ret = c.load('/file/not/exist')
    assert_false(ret)

    assert_raise(TypeError) {
      c2 = CascadeClassifier.new
      c2.load(DUMMY_OBJ)
    }
  end

  def test_detect_multi_scale
    c = CascadeClassifier.new(HAARCASCADE_FRONTALFACE_ALT)
    m = OpenCV::imread(FILENAME_GIRLS_PLAY_AND_PLANT_FLOWERS_IN_THE_PARK, IMREAD_ANYDEPTH | IMREAD_ANYCOLOR)

    rects = c.detect_multi_scale(m)
    assert_equal(3, rects.size)

    rects = c.detect_multi_scale(m, scale_factor: 1.2, min_neighbors: 5, min_size: Size.new(50, 50), max_size: Size.new(60, 60))
    assert_equal(1, rects.size)

    assert_raise(TypeError) {
      c.detect_multi_scale(DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      c.detect_multi_scale(m, DUMMY_OBJ)
    }

    # rects = c.detect_multi_scale(m)
    # rects.each do |r|
    #   puts r.to_s
    #   pt1 = Point.new(r.x, r.y)
    #   pt2 = Point.new(r.x + r.width, r.y + r.height)
    #   m.rectangle!(pt1, pt2, Scalar.new(0, 255, 255), thickness: 3, line_type: CV_AA)
    # end
    # snap(m)
  end
end
