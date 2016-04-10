#!/usr/bin/env ruby
# -*- mode: ruby; coding: utf-8 -*-
require 'test/unit'
require 'opencv'
require File.expand_path(File.dirname(__FILE__)) + '/../helper'

include OpenCV

# Tests for OpenCV::CvHaarClassifierCascade
class TestCvHaarClassifierCascade < OpenCVTestCase
  def setup
    @cascade = CvHaarClassifierCascade.load(HAARCASCADE_FRONTALFACE_ALT)
  end

  def test_load
    assert_equal(CvHaarClassifierCascade, @cascade.class)
    assert_raise(TypeError) {
      CvHaarClassifierCascade.load(DUMMY_OBJ)
    }
  end

  def test_detect_objects
    img = CvMat.load(FILENAME_LENA256x256)

    detected = @cascade.detect_objects(img)
    assert_equal(Array, detected.class)
    assert_equal(1, detected.size)
    assert_equal(Rect, detected[0].class)
    assert_in_delta(100, detected[0].x, 20)
    assert_in_delta(100, detected[0].y, 20)
    assert_in_delta(85, detected[0].width, 20)
    assert_in_delta(85, detected[0].height, 20)

    detected = @cascade.detect_objects(img) { |face|
      assert_in_delta(100, detected[0].x, 20)
      assert_in_delta(100, detected[0].y, 20)
      assert_in_delta(85, detected[0].width, 20)
      assert_in_delta(85, detected[0].height, 20)
    }
    assert_equal(Array, detected.class)
    assert_equal(1, detected.size)
    assert_equal(Rect, detected[0].class)

    detected = @cascade.detect_objects(img, :scale_factor => 2.0, :flags => CV_HAAR_DO_CANNY_PRUNING,
                                       :min_neighbors => 5, :min_size => CvSize.new(10, 10),
                                       :max_size => CvSize.new(100, 100))
    assert_equal(Array, detected.class)
    assert_equal(1, detected.size)
    assert_equal(Rect, detected[0].class)
    assert_in_delta(100, detected[0].x, 20)
    assert_in_delta(100, detected[0].y, 20)
    assert_in_delta(85, detected[0].width, 20)
    assert_in_delta(85, detected[0].height, 20)

    assert_raise(TypeError) {
      @cascade.detect_objects(DUMMY_OBJ)
    }

    # Uncomment the following lines to show the result
    #
    # detected.each { |r|
    #   pt1 = Point.new(r.x, r.y)
    #   pt2 = Point.new(r.x + r.width, r.y + r.height)
    #   img.rectangle!(pt1, pt2, Scalar.new(0, 255, 255), thickness: 3, line_type: Cv::CV_AA)
    # }
    # snap(['Test', img])
  end
end

