#!/usr/bin/env ruby
# -*- mode: ruby; coding: utf-8 -*-
require 'test/unit'
require 'opencv'
require File.expand_path(File.dirname(__FILE__)) + '/helper'

include OpenCV

# Tests for image processing functions of OpenCV::CvMat
class TestCvMat_imageprocessing < OpenCVTestCase
  FILENAME_LENA256x256 = File.expand_path(File.dirname(__FILE__)) + '/samples/lena-256x256.jpg'
  FILENAME_LENA_INPAINT = File.expand_path(File.dirname(__FILE__)) + '/samples/lena-inpaint.jpg'
  FILENAME_INPAINT_MASK = File.expand_path(File.dirname(__FILE__)) + '/samples/inpaint-mask.bmp'
  FILENAME_LENA32x32 = File.expand_path(File.dirname(__FILE__)) + '/samples/lena-32x32.jpg'
  FILENAME_LINES = File.expand_path(File.dirname(__FILE__)) + '/samples/lines.jpg'
  FILENAME_LENA_EYES = File.expand_path(File.dirname(__FILE__)) + '/samples/lena-eyes.jpg'
  FILENAME_STR_CV = File.expand_path(File.dirname(__FILE__)) + '/samples/str-cv.jpg'
  FILENAME_STR_OV = File.expand_path(File.dirname(__FILE__)) + '/samples/str-ov.jpg'
  FILENAME_STR_CV_ROTATED = File.expand_path(File.dirname(__FILE__)) + '/samples/str-cv-rotated.jpg'

  def test_sobel
    mat0 = CvMat.load(FILENAME_LENA256x256, CV_LOAD_IMAGE_GRAYSCALE)

    mat1 = mat0.sobel(1, 0).convert_scale_abs(:scale => 1, :shift => 0)
    mat2 = mat0.sobel(0, 1).convert_scale_abs(:scale => 1, :shift => 0)
    mat3 = mat0.sobel(1, 1).convert_scale_abs(:scale => 1, :shift => 0)
    mat4 = mat0.sobel(1, 1, 3).convert_scale_abs(:scale => 1, :shift => 0)
    mat5 = mat0.sobel(1, 1, 5).convert_scale_abs(:scale => 1, :shift => 0)

    assert_equal('30a26b7287fac75bb697bc7eef6bb53a', hash_img(mat1))
    assert_equal('b740afb13b556d55280fa785190ac902', hash_img(mat2))
    assert_equal('36c29ca64a599e0f5633f4f3948ed858', hash_img(mat3))
    assert_equal('36c29ca64a599e0f5633f4f3948ed858', hash_img(mat4))
    assert_equal('30b9e8fd64e7f86c50fb67d8703628e3', hash_img(mat5))

    assert_equal(:cv16s, CvMat.new(16, 16, :cv8u, 1).sobel(1, 1).depth)
    assert_equal(:cv32f, CvMat.new(16, 16, :cv32f, 1).sobel(1, 1).depth)

    (DEPTH.keys - [:cv8u, :cv32f]).each { |depth|
      assert_raise(ArgumentError) {
        CvMat.new(3, 3, depth).sobel(1, 1)
      }
    }

    # Uncomment the following lines to view the images
    # snap(['original', mat0], ['sobel(1,0)', mat1], ['sobel(0,1)', mat2],
    #      ['sobel(1,1)', mat3], ['sobel(1,1,3)', mat4], ['sobel(1,1,5)', mat5])

    assert_raise(TypeError) {
      mat0.sobel(DUMMY_OBJ, 0)
    }
    assert_raise(TypeError) {
      mat0.sobel(1, DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      mat0.sobel(1, 0, DUMMY_OBJ)
    }
  end

  def test_laplace
    mat0 = CvMat.load(FILENAME_LENA256x256, CV_LOAD_IMAGE_GRAYSCALE)

    mat1 = mat0.laplace.convert_scale_abs(:scale => 1, :shift => 0)
    mat2 = mat0.laplace(3).convert_scale_abs(:scale => 1, :shift => 0)
    mat3 = mat0.laplace(5).convert_scale_abs(:scale => 1, :shift => 0)

    assert_equal('824f8de75bfead5d83c4226f3948ce69', hash_img(mat1))
    assert_equal('824f8de75bfead5d83c4226f3948ce69', hash_img(mat2))
    assert_equal('23850bb8cfe9fd1b82cd73b7b4659369', hash_img(mat3))

    assert_equal(:cv16s, CvMat.new(16, 16, :cv8u, 1).laplace.depth)
    assert_equal(:cv32f, CvMat.new(16, 16, :cv32f, 1).laplace.depth)

    (DEPTH.keys - [:cv8u, :cv32f]).each { |depth|
      assert_raise(ArgumentError) {
        CvMat.new(3, 3, depth).laplace
      }
    }

    # Uncomment the following line to view the images
    # snap(['original', mat0], ['laplace', mat1], ['laplace(3)', mat2], ['laplace(5)', mat3])

    assert_raise(TypeError) {
      mat0.laplace(DUMMY_OBJ)
    }
  end

  def test_canny
    mat0 = CvMat.load(FILENAME_LENA256x256, CV_LOAD_IMAGE_GRAYSCALE)
    mat1 = mat0.canny(50, 200)
    mat2 = mat0.canny(50, 200, 3)
    mat3 = mat0.canny(50, 200, 5)

    assert_equal('ec3e88035bb98b5c5f1a08c8e07ab0a8', hash_img(mat1))
    assert_equal('ec3e88035bb98b5c5f1a08c8e07ab0a8', hash_img(mat2))
    assert_equal('1983a6d325d11eea3261462103b0dae1', hash_img(mat3))

    # Uncomment the following line to view the images
    # snap(['canny(50,200)', mat1], ['canny(50,200,3)', mat2], ['canny(50,200,5)', mat3])

    assert_raise(TypeError) {
      mat0.canny(DUMMY_OBJ, 200)
    }
    assert_raise(TypeError) {
      mat0.canny(50, DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      mat0.canny(50, 200, DUMMY_OBJ)
    }
  end

  def test_pre_corner_detect
    mat0 = CvMat.load(FILENAME_LENA256x256, CV_LOAD_IMAGE_GRAYSCALE)
    mat1 = mat0.pre_corner_detect
    mat2 = mat0.pre_corner_detect(3)
    mat3 = mat0.pre_corner_detect(5)

    assert_in_delta(0, count_threshold(mat1, 0.1), 30)
    assert_in_delta(0, count_threshold(mat2, 0.1), 30)
    assert_in_delta(380, count_threshold(mat3, 0.1), 30)

    # Uncomment the following lines to show the images
    # snap(['original', mat0], ['pre_coner_detect', mat1],
    #      ['pre_coner_detect(3)', mat2], ['pre_coner_detect(5)', mat3])

    assert_raise(TypeError) {
      mat0.pre_corner_detect(DUMMY_OBJ)
    }
  end

  def test_corner_eigenvv
    mat0 = CvMat.load(FILENAME_LENA256x256, CV_LOAD_IMAGE_GRAYSCALE)
    mat1 = mat0.corner_eigenvv(3)
    mat2 = mat0.corner_eigenvv(3, 3)

    assert_raise(TypeError) {
      mat0.corner_eigenvv(DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      mat0.corner_eigenvv(3, DUMMY_OBJ)
    }
  end

  def test_corner_min_eigen_val
    mat0 = CvMat.load(FILENAME_LENA256x256, CV_LOAD_IMAGE_GRAYSCALE)
    mat1 = mat0.corner_min_eigen_val(3)
    mat2 = mat0.corner_min_eigen_val(3, 3)

    assert_raise(TypeError) {
      mat0.corner_min_eigen_val(DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      mat0.corner_min_eigen_val(3, DUMMY_OBJ)
    }
  end

  def test_corner_harris
    mat0 = CvMat.load(FILENAME_LENA256x256, CV_LOAD_IMAGE_GRAYSCALE)
    mat1 = mat0.corner_harris(3)
    mat2 = mat0.corner_harris(3, 3)
    mat3 = mat0.corner_harris(3, 3, 0.04)
    mat4 = mat0.corner_harris(3, 7, 0.01)

    [mat1, mat2, mat3].each { |mat|
      assert_equal(mat0.rows, mat.rows)
      assert_equal(mat0.cols, mat.cols)
      assert_in_delta(0, count_threshold(mat, 10), 10)
    }
    assert_equal(mat0.rows, mat4.rows)
    assert_equal(mat0.cols, mat4.cols)
    assert_in_delta(90, count_threshold(mat4, 10), 10)

    # Uncomment the following lines to show the images
    # snap(['original', mat0], ['corner_harris(3)', mat1], ['corner_harris(3,3)', mat2],
    #      ['corner_harris(3,3,0.04)', mat3], ['corner_harris(3,7,0.01)', mat4])

    assert_raise(TypeError) {
      mat0.corner_harris(DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      mat0.corner_harris(3, DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      mat0.corner_harris(3, 3, DUMMY_OBJ)
    }
  end

  def test_find_chessboard_corners
    mat = CvMat.load(FILENAME_CHESSBOARD, CV_LOAD_IMAGE_GRAYSCALE)
    pattern_size = CvSize.new(4, 4)
    corners1, found1 = mat.find_chessboard_corners(pattern_size)
    corners2, found2 = mat.find_chessboard_corners(pattern_size, CV_CALIB_CB_ADAPTIVE_THRESH)
    corners3, found3 = mat.find_chessboard_corners(pattern_size, CV_CALIB_CB_NORMALIZE_IMAGE)
    corners4, found4 = mat.find_chessboard_corners(pattern_size, CV_CALIB_CB_FILTER_QUADS)
    corners5, found5 = mat.find_chessboard_corners(pattern_size, CV_CALIB_CB_FAST_CHECK)

    expected = [[39, 39], [79, 39], [119, 39], [159, 39], [39, 79], [79, 79],
                [119, 79], [159, 78], [38, 119], [79, 119], [119, 119], [158, 118],
                [39, 159], [79, 159], [119, 159], [159, 159]]
    [corners1, corners2, corners3, corners4, corners5].each { |corners|
      assert_equal(expected.size, corners.size)
      expected.zip(corners).each { |e, a|
        assert_in_delta(e[0], a.x, 3.0)
        assert_in_delta(e[1], a.y, 3.0)
      }
    }
    [found1, found2, found3, found4, found5].each { |found|
      assert(found)
    }

    assert_raise(TypeError) {
      mat.find_chessboard_corners(DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      mat.find_chessboard_corners(pattern_size, DUMMY_OBJ)
    }
  end

  def test_find_corner_sub_pix
    mat = CvMat.load(FILENAME_CHESSBOARD, CV_LOAD_IMAGE_GRAYSCALE)
    pattern_size = CvSize.new(4, 4)
    corners, found = mat.find_chessboard_corners(pattern_size)
    expected = [[39, 39], [79, 39], [119, 39], [159, 39], [39, 79], [79, 79],
                [119, 79], [159, 78], [38, 119], [79, 119], [119, 119], [158, 118],
                [39, 159], [79, 159], [119, 159], [159, 159]]

    refined_corners = mat.find_corner_sub_pix(corners, CvSize.new(3, 3), CvSize.new(-1, -1),
                                              CvTermCriteria.new(20, 0.03));
    assert_equal(expected.size, refined_corners.size)
    assert(found)
    expected.zip(refined_corners).each { |e, a|
      assert_in_delta(e[0], a.x, 3.0)
      assert_in_delta(e[1], a.y, 3.0)
    }

    assert_raise(TypeError) {
      mat.find_corner_sub_pix(DUMMY_OBJ, CvSize.new(3, 3), CvSize.new(-1, -1),
                              CvTermCriteria.new(20, 0.03));
    }
    assert_raise(TypeError) {
      mat.find_corner_sub_pix(corners, DUMMY_OBJ, CvSize.new(-1, -1),
                              CvTermCriteria.new(20, 0.03));
    }
    assert_raise(TypeError) {
      mat.find_corner_sub_pix(corners, CvSize.new(3, 3), DUMMY_OBJ,
                              CvTermCriteria.new(20, 0.03));
    }
    assert_raise(TypeError) {
      mat.find_corner_sub_pix(corners, CvSize.new(3, 3), CvSize.new(-1, -1), DUMMY_OBJ);
    }
  end

  def test_good_features_to_track
    mat0 = CvMat.load(FILENAME_LENA32x32, CV_LOAD_IMAGE_GRAYSCALE)
    mask = create_cvmat(mat0.rows, mat0.cols, :cv8u, 1) { |j, i, c|
      if (i > 8 and i < 18) and (j > 8 and j < 18)
        CvScalar.new(1)
      else
        CvScalar.new(0)
      end
    }

    corners1 = mat0.good_features_to_track(0.2, 5)
    corners2 = mat0.good_features_to_track(0.2, 5, :mask => mask)
    corners3 = mat0.good_features_to_track(0.2, 5, :block_size => 7)
    corners4 = mat0.good_features_to_track(0.2, 5, :use_harris => true)
    corners5 = mat0.good_features_to_track(0.2, 5, :k => 0.01)
    corners6 = mat0.good_features_to_track(0.2, 5, :max => 1)

    expected1 = [[24, 7], [20, 23], [17, 11], [26, 29], [30, 24],
                 [19, 16], [28, 2], [13, 18], [14, 4]]
    assert_equal(expected1.size, corners1.size)
    expected1.each_with_index { |e, i|
      assert_equal(e[0], corners1[i].x.to_i)
      assert_equal(e[1], corners1[i].y.to_i)
    }
    expected2 = [[17, 11], [17, 16]]
    assert_equal(expected2.size, corners2.size)
    expected2.each_with_index { |e, i|
      assert_equal(e[0], corners2[i].x.to_i)
      assert_equal(e[1], corners2[i].y.to_i)
    }

    expected3 = [[21, 7], [22, 23], [18, 12], [28, 4], [28, 26],
                 [17, 27], [13, 20], [10, 11], [14, 5]]
    assert_equal(expected3.size, corners3.size)
    expected3.each_with_index { |e, i|
      assert_equal(e[0], corners3[i].x.to_i)
      assert_equal(e[1], corners3[i].y.to_i)
    }

    expected4 = [[24, 8], [20, 23], [16, 11],
                 [20, 16],[27, 28], [28, 2]]
    assert_equal(expected4.size, corners4.size)
    expected4.each_with_index { |e, i|
      assert_equal(e[0], corners4[i].x.to_i)
      assert_equal(e[1], corners4[i].y.to_i)
    }

    expected5 = [[24, 7], [20, 23], [17, 11], [26, 29], [30, 24],
                 [19, 16], [28, 2], [13, 18], [14, 4]]
    assert_equal(expected5.size, corners5.size)
    expected5.each_with_index { |e, i|
      assert_equal(e[0], corners5[i].x.to_i)
      assert_equal(e[1], corners5[i].y.to_i)
    }

    assert_equal(1, corners6.size)
    assert_equal(24, corners6[0].x.to_i)
    assert_equal(7, corners6[0].y.to_i)

    assert_raise(ArgumentError) {
      mat0.good_features_to_track(0.2, 5, :max => 0)
    }

    assert_raise(TypeError) {
      mat0.good_features_to_track(DUMMY_OBJ, 5)
    }
    assert_raise(TypeError) {
      mat0.good_features_to_track(0.2, DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      mat0.good_features_to_track(0.2, 5, :mask => DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      mat0.good_features_to_track(0.2, 5, :block_size => DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      mat0.good_features_to_track(0.2, 5, :k => DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      mat0.good_features_to_track(0.2, 5, :max => DUMMY_OBJ)
    }
    mat0.good_features_to_track(0.2, 5, :use_harris => DUMMY_OBJ)
  end

  def test_rect_sub_pix
    mat0 = CvMat.load(FILENAME_LENA256x256, CV_LOAD_IMAGE_ANYCOLOR | CV_LOAD_IMAGE_ANYDEPTH)
    center = CvPoint2D32f.new(mat0.width / 2, mat0.height / 2)
    mat1 = mat0.rect_sub_pix(center)
    mat2 = mat0.rect_sub_pix(center, mat0.size)
    mat3 = mat0.rect_sub_pix(center, CvSize.new(512, 512))

    assert_raise(TypeError) {
      mat0.rect_sub_pix(DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      mat0.rect_sub_pix(center, DUMMY_OBJ)
    }
  end

  def test_quadrangle_sub_pix
    mat0 = CvMat.load(FILENAME_LENA256x256, CV_LOAD_IMAGE_ANYCOLOR | CV_LOAD_IMAGE_ANYDEPTH)
    angle = 60 * Math::PI / 180
    map_matrix = CvMat.new(2, 3, :cv32f, 1)
    map_matrix[0] = CvScalar.new(Math.cos(angle))
    map_matrix[1] = CvScalar.new(-Math.sin(angle))
    map_matrix[2] = CvScalar.new(mat0.width * 0.5)
    map_matrix[3] = CvScalar.new(-map_matrix[1][0])
    map_matrix[4] = map_matrix[0]
    map_matrix[5] = CvScalar.new(mat0.height * 0.5)

    mat1 = mat0.quadrangle_sub_pix(map_matrix)
    mat2 = mat0.quadrangle_sub_pix(map_matrix, mat0.size)
    mat3 = mat0.quadrangle_sub_pix(map_matrix, CvSize.new(512, 512))

    assert_raise(TypeError) {
      mat0.quadrangle_sub_pix(DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      mat0.quadrangle_sub_pix(map_matrix, DUMMY_OBJ)
    }
    # assert_raise(CvError) {
    #   mat0.quadrangle_sub_pix(CvMat.new(3, 3))
    # }
  end

  def test_resize
    mat0 = CvMat.load(FILENAME_LENA256x256, CV_LOAD_IMAGE_ANYCOLOR | CV_LOAD_IMAGE_ANYDEPTH)
    size = CvSize.new(384, 384)
    mat1 = mat0.resize(size)
    mat2 = mat0.resize(size, CV_INTER_LINEAR)
    mat3 = mat0.resize(size, CV_INTER_NN)
    mat4 = mat0.resize(size, CV_INTER_AREA)
    mat5 = mat0.resize(size, CV_INTER_CUBIC)
    mat6 = mat0.resize(size, CV_INTER_LANCZOS4)

    [mat1, mat2, mat3, mat4, mat5, mat6].each { |m|
      assert_equal(size.width, m.cols)
      assert_equal(size.height, m.rows)
      assert_equal(mat0.depth, m.depth)
      assert_equal(mat0.channel, m.channel)
    }

    assert_raise(TypeError) {
      mat0.resize(DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      mat0.resize(size, DUMMY_OBJ)
    }

    # Uncomment the following lines to show the results
    # snap(['original', mat0], ['default(linear)', mat1], ['linear', mat2],
    #      ['nn', mat3], ['area', mat4], ['cubic', mat5] , ['lanczos4', mat6])
  end

  def test_warp_affine
    mat0 = CvMat.load(FILENAME_LENA256x256, CV_LOAD_IMAGE_ANYCOLOR | CV_LOAD_IMAGE_ANYDEPTH)
    map_matrix = CvMat.new(2, 3, :cv32f, 1)
    # center: (128, 128), angle: 25 deg., scale: 1.0
    map_matrix[0] = CvScalar.new(0.90631)
    map_matrix[1] = CvScalar.new(0.42262)
    map_matrix[2] = CvScalar.new(-42.10254)
    map_matrix[3] = CvScalar.new(-0.42262)
    map_matrix[4] = CvScalar.new(0.90631)
    map_matrix[5] = CvScalar.new(66.08774)

    mat1 = mat0.warp_affine(map_matrix)
    mat2 = mat0.warp_affine(map_matrix, CV_INTER_NN | CV_WARP_FILL_OUTLIERS)
    mat3 = mat0.warp_affine(map_matrix, CV_INTER_LINEAR | CV_WARP_FILL_OUTLIERS, CvColor::Yellow)
    mat4 = mat0.warp_affine(map_matrix, CV_INTER_LINEAR | CV_WARP_FILL_OUTLIERS | CV_WARP_INVERSE_MAP)

    [mat1, mat2, mat3, mat4].each { |m|
      assert_equal(mat0.cols, m.cols)
      assert_equal(mat0.rows, m.rows)
      assert_equal(mat0.depth, m.depth)
      assert_equal(mat0.channel, m.channel)
    }

    assert_raise(TypeError) {
      mat0.warp_affine(DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      mat0.warp_affine(map_matrix, DUMMY_OBJ)
    }

    # Uncomment the following lines to show the results
    # snap mat0, mat1, mat2, mat3, mat4
  end

  def test_get_perspective_transform
    from = [
      OpenCV::CvPoint2D32f.new(540, 382),
      OpenCV::CvPoint2D32f.new(802, 400),
      OpenCV::CvPoint2D32f.new(850, 731),
      OpenCV::CvPoint2D32f.new(540, 731),
    ]
    to = [
      OpenCV::CvPoint2D32f.new(0, 0),
      OpenCV::CvPoint2D32f.new(233, 0),
      OpenCV::CvPoint2D32f.new(233, 310),
      OpenCV::CvPoint2D32f.new(0, 310),
    ]
    transform = OpenCV::CvMat.get_perspective_transform(from, to)
    assert_equal 3, transform.rows
    assert_equal 3, transform.columns
    expected = [
      0.923332154750824,
      0.0,
      0.0,
      1.4432899320127035e-15,
      0.0,
      0.0,
      -498.599365234375,
      0.0,
      0.0,
    ]
    3.times do |i|
      3.times do |j|
        assert_in_delta(expected.shift, transform[i][j], 0.001)
      end
    end
  end

  def test_rotation_matrix2D
    mat1 = CvMat.rotation_matrix2D(CvPoint2D32f.new(10, 20), 60, 2.0)
    expected = [1.0, 1.73205, -34.64102,
                -1.73205, 1.0, 17.32051]
    assert_equal(2, mat1.rows)
    assert_equal(3, mat1.cols)
    assert_equal(:cv32f, mat1.depth)
    assert_equal(1, mat1.channel)
    expected.each_with_index { |x, i|
      assert_in_delta(x, mat1[i][0], 0.001)
    }

    assert_raise(TypeError) {
      CvMat.rotation_matrix2D(DUMMY_OBJ, 60, 2.0)
    }
    assert_raise(TypeError) {
      CvMat.rotation_matrix2D(CvPoint2D32f.new(10, 20), DUMMY_OBJ, 2.0)
    }
    assert_raise(TypeError) {
      CvMat.rotation_matrix2D(CvPoint2D32f.new(10, 20), 60, DUMMY_OBJ)
    }
  end

  def test_warp_perspective
    mat0 = CvMat.load(FILENAME_LENA256x256, CV_LOAD_IMAGE_ANYCOLOR | CV_LOAD_IMAGE_ANYDEPTH)
    # Homography
    #   <src>     =>    <dst>
    # (0, 0)      =>  (50, 0)
    # (255, 0)    =>  (205, 0)
    # (255, 255)  =>  (255, 220)
    # (0, 255)    =>  (0, 275)
    map_matrix = CvMat.new(3, 3, :cv32f, 1)
    map_matrix[0] = CvScalar.new(0.72430)
    map_matrix[1] = CvScalar.new(-0.19608) 
    map_matrix[2] = CvScalar.new(50.00000) 
    map_matrix[3] = CvScalar.new(0.0) 
    map_matrix[4] = CvScalar.new(0.62489)
    map_matrix[5] = CvScalar.new(0.0)
    map_matrix[6] = CvScalar.new(0.00057)
    map_matrix[7] = CvScalar.new(-0.00165)
    map_matrix[8] = CvScalar.new(1.00000)
    
    mat1 = mat0.warp_perspective(map_matrix)
    mat2 = mat0.warp_perspective(map_matrix, CV_INTER_NN)
    mat3 = mat0.warp_perspective(map_matrix, CV_INTER_LINEAR | CV_WARP_INVERSE_MAP)
    mat4 = mat0.warp_perspective(map_matrix, CV_INTER_LINEAR | CV_WARP_FILL_OUTLIERS, CvColor::Yellow)

    [mat1, mat2, mat3, mat4].each { |m|
      assert_equal(mat0.cols, m.cols)
      assert_equal(mat0.rows, m.rows)
      assert_equal(mat0.depth, m.depth)
      assert_equal(mat0.channel, m.channel)
    }

    assert_raise(TypeError) {
      mat0.warp_perspective(DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      mat0.warp_perspective(map_matrix, DUMMY_OBJ)
    }

    # Uncomment the following line to show the results
    # snap mat0, mat1, mat2, mat3, mat4
  end

  def test_remap
    mat0 = CvMat.load(FILENAME_LENA256x256, CV_LOAD_IMAGE_ANYCOLOR | CV_LOAD_IMAGE_ANYDEPTH)
    matx = CvMat.new(mat0.height, mat0.width, :cv32f, 1).clear
    maty = CvMat.new(mat0.height, mat0.width, :cv32f, 1).clear

    cos30, sin30 = Math.cos(30 * Math::PI / 180), Math.sin(30 * Math::PI / 180)
    half_width, half_height = mat0.width / 2, mat0.height / 2
    mat0.height.times { |j|
      mat0.width.times { |i|
        x0 = i - half_width
        y0 = j - half_height
        x = x0 * cos30 - y0 * sin30 + half_width
        y = x0 * sin30 + y0 * cos30 + half_height
        matx[j, i] = CvScalar.new(x)
        maty[j, i] = CvScalar.new(y)
      }
    }

    mat1 = mat0.remap(matx, maty)
    mat2 = mat0.remap(matx, maty, CV_INTER_NN)
    mat3 = mat0.remap(matx, maty, CV_INTER_LINEAR | CV_WARP_FILL_OUTLIERS, CvColor::Yellow)

    [mat1, mat2, mat3].each { |m|
      assert_equal(mat0.cols, m.cols)
      assert_equal(mat0.rows, m.rows)
      assert_equal(mat0.depth, m.depth)
      assert_equal(mat0.channel, m.channel)
    }

    assert_raise(TypeError) {
      mat0.remap(DUMMY_OBJ, maty)
    }
    assert_raise(TypeError) {
      mat0.remap(matx, DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      mat0.remap(matx, maty, DUMMY_OBJ)
    }

    # Uncomment the following line to show the results
    # snap mat0, mat1, mat2, mat3
  end

  def test_log_polar
    mat0 = CvMat.load(FILENAME_FRUITS, CV_LOAD_IMAGE_ANYCOLOR | CV_LOAD_IMAGE_ANYDEPTH)

    mat1 = mat0.log_polar(CvSize.new(255, 255), CvPoint2D32f.new(mat0.width / 2, mat0.height / 2), 40)
    mat2 = mat0.log_polar(CvSize.new(255, 255), CvPoint2D32f.new(mat0.width / 2, mat0.height / 2), 40,
                          CV_INTER_LINEAR | CV_WARP_FILL_OUTLIERS)
    mat3 = mat1.log_polar(mat0.size, CvPoint2D32f.new(mat0.width / 2, mat0.height / 2), 40,
                          CV_INTER_LINEAR | CV_WARP_FILL_OUTLIERS | CV_WARP_INVERSE_MAP)

    [mat1, mat2].each { |mat|
      assert_equal(mat0.depth, mat.depth)
      assert_equal(mat0.channel, mat.channel)
      b, g, r =  color_hists(mat)
      assert_in_delta(3800000, b, 100000)
      assert_in_delta(5860000, g, 100000)
      assert_in_delta(7700000, r, 100000)
    }

    b, g, r =  color_hists(mat3)
    assert_equal(mat0.depth, mat3.depth)
    assert_equal(mat0.channel, mat3.channel)
    assert_in_delta(11200000, b, 1000000)
    assert_in_delta(20800000, g, 1000000)
    assert_in_delta(26900000, r, 1000000)

    # Uncomment the following line to show the results
    # snap mat0, mat1, mat2
  end

  def test_erode
    mat0 = create_cvmat(9, 9, :cv8u, 1) { |j, i, c|
      if i >= 3 and i < 6 and j >= 3 and j < 6
        CvScalar.new(255)
      else
        CvScalar.new(0)
      end
    }

    mat1 = create_cvmat(9, 9, :cv8u, 1) { |j, i, c|
      if i >= 1 and i < 8 and j >= 1 and j < 8
        CvScalar.new(255)
      else
        CvScalar.new(0)
      end
    }

    mat2 = create_cvmat(5, 5, :cv8u, 1) { |j, i, c|
      if i == 2 or j == 2
        CvScalar.new(255)
      else
        CvScalar.new(0)
      end
    }

    mat3 = mat0.erode
    mat4 = mat0.erode(nil, 1)
    mat5 = mat1.erode(nil, 2)
    mat6 = mat1.erode(IplConvKernel.new(5, 5, 2, 2, :cross))
    mat7 = mat0.clone
    mat7.erode!
    
    assert_equal('075eb0e281328f768eb862735d16979d', hash_img(mat3))
    assert_equal('075eb0e281328f768eb862735d16979d', hash_img(mat4))
    assert_equal('9f02fc4438b1d69fea75a10dfd2b66b0', hash_img(mat5))
    assert_equal('9f02fc4438b1d69fea75a10dfd2b66b0', hash_img(mat6))
    assert_equal('075eb0e281328f768eb862735d16979d', hash_img(mat7))

    assert_raise(TypeError) {
      mat0.erode(DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      mat0.erode(nil, DUMMY_OBJ)
    }
  end

  def test_dilate
    mat0 = create_cvmat(9, 9, :cv8u, 1) { |j, i, c|
      if i == 4 and j == 4
        CvScalar.new(255)
      else
        CvScalar.new(0)
      end
    }

    mat1 = create_cvmat(5, 5, :cv8u, 1) { |j, i, c|
      if i == 2 or j == 2
        CvScalar.new(255)
      else
        CvScalar.new(0)
      end
    }

    mat2 = mat0.dilate
    mat3 = mat0.dilate(nil, 1)
    mat4 = mat0.dilate(nil, 2)
    mat5 = mat1.dilate(IplConvKernel.new(5, 5, 2, 2, :cross))
    mat6 = mat0.clone
    mat6.dilate!

    assert_equal('9f02fc4438b1d69fea75a10dfd2b66b0', hash_img(mat2))
    assert_equal('9f02fc4438b1d69fea75a10dfd2b66b0', hash_img(mat3))
    assert_equal('ebf07f2a0edd2fd0fe26ff5921c6871b', hash_img(mat4))
    assert_equal('2841937c35c311e947bee49864b9d295', hash_img(mat5))
    assert_equal('9f02fc4438b1d69fea75a10dfd2b66b0', hash_img(mat6))

    assert_raise(TypeError) {
      mat0.dilate(DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      mat0.dilate(nil, DUMMY_OBJ)
    }
  end

  def test_morphology
    mat0 = create_cvmat(64, 64, :cv8u, 1) { |j, i, c|
      if i >= 8 and i < 56 and j >= 8 and j < 56 and (i + j) % 15 != 0
        CvScalar.new(255)
      else
        CvScalar.new(0)
      end
    }

    # Open
    kernel = IplConvKernel.new(5, 5, 2, 2, :cross)
    mat1 = mat0.morphology(CV_MOP_OPEN, kernel)
    mat2 = mat0.morphology(:open, kernel)
    assert_equal('63ccb07cb93efb1563657f51e3d89252', hash_img(mat1))
    assert_equal('63ccb07cb93efb1563657f51e3d89252', hash_img(mat2))

    # Close
    mat1 = mat0.morphology(CV_MOP_CLOSE, kernel)
    mat2 = mat0.morphology(:close, kernel)
    assert_equal('831c513d6ed86bce3f15c697de4a72f8', hash_img(mat1))
    assert_equal('831c513d6ed86bce3f15c697de4a72f8', hash_img(mat2))

    # Gradient
    mat1 = mat0.morphology(CV_MOP_GRADIENT, kernel)
    mat2 = mat0.morphology(:gradient, kernel)
    assert_equal('1e8007c211d6f464cf8584e8e83b3c35', hash_img(mat1))
    assert_equal('1e8007c211d6f464cf8584e8e83b3c35', hash_img(mat2))

    # Top hat
    mat1 = mat0.morphology(CV_MOP_TOPHAT, kernel)
    mat2 = mat0.morphology(:tophat, kernel)
    assert_equal('1760c5b63a52df37069164fe3e901aa4', hash_img(mat1))
    assert_equal('1760c5b63a52df37069164fe3e901aa4', hash_img(mat2))

    # Black hat
    mat1 = mat0.morphology(CV_MOP_BLACKHAT, kernel)
    mat2 = mat0.morphology(:blackhat, kernel)
    assert_equal('18b1d51637b912a38133341ee006c6ff', hash_img(mat1))
    assert_equal('18b1d51637b912a38133341ee006c6ff', hash_img(mat2))

    [:open, :close, :gradient, :tophat, :blackhat].each { |type|
      assert_raise(TypeError) {
        mat0.morphology(type, DUMMY_OBJ)
      }
    }
  end
  
  def test_smooth
    mat0 = CvMat.load(FILENAME_LENA32x32, CV_LOAD_IMAGE_GRAYSCALE)

    assert_raise(TypeError) {
      mat0.smooth(DUMMY_OBJ)
    }

    # Blur no scale
    mat1 = mat0.smooth(CV_BLUR_NO_SCALE)
    mat2 = mat0.smooth(:blur_no_scale, 3, 3)
    mat3 = mat0.smooth(CV_BLUR_NO_SCALE, 7, 7)
    mat4 = CvMat.new(32, 32, :cv32f, 1).smooth(:blur_no_scale)

    [mat1, mat2, mat3].each { |m|
      assert_equal(1, m.channel)
      assert_equal(:cv16u, m.depth)
    }
    assert_equal(1, mat4.channel)
    assert_equal(:cv32f, mat4.depth)

    assert_raise(TypeError) {
      mat0.smooth(CV_BLUR_NO_SCALE, DUMMY_OBJ, 0, 0, 0)
    }
    assert_raise(TypeError) {
      mat0.smooth(CV_BLUR_NO_SCALE, 3, DUMMY_OBJ, 0, 0)
    }

    # Blur
    mat1 = mat0.smooth(CV_BLUR)
    mat2 = mat0.smooth(:blur, 3, 3)
    mat3 = mat0.smooth(CV_BLUR, 7, 7)
    mat4 = CvMat.new(32, 32, :cv16u, 1).smooth(:blur)
    mat5 = CvMat.new(32, 32, :cv32f, 1).smooth(CV_BLUR)
    mat6 = CvMat.new(32, 32, :cv8u, 3).smooth(:blur)

    [mat1, mat2, mat3].each { |m|
      assert_equal(1, m.channel)
      assert_equal(:cv8u, m.depth)
    }
    assert_equal(1, mat4.channel)
    assert_equal(:cv16u, mat4.depth)
    assert_equal(1, mat5.channel)
    assert_equal(:cv32f, mat5.depth)
    assert_equal(3, mat6.channel)
    assert_equal(:cv8u, mat6.depth)

    assert_raise(TypeError) {
      mat0.smooth(CV_BLUR, DUMMY_OBJ, 0, 0, 0)
    }
    assert_raise(TypeError) {
      mat0.smooth(CV_BLUR, 3, DUMMY_OBJ, 0, 0)
    }

    # Gaussian
    mat1 = mat0.smooth(CV_GAUSSIAN)
    mat2 = mat0.smooth(:gaussian, 3, 3)
    mat3 = mat0.smooth(CV_GAUSSIAN, 3, 3, 3)
    mat4 = mat0.smooth(:gaussian, 3, 3, 3, 3)
    mat5 = mat0.smooth(CV_GAUSSIAN, 7, 7, 5, 3)

    mat6 = CvMat.new(32, 32, :cv16u, 1).smooth(CV_GAUSSIAN)
    mat7 = CvMat.new(32, 32, :cv32f, 1).smooth(CV_GAUSSIAN)
    mat8 = CvMat.new(32, 32, :cv8u, 3).smooth(CV_GAUSSIAN)

    [mat1, mat2, mat3, mat4, mat5].each { |m|
      assert_equal(1, m.channel)
      assert_equal(:cv8u, m.depth)
    }
    assert_equal(1, mat6.channel)
    assert_equal(:cv16u, mat6.depth)
    assert_equal(1, mat7.channel)
    assert_equal(:cv32f, mat7.depth)
    assert_equal(3, mat8.channel)
    assert_equal(:cv8u, mat8.depth)

    assert_raise(TypeError) {
      mat0.smooth(CV_GAUSSIAN, DUMMY_OBJ, 0, 0, 0)
    }
    assert_raise(TypeError) {
      mat0.smooth(CV_GAUSSIAN, 3, DUMMY_OBJ, 0, 0)
    }
    assert_raise(TypeError) {
      mat0.smooth(CV_GAUSSIAN, 3, 0, DUMMY_OBJ, 0)
    }
    assert_raise(TypeError) {
      mat0.smooth(CV_GAUSSIAN, 3, 0, 0, DUMMY_OBJ)
    }
    
    # Median
    mat0 = create_cvmat(64, 64, :cv8u, 1) { |j, i, c|
      if (i + j) % 15 != 0
        CvScalar.new(255)
      else
        CvScalar.new(0)
      end
    }
    (-1..1).each { |dy|
      (-1..1).each { |dx|
        mat0[32 + dy, 32 + dx] = CvScalar.new(0)
      }
    }

    mat1 = mat0.smooth(CV_MEDIAN)
    mat2 = mat0.smooth(:median, 3)
    mat3 = mat0.smooth(CV_MEDIAN, 7)
    mat4 = CvMat.new(64, 64, :cv8u, 3).smooth(CV_MEDIAN)

    assert_raise(TypeError) {
      mat0.smooth(CV_MEDIAN, DUMMY_OBJ, 0, 0, 0)
    }

    # Bilateral
    mat0 = create_cvmat(64, 64, :cv8u, 1) { |j, i, c|
      if i > 32
        (i + j) % 15 != 0 ? CvScalar.new(32) : CvScalar.new(224)
      else
        (i + j) % 15 != 0 ? CvScalar.new(224) : CvScalar.new(32)
      end
    }

    mat1 = mat0.smooth(CV_BILATERAL)
    mat2 = mat0.smooth(:bilateral, 3, 3)
    mat3 = mat0.smooth(CV_BILATERAL, 7, 7)
    mat4 = CvMat.new(64, 64, :cv8u, 3).smooth(CV_BILATERAL)

    assert_raise(TypeError) {
      mat0.smooth(CV_BILATERAL, DUMMY_OBJ, 0, 0, 0)
    }
    assert_raise(TypeError) {
      mat0.smooth(CV_BILATERAL, 3, DUMMY_OBJ, 0, 0)
    }
  end

  def test_filter2d
    mat0 = CvMat.load(FILENAME_LENA256x256, CV_LOAD_IMAGE_GRAYSCALE)
    kernel = CvMat.new(3, 3, :cv32f, 1)
    
    # Laplacian filter kernel
    laplace4 = [0, 1, 0,
                1, -4, 1,
                0, 1, 0]
    laplace4.each_with_index { |x, i| kernel[i] = CvScalar.new(x) }

    mat1 = mat0.filter2d(kernel)
    mat2 = mat0.filter2d(kernel, CvPoint.new(-1, -1))
    mat3 = mat0.filter2d(kernel, CvPoint.new(0, 0))

    assert_raise(TypeError) {
      mat0.filter2d(DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      mat0.filter2d(kernel, DUMMY_OBJ)
    }
  end

  def test_copy_make_border
    mat0 = create_cvmat(32, 32, :cv8u, 1) { CvScalar.new(128) }

    [IPL_BORDER_CONSTANT, :constant].each { |type|
      mat1 = mat0.copy_make_border(type, CvSize.new(64, 48), CvPoint.new(16, 8), 255)
      assert_equal('5e231f8ca051b8f93e4aaa42d193d095', hash_img(mat1))
    }

    [IPL_BORDER_REPLICATE, :replicate].each { |type|
      mat2 = mat0.copy_make_border(type, CvSize.new(300, 300), CvPoint.new(30, 30))
      assert_equal('96940dc9e3abb6e2556ea51af1468031', hash_img(mat2))
    }
    
    assert_raise(TypeError) {
      mat0.copy_make_border(DUMMY_OBJ, CvSize.new(64, 48), CvPoint.new(16, 8))
    }
    assert_raise(TypeError) {
      mat0.copy_make_border(IPL_BORDER_CONSTANT, CvSize.new(64, 48), DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      mat0.copy_make_border(IPL_BORDER_CONSTANT, CvSize.new(64, 48), CvPoint.new(16, 8), DUMMY_OBJ)
    }
    assert_raise(ArgumentError) {
      mat0.copy_make_border(:dummy, CvSize.new(64, 48), CvPoint.new(16, 8), DUMMY_OBJ)
    }
  end

  def test_integral
    mat0 = create_cvmat(3, 3, :cv8u, 1) { |j, i, n| CvScalar.new(n) }

    result_sum = []
    result_sqsum = []
    result_tiled_sum = []

    result1 = mat0.integral
    assert_equal(CvMat, result1.class)
    result_sum << result1

    result2 = mat0.integral(true)
    assert_equal(Array, result2.class)
    assert_equal(2, result2.size)
    assert(result2.all? {|a| a.class == CvMat})
    result_sum << result2[0]
    result_sqsum << result2[1]

    result3 = mat0.integral(true, true)
    assert_equal(Array, result3.class)
    assert_equal(3, result3.size)
    assert(result3.all? {|a| a.class == CvMat})
    result_sum << result3[0]
    result_sqsum << result3[1]
    result_tiled_sum << result3[2]

    result4 = mat0.integral(true, false)
    assert_equal(Array, result4.class)
    assert_equal(2, result4.size)
    assert(result4.all? {|a| a.class == CvMat})
    result_sum << result4[0]
    result_sqsum << result4[1]

    result5 = mat0.integral(false, true)
    assert_equal(Array, result5.class)
    assert_equal(2, result5.size)
    assert(result5.all? {|a| a.class == CvMat})
    result_sum << result5[0]
    result_tiled_sum << result5[1]
    
    (result_sum + result_sqsum + result_tiled_sum).each { |s|
      assert_equal(mat0.height + 1, s.height)
      assert_equal(mat0.width + 1, s.width)
      assert_equal(:cv64f, s.depth)
      assert_equal(1, s.channel)
    }

    expected_sum = [0, 0, 0, 0,
                    0, 0, 1, 3,
                    0, 3, 8, 15,
                    0, 9, 21, 36]
    result_sum.each { |sum|
      expected_sum.each_with_index { |x, i|
        assert_in_delta(x, sum[i][0], 0.001)
      }
    }
    
    expected_sqsum = [0, 0, 0, 0,
                      0, 0, 1, 5,
                      0, 9, 26, 55,
                      0, 45, 111, 204]
    result_sqsum.each { |sqsum|
      expected_sqsum.each_with_index { |x, i|
        assert_in_delta(x, sqsum[i][0], 0.001)
      }
    }

    expected_tilted_sum = [0, 0, 0, 0,
                           0, 0, 1, 2,
                           0, 4, 7, 8,
                           4, 16, 22, 20]
    result_tiled_sum.each { |tiled_sum|
      expected_tilted_sum.each_with_index { |x, i|
        assert_in_delta(x, tiled_sum[i][0], 0.001)
      }
    }

    mat0.integral(DUMMY_OBJ, DUMMY_OBJ)
  end

  def test_threshold
    mat0 = create_cvmat(3, 3, :cv8u, 1) { |j, i, n| CvScalar.new(n) }
    test_proc = lambda { |type, type_sym, expected_mat, expected_threshold|
      mat1 = mat0.threshold(expected_threshold, 7, type)
      mat2 = mat0.threshold(expected_threshold, 7, type_sym)
      [mat1, mat2].each { |m|
        expected_mat.each_with_index { |x, i|
          assert_equal(x, m[i][0])
        }
      }
    }

    test_proc_with_otsu = lambda { |type, type_sym, expected_mat, expected_threshold|
      mat3, th3 = mat0.threshold(5, 7, type | CV_THRESH_OTSU)
      mat4, th4 = mat0.threshold(3, 7, type_sym, true)
      mat5, th5 = mat0.threshold(5, 7, type | CV_THRESH_OTSU, true)
      [mat3, mat4, mat5].each { |m|
        expected_mat.each_with_index { |x, i|
          assert_equal(x, m[i][0])
        }
      }
      [th3, th4, th5].each { |th|
        assert_in_delta(expected_threshold, th, 0.001)
      }
    }

    # Binary
    expected = [0, 0, 0,
                0, 0, 7,
                7, 7, 7]
    test_proc.call(CV_THRESH_BINARY, :binary, expected, 4)

    expected = [0, 0, 0,
                0, 7, 7,
                7, 7, 7]
    test_proc_with_otsu.call(CV_THRESH_BINARY, :binary, expected, 3)

    # Binary inverse
    expected = [7, 7, 7,
                7, 7, 0,
                0, 0, 0]
    test_proc.call(CV_THRESH_BINARY_INV, :binary_inv, expected, 4)

    expected = [7, 7, 7,
                7, 0, 0,
                0, 0, 0]
    test_proc_with_otsu.call(CV_THRESH_BINARY_INV, :binary_inv, expected, 3)

    # Trunc
    expected = [0, 1, 2,
                3, 4, 4,
                4, 4, 4]
    test_proc.call(CV_THRESH_TRUNC, :trunc, expected, 4)

    expected = [0, 1, 2,
                3, 3, 3,
                3, 3, 3]
    test_proc_with_otsu.call(CV_THRESH_TRUNC, :trunc, expected, 3)

    # To zero
    expected = [0, 0, 0,
                0, 0, 5,
                6, 7, 8]
    test_proc.call(CV_THRESH_TOZERO, :tozero, expected, 4)

    expected = [0, 0, 0,
                0, 4, 5,
                6, 7, 8]
    test_proc_with_otsu.call(CV_THRESH_TOZERO, :tozero, expected, 3)

    # To zero inverse
    expected = [0, 1, 2,
                3, 4, 0,
                0, 0, 0]
    test_proc.call(CV_THRESH_TOZERO_INV, :tozero_inv, expected, 4)

    expected = [0, 1, 2,
                3, 0, 0,
                0, 0, 0]
    test_proc_with_otsu.call(CV_THRESH_TOZERO_INV, :tozero_inv, expected, 3)

    assert_raise(TypeError) {
      mat0.threshold(DUMMY_OBJ, 2, :binary)
    }
    assert_raise(TypeError) {
      mat0.threshold(1, DUMMY_OBJ, :binary)
    }
    assert_raise(TypeError) {
      mat0.threshold(1, 2, DUMMY_OBJ)
    }
    assert_raise(ArgumentError) {
      mat0.threshold(1, 2, :dummy)
    }
    mat0.threshold(1, 2, :binary, DUMMY_OBJ)
  end

  def test_adaptive_threshold
    mat0 = create_cvmat(5, 5, :cv8u, 1) { |j, i, c| (c + 1) * 10 }
    
    mat1 = mat0.adaptive_threshold(128)
    expected1 = [0, 0, 0, 0, 0, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128]
    expected1.each_with_index { |expected, i|
      assert_equal(expected, mat1[i][0])
    }

    mat2a = mat0.adaptive_threshold(255, :adaptive_method => :mean_c,
                                    :threshold_type => :binary, :block_size => 5,
                                    :param1 => 10)
    mat2b = mat0.adaptive_threshold(255, :adaptive_method => CV_THRESH_BINARY,
                                    :threshold_type => CV_ADAPTIVE_THRESH_MEAN_C, :block_size => 5,
                                    :param1 => 10)
    expected2 = [0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 255, 255, 255, 255]
    [mat2a, mat2b].each { |mat2|
      assert_equal(CvMat, mat2.class)
      assert_equal(mat0.rows, mat2.rows)
      assert_equal(mat0.cols, mat2.cols)
      assert_equal(mat0.depth, mat2.depth)
      assert_equal(mat0.channel, mat2.channel)
      expected2.each_with_index { |expected, i|
        assert_equal(expected, mat2[i][0])
      }
    }


    mat3a = mat0.adaptive_threshold(255, :adaptive_method => :gaussian_c,
                                    :threshold_type => :binary_inv, :block_size => 5,
                                    :param1 => 10)
    mat3b = mat0.adaptive_threshold(255, :adaptive_method => CV_ADAPTIVE_THRESH_GAUSSIAN_C,
                                    :threshold_type => CV_THRESH_BINARY_INV, :block_size => 5,
                                    :param1 => 10)
    expected3 = [255, 255, 255, 255, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
    [mat3a, mat3b].each { |mat3|
      assert_equal(CvMat, mat3.class)
      assert_equal(mat0.rows, mat3.rows)
      assert_equal(mat0.cols, mat3.cols)
      assert_equal(mat0.depth, mat3.depth)
      assert_equal(mat0.channel, mat3.channel)
      expected3.each_with_index { |expected, i|
        assert_equal(expected, mat3[i][0])
      }
    }

    assert_raise(TypeError) {
      mat0.adaptive_threshold(DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      mat0.adaptive_threshold(0, DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      mat0.adaptive_threshold(0, :adaptive_method => DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      mat0.adaptive_threshold(0, :threshold_type => DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      mat0.adaptive_threshold(0, :block_size => DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      mat0.adaptive_threshold(0, :param1 => DUMMY_OBJ)
    }
  end

  def test_pyr_down
    mat0 = CvMat.load(FILENAME_LENA256x256, CV_LOAD_IMAGE_ANYCOLOR | CV_LOAD_IMAGE_ANYDEPTH)
    mat1 = mat0.pyr_down
    mat2 = mat0.pyr_down(:gaussian_5x5)

    assert_equal('de9ff2ffcf8e43f28564a201cf90b7f4', hash_img(mat1))
    assert_equal('de9ff2ffcf8e43f28564a201cf90b7f4', hash_img(mat2))

    assert_raise(TypeError) {
      mat0.pyr_down(DUMMY_OBJ)
    }
  end

  def test_pyr_up
    mat0 = CvMat.load(FILENAME_LENA256x256, CV_LOAD_IMAGE_ANYCOLOR | CV_LOAD_IMAGE_ANYDEPTH)
    mat1 = mat0.pyr_up
    mat2 = mat0.pyr_up(:gaussian_5x5)

    [mat1, mat2].each { |mat|
      assert_equal(mat0.cols * 2, mat.cols)
      assert_equal(mat0.rows * 2, mat.rows)
      assert_equal(mat0.depth, mat.depth)
      assert_equal(mat0.channel, mat.channel)
      b, g, r = color_hists(mat)
      assert_in_delta(27500000, b, 1000000)
      assert_in_delta(26000000, g, 1000000)
      assert_in_delta(47000000, r, 1000000)
    }
    # Uncomment the following lines to show the result
    # snap mat0, mat1, mat2

    assert_raise(TypeError) {
      mat0.pyr_up(DUMMY_OBJ)
    }
  end

  def test_flood_fill
    mat0 = create_cvmat(128, 256, :cv8u, 1) { |j, i, c|
      if (i >= 32 and i < 224) and (j >= 32 and j < 96)
        CvScalar.new(255)
      elsif (i >= 16 and i < 240) and (j >= 16 and j < 112)
        CvScalar.new(192)
      else
        CvScalar.new(128)
      end
    }

    point = CvPoint.new(20, 20)
    mat1, comp1, mask1 = mat0.flood_fill(point, 0)
    mat2, comp2, mask2 = mat0.flood_fill(point, 0, CvScalar.new(64))
    mat3, comp3, mask3 = mat0.flood_fill(point, 0, CvScalar.new(0), CvScalar.new(64))
    mat4, comp4, mask4 = mat0.flood_fill(point, 0, CvScalar.new(0), CvScalar.new(64),
                                         {:connectivity => 8, :fixed_range => true, :mask_only => true})
    mat05 = mat0.clone
    mat5, comp5, mask5 = mat05.flood_fill!(point, 0, CvScalar.new(0), CvScalar.new(64),
                                           {:connectivity => 8, :fixed_range => true, :mask_only => true})

    assert_equal(9216.0, comp1.area)
    assert_equal(16, comp1.rect.x)
    assert_equal(16, comp1.rect.y)
    assert_equal(224, comp1.rect.width)
    assert_equal(96, comp1.rect.height)
    assert_cvscalar_equal(CvScalar.new(0, 0, 0, 0), comp1.value)

    assert_equal(20480.0, comp2.area)
    assert_equal(0, comp2.rect.x)
    assert_equal(0, comp2.rect.y)
    assert_equal(256, comp2.rect.width)
    assert_equal(128, comp2.rect.height)
    assert_cvscalar_equal(CvScalar.new(0, 0, 0, 0), comp2.value)

    assert_equal(21504.0, comp3.area)
    assert_equal(16, comp3.rect.x)
    assert_equal(16, comp3.rect.y)
    assert_equal(224, comp3.rect.width)
    assert_equal(96, comp3.rect.height)
    assert_cvscalar_equal(CvScalar.new(0, 0, 0, 0), comp3.value)

    assert_equal(21504.0, comp4.area)
    assert_equal(16, comp4.rect.x)
    assert_equal(16, comp4.rect.y)
    assert_equal(224, comp4.rect.width)
    assert_equal(96, comp4.rect.height)
    assert_cvscalar_equal(CvScalar.new(228, 0, 0, 0), comp4.value)

    assert_equal(21504.0, comp5.area)
    assert_equal(16, comp5.rect.x)
    assert_equal(16, comp5.rect.y)
    assert_equal(224, comp5.rect.width)
    assert_equal(96, comp5.rect.height)
    assert_cvscalar_equal(CvScalar.new(228, 0, 0, 0), comp5.value)

    assert_raise(TypeError) {
      mat0.flood_fill(DUMMY_OBJ, 0)
    }
    assert_raise(TypeError) {
      mat0.flood_fill(point, DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      mat0.flood_fill(point, 0, DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      mat0.flood_fill(point, 0, CvScalar.new(0), DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      mat0.flood_fill(point, 0, CvScalar.new(0), CvScalar.new(64), DUMMY_OBJ)
    }
  end

  def test_find_contours
    mat0 = CvMat.load(FILENAME_CONTOURS, CV_LOAD_IMAGE_GRAYSCALE)

    # Make binary image
    mat0.height.times { |j|
      mat0.width.times { |i|
        mat0[j, i] = (mat0[j, i][0] < 128) ? CvColor::Black : CvColor::White
      }
    }

    [mat0.find_contours, mat0.find_contours(:mode => CV_RETR_LIST),
     mat0.find_contours(:method => CV_CHAIN_APPROX_SIMPLE),
     mat0.find_contours(:mode => CV_RETR_LIST, :method => CV_CHAIN_APPROX_SIMPLE)].each { |contours|
      assert_not_nil(contours)
      assert_equal(8, contours.total)
      assert_not_nil(contours.h_next)
      assert_equal(4, contours.h_next.total)
      assert_not_nil(contours.h_next.h_next)
      assert_equal(8, contours.h_next.h_next.total)
      assert_not_nil(contours.h_next.h_next.h_next)
      assert_equal(4, contours.h_next.h_next.h_next.total)
      assert_nil(contours.v_next)
      assert_nil(contours.h_next.v_next)
      assert_nil(contours.h_next.h_next.v_next)
      assert_nil(contours.h_next.h_next.h_next.v_next)
    }
    
    contours = mat0.find_contours(:mode => CV_RETR_TREE)
    assert_not_nil(contours)
    assert_equal(4, contours.total)
    assert_not_nil(contours.v_next)
    assert_equal(8, contours.v_next.total)
    assert_nil(contours.v_next.v_next)
    assert_not_nil(contours.h_next)
    assert_equal(4, contours.h_next.total)
    assert_not_nil(contours.h_next.v_next)
    assert_equal(8, contours.h_next.v_next.total)
    assert_nil(contours.h_next.v_next.v_next)

    contours = mat0.find_contours(:mode => CV_RETR_CCOMP)
    assert_not_nil(contours)
    assert_equal(4, contours.total)
    assert_not_nil(contours.v_next)
    assert_equal(8, contours.v_next.total)
    assert_nil(contours.v_next.v_next)
    assert_not_nil(contours.h_next)
    assert_equal(4, contours.h_next.total)
    assert_not_nil(contours.h_next.v_next)
    assert_equal(8, contours.h_next.v_next.total)
    assert_nil(contours.h_next.v_next.v_next)

    contours = mat0.find_contours(:mode => CV_RETR_EXTERNAL)
    assert_not_nil(contours)
    assert_equal(4, contours.total)
    assert_nil(contours.v_next)
    assert_not_nil(contours.h_next)
    assert_equal(4, contours.h_next.total)
    assert_nil(contours.h_next.v_next)

    contours = mat0.find_contours(:mode => CV_RETR_TREE, :method => CV_CHAIN_APPROX_NONE)
    assert_not_nil(contours)
    assert_equal(474, contours.total)
    assert_not_nil(contours.v_next)
    assert_equal(318, contours.v_next.total)
    assert_nil(contours.v_next.v_next)
    assert_not_nil(contours.h_next)
    assert_equal(396, contours.h_next.total)
    assert_not_nil(contours.h_next.v_next)
    assert_equal(240, contours.h_next.v_next.total)
    assert_nil(contours.h_next.v_next.v_next)

    contours = mat0.find_contours(:mode => CV_RETR_EXTERNAL, :method => CV_CHAIN_CODE)
    assert_equal(474, contours.total)
    assert_equal(396, contours.h_next.total)

    contours = mat0.find_contours(:mode => CV_RETR_EXTERNAL, :method => CV_CHAIN_APPROX_TC89_L1)
    assert_equal(4, contours.total)
    assert_equal(4, contours.h_next.total)

    contours = mat0.find_contours(:mode => CV_RETR_EXTERNAL, :method => CV_CHAIN_APPROX_TC89_KCOS)
    assert_equal(4, contours.total)
    assert_equal(4, contours.h_next.total)

    assert_raise(TypeError) {
      mat0.find_contours(DUMMY_OBJ)
    }
    assert_raise(CvStsUnsupportedFormat) {
      CvMat.new(10, 10, :cv32f, 3).find_contours
    }
  end

  def test_pyr_mean_shift_filtering
    mat0 = CvMat.load(FILENAME_LENA256x256, CV_LOAD_IMAGE_ANYCOLOR | CV_LOAD_IMAGE_ANYDEPTH)
    mat1 = mat0.pyr_mean_shift_filtering(30, 30)
    mat2 = mat0.pyr_mean_shift_filtering(30, 30, 2)
    mat3 = mat0.pyr_mean_shift_filtering(30, 30, nil, CvTermCriteria.new(3, 0.01))

    [mat1, mat2, mat3].each { |mat|
      b, g, r = color_hists(mat)
      assert_in_delta(6900000, b, 100000)
      assert_in_delta(6500000, g, 100000)
      assert_in_delta(11800000, r, 100000)
    }

    assert_raise(TypeError) {
      mat0.pyr_mean_shift_filtering(DUMMY_OBJ, 30)
    }
    assert_raise(TypeError) {
      mat0.pyr_mean_shift_filtering(30, DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      mat0.pyr_mean_shift_filtering(30, 30, 2, DUMMY_OBJ)
    }
  end

  def test_watershed
    mat0 = CvMat.load(FILENAME_LENA256x256, CV_LOAD_IMAGE_ANYCOLOR | CV_LOAD_IMAGE_ANYDEPTH)
    marker = CvMat.new(mat0.cols, mat0.rows, :cv32s, 1).set_zero
    marker[150, 150] = CvScalar.new(1, 1, 1, 1)
    marker[210, 210] = CvScalar.new(2, 2, 2, 2)
    marker[40, 90] = CvScalar.new(3, 3, 3, 3)

    mat1 = mat0.watershed(marker)
    assert_equal('ee6bec03296039c8df1899d3edc4684e', hash_img(mat1))

    assert_raise(TypeError) {
      mat0.watershed(DUMMY_OBJ)
    }
  end

  def test_moments
    mat = create_cvmat(128, 128, :cv8u, 1) { |j, i|
      if j >= 32 and j < 96 and i >= 16 and i < 112
        CvScalar.new(0)
      elsif j >= 16 and j < 112 and i >= 16 and i < 112
        CvScalar.new(128)
      else
        CvScalar.new(255)
      end
    }

    moments1 = mat.moments
    moments2 = mat.moments(false)
    moments3 = mat.moments(true)

    [moments1, moments2].each { |m|
      assert_in_delta(2221056, m.spatial(0, 0), 0.1)
      assert_in_delta(2221056, m.central(0, 0), 0.1)
      assert_in_delta(1, m.normalized_central(0, 0), 0.1)

      hu_moments = m.hu
      assert_equal(CvHuMoments, hu_moments.class)
      assert_in_delta(0.001771, hu_moments.hu1, 0.000001)
      hu_moments.to_a[1..7].each { |hu|
        assert_in_delta(0.0, hu, 0.000001)
      }

      center = m.gravity_center
      assert_equal(CvPoint2D32f, center.class)
      assert_in_delta(63.5, center.x, 0.001)
      assert_in_delta(63.5, center.y, 0.001)

      assert_in_delta(0, m.angle, 0.001)

      assert_in_delta(2221056, m.m00, 0.001)
      assert_in_delta(141037056, m.m10, 0.001)
      assert_in_delta(141037056, m.m01, 0.001)
      assert_in_delta(13157049856, m.m20, 0.001)
      assert_in_delta(8955853056, m.m11, 0.001)
      assert_in_delta(13492594176, m.m02, 0.001)
      assert_in_delta(1369024659456, m.m30, 0.001)
      assert_in_delta(835472665856, m.m21, 0.001)
      assert_in_delta(856779730176, m.m12, 0.001)
      assert_in_delta(1432945852416, m.m03, 0.001)
      assert_in_delta(4201196800, m.mu20, 0.001)
      assert_in_delta(0, m.mu11, 0.001)
      assert_in_delta(4536741120, m.mu02, 0.001)
      assert_in_delta(0, m.mu30, 0.001)
      assert_in_delta(0, m.mu21, 0.001)
      assert_in_delta(0, m.mu12, 0.001)
      assert_in_delta(0, m.mu03, 0.001)
      assert_in_delta(0.000671, m.inv_sqrt_m00, 0.000001)
    }

    m = moments3
    assert_in_delta(10240, m.spatial(0, 0), 0.1)
    assert_in_delta(10240, m.central(0, 0), 0.1)
    assert_in_delta(1, m.normalized_central(0, 0), 0.1)

    hu_moments = m.hu
    assert_equal(CvHuMoments, hu_moments.class)
    assert_in_delta(0.361650, hu_moments.hu1, 0.000001)
    assert_in_delta(0.000625, hu_moments.hu2, 0.000001)
    hu_moments.to_a[2..7].each { |hu|
      assert_in_delta(0.0, hu, 0.000001)
    }

    center = m.gravity_center
    assert_equal(CvPoint2D32f, center.class)
    assert_in_delta(63.5, center.x, 0.001)
    assert_in_delta(63.5, center.y, 0.001)

    assert_in_delta(0, m.angle, 0.001)

    assert_in_delta(10240, m.m00, 0.001)
    assert_in_delta(650240, m.m10, 0.001)
    assert_in_delta(650240, m.m01, 0.001)
    assert_in_delta(58940416, m.m20, 0.001)
    assert_in_delta(41290240, m.m11, 0.001)
    assert_in_delta(61561856, m.m02, 0.001)
    assert_in_delta(5984288768, m.m30, 0.001)
    assert_in_delta(3742716416, m.m21, 0.001)
    assert_in_delta(3909177856, m.m12, 0.001)
    assert_in_delta(6483673088, m.m03, 0.001)
    assert_in_delta(17650176, m.mu20, 0.001)
    assert_in_delta(0, m.mu11, 0.001)
    assert_in_delta(20271616, m.mu02, 0.001)
    assert_in_delta(0, m.mu30, 0.001)
    assert_in_delta(0, m.mu21, 0.001)
    assert_in_delta(0, m.mu12, 0.001)
    assert_in_delta(0, m.mu03, 0.001)
    assert_in_delta(0.009882, m.inv_sqrt_m00, 0.000001)
  end

  def test_hough_lines
    mat0 = CvMat.load(FILENAME_LINES, CV_LOAD_IMAGE_ANYCOLOR | CV_LOAD_IMAGE_ANYDEPTH)
    # make a binary image
    mat = CvMat.new(mat0.rows, mat0.cols, :cv8u, 1)
    (mat0.rows * mat0.cols).times { |i|
      mat[i] = (mat0[i][0] <= 100) ? CvScalar.new(0) : CvScalar.new(255);
    }

    [CV_HOUGH_STANDARD, :standard].each { |method|
      seq = mat.hough_lines(method, 1, Math::PI / 180, 65)
      assert_equal(4, seq.size)
    }

    [CV_HOUGH_PROBABILISTIC, :probabilistic].each { |method|
      seq = mat.hough_lines(method, 1, Math::PI / 180, 40, 30, 10)
      assert_equal(4, seq.size)
    }

    # [CV_HOUGH_MULTI_SCALE, :multi_scale].each { |method|
    #   seq = mat.hough_lines(method, 1, Math::PI / 180, 40, 2, 3)
    #   assert_equal(9, seq.size)
    # }

    assert_raise(TypeError) {
      mat.hough_lines(DUMMY_OBJ, 1, Math::PI / 180, 40, 2, 3)
    }
    assert_raise(TypeError) {
      mat.hough_lines(CV_HOUGH_STANDARD, DUMMY_OBJ, Math::PI / 180, 40, 2, 3)
    }
    assert_raise(TypeError) {
      mat.hough_lines(CV_HOUGH_STANDARD, 1, DUMMY_OBJ, 40, 2, 3)
    }
    assert_raise(TypeError) {
      mat.hough_lines(CV_HOUGH_STANDARD, 1, Math::PI / 180, DUMMY_OBJ, 2, 3)
    }
    assert_raise(TypeError) {
      mat.hough_lines(CV_HOUGH_STANDARD, 1, Math::PI / 180, 40, DUMMY_OBJ, 3)
    }
    assert_raise(TypeError) {
      mat.hough_lines(CV_HOUGH_STANDARD, 1, Math::PI / 180, 40, 2, DUMMY_OBJ)
    }
    assert_raise(ArgumentError) {
      mat.hough_lines(:dummy, 1, Math::PI / 180, 40, 2, DUMMY_OBJ)
    }
    ex = IS_OPENCV2 ? CvStsBadArg : CvStsAssert
    assert_raise(ex) {
      CvMat.new(10, 10, :cv32f, 3).hough_lines(:standard, 1, Math::PI / 180, 65)
    }
  end

  def test_hough_circles
    mat0 = CvMat.load(FILENAME_LINES, CV_LOAD_IMAGE_ANYCOLOR | CV_LOAD_IMAGE_ANYDEPTH)
    # make a binary image
    mat = CvMat.new(mat0.rows, mat0.cols, :cv8u, 1)
    (mat0.rows * mat0.cols).times { |i|
      mat[i] = (mat0[i][0] <= 100) ? CvScalar.new(0) : CvScalar.new(255);
    }

    [mat.hough_circles(CV_HOUGH_GRADIENT, 1.5, 40, 100, 40, 10, 50),
     mat.hough_circles(:gradient, 1.5, 40, 100, 40, 10, 50),
     mat.hough_circles(CV_HOUGH_GRADIENT, 1.5, 40, 100, 40),
     mat.hough_circles(:gradient, 1.5, 40, 100, 40)].each { |seq|
      assert_equal(2, seq.size)
    }

    # Uncomment the following lines to show the result
    # seq = mat.hough_circles(:gradient, 1.5, 40, 100, 40, 10, 50)
    # seq.each { |circle|
    #   mat0.circle!(circle.center, circle.radius, :color => CvColor::Red, :thickness => 2)
    # }
    # snap mat0

    assert_raise(TypeError) {
      mat.hough_circles(DUMMY_OBJ, 1.5, 40, 100, 50, 10, 50)
    }
    assert_raise(TypeError) {
      mat.hough_circles(CV_HOUGH_GRADIENT, DUMMY_OBJ, 40, 100, 50, 10, 50)
    }
    assert_raise(TypeError) {
      mat.hough_circles(CV_HOUGH_GRADIENT, 1.5, DUMMY_OBJ, 100, 50, 10, 50)
    }
    assert_raise(TypeError) {
      mat.hough_circles(CV_HOUGH_GRADIENT, 1.5, 40, DUMMY_OBJ, 50, 10, 50)
    }
    assert_raise(TypeError) {
      mat.hough_circles(CV_HOUGH_GRADIENT, 1.5, 40, 100, DUMMY_OBJ, 10, 50)
    }
    assert_raise(TypeError) {
      mat.hough_circles(CV_HOUGH_GRADIENT, 1.5, 40, 100, 50, DUMMY_OBJ, 50)
    }
    assert_raise(TypeError) {
      mat.hough_circles(CV_HOUGH_GRADIENT, 1.5, 40, 100, 50, 10, DUMMY_OBJ)
    }
    assert_raise(ArgumentError) {
      mat.hough_circles(:dummy, 1.5, 40, 100, 50, 10, DUMMY_OBJ)
    }
    assert_raise(CvStsBadArg) {
      CvMat.new(10, 10, :cv32f, 3).hough_circles(:gradient, 1.5, 40, 100, 50, 10, 50)
    }
  end

  def test_inpaint
    mat = CvMat.load(FILENAME_LENA_INPAINT, CV_LOAD_IMAGE_ANYCOLOR | CV_LOAD_IMAGE_ANYDEPTH)
    mask = CvMat.load(FILENAME_INPAINT_MASK, CV_LOAD_IMAGE_GRAYSCALE)

    [CV_INPAINT_NS, :ns].each { |method|
      result_ns = mat.inpaint(method, mask, 10)
      assert_in_delta(14000, count_threshold(result_ns, 128), 1000)
    }
    [CV_INPAINT_TELEA, :telea].each { |method|
      result_telea = mat.inpaint(method, mask, 10)
      assert_in_delta(13500, count_threshold(result_telea, 128), 1000)
    }

    # Uncomment the following lines to show the results
    # result_ns = mat.inpaint(:ns, mask, 10)
    # result_telea = mat.inpaint(:telea, mask, 10)
    # snap mat, result_ns, result_telea

    assert_raise(TypeError) {
      mat.inpaint(DUMMY_OBJ, mask, 10)
    }
    assert_raise(TypeError) {
      mat.inpaint(:ns, DUMMY_OBJ, 10)
    }
    assert_raise(TypeError) {
      mat.inpaint(:ns, mask, DUMMY_OBJ)
    }
    assert_raise(ArgumentError) {
      mat.inpaint(:dummy, mask, 10)
    }
    assert_raise(CvStsUnsupportedFormat) {
      CvMat.new(10, 10, :cv32f, 3).inpaint(:ns, CvMat.new(10, 10, :cv8u, 1), 10)
    }
  end

  def test_equalize_hist
    mat = CvMat.load(FILENAME_LENA256x256, CV_LOAD_IMAGE_GRAYSCALE)
    result = mat.equalize_hist
    assert_equal(CvMat, result.class)
    assert_equal(mat.rows, result.rows)
    assert_equal(mat.cols, result.cols)

    assert_raise(CvStsAssert) {
      CvMat.new(10, 10, :cv32f, 3).equalize_hist
    }

    # Uncomment the following lines to show the result
    # snap mat, result
  end

  def test_match_template
    mat = CvMat.load(FILENAME_LENA256x256, CV_LOAD_IMAGE_ANYCOLOR | CV_LOAD_IMAGE_ANYDEPTH)
    templ = CvMat.load(FILENAME_LENA_EYES, CV_LOAD_IMAGE_ANYCOLOR | CV_LOAD_IMAGE_ANYDEPTH)

    expected_pt = CvPoint.new(100, 120)

    # sqdiff
    result = mat.match_template(templ)
    pt = result.min_max_loc[2]
    assert_in_delta(expected_pt.x, pt.x, 20)
    assert_in_delta(expected_pt.y, pt.y, 20)

    [CV_TM_SQDIFF, :sqdiff].each { |method|
      result = mat.match_template(templ, method)
      assert_in_delta(expected_pt.x, pt.x, 20)
      assert_in_delta(expected_pt.y, pt.y, 20)
    }

    # sqdiff_normed
    [CV_TM_SQDIFF_NORMED, :sqdiff_normed].each { |method|
      result = mat.match_template(templ, method)
      pt =  result.min_max_loc[2]
      assert_in_delta(expected_pt.x, pt.x, 20)
      assert_in_delta(expected_pt.y, pt.y, 20)
    }

    # ccorr
    [CV_TM_CCORR, :ccorr].each { |method|
      result = mat.match_template(templ, method)
      pt =  result.min_max_loc[3]
      assert_in_delta(110, pt.x, 20)
      assert_in_delta(60, pt.y, 20)
    }

    # ccorr_normed
    [CV_TM_CCORR_NORMED, :ccorr_normed].each { |method|
      result = mat.match_template(templ, method)
      pt =  result.min_max_loc[3]
      assert_in_delta(expected_pt.x, pt.x, 20)
      assert_in_delta(expected_pt.y, pt.y, 20)
    }

    # ccoeff
    [CV_TM_CCOEFF, :ccoeff].each { |method|
      result = mat.match_template(templ, method)
      pt =  result.min_max_loc[3]
      assert_in_delta(expected_pt.x, pt.x, 20)
      assert_in_delta(expected_pt.y, pt.y, 20)
    }
    
    # ccoeff_normed
    [CV_TM_CCOEFF_NORMED, :ccoeff_normed].each { |method|
      result = mat.match_template(templ, method)
      pt =  result.min_max_loc[3]
      assert_in_delta(expected_pt.x, pt.x, 20)
      assert_in_delta(expected_pt.y, pt.y, 20)
    }

    # Uncomment the following lines to show the result
    # result = mat.match_template(templ)
    # pt1 = result.min_max_loc[2] # minimum location
    # pt2 = CvPoint.new(pt1.x + templ.width, pt1.y + templ.height)
    # mat.rectangle!(pt1, pt2, :color => CvColor::Black, :thickness => 3)
    # snap mat, templ, result

    assert_raise(TypeError) {
      mat.match_template(DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      mat.match_template(templ, DUMMY_OBJ)
    }
  end

  def test_match_shapes
    mat_cv = CvMat.load(FILENAME_STR_CV, CV_LOAD_IMAGE_GRAYSCALE)
    mat_ov = CvMat.load(FILENAME_STR_OV, CV_LOAD_IMAGE_GRAYSCALE)
    mat_cv_rotated = CvMat.load(FILENAME_STR_CV_ROTATED, CV_LOAD_IMAGE_GRAYSCALE)

    [CV_CONTOURS_MATCH_I1, :i1].each { |method|
      assert_in_delta(0, mat_cv.match_shapes(mat_cv_rotated, method), 0.00001)
      assert_in_delta(0.0010649, mat_cv.match_shapes(mat_ov, method), 0.00001)
    }

    [CV_CONTOURS_MATCH_I2, :i2].each { |method|
      assert_in_delta(0, mat_cv.match_shapes(mat_cv_rotated, method), 0.00001)
      assert_in_delta(0.0104650, mat_cv.match_shapes(mat_ov, method), 0.00001)
    }

    [CV_CONTOURS_MATCH_I3, :i3].each { |method|
      assert_in_delta(0, mat_cv.match_shapes(mat_cv_rotated, method), 0.00001)
      assert_in_delta(0.0033327, mat_cv.match_shapes(mat_ov, method), 0.00001)
    }
  end

  def test_snake_image
    omit_unless(IS_OPENCV2, 'CvMat#snake_image is not supported in OpenCV 3 or later')
    radius = 40
    center = CvPoint.new(128, 128)
    mat = CvMat.new(center.y * 2, center.x * 2, :cv8u, 1).zero!
    mat.circle!(center, radius, :color => CvColor::White, :thickness => -1)

    num_points = 10
    alpha = 0.05
    beta = 0.05
    gamma = 0.9

    arr_alpha = [alpha] * num_points
    arr_beta = [beta] * num_points
    arr_gamma = [gamma] * num_points
    size = CvSize.new(3, 3)
    term_criteria = CvTermCriteria.new(100, num_points / 2)

    # initialize contours
    points = []
    num_points.times { |i|
      x = center.x * Math.cos(2 * Math::PI * i / num_points) + center.x
      y = center.y * Math.sin(2 * Math::PI * i / num_points) + center.y
      points << CvPoint.new(x, y)
    }

    acceptable_error = 50

    # test snake_image
    # calc_gradient = true
    [mat.snake_image(points, alpha, beta, gamma, size, term_criteria),
     mat.snake_image(points, alpha, beta, gamma, size, term_criteria, true),
     mat.snake_image(points, arr_alpha, arr_beta, arr_gamma, size, term_criteria),
     mat.snake_image(points, arr_alpha, arr_beta, arr_gamma, size, term_criteria, true)].each { |result|
      assert_equal(num_points, result.size)
      result.each { |pt|
        x = pt.x - center.x
        y = pt.y - center.y
        error = Math.sqrt((x * x + y * y - radius * radius).abs)
        assert(error < acceptable_error)
      }
    }

    # calc_gradient = false
    [mat.snake_image(points, alpha, beta, gamma, size, term_criteria, false),
     mat.snake_image(points, arr_alpha, arr_beta, arr_gamma, size, term_criteria, false)].each { |result|
      expected_points = [[149, 102], [139, 144], [95, 144], [56, 124], [17, 105],
                         [25, 61], [63, 39], [101, 17], [145, 17], [158, 59]]
      assert_equal(num_points, result.size)
      result.each { |pt|
        x = pt.x - center.x
        y = pt.y - center.y
        error = Math.sqrt((x * x + y * y - radius * radius).abs)
        assert(error < acceptable_error)
      }
    }

    # raise error
    assert_raise(TypeError) {
      mat.snake_image(DUMMY_OBJ, arr_alpha, arr_beta, arr_gamma, size, term_criteria)
    }
    assert_raise(TypeError) {
      mat.snake_image(points, DUMMY_OBJ, arr_beta, arr_gamma, size, term_criteria)
    }
    assert_raise(TypeError) {
      mat.snake_image(points, arr_alpha, DUMMY_OBJ, arr_gamma, size, term_criteria)
    }
    assert_raise(TypeError) {
      mat.snake_image(points, arr_alpha, arr_beta, DUMMY_OBJ, size, term_criteria)
    }
    assert_raise(TypeError) {
      mat.snake_image(points, arr_alpha, arr_beta, arr_gamma, DUMMY_OBJ, term_criteria)
    }
    assert_raise(TypeError) {
      mat.snake_image(points, arr_alpha, arr_beta, arr_gamma, size, DUMMY_OBJ)
    }
    mat.snake_image(points, arr_alpha, arr_beta, arr_gamma, size, term_criteria, DUMMY_OBJ)

    assert_raise(ArgumentError) {
      mat.snake_image(points, arr_alpha[0 .. num_points / 2], arr_beta, arr_gamma, size, term_criteria)
    }
    assert_raise(CvBadNumChannels) {
      CvMat.new(10, 10, :cv8u, 3).snake_image(points, alpha, beta, gamma, size, term_criteria)
    }

    # Uncomment the following lines to show the result
    # result = mat.clone.GRAY2BGR
    # pts = mat.snake_image(points, alpha, beta, gamma, size, term_criteria)
    # w = GUI::Window.new('HoughCircle')
    # result.poly_line!([pts], :color => CvColor::Red, :is_closed => true, :thickness => 2)
    # result.poly_line!([points], :color => CvColor::Yellow, :is_closed => true, :thickness => 2)
    # w.show result
    # GUI::wait_key
  end
end
