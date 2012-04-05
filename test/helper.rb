#!/usr/bin/env ruby
# -*- mode: ruby; coding: utf-8-unix -*- 
require 'test/unit'
require 'digest/md5'
require 'opencv'
include OpenCV

class OpenCVTestCase < Test::Unit::TestCase
  SAMPLE_DIR = File.expand_path(File.dirname(__FILE__)) + '/samples/'
  FILENAME_CAT = SAMPLE_DIR + 'cat.jpg'
  FILENAME_LENA256x256 = SAMPLE_DIR + 'lena-256x256.jpg'
  FILENAME_LENA32x32 = SAMPLE_DIR + 'lena-32x32.jpg'
  FILENAME_LENA_EYES = File.expand_path(File.dirname(__FILE__)) + '/samples/lena-eyes.jpg'
  FILENAME_FRUITS = SAMPLE_DIR + 'fruits.jpg'
  FILENAME_CONTOURS = File.expand_path(File.dirname(__FILE__)) + '/samples/contours.jpg'
  HAARCASCADE_FRONTALFACE_ALT = SAMPLE_DIR + 'haarcascade_frontalface_alt.xml.gz'
  AVI_SAMPLE = SAMPLE_DIR + 'movie_sample.avi'

  DUMMY_OBJ = Digest::MD5.new # dummy object for argument type check test

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

    pos = CvPoint.new(0, 0)
    images.each { |img|
      w = GUI::Window.new(img[:title])
      w.show(img[:image])
      w.move(pos)
      pos.x += img[:image].width
      if pos.x > 800
        pos.y += img[:image].height
        pos.x = 0
      end
    }
    
    GUI::wait_key
    GUI::Window::destroy_all
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

  alias original_assert_in_delta assert_in_delta

  def assert_cvscalar_equal(expected, actual, message = nil)
    assert_equal(CvScalar, actual.class, message)
    assert_array_equal(expected.to_ary, actual.to_ary, message)
  end

  def assert_array_equal(expected, actual, message = nil)
    assert_equal(expected.size, actual.size, message)
    expected.zip(actual) { |e, a|
      assert_equal(e, a, message)
    }
  end
  
  def assert_in_delta(expected, actual, delta)
    if expected.is_a? CvScalar or actual.is_a? CvScalar
      expected = expected.to_ary if expected.is_a? CvScalar
      actual = actual.to_ary if actual.is_a? CvScalar
      assert_in_delta(expected, actual ,delta)
    elsif expected.is_a? Array and actual.is_a? Array
      assert_equal(expected.size, actual.size)
      expected.zip(actual) { |e, a|
        original_assert_in_delta(e, a, delta)
      }
    else
      original_assert_in_delta(expected, actual, delta)
    end
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

  def create_iplimage(width, height, depth = :cv8u, channel = 4, &block)
    m = IplImage.new(width, height, depth, channel)
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

  def assert_each_cvscalar(actual, delta = 0, &block)
    raise unless block_given?
    count = 0
    actual.height.times { |j|
      actual.width.times { |i|
        expected = block.call(j, i, count)
        if delta == 0
          expected = expected.to_ary if expected.is_a? CvScalar
          assert_array_equal(expected, actual[j, i].to_ary)
        else
          assert_in_delta(expected, actual[j, i], delta)
        end
        count += 1
      }
    }
  end

  def print_cvmat(mat)
    s = []
    mat.height.times { |j|
      a = []
      mat.width.times { |i|
        tmp = mat[j, i].to_ary.map {|m| m.to_f.round(2) }.join(',')
        a << "[#{tmp}]"
      }
      s << a.join(' ')
    }
    puts s.join("\n")
  end
end

