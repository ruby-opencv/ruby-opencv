#!/usr/bin/env ruby
# -*- mode: ruby; coding: utf-8 -*-
require 'opencv'
require File.expand_path(File.dirname(__FILE__)) + '/helper'

include Cv

class TestMat < OpenCVTestCase
  DEPTH = [CV_8U, CV_8S, CV_16U, CV_16S, CV_32F, CV_32S, CV_64F]

  def test_initialize
    m = Mat.new(10, 20)
    assert_equal(10, m.rows)
    assert_equal(m.rows, m.height)
    assert_equal(20, m.cols)
    assert_equal(m.cols, m.width)
    assert_equal(CV_8U, m.depth)
    assert_equal(2, m.dims)
    assert_equal(1, m.channels)

    [CV_8U, CV_8S, CV_16U, CV_16S, CV_32S, CV_32F, CV_64F].each { |depth|
      [1, 2, 3, 4].each { |channel|
        type = Cv::CV_MAKETYPE(depth, channel)
        m = Mat.new(10, 20, type)
        assert_equal(10, m.rows)
        assert_equal(20, m.cols)
        assert_equal(depth, m.depth)
        assert_equal(channel, m.channels)
      }
    }

    # ROI
    m0 = Mat.zeros(5, 5, CV_8U)
    roi = Rect.new(0, 0, 2, 3)
    m = Mat.new(m0, roi)
    assert_equal(roi.height, m.rows)
    assert_equal(roi.width, m.cols)
    assert_equal(m0.depth, m.depth)
    assert_equal(m0.dims, m.dims)
    assert_equal(m0.channels, m.channels)
    m.set_to(Scalar.new(1))
    assert_equal(m[0, 0][0], m0[0, 0][0])

    assert_raise(TypeError) {
      m = Mat.new(DUMMY_OBJ, 20, CV_8U)
    }
    assert_raise(TypeError) {
      m = Mat.new(10, DUMMY_OBJ, CV_8U)
    }
    assert_raise(TypeError) {
      m = Mat.new(10, 20, DUMMY_OBJ)
    }
  end

  def test_save_image
    filename_jpg = 'save_image_test.jpg'
    filename_png = 'save_image_test.png'
    begin
      m = Mat.new(20, 20, CV_8U)
      
      File.delete filename_jpg if File.exists? filename_jpg
      m.save(filename_jpg)
      assert(File.exists? filename_jpg)

      File.delete filename_jpg if File.exists? filename_jpg
      m.save(filename_jpg, [10])
      assert(File.exists? filename_jpg)

      File.delete filename_png if File.exists? filename_png
      m.save(filename_png, [9])
      assert(File.exists? filename_png)

      # Alias
      File.delete filename_jpg if File.exists? filename_jpg
      Cv::imwrite(filename_jpg, m)
      assert(File.exists? filename_jpg)
      
      assert_raise(TypeError) {
        m.save(DUMMY_OBJ)
      }
      assert_raise(TypeError) {
        m.save(filename_jpg, DUMMY_OBJ)
      }
    ensure
      File.delete filename_jpg if File.exists? filename_jpg
      File.delete filename_png if File.exists? filename_png
    end
  end

  def test_zeros
    m = Mat.zeros(3, 3, CV_8U)

    assert_equal(3, m.rows)
    assert_equal(m.rows, m.height)
    assert_equal(3, m.cols)
    assert_equal(m.cols, m.width)
    assert_equal(CV_8U, m.depth)
    assert_equal(2, m.dims)
    assert_equal(1, m.channels)

    elems = m.to_s.scan(/(\[[^\]]+\])/m).flatten[0]
    assert_equal("[  0,   0,   0;\n   0,   0,   0;\n   0,   0,   0]", elems)

    assert_raise(TypeError) {
      Mat.zeros(DUMMY_OBJ, 3, CV_8U)
    }
    assert_raise(TypeError) {
      Mat.zeros(3, DUMMY_OBJ, CV_8U)
    }
    assert_raise(TypeError) {
      Mat.zeros(3, 3, DUMMY_OBJ)
    }
  end

  def test_ones
    m = Mat.ones(3, 3, CV_8U)

    assert_equal(3, m.rows)
    assert_equal(m.rows, m.height)
    assert_equal(3, m.cols)
    assert_equal(m.cols, m.width)
    assert_equal(CV_8U, m.depth)
    assert_equal(2, m.dims)
    assert_equal(1, m.channels)

    elems = m.to_s.scan(/(\[[^\]]+\])/m).flatten[0]
    assert_equal("[  1,   1,   1;\n   1,   1,   1;\n   1,   1,   1]", elems)

    assert_raise(TypeError) {
      Mat.ones(DUMMY_OBJ, 3, CV_8U)
    }
    assert_raise(TypeError) {
      Mat.ones(3, DUMMY_OBJ, CV_8U)
    }
    assert_raise(TypeError) {
      Mat.ones(3, 3, DUMMY_OBJ)
    }
  end

  def test_eye
    m = Mat.eye(3, 3, CV_8U)

    assert_equal(3, m.rows)
    assert_equal(m.rows, m.height)
    assert_equal(3, m.cols)
    assert_equal(m.cols, m.width)
    assert_equal(CV_8U, m.depth)
    assert_equal(2, m.dims)
    assert_equal(1, m.channels)

    elems = m.to_s.scan(/(\[[^\]]+\])/m).flatten[0]
    assert_equal("[  1,   0,   0;\n   0,   1,   0;\n   0,   0,   1]", elems)

    assert_raise(TypeError) {
      Mat.eye(DUMMY_OBJ, 3, CV_8U)
    }
    assert_raise(TypeError) {
      Mat.eye(3, DUMMY_OBJ, CV_8U)
    }
    assert_raise(TypeError) {
      Mat.eye(3, 3, DUMMY_OBJ)
    }
  end

  def test_aref
    DEPTH.each { |type|
      m = Mat.eye(3, 3, type)
      0.upto(m.rows - 1) { |r|
        0.upto(m.cols - 1) { |c|
          s = m[r, c]
          expected = (r == c) ? 1 : 0
          assert_in_delta(expected, m[r, c][0], 0.01)
        }
      }
    }
    DEPTH.each { |depth|
      (1..4).each { |channel|
        m = Mat.eye(3, 3, Cv::CV_MAKETYPE(depth, channel))
        0.upto(m.rows - 1) { |r|
          0.upto(m.cols - 1) { |c|
            s = m[r, c]
            expected = (r == c) ? 1 : 0
            assert_equal(expected, m[r, c][0])
            (1...channel).each { |i|
              assert_in_delta(0, m[r, c][i], 0.01)
            }
          }
        }
      }
    }
  end

  def test_aset
    DEPTH.each { |type|
      m = Mat.zeros(3, 3, type)
      0.upto(m.rows - 1) { |r|
        0.upto(m.cols - 1) { |c|
          n = (r == c) ? 1 : 0
          a = [n] * m.channels
          m[r, c] = Scalar.new(*a)
        }
      }
      assert_mat_in_delta(Mat.eye(m.rows, m.cols, type), m, 0.01)
    }
    DEPTH.each { |depth|
      (1..4).each { |channel|
        type = Cv::CV_MAKETYPE(depth, channel)
        m = Mat.zeros(3, 3, type)
        0.upto(m.rows - 1) { |r|
          0.upto(m.cols - 1) { |c|
            n = (r == c) ? 1 : 0
            a = [n, 0, 0, 0]
            m[r, c] = Scalar.new(*a)
          }
        }
        assert_mat_in_delta(Mat.eye(m.rows, m.cols, type), m, 0.01)
      }
    }
  end

  def test_clone
    m1 = Mat.eye(3, 3, CV_8U)
    m2 = m1.clone
    assert_equal(m1.to_s, m2.to_s)
    assert_not_equal(m1.object_id, m2.object_id)
    m1[0, 0] = Scalar.new(10)
    assert_not_equal(m1.to_s, m2.to_s)
  end

  def test_add
    # Mat + Number
    m = Mat.zeros(3, 3, CV_8U) + 100
    elems = m.to_s.scan(/(\[[^\]]+\])/m).flatten[0]
    assert_equal("[100, 100, 100;\n 100, 100, 100;\n 100, 100, 100]", elems)

    # Mat + Scalar
    m = Mat.ones(3, 3, CV_8U) + Scalar.new(1)
    elems = m.to_s.scan(/(\[[^\]]+\])/m).flatten[0]
    assert_equal("[  2,   2,   2;\n   2,   2,   2;\n   2,   2,   2]", elems)

    # Mat + Mat
    a = Mat.ones(3, 3, CV_8U) * 100
    m = Mat.ones(3, 3, CV_8U) + a
    elems = m.to_s.scan(/(\[[^\]]+\])/m).flatten[0]
    assert_equal("[101, 101, 101;\n 101, 101, 101;\n 101, 101, 101]", elems)

    assert_raise(TypeError) {
      m + DUMMY_OBJ
    }
  end

  def test_sub
    # Mat - Number
    m0 = Mat.ones(3, 3, CV_8U) * 100
    m = m0 - 10
    elems = m.to_s.scan(/(\[[^\]]+\])/m).flatten[0]
    assert_equal("[ 90,  90,  90;\n  90,  90,  90;\n  90,  90,  90]", elems)

    # Mat + Scalar
    m = m0 - Scalar.new(5)
    elems = m.to_s.scan(/(\[[^\]]+\])/m).flatten[0]
    assert_equal("[ 95,  95,  95;\n  95,  95,  95;\n  95,  95,  95]", elems)

    # Mat + Mat
    a = Mat.ones(3, 3, CV_8U) * 2
    m = m0 - a
    elems = m.to_s.scan(/(\[[^\]]+\])/m).flatten[0]
    assert_equal("[ 98,  98,  98;\n  98,  98,  98;\n  98,  98,  98]", elems)

    assert_raise(TypeError) {
      m - DUMMY_OBJ
    }
  end

  def test_mul
    # Mat * Number
    m = Mat.ones(3, 3, CV_8U) * 100
    elems = m.to_s.scan(/(\[[^\]]+\])/m).flatten[0]
    assert_equal("[100, 100, 100;\n 100, 100, 100;\n 100, 100, 100]", elems)

    # Mat * Mat
    a = Mat.ones(1, 2, CV_32F) * 100
    m0 = Mat.ones(2, 1, CV_32F) * 2
    m = m0 * a
    elems = m.to_s.scan(/(\[[^\]]+\])/m).flatten[0]
    assert_equal("[200, 200;\n 200, 200]", elems)

    assert_raise(TypeError) {
      m * DUMMY_OBJ
    }
  end

  def test_div
    # Mat / Number
    m = Mat.ones(3, 3, CV_32F) / 10
    elems = m.to_s.scan(/(\[[^\]]+\])/m).flatten[0]
    assert_equal("[0.1, 0.1, 0.1;\n 0.1, 0.1, 0.1;\n 0.1, 0.1, 0.1]", elems)

    # Mat / Mat
    a = Mat.ones(3, 3, CV_32F) * 2
    m0 = Mat.ones(3, 3, CV_32F) * 10
    m = m0 / a
    elems = m.to_s.scan(/(\[[^\]]+\])/m).flatten[0]
    assert_equal("[5, 5, 5;\n 5, 5, 5;\n 5, 5, 5]", elems)

    assert_raise(TypeError) {
      m / DUMMY_OBJ
    }
  end

  def test_resize
    m0 = Mat.ones(200, 300, CV_8U)
    s = Size.new(150, 100)

    m = m0.resize(s)
    assert_equal(s.height, m.rows)
    assert_equal(s.width, m.cols)
    assert_equal(m0.depth, m.depth)
    assert_equal(m0.dims, m.dims)
    assert_equal(m0.channels, m.channels)

    [INTER_NEAREST, INTER_LINEAR, INTER_AREA,
     INTER_CUBIC, INTER_LANCZOS4].each { |interpolation|
      m = m0.resize(s, interpolation)
      assert_equal(s.height, m.rows)
      assert_equal(s.width, m.cols)
      assert_equal(m0.depth, m.depth)
      assert_equal(m0.dims, m.dims)
      assert_equal(m0.channels, m.channels)
    }

    assert_raise(TypeError) {
      m.resize(DUMMY_OBJ)
    }
  end 

  def test_diag
    m0 = Mat.new(3, 3, CV_8U)
    i = 1
    m0.rows.times { |r|
      m0.cols.times { |c|
        m0[r, c] = Scalar.new(i)
        i += 1
      }
    }

    m1 = m0.diag
    elems = m1.to_s.scan(/(\[[^\]]+\])/m).flatten[0]
    assert_equal("[  1;\n   5;\n   9]", elems)

    m2 = m0.diag(0)
    elems = m2.to_s.scan(/(\[[^\]]+\])/m).flatten[0]
    assert_equal("[  1;\n   5;\n   9]", elems)

    m3 = m0.diag(1)
    elems = m3.to_s.scan(/(\[[^\]]+\])/m).flatten[0]
    assert_equal("[  2;\n   6]", elems)

    m4 = m0.diag(-1)
    elems = m4.to_s.scan(/(\[[^\]]+\])/m).flatten[0]
    assert_equal("[  4;\n   8]", elems)

    assert_raise(TypeError) {
      m0.diag(DUMMY_OBJ)
    }
  end

  def test_dot
    m0 = Mat.new(2, 1, CV_8U);
    m0[0, 0] = Scalar.new(1)
    m0[1, 0] = Scalar.new(2)
    m1 = Mat.new(2, 1, CV_8U);
    m1[0, 0] = Scalar.new(3)
    m1[1, 0] = Scalar.new(4)

    a = m0.dot(m1)
    assert_in_delta(11.0, a, 0.01)

    assert_raise(TypeError) {
      m0.dot(DUMMY_OBJ)
    }
  end

  def test_cross
    m0 = Mat.new(3, 1, CV_32F)
    m0[0, 0] = Scalar.new(1)
    m0[1, 0] = Scalar.new(2)
    m0[2, 0] = Scalar.new(0)

    m1 = Mat.new(3, 1, CV_32F)
    m1[0, 0] = Scalar.new(0)
    m1[1, 0] = Scalar.new(1)
    m1[2, 0] = Scalar.new(-1)

    m = m0.cross(m1)
    elems = m.to_s.scan(/(\[[^\]]+\])/m).flatten[0]
    assert_equal("[-2;\n 1;\n 1]", elems)

    assert_raise(TypeError) {
      m0.cross(DUMMY_OBJ)
    }
  end

  def test_cvt_color
    m = Mat.new(1, 1, CV_32FC3)
    m[0, 0] = Scalar.new(1.0, 2.0, 3.0)
    delta = 0.01

    m2 = m.cvt_color(COLOR_BGR2GRAY)
    assert_in_delta(2.1849999, m2[0, 0][0], delta)

    m2 = m.cvt_color(COLOR_BGR2HSV)
    [30, 0.66666669, 3].each_with_index { |expected, i|
      assert_in_delta(expected, m2[0, 0][i], delta, "Failed at m2[0, 0][#{i}]")
    }

    assert_raise(TypeError) {
      m.cvt_color(DUMMY_OBJ)
    }
  end

  def test_convert_scale_abs
    m0 = Mat.ones(1, 1, CV_32F) * -128

    results = []
    results << m0.convert_scale_abs
    results << m0.convert_scale_abs(0.1)
    results << m0.convert_scale_abs(0.1, 10)
    results.each { |m|
      assert_equal(m0.rows, m.rows)
      assert_equal(m0.cols, m.cols)
      assert_equal(CV_8U, m.depth)
      assert_equal(m0.dims, m.dims)
      assert_equal(m0.channels, m.channels)
    }

    assert_raise(TypeError) {
      m0.convert_scale_abs(DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      m0.convert_scale_abs(0.1, DUMMY_OBJ)
    }
  end

  def test_convert_to
    m0 = Mat.ones(3, 3, CV_32F) * 1.1
    DEPTH.each { |depth|
      m = m0.convert_to(depth)
      assert_equal(m0.rows, m.rows)
      assert_equal(m0.cols, m.cols)
      assert_equal(m0.channels, m.channels)
      assert_equal(depth, m.depth)
      assert_equal(m0[0, 0][0].to_i, m[0, 0][0].to_i)
    }

    alpha = 3.0
    beta = 1.0
    m1 = (Mat.ones(3, 3, CV_32F) * alpha) + beta
    DEPTH.each { |depth|
      m = m0.convert_to(depth, alpha, beta)
      assert_equal(m1.rows, m.rows)
      assert_equal(m1.cols, m.cols)
      assert_equal(m1.channels, m.channels)
      assert_equal(depth, m.depth)
      assert_equal(m1[0, 0][0].to_i, m[0, 0][0].to_i)
    }

    assert_raise(TypeError) {
      m0.convert_to(DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      m0.convert_to(CV_8U, DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      m0.convert_to(CV_8U, 0.1, DUMMY_OBJ)
    }
  end

  def test_imencode
    m = Cv::imread(FILENAME_LENA32x32, -1)

    results = []
    results << m.imencode('.jpg')
    results << m.imencode('.jpg', [Cv::IMWRITE_JPEG_QUALITY, 10])
    results.each { |jpg|
      assert_equal('JFIF', jpg[6, 4].map(&:chr).join)
    }

    results = []
    results << m.imencode('.png')
    results << m.imencode('.png', [Cv::IMWRITE_PNG_COMPRESSION, 9])
    results.each { |png|
      assert_equal('PNG', png[1, 3].map(&:chr).join)
    }

    assert_raise(TypeError) {
      m.imencode(DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      m.imencode('.jpg', DUMMY_OBJ)
    }
  end

  def test_set_to
    s0 = Scalar.new(0, 0, 0, 0)
    s1 = Scalar.new(1, 1, 1, 1)
    DEPTH.each { |depth|
      (1..4).each { |channel|
        type = Cv::CV_MAKETYPE(depth, channel)
        m = Mat::zeros(3, 3, type)

        a = m.set_to(s1)
        assert_equal(a.class, m.class)
        assert_equal(a.rows, m.rows)
        assert_equal(a.cols, m.cols)
        assert_equal(a.depth, m.depth)
        assert_equal(a.dims, m.dims)
        assert_equal(a.channels, m.channels)

        expected = Mat.new(m.rows, m.cols, type)
        expected.rows.times { |r|
          expected.cols.times { |c|
            expected[r, c] = s1
          }
        }
        assert_mat_in_delta(expected, m, 0.01)
        assert_mat_in_delta(expected, a, 0.01)
      }
    }

    mask = Mat::zeros(3, 3, CV_8U)
    0.upto(1) { |r|
      0.upto(1) { |c|
        mask[r, c] = s1
      }
    }
    DEPTH.each { |depth|
      (1..4).each { |channel|
        type = Cv::CV_MAKETYPE(depth, channel)
        m = Mat::zeros(3, 3, type)

        a = m.set_to(s1, mask)
        assert_equal(a.class, m.class)
        assert_equal(a.rows, m.rows)
        assert_equal(a.cols, m.cols)
        assert_equal(a.depth, m.depth)
        assert_equal(a.dims, m.dims)
        assert_equal(a.channels, m.channels)

        expected = Mat.new(m.rows, m.cols, type)
        expected.rows.times { |r|
          expected.cols.times { |c|
            expected[r, c] = (mask[r, c][0] > 0) ? s1 : s0
          }
        }
        assert_mat_in_delta(expected, m, 0.01)
        assert_mat_in_delta(expected, a, 0.01)
      }
    }

    m = Mat::zeros(3, 3, CV_8U)
    assert_raise(TypeError) {
      m.set_to(DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      m.set_to(s1, DUMMY_OBJ)
    }
  end
end
