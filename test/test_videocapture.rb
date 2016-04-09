#!/usr/bin/env ruby
# -*- mode: ruby; coding: utf-8 -*-
require 'test/unit'
require 'opencv'
require File.expand_path(File.dirname(__FILE__)) + '/helper'

include Cv

# Tests for Cv::VideoCapture
class TestVideoCapture < OpenCVTestCase
  def setup
    @cap = VideoCapture.new(AVI_SAMPLE)
  end

  def teardown
    @cap = nil
  end

  def test_open
    cap1 = VideoCapture.new(AVI_SAMPLE)
    assert_equal(VideoCapture, cap1.class)
  end

  def test_grab
    assert(@cap.grab)
  end

  def test_retrieve
    @cap.grab
    img = @cap.retrieve
    assert_equal(Mat, img.class)
  end

  def test_get
    assert_equal(0, @cap.get(CAP_PROP_POS_MSEC))
    assert_equal(0, @cap.get(CAP_PROP_POS_FRAMES))
    assert_in_delta(0.08333333333333333, @cap.get(CAP_PROP_POS_AVI_RATIO), 0.01)
    assert_equal(160, @cap.get(CAP_PROP_FRAME_WIDTH))
    assert_equal(120, @cap.get(CAP_PROP_FRAME_HEIGHT))
    assert_equal(12, @cap.get(CAP_PROP_FPS))
    assert_equal(0, @cap.get(CAP_PROP_FOURCC))
    assert_equal(37, @cap.get(CAP_PROP_FRAME_COUNT))
    assert_equal(0, @cap.get(CAP_PROP_FORMAT))
    assert_equal(0, @cap.get(CAP_PROP_MODE))
    assert_equal(0, @cap.get(CAP_PROP_BRIGHTNESS))
    assert_equal(0, @cap.get(CAP_PROP_CONTRAST))
    assert_equal(0, @cap.get(CAP_PROP_SATURATION))
    assert_equal(0, @cap.get(CAP_PROP_HUE))
    assert_equal(0, @cap.get(CAP_PROP_GAIN))
    assert_equal(0, @cap.get(CAP_PROP_EXPOSURE))
    assert_equal(0, @cap.get(CAP_PROP_CONVERT_RGB))
    assert_equal(0, @cap.get(CAP_PROP_RECTIFICATION))
  end

  def test_set
    @cap.set(CAP_PROP_POS_MSEC, 1000)
    assert_equal(1000, @cap.get(CAP_PROP_POS_MSEC))
  end

  def test_release
    @cap.release
    assert_false(@cap.opened?)
  end
end
