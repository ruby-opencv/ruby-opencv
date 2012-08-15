#!/usr/bin/env ruby
# -*- mode: ruby; coding: utf-8-unix -*- 
require 'test/unit'
require 'opencv'
require File.expand_path(File.dirname(__FILE__)) + '/helper'

include OpenCV

# Tests for matching functions of OpenCV::CvMat
class TestCvMat_matching < OpenCVTestCase
  def setup
    @query = read_test_image('query.png')
    @images = ['1.png', '2.png', '3.png'].map{|f| read_test_image('train', f)}
  end

  def data_dir
    File.join(File.dirname(__FILE__), '..', 'examples', 'matching_to_many_images')
  end

  def read_test_image(*path)
    IplImage.load File.join(data_dir, *path), CV_LOAD_IMAGE_GRAYSCALE
  end

  def test_match_descriptors
    matchs = @query.match_descriptors(@images)
    match_index, count = matchs.max_by {|image_index, count| count}
    assert_equal 2, match_index
  end

  def test_match_descriptors_with_empty_image_array
    assert_equal({}, @query.match_descriptors([]))
  end

  def test_match_descriptors_with_wrong_detector_type_argument
    assert_raise ArgumentError do
      @query.match_descriptors(@images, "Wrong")
    end
  end

  def test_match_descriptors_with_wrong_descriptor_type_argument
    assert_raise ArgumentError do
      @query.match_descriptors(@images, "SURF", "wrong")
    end
  end

  def test_match_descriptors_with_wrong_matcher_type_argument
    assert_raise ArgumentError do
      @query.match_descriptors(@images, "SURF", "SURF", "wrong")
    end
  end
end
