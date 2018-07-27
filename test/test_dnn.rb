#!/usr/bin/env ruby
# -*- mode: ruby; coding: utf-8 -*-
require 'opencv'
require File.expand_path(File.dirname(__FILE__)) + '/helper'

include Cv

class TestDnn < OpenCVTestCase
  def test_read_net
    c = Dnn.read_net(BVLC_GOOGLENET_PROTXT, BVLC_GOOGLENET_CAFFEMODEL)
    assert_equal(Dnn::Net, c.class)

    assert_raise(TypeError) {
      Dnn.read_net(DUMMY_OBJ)
    }
  end

  def test_read_net_from_caffe
    c = Dnn.read_net_from_caffe(BVLC_GOOGLENET_PROTXT, BVLC_GOOGLENET_CAFFEMODEL)
    assert_equal(Dnn::Net, c.class)

    assert_raise(ArgumentError) {
      Dnn.read_net_from_caffe(DUMMY_OBJ)
    }
  end

  def test_blog_from_image
    b = blob_from_image
    assert_equal(Mat, b.class)
    assert_equal(224, b.size(2))
    assert_equal(224, b.size(3))
  end

  def test_net_initialize
    c = Dnn::Net.new
    assert_equal(Dnn::Net, c.class)

    c = bvlc_googlenet
    assert_equal(Dnn::Net, c.class)

    assert_raise(TypeError) {
      Dnn::Net.new(DUMMY_OBJ)
    }
  end

  def test_net_empty
    c = bvlc_googlenet
    assert_equal(false, c.empty?)

    c = Dnn::Net.new
    assert_equal(true, c.empty?)
  end

  def test_net_forward
    c = bvlc_googlenet
    c.input = blob_from_image
    m = c.forward

    assert_equal(Mat, m.class)
  end

  private

  def bvlc_googlenet
    Dnn::Net.new(BVLC_GOOGLENET_PROTXT, BVLC_GOOGLENET_CAFFEMODEL)
  end

  def blob_from_image
    i = Cv.imread(FILENAME_GIRLS_PLAY_AND_PLANT_FLOWERS_IN_THE_PARK)
    Dnn.blob_from_image(i, size: Size.new(224, 224), mean: Scalar.new(104, 117, 123))
  end
end
