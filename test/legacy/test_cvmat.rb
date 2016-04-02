#!/usr/bin/env ruby
# -*- mode: ruby; coding: utf-8 -*-
require 'test/unit'
require 'opencv'
require File.expand_path(File.dirname(__FILE__)) + '/../helper'

include OpenCV

# Tests for OpenCV::CvMat
module Legacy
  class TestCvMat < OpenCVTestCase
    def test_initialize
      m = CvMat.new(10, 20)
      assert_equal(10, m.rows)
      assert_equal(20, m.cols)
      assert_equal(:cv8u, m.depth)
      assert_equal(3, m.channel)

      depth_table = {
        CV_8U => :cv8u,
        CV_8S => :cv8s,
        CV_16U => :cv16u,
        CV_16S => :cv16s,
        CV_32S => :cv32s,
        CV_32F => :cv32f,
        CV_64F => :cv64f
      }

      [CV_8U, CV_8S, CV_16U, CV_16S, CV_32S, CV_32F, CV_64F,
       :cv8u, :cv8s, :cv16u, :cv16s, :cv32s, :cv32f, :cv64f].each { |depth|
        [1, 2, 3, 4].each { |ch|
          m = CvMat.new(10, 20, depth, ch)
          assert_equal(10, m.rows)
          assert_equal(20, m.cols)
          depth = depth_table[depth] unless depth.is_a? Symbol
          assert_equal(depth, m.depth)
          assert_equal(ch, m.channel)
        }
      }

      assert_raise(TypeError) {
        m = CvMat.new(DUMMY_OBJ, 20, :cv8u, 1)
      }
      assert_raise(TypeError) {
        m = CvMat.new(10, DUMMY_OBJ, :cv8u, 1)
      }
      assert_raise(TypeError) {
        m = CvMat.new(10, 20, :cv8u, DUMMY_OBJ)
      }
    end

    def test_load
      mat = CvMat.load(FILENAME_CAT)
      assert_equal(CvMat, mat.class)
      assert_equal(375, mat.cols)
      assert_equal(500, mat.rows)
      assert_equal(:cv8u, mat.depth)
      assert_equal(3, mat.channel)

      mat = CvMat.load(FILENAME_CAT, CV_LOAD_IMAGE_GRAYSCALE)
      assert_equal(CvMat, mat.class)
      assert_equal(375, mat.cols)
      assert_equal(500, mat.rows)
      assert_equal(:cv8u, mat.depth)
      assert_equal(1, mat.channel)

      mat = CvMat.load(FILENAME_CAT, CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_ANYCOLOR)
      assert_equal(CvMat, mat.class)
      assert_equal(375, mat.cols)
      assert_equal(500, mat.rows)
      assert_equal(:cv8u, mat.depth)
      assert_equal(3, mat.channel)

      assert_raise(ArgumentError) {
        CvMat.load
      }
      assert_raise(TypeError) {
        CvMat.load(DUMMY_OBJ)
      }
      assert_raise(TypeError) {
        CvMat.load(FILENAME_CAT, DUMMY_OBJ)
      }
      assert_raise(StandardError) {
        CvMat.load('file/does/not/exist')
      }
    end

    def test_save_image
      filename_jpg = 'save_image_test.jpg'
      filename_png = 'save_image_test.png'
      m = CvMat.new(20, 20, :cv8u, 1)

      File.delete filename_jpg if File.exists? filename_jpg
      m.save_image filename_jpg
      assert(File.exists? filename_jpg)

      File.delete filename_jpg if File.exists? filename_jpg
      m.save_image(filename_jpg, CV_IMWRITE_JPEG_QUALITY => 10)
      assert(File.exists? filename_jpg)

      File.delete filename_png if File.exists? filename_png
      m.save_image(filename_png, CV_IMWRITE_PNG_COMPRESSION => 9)
      assert(File.exists? filename_png)

      # Alias
      File.delete filename_jpg if File.exists? filename_jpg
      m.save filename_jpg
      assert(File.exists? filename_jpg)

      assert_raise(TypeError) {
        m.save_image(DUMMY_OBJ)
      }
      assert_raise(TypeError) {
        m.save_image(filename_jpg, DUMMY_OBJ)
      }

      File.delete filename_jpg if File.exists? filename_jpg
      File.delete filename_png if File.exists? filename_png
    end

    def test_add_weighted
      m1 = create_cvmat(3, 2, :cv8u) { |j, i, c| CvScalar.new(c + 1) }
      m2 = create_cvmat(3, 2, :cv8u) { |j, i, c| CvScalar.new((c + 1) * 10) }
      a = 2.0
      b = 0.1
      g = 100
      m3 = CvMat.add_weighted(m1, a, m2, b, g)
      assert_equal(m1.class, m3.class)
      assert_equal(m1.rows, m3.rows)
      assert_equal(m1.cols, m3.cols)
      assert_equal(m1.depth, m3.depth)
      assert_equal(m1.channel, m3.channel)

      m1.rows.times { |j|
        m1.cols.times { |i|
          expected = m1[j, i][0] * a + m2[j, i][0] * b + g
          assert_equal(expected, m3[j, i][0])
        }
      }

      assert_raise(TypeError) {
        CvMat.add_weighted(DUMMY_OBJ, a, m2, b, g)
      }
      assert_raise(TypeError) {
        CvMat.add_weighted(m1, DUMMY_OBJ, m2, b, g)
      }
      assert_raise(TypeError) {
        CvMat.add_weighted(m1, a, DUMMY_OBJ, b, g)
      }
      assert_raise(TypeError) {
        CvMat.add_weighted(m1, a, m2, DUMMY_OBJ, g)
      }
      assert_raise(TypeError) {
        CvMat.add_weighted(m1, a, m2, b, DUMMY_OBJ)
      }
    end

    def test_encode
      mat = CvMat.load(FILENAME_CAT);

      jpg = mat.encode('.jpg')
      assert_equal('JFIF', jpg[6, 4].map(&:chr).join) # Is jpeg format?

      jpg = mat.encode('.jpg', CV_IMWRITE_JPEG_QUALITY => 10)
      assert_equal('JFIF', jpg[6, 4].map(&:chr).join)

      png = mat.encode('.png')
      assert_equal('PNG', png[1, 3].map(&:chr).join) # Is png format?

      png = mat.encode('.png', CV_IMWRITE_PNG_COMPRESSION => 9)
      assert_equal('PNG', png[1, 3].map(&:chr).join)

      assert_raise(TypeError) {
        mat.encode(DUMMY_OBJ)
      }
      assert_raise(TypeError) {
        mat.encode('.jpg', DUMMY_OBJ)
      }
    end

    def test_decode
      data = OpenCV::imread(FILENAME_CAT, -1).imencode('.jpg')
      expected = CvMat.load(FILENAME_CAT)

      mat1 = CvMat.decode(data)
      mat2 = CvMat.decode(data, CV_LOAD_IMAGE_COLOR)

      [mat1, mat2].each { |mat|
        assert_equal(CvMat, mat.class)
        assert_equal(expected.rows, mat.rows)
        assert_equal(expected.cols, mat.cols)
        assert_equal(expected.channel, mat.channel)
      }

      expected_c1 = CvMat.load(FILENAME_CAT, CV_LOAD_IMAGE_GRAYSCALE)
      mat1c1 = CvMat.decode(data, CV_LOAD_IMAGE_GRAYSCALE)

      [mat1c1].each { |mat|
        assert_equal(CvMat, mat.class)
        assert_equal(expected_c1.rows, mat.rows)
        assert_equal(expected_c1.cols, mat.cols)
        assert_equal(expected_c1.channel, mat.channel)
      }

      assert_raise(TypeError) {
        CvMat.decode(DUMMY_OBJ)
      }
      assert_raise(TypeError) {
        CvMat.decode(data, DUMMY_OBJ)
      }
    end
  end
end
