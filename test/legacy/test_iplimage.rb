#!/usr/bin/env ruby
# -*- mode: ruby; coding: utf-8 -*-
require 'test/unit'
require 'opencv'
require File.expand_path(File.dirname(__FILE__)) + '/../helper'

include OpenCV

# Tests for OpenCV::IplImage
class TestIplImage < OpenCVTestCase

  def test_initialize
    img = IplImage.new(10, 20)
    assert_equal(10, img.width)
    assert_equal(20, img.height)
    assert_equal(:cv8u, img.depth)
    assert_equal(3, img.channel)

    img = IplImage.new(30, 40, :cv32f, 1)
    assert_equal(30, img.width)
    assert_equal(40, img.height)
    assert_equal(:cv32f, img.depth)
    assert_equal(1, img.channel)
  end

  def test_load
    img1 = IplImage.load(FILENAME_CAT)
    assert_equal(IplImage, img1.class)
    assert_equal(375, img1.width)
    assert_equal(500, img1.height)
    assert_equal(:cv8u, img1.depth)
    assert_equal(3, img1.channel)

    img2 = IplImage.load(FILENAME_CAT, CV_LOAD_IMAGE_GRAYSCALE)
    assert_equal(IplImage, img2.class)
    assert_equal(375, img2.width)
    assert_equal(500, img2.height)
    assert_equal(:cv8u, img2.depth)
    assert_equal(1, img2.channel)

    img3 = IplImage.load(FILENAME_CAT, CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_ANYCOLOR)
    assert_equal(IplImage, img3.class)
    assert_equal(375, img3.width)
    assert_equal(500, img3.height)
    assert_equal(:cv8u, img3.depth)
    assert_equal(3, img3.channel)

    assert_raise(ArgumentError) {
      IplImage.load
    }
    assert_raise(TypeError) {
      IplImage.load(123)
    }
    assert_raise(TypeError) {
      IplImage.load(FILENAME_CAT, 'foobar')
    }
    assert_raise(StandardError) {
      IplImage.load('file/does/not/exist')
    }

    # Uncomment the following lines to show the results
    # snap img1, img2, img3
  end

  def test_decode
    data = open(FILENAME_CAT, 'rb') { |f| f.read }

    data_ary = data.unpack("c*")
    expected = IplImage.load(FILENAME_CAT)

    img1 = IplImage.decode(data)
    img2 = IplImage.decode(data_ary)
    img3 = IplImage.decode(data, CV_LOAD_IMAGE_COLOR)
    img4 = IplImage.decode(data_ary, CV_LOAD_IMAGE_COLOR)

    [img1, img2, img3, img4].each { |img|
      assert_equal(IplImage, img.class)
      assert_equal(expected.rows, img.rows)
      assert_equal(expected.cols, img.cols)
      assert_equal(expected.channel, img.channel)
    }

    expected_c1 = IplImage.load(FILENAME_CAT, CV_LOAD_IMAGE_GRAYSCALE)
    img1c1 = IplImage.decode(data, CV_LOAD_IMAGE_GRAYSCALE)
    img2c1 = IplImage.decode(data_ary, CV_LOAD_IMAGE_GRAYSCALE)

    [img1c1, img2c1].each { |img|
      assert_equal(IplImage, img.class)
      assert_equal(expected_c1.rows, img.rows)
      assert_equal(expected_c1.cols, img.cols)
      assert_equal(expected_c1.channel, img.channel)
    }

    assert_raise(TypeError) {
      IplImage.decode(DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      IplImage.decode(data, DUMMY_OBJ)
    }

    # Uncomment the following line to show the result images
    # snap img1, img2, img3
  end
end
