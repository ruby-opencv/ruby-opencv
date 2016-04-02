#!/usr/bin/env ruby
# -*- mode: ruby; coding: utf-8 -*-
require 'test/unit'
require 'digest/md5'
require File.expand_path(File.dirname(__FILE__)) + '/../lib/opencv'
include Cv

class OpenCVTestCase < Test::Unit::TestCase
  SAMPLE_DIR = File.expand_path(File.dirname(__FILE__)) + '/samples/'
  FILENAME_CAT = SAMPLE_DIR + 'cat.jpg'
  FILENAME_LENA256x256 = SAMPLE_DIR + 'lena-256x256.jpg'
  FILENAME_LENA32x32 = SAMPLE_DIR + 'lena-32x32.jpg'
  FILENAME_GIRLS_PLAY_AND_PLANT_FLOWERS_IN_THE_PARK = SAMPLE_DIR + 'girls-play-and-plant-flowers-in-the-park-725x480.jpg'
  HAARCASCADE_FRONTALFACE_ALT = SAMPLE_DIR + 'haarcascade_frontalface_alt.xml'
  AVI_SAMPLE = SAMPLE_DIR + 'movie_sample.avi'

  DUMMY_OBJ = Digest::MD5.new # dummy object for argument type check test

  alias original_assert_in_delta assert_in_delta

  def snap(*images)
    n = -1
    images.map! { |val|
      n += 1
      if val.is_a? Hash
        val
      elsif val.is_a? Array
        {:title => val[0], :image => val[1] }
      else
        {:title => "snap-#{n}", :image => val }
      end
    }

    pos = Point.new(0, 0)
    images.each { |img|
      w = Window.new(img[:title])
      w.show(img[:image])
      w.move(pos.x, pos.y)
      pos.x += img[:image].width
      if pos.x > 800
        pos.y += img[:image].height
        pos.x = 0
      end
    }

    Cv::wait_key
    Window::destroy_all
  end

  def hash_img(img)
    # Compute a hash for an image, useful for image comparisons
    Digest::MD5.hexdigest(img.data)
  end

  unless Test::Unit::TestCase.instance_methods.map {|m| m.to_sym }.include? :assert_false
    def assert_false(actual, message = nil)
      assert_equal(false, actual, message)
    end
  end

  def assert_mat_in_delta(expected, actual, delta)
    assert_equal(expected.rows, actual.rows)
    assert_equal(expected.cols, actual.cols)
    assert_equal(expected.depth, actual.depth)
    assert_equal(expected.dims, actual.dims)
    assert_equal(expected.channels, actual.channels)
    0.upto(expected.rows - 1) { |r|
      0.upto(expected.cols - 1) { |c|
        0.upto(expected.channels - 1) { |i|
          msg = "Failed at #{actual.class.to_s}<depth=#{actual.depth},channels=#{actual.channels}>(#{r}, #{c})[#{i}]"
          original_assert_in_delta(expected[r, c][i], actual[r, c][i], delta, msg)
        }
      }
    }
  end

  def create_cvmat(height, width, depth = :cv8u, channel = 4, &block)
    m = CvMat.new(height, width, depth, channel)
    block = lambda { |j, i, c| CvScalar.new(*([c + 1] * channel)) } unless block_given?
    count = 0
    height.times { |j|
      width.times { |i|
        m[j, i] = block.call(j, i, count)
        count += 1
      }
    }
    m
  end

  def assert_in_delta(expected, actual, delta, msg = nil)
    if expected.is_a? Scalar or actual.is_a? Scalar
      expected = expected.to_a if expected.is_a? Scalar
      actual = actual.to_a if actual.is_a? Scalar
      assert_in_delta(expected, actual, delta, msg)
    elsif expected.is_a? Array and actual.is_a? Array
      assert_equal(expected.size, actual.size)
      expected.zip(actual) { |e, a|
        original_assert_in_delta(e, a, delta, msg)
      }
    else
      original_assert_in_delta(expected, actual, delta, msg)
    end
  end
end

