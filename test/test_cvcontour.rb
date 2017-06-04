#!/usr/bin/env ruby
# -*- mode: ruby; coding: utf-8 -*-
require 'test/unit'
require 'opencv'
require File.expand_path(File.dirname(__FILE__)) + '/helper'

include OpenCV

# Tests for OpenCV::CvContour
class TestCvContour < OpenCVTestCase
  def test_APPROX_OPTION
    assert_equal(0, CvContour::APPROX_OPTION[:method])
    assert_equal(1.0, CvContour::APPROX_OPTION[:accuracy])
    assert_false(CvContour::APPROX_OPTION[:recursive])
  end
  
  def test_initialize
    contour = CvContour.new
    assert_not_nil(contour)
    assert_equal(CvContour, contour.class)
    assert(contour.is_a? CvSeq)
  end

  def test_rect
    contour = CvContour.new
    assert_not_nil(contour.rect)
    assert_equal(CvRect, contour.rect.class)
  end

  def test_color
    contour = CvContour.new
    assert_equal(0, contour.color)
    contour.color = 1
    assert_equal(1, contour.color)
  end

  def test_reserved
    reserved = CvContour.new.reserved
    assert_equal(Array, reserved.class)
    assert_equal(3, reserved.size)
  end

  def test_approx_poly
    mat0 = create_cvmat(128, 128, :cv8u, 1) { |j, i|
      (j - 64) ** 2 + (i - 64) ** 2 <= (32 ** 2) ? CvColor::White : CvColor::Black
    }
    contours = mat0.find_contours(:mode => CV_RETR_EXTERNAL)

    poly = contours.approx_poly
    assert_equal(CvContour, poly.class)
    assert(poly.size > 0)
    assert(poly.all? { |c| c.class == CvPoint })

    poly = contours.approx_poly(:method => :dp)
    assert_equal(CvContour, poly.class)
    assert(poly.size > 0)
    assert(poly.all? { |c| c.class == CvPoint })

    poly = contours.approx_poly(:accuracy => 2.0)
    assert_equal(CvContour, poly.class)
    assert(poly.size > 0)
    assert(poly.all? { |c| c.class == CvPoint })

    [true, false, 1, 0].each { |recursive|
      poly = contours.approx_poly(:recursive => recursive)
      assert_equal(CvContour, poly.class)
      assert(poly.size > 0)
      assert(poly.all? { |c| c.class == CvPoint })
    }

    poly = contours.approx_poly(:method => :dp, :accuracy => 2.0, :recursive => false)
    assert_equal(CvContour, poly.class)
    assert(poly.size > 0)
    assert(poly.all? { |c| c.class == CvPoint })

    # Uncomment the following lines to show the result
    # poly = contours.approx_poly(:accuracy => 3.0)
    # dst = mat0.clone.zero
    # begin
    #   dst.draw_contours!(poly, CvColor::White, CvColor::Black, 2,
    #                      :thickness => 1, :line_type => :aa)
    # end while (poly = poly.h_next)
    # snap dst
  end
  
  def test_bounding_rect
    mat0 = create_cvmat(128, 128, :cv8u, 1) { |j, i|
      (j - 64) ** 2 + (i - 64) ** 2 <= (32 ** 2) ? CvColor::White : CvColor::Black
    }
    contours = mat0.find_contours
    rect = contours.bounding_rect
    assert_equal(CvRect, rect.class)
    assert_equal(32, rect.x)
    assert_equal(32, rect.y)
    assert_equal(65, rect.width)
    assert_equal(65, rect.height)
  end

  def test_in
    mat0 = create_cvmat(128, 128, :cv8u, 1) { |j, i|
      (j - 64) ** 2 + (i - 64) ** 2 <= (32 ** 2) ? CvColor::White : CvColor::Black
    }
    contour = mat0.find_contours
    assert(contour.in? CvPoint.new(64, 64))
    assert_false(contour.in? CvPoint.new(0, 0))
    assert_nil(contour.in? CvPoint.new(64, 32))
  end

  def test_measure_distance
    mat0 = create_cvmat(128, 128, :cv8u, 1) { |j, i|
      (j - 64) ** 2 + (i - 64) ** 2 <= (32 ** 2) ? CvColor::White : CvColor::Black
    }
    contour = mat0.find_contours
    assert_in_delta(-0.7071, contour.measure_distance(CvPoint.new(63, 32)), 0.01)
    assert_in_delta(31.01, contour.measure_distance(CvPoint.new(64, 64)), 0.01)
  end

  def test_point_polygon_test
    mat0 = create_cvmat(128, 128, :cv8u, 1) { |j, i|
      (j - 64) ** 2 + (i - 64) ** 2 <= (32 ** 2) ? CvColor::White : CvColor::Black
    }
    contour = mat0.find_contours

    assert_equal(1, contour.point_polygon_test(CvPoint.new(64, 64), 0))
    assert_equal(1, contour.point_polygon_test(CvPoint.new(64, 64), false))
    assert_equal(-1, contour.point_polygon_test(CvPoint.new(0, 0), 0))
    assert_equal(-1, contour.point_polygon_test(CvPoint.new(0, 0), false))
    assert_equal(0, contour.point_polygon_test(CvPoint.new(64, 32), 0))
    assert_equal(0, contour.point_polygon_test(CvPoint.new(64, 32), false))

    assert_in_delta(-0.7071, contour.point_polygon_test(CvPoint.new(63, 32), 1), 0.01)
    assert_in_delta(-0.7071, contour.point_polygon_test(CvPoint.new(63, 32), true), 0.01)
    assert_in_delta(31.01, contour.point_polygon_test(CvPoint.new(64, 64), 1), 0.01)
    assert_in_delta(31.01, contour.point_polygon_test(CvPoint.new(64, 64), true), 0.01)
  end

  def test_match_shapes
    img1 = CvMat.load(FILENAME_CONTOURS, CV_LOAD_IMAGE_GRAYSCALE).threshold(127, 255, CV_THRESH_BINARY)
    img2 = CvMat.load(FILENAME_LINES, CV_LOAD_IMAGE_GRAYSCALE).threshold(127, 255, CV_THRESH_BINARY)
    c1 = img1.find_contours(mode: CV_RETR_EXTERNAL)
    c2 = img2.find_contours(mode: CV_RETR_EXTERNAL)

    [CV_CONTOURS_MATCH_I1, CV_CONTOURS_MATCH_I2, CV_CONTOURS_MATCH_I3].each { |method|
      assert_in_delta(0, c1.match_shapes(c1, method), 0.01)
      assert_in_delta(0, c1.match_shapes(c1, method, nil), 0.01)

      assert(c1.match_shapes(c2, method) > 0)
      assert(c1.match_shapes(c2, method, nil) > 0)
    }

    assert_raise(TypeError) {
      c1.match_shapes(DUMMY_OBJ, CV_CONTOURS_MATCH_I1)
    }
    assert_raise(TypeError) {
      c1.match_shapes(c2, DUMMY_OBJ)
    }
  end

  def test_create_tree
    omit_unless(IS_OPENCV2, 'CvContour#create_tree is not supported in OpenCV 3 or later')
    mat0 = create_cvmat(128, 128, :cv8u, 1) { |j, i|
      (j - 64) ** 2 + (i - 64) ** 2 <= (32 ** 2) ? CvColor::White : CvColor::Black
    }
    contour = mat0.find_contours
    tree = contour.create_tree
    assert_equal(CvContourTree, tree.class)
    assert_equal(34, tree.p1.x)
    assert_equal(53, tree.p1.y)
    assert_equal(0, tree.p2.x)
    assert_equal(0, tree.p2.y)
  end
end
