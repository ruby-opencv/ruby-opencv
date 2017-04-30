#!/usr/bin/env ruby
# -*- mode: ruby; coding: utf-8 -*-
require 'test/unit'
require 'opencv'
require File.expand_path(File.dirname(__FILE__)) + '/helper'

include OpenCV

# Tests for OpenCV::IplImage
class TestIplImage < OpenCVTestCase
  def should_classify_images_as(filename, classification)
    assert_equal(OpenCV::IplImage::load(filename, OpenCV::CV_LOAD_IMAGE_GRAYSCALE).smoothness[0], classification)
  end

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
    data = nil
    open(FILENAME_CAT, 'rb') { |f|
      data = f.read
    }
    data_ary = data.unpack("c*")
    data_mat = CvMat.new(1, data_ary.size).set_data(data_ary)
    expected = IplImage.load(FILENAME_CAT)

    img1 = IplImage.decode(data)
    img2 = IplImage.decode(data_ary)
    img3 = IplImage.decode(data_mat)
    img4 = IplImage.decode(data, CV_LOAD_IMAGE_COLOR)
    img5 = IplImage.decode(data_ary, CV_LOAD_IMAGE_COLOR)
    img6 = IplImage.decode(data_mat, CV_LOAD_IMAGE_COLOR)

    [img1, img2, img3, img4, img5, img6].each { |img|
      assert_equal(IplImage, img.class)
      assert_equal(expected.rows, img.rows)
      assert_equal(expected.cols, img.cols)
      assert_equal(expected.channel, img.channel)
    }

    expected_c1 = IplImage.load(FILENAME_CAT, CV_LOAD_IMAGE_GRAYSCALE)
    img1c1 = IplImage.decode(data, CV_LOAD_IMAGE_GRAYSCALE)
    img2c1 = IplImage.decode(data_ary, CV_LOAD_IMAGE_GRAYSCALE)
    img3c1 = IplImage.decode(data_mat, CV_LOAD_IMAGE_GRAYSCALE)

    [img1c1, img2c1, img3c1].each { |img|
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

  def test_roi
    img = IplImage.new(20, 30)
    rect = img.roi
    assert_equal(0, rect.x)
    assert_equal(0, rect.y)
    assert_equal(img.width, rect.width)
    assert_equal(img.height, rect.height)

    img.set_roi(CvRect.new(2, 3, 10, 20))
    rect = img.roi
    assert_equal(2, rect.x)
    assert_equal(3, rect.y)
    assert_equal(10, rect.width)
    assert_equal(20, rect.height)

    img.reset_roi
    rect = img.roi
    assert_equal(0, rect.x)
    assert_equal(0, rect.y)
    assert_equal(img.width, rect.width)
    assert_equal(img.height, rect.height)

    img.set_roi(CvRect.new(1, 2, 5, 6)) {|image|
      rect = image.roi
      assert_equal(1, rect.x)
      assert_equal(2, rect.y)
      assert_equal(5, rect.width)
      assert_equal(6, rect.height)
    }
    rect = img.roi
    assert_equal(0, rect.x)
    assert_equal(0, rect.y)
    assert_equal(img.width, rect.width)
    assert_equal(img.height, rect.height)

    # Alias
    img.roi = CvRect.new(4, 5, 11, 12)
    rect = img.roi
    assert_equal(4, rect.x)
    assert_equal(5, rect.y)
    assert_equal(11, rect.width)
    assert_equal(12, rect.height)
  end

  def test_coi
    img = IplImage.new(20, 30)
    assert_equal(0, img.coi)

    img.set_coi(1)
    assert_equal(1, img.coi)

    img.reset_coi
    assert_equal(0, img.coi)

    img.set_coi(2) {|image|
      assert_equal(2, image.coi)
    }
    assert_equal(0, img.coi)

    # Alias
    img.coi = 1
    assert_equal(1, img.coi)
  end

  def test_smoothness
    asset_path = File.join(File.dirname(__FILE__), 'samples')

    for image in Array.new(7) { |e| e = File.join(asset_path, "smooth%d.jpg") % e } do
      should_classify_images_as image, :smooth
    end

    for image in Array.new(2) { |e| e = File.join(asset_path, "messy%d.jpg") % e } do
      should_classify_images_as image, :messy
    end

    for image in Array.new(10) { |e| e = File.join(asset_path, "blank%d.jpg") % e } do
      should_classify_images_as image, :blank
    end

    for image in Array.new(2) { |e| e = File.join(asset_path, "partially_blank%d.jpg") % e } do
      should_classify_images_as image, :blank
    end
  end
end
