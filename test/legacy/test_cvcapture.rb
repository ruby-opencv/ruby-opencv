#!/usr/bin/env ruby
# -*- mode: ruby; coding: utf-8 -*-
require 'test/unit'
require 'opencv'
require File.expand_path(File.dirname(__FILE__)) + '/../helper'

include OpenCV

# Tests for OpenCV::CvCapture
class TestCvCapture < OpenCVTestCase
  def setup
    @cap = CvCapture.open(AVI_SAMPLE)
    @cap.query
  end

  def teardown
    @cap = nil
  end

  def test_open
    cap1 = CvCapture.open(AVI_SAMPLE)
    assert_equal(CvCapture, cap1.class)

    # Uncomment the following lines to test capturing from camera
    #
    # cap2 = CvCapture.open(0)
    # assert_equal(CvCapture, cap2.class)
  end

  def test_close
    cap1 = CvCapture.open(AVI_SAMPLE)
    cap1.close
    assert_false(cap1.opened?)
  end

  def test_grab
    assert(@cap.grab)
  end

  def test_retrieve
    @cap.grab
    img = @cap.retrieve
    assert_equal(Mat, img.class)
  end

  def test_query
    img = @cap.query
    assert_equal(Mat, img.class)
  end
end
