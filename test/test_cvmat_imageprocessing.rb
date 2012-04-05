#!/usr/bin/env ruby
# -*- mode: ruby; coding: utf-8-unix -*-
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

    assert_equal('1ec909dfa2e497c7f454e39aefd764f5', hash_img(mat1))
    assert_equal('1ec909dfa2e497c7f454e39aefd764f5', hash_img(mat2))
    assert_equal('42e7443ffd389d15343d3c6bdc42f553', hash_img(mat3))

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

    flunk('FIXME: CvMat#corner_eigenvv is not tested yet.')
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

    flunk('FIXME: CvMat#corner_min_eigen_val is not tested yet.')
  end

  def test_corner_harris
    mat0 = CvMat.load(FILENAME_LENA256x256, CV_LOAD_IMAGE_GRAYSCALE)
    mat1 = mat0.corner_harris(3)
    mat2 = mat0.corner_harris(3, 3)
    mat3 = mat0.corner_harris(3, 3, 0.04)
    mat4 = mat0.corner_harris(3, 7, 0.01)

    assert_equal('fbb4e04c86f906c83fe17fd148675f90', hash_img(mat1))
    assert_equal('fbb4e04c86f906c83fe17fd148675f90', hash_img(mat2))
    assert_equal('fbb4e04c86f906c83fe17fd148675f90', hash_img(mat3))
    assert_equal('6515d75f6223806f077cebc7b3927a13', hash_img(mat4))

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

  def test_find_corner_sub_pix
    flunk('FIXME: CvMat#find_corner_sub_pix is not implemented yet.')
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

  def test_sample_line
    flunk('FIXME: CvMat#sample_line is not implemented yet.')
  end

  def test_rect_sub_pix
    mat0 = CvMat.load(FILENAME_LENA256x256, CV_LOAD_IMAGE_ANYCOLOR | CV_LOAD_IMAGE_ANYDEPTH)
    center = CvPoint2D32f.new(mat0.width / 2, mat0.height / 2)
    mat1 = mat0.rect_sub_pix(center)
    mat2 = mat0.rect_sub_pix(center, mat0.size)
    mat3 = mat0.rect_sub_pix(center, CvSize.new(512, 512))

    assert_equal('b3dc0e31260dd42b5341471e23e825d3', hash_img(mat1))
    assert_equal('b3dc0e31260dd42b5341471e23e825d3', hash_img(mat2))
    assert_equal('cc27ce8f4068efedcd31c4c782c3825c', hash_img(mat3))

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

    assert_equal('f170c05fa50c3ac2a762d7b3f5c4ae2f', hash_img(mat1))
    assert_equal('f170c05fa50c3ac2a762d7b3f5c4ae2f', hash_img(mat2))
    assert_equal('4d949d5083405381ad9ea09dcd95e5a2', hash_img(mat3))

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
    size_512 = CvSize.new(512, 512)
    size_128 = CvSize.new(128, 128)
    mat1 = mat0.resize(size_512)
    mat2 = mat0.resize(size_512, :linear)
    mat3 = mat0.resize(size_512, :nn)
    mat4 = mat0.resize(size_128, :area)
    mat5 = mat0.resize(size_128, :cubic)
    mat6 = mat0.clone

    assert_equal('b2203ccca2c17b042a90b79704c0f535', hash_img(mat1))
    assert_equal('b2203ccca2c17b042a90b79704c0f535', hash_img(mat2))
    assert_equal('ba8f2dee2329aaa6309de4770fc8fa55', hash_img(mat3))
    assert_equal('10cf18adaa8548101cc230206624133a', hash_img(mat4))
    assert_equal('de5c30fcd9e817aa282ab05388de995b', hash_img(mat5))

    assert_raise(TypeError) {
      mat0.resize(DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      mat0.resize(size_128, DUMMY_OBJ)
    }
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
    mat2 = mat0.warp_affine(map_matrix, :nn)
    mat3 = mat0.warp_affine(map_matrix, :linear, :fill_outliers, CvColor::Yellow)
    mat4 = mat0.warp_affine(map_matrix, :linear, :inverse_map)
    
    assert_equal('da3d7cdefabbaf84c4080ecd40d00897', hash_img(mat1))
    assert_equal('b4abcd12c4e1103c3de87bf9ad854936', hash_img(mat2))
    assert_equal('26f6b10e955125c91fd7e63a63cc06a3', hash_img(mat3))
    assert_equal('cc4eb5d8eb7cb2c0b76941bc38fb91b1', hash_img(mat4))

    assert_raise(TypeError) {
      mat0.warp_affine(DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      mat0.warp_affine(map_matrix, DUMMY_OBJ)
    }
    # assert_raise(CvError) {
    #   mat0.warp_affine(CvMat.new(3, 3))
    # }
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
    mat2 = mat0.warp_perspective(map_matrix, :nn)
    mat3 = mat0.warp_perspective(map_matrix, :linear, :inverse_map)
    mat4 = mat0.warp_perspective(map_matrix, :linear, :fill_outliers, CvColor::Yellow)

    assert_equal('bba3a5395f9dd9a400a0083ae74d8986', hash_img(mat1))
    assert_equal('a0cc4f329f459410293b75b417fc4f25', hash_img(mat2))
    assert_equal('3e34e6ed2404056bb72e86edf02610cb', hash_img(mat3))
    assert_equal('71bd12857d2e4ac0c919652c2963b4e1', hash_img(mat4))

    assert_raise(TypeError) {
      mat0.warp_perspective(DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      mat0.warp_perspective(map_matrix, DUMMY_OBJ)
    }
    # assert_raise(CvError) {
    #   mat0.warp_perspective(CvMat.new(2, 3))
    # }
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
    mat2 = mat0.remap(matx, maty, :nn)
    mat3 = mat0.remap(matx, maty, :linear, :fill_outliers, CvColor::Yellow)

    assert_equal('586716c0262a3e03a54b9fc6e671e5f7', hash_img(mat1))
    assert_equal('5461ecdee23d5e8a9099500d631c9f0f', hash_img(mat2))
    assert_equal('1f6b73925056298c566e8e727627d929', hash_img(mat3))

    assert_raise(TypeError) {
      mat0.remap(DUMMY_OBJ, maty)
    }
    assert_raise(TypeError) {
      mat0.remap(matx, DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      mat0.remap(matx, maty, DUMMY_OBJ)
    }
    # assert_raise(CvError) {
    #   mat0.remap(CvMat.new(3, 3, :cv8u), maty)
    # }
  end

  def test_log_polar
    mat0 = CvMat.load(FILENAME_FRUITS, CV_LOAD_IMAGE_ANYCOLOR | CV_LOAD_IMAGE_ANYDEPTH)

    mat1 = mat0.log_polar(CvSize.new(255, 255), CvPoint2D32f.new(mat0.width / 2, mat0.height / 2), 40)
    assert_equal('d0425614b2f6e63ab2b6ef6637b4efcb', hash_img(mat1))
    mat1 = mat0.log_polar(CvSize.new(255, 255), CvPoint2D32f.new(mat0.width / 2, mat0.height / 2), 40,
                          CV_INTER_LINEAR | CV_WARP_FILL_OUTLIERS)
    assert_equal('d0425614b2f6e63ab2b6ef6637b4efcb', hash_img(mat1))
    
    mat2 = mat1.log_polar(mat0.size, CvPoint2D32f.new(mat0.width / 2, mat0.height / 2), 40,
                          CV_INTER_LINEAR | CV_WARP_FILL_OUTLIERS | CV_WARP_INVERSE_MAP)
    assert_equal('52587e593fec1b0383731be53147e8cd', hash_img(mat2))

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

    assert_equal('3c9074c87b65117798f48e41a17b2f30', hash_img(mat1))
    assert_equal('3c9074c87b65117798f48e41a17b2f30', hash_img(mat2))
    assert_equal('9c549aa406a425a65b036c2f9a2689e0', hash_img(mat3))

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

    assert_equal('f2473b5b964ae8950f6a7fa5cde4c67a', hash_img(mat1))
    assert_equal('f2473b5b964ae8950f6a7fa5cde4c67a', hash_img(mat2))
    assert_equal('d7bb344fc0f6ec0da4b9754d319e4e4a', hash_img(mat3))

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

    assert_equal('580c88f3e0e317a5770be3f28f31eda2', hash_img(mat1))
    assert_equal('580c88f3e0e317a5770be3f28f31eda2', hash_img(mat2))
    assert_equal('a1ffaa14522719e37d75eec18ff8b309', hash_img(mat3))
    assert_equal('a1ffaa14522719e37d75eec18ff8b309', hash_img(mat4))
    assert_equal('f7f8b4eff3240ffc8f259ce975936d92', hash_img(mat5))

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

    assert_equal('7343a41c542e034db356636c06134961', hash_img(mat1))
    assert_equal('7343a41c542e034db356636c06134961', hash_img(mat2))
    assert_equal('6ae59e64850377ee5470c854761551ea', hash_img(mat3))

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

    flunk('FIXME: Cases of CvMat#smooth(CV_BILATERAL) are not tested yet.')
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

    assert_equal('14a01cc47078e8f8fe4f0fd510d5521b', hash_img(mat1))
    assert_equal('14a01cc47078e8f8fe4f0fd510d5521b', hash_img(mat2))
    assert_equal('30e04de43f9240df6aadbaea6467b8fe', hash_img(mat3))

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
      mat3, th3 = mat0.threshold(5, 7, type | CV_THRESH_OTSU)
      mat4, th4 = mat0.threshold(3, 7, type_sym, true)
      mat5, th5 = mat0.threshold(5, 7, type | CV_THRESH_OTSU, true)
      [mat1, mat2, mat3, mat4, mat5].each { |m|
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

    # Binary inverse
    expected = [7, 7, 7,
                7, 7, 0,
                0, 0, 0]
    test_proc.call(CV_THRESH_BINARY_INV, :binary_inv, expected, 4)

    # Trunc
    expected = [0, 1, 2,
                3, 4, 4,
                4, 4, 4]
    test_proc.call(CV_THRESH_TRUNC, :trunc, expected, 4)

    # To zero
    expected = [0, 0, 0,
                0, 0, 5,
                6, 7, 8]
    test_proc.call(CV_THRESH_TOZERO, :tozero, expected, 4)

    # To zero inverse
    expected = [0, 1, 2,
                3, 4, 0,
                0, 0, 0]
    test_proc.call(CV_THRESH_TOZERO_INV, :tozero_inv, expected, 4)

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

    assert_equal('02430c6cf143d3d104e25bc829f1fa93', hash_img(mat1))
    assert_equal('02430c6cf143d3d104e25bc829f1fa93', hash_img(mat2))

    assert_raise(TypeError) {
      mat0.pyr_up(DUMMY_OBJ)
    }
  end

  def test_flood_fill
    mat0 = create_cvmat(128, 128, :cv8u, 1) { |j, i, c|
      if (i >= 32 and i < 96) and (j >= 32 and j < 96)
        CvScalar.new(255)
      elsif (i >= 16 and i < 112) and (j >= 16 and j < 112)
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

    assert_equal('8c6a235fdf4c9c4f6822a45daac5b1af', hash_img(mat1))
    assert_equal(5120.0, comp1.area)
    assert_equal(16, comp1.rect.x)
    assert_equal(16, comp1.rect.y)
    assert_equal(96, comp1.rect.width)
    assert_equal(96, comp1.rect.height)
    assert_cvscalar_equal(CvScalar.new(0, 0, 0, 0), comp1.value)
    assert_equal('1fd2537966283987b39c8b2c9d778383', hash_img(mask1))

    assert_equal('7456e5de74bb8b4e783d04bbf1904644', hash_img(mat2))
    assert_equal(12288.0, comp2.area)
    assert_equal(0, comp2.rect.x)
    assert_equal(0, comp2.rect.y)
    assert_equal(128, comp2.rect.width)
    assert_equal(128, comp2.rect.height)
    assert_cvscalar_equal(CvScalar.new(0, 0, 0, 0), comp2.value)
    assert_equal('847934f5170e2072cdfd63e16a1e06ad', hash_img(mask2))

    assert_equal('df720005423762ca1b68e06571f58b21', hash_img(mat3))
    assert_equal(9216.0, comp3.area)
    assert_equal(16, comp3.rect.x)
    assert_equal(16, comp3.rect.y)
    assert_equal(96, comp3.rect.width)
    assert_equal(96, comp3.rect.height)
    assert_cvscalar_equal(CvScalar.new(0, 0, 0, 0), comp3.value)

    assert_equal('7833f4c85c77056db71e33ae8072a1b5', hash_img(mat4))
    assert_equal(9216.0, comp4.area)
    assert_equal(16, comp4.rect.x)
    assert_equal(16, comp4.rect.y)
    assert_equal(96, comp4.rect.width)
    assert_equal(96, comp4.rect.height)
    assert_cvscalar_equal(CvScalar.new(220, 0, 0, 0), comp4.value)
    assert_equal('b34b0269872fe3acde0e0c73e5cdd23b', hash_img(mask4))

    assert_equal('7833f4c85c77056db71e33ae8072a1b5', hash_img(mat5))
    assert_equal('7833f4c85c77056db71e33ae8072a1b5', hash_img(mat05))
    assert_equal(9216.0, comp5.area)
    assert_equal(16, comp5.rect.x)
    assert_equal(16, comp5.rect.y)
    assert_equal(96, comp5.rect.width)
    assert_equal(96, comp5.rect.height)
    assert_cvscalar_equal(CvScalar.new(220, 0, 0, 0), comp5.value)
    assert_equal('b34b0269872fe3acde0e0c73e5cdd23b', hash_img(mask5))

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

  def test_pyr_segmentation
    mat0 = CvMat.load(FILENAME_LENA256x256, CV_LOAD_IMAGE_ANYCOLOR | CV_LOAD_IMAGE_ANYDEPTH)
    mat1, seq1 = mat0.pyr_segmentation(4, 255, 50)
    assert_equal('ebd9bad0bbc90b1d4a25289b7d59c958', hash_img(mat1))
    assert_equal(5, seq1.total)

    img0 = IplImage.load(FILENAME_CAT, CV_LOAD_IMAGE_ANYCOLOR | CV_LOAD_IMAGE_ANYDEPTH)
    img0.set_roi(CvRect.new(0, 0, 256, 512))
    img2, seq2 = img0.pyr_segmentation(2, 255, 50)
    assert_equal('963b26f51b14f175fbbf128e9b9e979f', hash_img(img2))
    assert_equal(11, seq2.total)

    assert_raise(CvStsAssert) {
      img0.pyr_segmentation(-1, 255, 50)
    }
    assert_raise(CvStsAssert) {
      img0.pyr_segmentation(1000, 255, 50)
    }
    assert_raise(CvStsAssert) {
      img0.pyr_segmentation(4, -1, 50)
    }
    assert_raise(CvStsAssert) {
      img0.pyr_segmentation(4, 255, -1)
    }
    assert_raise(TypeError) {
      img0.pyr_segmentation(DUMMY_OBJ, 255, 50)
    }
    assert_raise(TypeError) {
      img0.pyr_segmentation(4, DUMMY_OBJ, 50)
    }
    assert_raise(TypeError) {
      img0.pyr_segmentation(4, 255, DUMMY_OBJ)
    }
    assert_raise(CvBadDepth) {
      IplImage.new(10, 10, :cv32f, 2).pyr_segmentation(4, 255, 50)
    }
  end

  def test_pyr_mean_shift_filtering
    mat0 = CvMat.load(FILENAME_LENA256x256, CV_LOAD_IMAGE_ANYCOLOR | CV_LOAD_IMAGE_ANYDEPTH)
    mat1 = mat0.pyr_mean_shift_filtering(30, 30)
    mat2 = mat0.pyr_mean_shift_filtering(30, 30, 2)
    mat3 = mat0.pyr_mean_shift_filtering(30, 30, nil, CvTermCriteria.new(3, 0.01))

    assert_equal('6887e96bc5dfd552f76ac5411b394775', hash_img(mat1))
    assert_equal('3cd9c4983fcabeafa04be200d5e08841', hash_img(mat2))
    assert_equal('e37f0157f93fe2a98312ae6b768e8295', hash_img(mat3))

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

    [moments1, moments2].each { |moments|
      assert_equal(1, moments.size)
      m = moments[0]
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

    assert_equal(1, moments3.size)
    m = moments3[0]

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
    assert_raise(CvStsBadArg) {
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

    [mat.hough_circles(CV_HOUGH_GRADIENT, 1.5, 40, 100, 50, 10, 50),
     mat.hough_circles(:gradient, 1.5, 40, 100, 50, 10, 50),
     mat.hough_circles(CV_HOUGH_GRADIENT, 1.5, 40, 100, 50),
     mat.hough_circles(:gradient, 1.5, 40, 100, 50)].each { |seq|
      assert_equal(2, seq.size)
    }

    # Uncomment the following lines to show the result
    # seq = mat.hough_circles(:gradient, 1.5, 40, 100, 50, 10, 50)
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
      assert_equal('d3df4dda8642c83512fb417ffa5e1457', hash_img(result_ns))
    }
    [CV_INPAINT_TELEA, :telea].each { |method|
      result_telea = mat.inpaint(method, mask, 10)
      assert_equal('d45bec22d03067578703f2ec68567167', hash_img(result_telea))
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
    assert_equal('de235065c746193d7f3de9359f63a7af', hash_img(result))

    assert_raise(CvStsAssert) {
      CvMat.new(10, 10, :cv32f, 3).equalize_hist
    }

    # Uncomment the following lines to show the result
    # snap mat, result
  end

  def test_match_template
    mat = CvMat.load(FILENAME_LENA256x256, CV_LOAD_IMAGE_ANYCOLOR | CV_LOAD_IMAGE_ANYDEPTH)
    templ = CvMat.load(FILENAME_LENA_EYES, CV_LOAD_IMAGE_ANYCOLOR | CV_LOAD_IMAGE_ANYDEPTH)
    
    # sqdiff
    result = mat.match_template(templ)
    assert_equal('88663ec44be797ca883fc87bb6d7c09b', hash_img(result))
    [CV_TM_SQDIFF, :sqdiff].each { |method|
      result = mat.match_template(templ, method)
      assert_equal('88663ec44be797ca883fc87bb6d7c09b', hash_img(result))
    }

    # sqdiff_normed
    [CV_TM_SQDIFF_NORMED, :sqdiff_normed].each { |method|
      result = mat.match_template(templ, method)
      assert_equal('75c812f87184b2ccd8f83b70a8436356', hash_img(result))
    }

    # ccorr
    [CV_TM_CCORR, :ccorr].each { |method|
      result = mat.match_template(templ, method)
      assert_equal('6ebe7e48edf8fc64bcc0fd7f1e96555c', hash_img(result))
    }

    # ccorr_normed
    [CV_TM_CCORR_NORMED, :ccorr_normed].each { |method|
      result = mat.match_template(templ, method)
      assert_equal('4cf8ebcec870f8295d615a9aa345ae4d', hash_img(result))
    }

    # ccoeff
    [CV_TM_CCOEFF, :ccoeff].each { |method|
      result = mat.match_template(templ, method)
      assert_equal('248a391c5a1e1dbcf7a19f3310b5cd7a', hash_img(result))
    }
    
    # ccoeff_normed
    [CV_TM_CCOEFF_NORMED, :ccoeff_normed].each { |method|
      result = mat.match_template(templ, method)
      assert_equal('27a4e9b45ed648848f0498356bd2c5b5', hash_img(result))
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
  
  def test_mean_shift
    flunk('FIXME: CvMat#mean_shift is not tested yet.')
  end

  def test_cam_shift
    flunk('FIXME: CvMat#cam_shift is not tested yet.')
  end

  def test_snake_image
    mat = CvMat.load(FILENAME_LINES, CV_LOAD_IMAGE_GRAYSCALE)
    num_points = 10
    alpha = 0.45
    beta = 0.35
    gamma = 0.2
    arr_alpha = [alpha] * num_points
    arr_beta = [beta] * num_points
    arr_gamma = [gamma] * num_points

    size = CvSize.new(15, 15)
    term_criteria = CvTermCriteria.new(100, 0.0)
    
    # initialize contours
    points = []
    center = CvPoint.new(mat.cols / 2, mat.rows / 2)
    num_points.times { |i|
      x = center.x * Math.cos(2 * Math::PI * i / num_points) + center.x
      y = center.y * Math.sin(2 * Math::PI * i / num_points) + center.y
      points << CvPoint.new(x, y)
    }

    # test snake_image
    # calc_gradient = true
    [mat.snake_image(points, alpha, beta, gamma, size, term_criteria),
     mat.snake_image(points, alpha, beta, gamma, size, term_criteria, true),
     mat.snake_image(points, arr_alpha, arr_beta, arr_gamma, size, term_criteria),
     mat.snake_image(points, arr_alpha, arr_beta, arr_gamma, size, term_criteria, true)].each { |result|
      expected_points = [[147, 101], [138, 144], [95, 142], [56, 123], [17, 104],
                         [25, 62], [62, 40], [99, 18], [142, 18], [157, 59]]
      assert_equal(num_points, result.size)
      result.zip(expected_points) { |pair|
        actual = pair[0]
        expected = pair[1]
        assert_equal(expected[0], actual.x)
        assert_equal(expected[1], actual.y)
      }
    }

    # calc_gradient = false
    [mat.snake_image(points, alpha, beta, gamma, size, term_criteria, false),
     mat.snake_image(points, arr_alpha, arr_beta, arr_gamma, size, term_criteria, false)].each { |result|
      expected_points = [[149, 102], [139, 144], [95, 144], [56, 124], [17, 105],
                         [25, 61], [63, 39], [101, 17], [145, 17], [158, 59]]
      assert_equal(num_points, result.size)
      result.zip(expected_points) { |pair|
        actual = pair[0]
        expected = pair[1]
        assert_equal(expected[0], actual.x)
        assert_equal(expected[1], actual.y)
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
  end

  def test_optical_flow_hs
    size = 128
    prev = create_cvmat(size, size, :cv8u, 1) { |j, i|
      if ((i - (size / 2)) ** 2 ) + ((j - (size / 2)) ** 2 ) < size
        CvColor::Black
      else
        CvColor::White
      end
    }
    curr = create_cvmat(size, size, :cv8u, 1) { |j, i|
      if ((i - (size / 2) - 10) ** 2) + ((j - (size / 2) - 7) ** 2 ) < size
        CvColor::Black
      else
        CvColor::White
      end
    }

    [curr.optical_flow_hs(prev, nil, nil, :lambda => 0.0005, :criteria => CvTermCriteria.new(1, 0.001)),
     curr.optical_flow_hs(prev)].each { |velx, vely|
      assert_equal('d437cd896365c509b5d16fd5f2d7e498', hash_img(velx))
      assert_equal('36ccbcafcd58b4d07dab058fb60eede6', hash_img(vely))
    }

    velx, vely = curr.optical_flow_hs(prev, nil, nil, :lambda => 0.001)
    assert_equal('6a0133bcfa5057ef3d607429753d1f27', hash_img(velx))
    assert_equal('dffbc0e267b08f3ca0098f27ecf61f6e', hash_img(vely))

    velx, vely = curr.optical_flow_hs(prev, nil, nil, :criteria => CvTermCriteria.new(10, 0.01))
    assert_equal('5dc2bb2aaee70383da8b12c99cbf388b', hash_img(velx))
    assert_equal('b53cec7a363ba7491cde61540813b827', hash_img(vely))

    prev_velx, prev_vely = curr.optical_flow_hs(prev)
    velx, vely = curr.optical_flow_hs(prev, prev_velx, prev_vely)
    assert_equal('08b50f3d4cb4cbbe443fada293e6af02', hash_img(velx))
    assert_equal('4d302c8267388995ec85a4a26da5ffcc', hash_img(vely))

    velx, vely = curr.optical_flow_hs(prev, prev_velx, prev_vely, :lambda => 0.001)
    assert_equal('8bb08ee394719a70e24bfb8b428662cd', hash_img(velx))
    assert_equal('f6c09e73160f0792e4527cdeea0e5573', hash_img(vely))

    velx, vely = curr.optical_flow_hs(prev, prev_velx, prev_vely, :criteria => CvTermCriteria.new(10, 0.01))
    assert_equal('c32a8483e3aec3cd6c33bceeefb8d2f2', hash_img(velx))
    assert_equal('da33e266aece70ed69dcf074acd8fd4e', hash_img(vely))
    
    assert_raise(TypeError) {
      curr.optical_flow_hs(DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      curr.optical_flow_hs(prev, DUMMY_OBJ, prev_vely)
    }
    assert_raise(TypeError) {
      curr.optical_flow_hs(prev, prev_velx, DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      curr.optical_flow_hs(prev, prev_velx, prev_vely, DUMMY_OBJ)
    }
    assert_raise(CvStsUnmatchedFormats) {
      CvMat.new(10, 10, :cv8u, 3).optical_flow_hs(prev)
    }
  end

  def test_optical_flow_lk
    size = 128
    prev = create_cvmat(size, size, :cv8u, 1) { |j, i|
      if ((i - (size / 2)) ** 2 ) + ((j - (size / 2)) ** 2 ) < size
        CvColor::Black
      else
        CvColor::White
      end
    }
    curr = create_cvmat(size, size, :cv8u, 1) { |j, i|
      if ((i - (size / 2) - 10) ** 2) + ((j - (size / 2) - 7) ** 2 ) < size
        CvColor::Black
      else
        CvColor::White
      end
    }
    
    velx, vely = curr.optical_flow_lk(prev, CvSize.new(3, 3))
    assert_equal('13333362f0daf6ad732006bd2a32e177', hash_img(velx))
    assert_equal('45dc42034ab606dd61e34a5adc6a1c1b', hash_img(vely))

    velx, vely = curr.optical_flow_lk(prev, CvSize.new(5, 5))
    assert_equal('d83c57805f9c074d0ad33a7522a75952', hash_img(velx))
    assert_equal('df8fb2010b00d89090e7d9653781a68d', hash_img(vely))

    assert_raise(TypeError) {
      curr.optical_flow_lk(DUMMY_OBJ, CvSize.new(3, 3))
    }
    assert_raise(TypeError) {
      curr.optical_flow_lk(prev, DUMMY_OBJ)
    }
    assert_raise(CvStsUnmatchedFormats) {
      CvMat.new(10, 10, :cv8u, 3).optical_flow_lk(prev, CvSize.new(3, 3))
    }
  end

  def test_optical_flow_bm
    size = 128
    prev = create_cvmat(size, size, :cv8u, 1) { |j, i|
      if ((i - (size / 2)) ** 2 ) + ((j - (size / 2)) ** 2 ) < size
        CvColor::Black
      else
        CvColor::White
      end
    }
    curr = create_cvmat(size, size, :cv8u, 1) { |j, i|
      if ((i - (size / 2) - 10) ** 2) + ((j - (size / 2) - 7) ** 2 ) < size
        CvColor::Black
      else
        CvColor::White
      end
    }

    [curr.optical_flow_bm(prev, nil, nil, :block_size => CvSize.new(4, 4),
                          :shift_size => CvSize.new(1, 1), :max_range => CvSize.new(4, 4)),
     curr.optical_flow_bm(prev)].each { |velx, vely|
      assert_equal('08e73a6fa9af7684a5eddc4f30fd46e7', hash_img(velx))
      assert_equal('aabaf1b7393b950c2297f567b6f57d5d', hash_img(vely))
    }
    
    velx, vely = curr.optical_flow_bm(prev, nil, nil, :block_size => CvSize.new(3, 3))
    assert_equal('fe540dc1f0aec2d70b774286eafa3602', hash_img(velx))
    assert_equal('c7cc0f055fe4708396ba6046c0f1c6b5', hash_img(vely))

    velx, vely = curr.optical_flow_bm(prev, nil, nil, :shift_size => CvSize.new(2, 2))
    assert_equal('00ad7854afb6eb4e46f0fb31c312c5eb', hash_img(velx))
    assert_equal('33215ec0bfa7f6b1424d6fadb2a48e0f', hash_img(vely))

    velx, vely = curr.optical_flow_bm(prev, nil, nil, :max_range => CvSize.new(5, 5))
    assert_equal('ac10837eeee45fd80a24695cfaf9cfc7', hash_img(velx))
    assert_equal('8c7011c26ac53eaf1fae1aa9324e5979', hash_img(vely))
    
    prev_velx, prev_vely = curr.optical_flow_bm(prev)
    velx, vely = curr.optical_flow_bm(prev, prev_velx, prev_vely)
    assert_equal('6ad6b7a5c935379c0df4b9ec5666f3de', hash_img(velx))
    assert_equal('b317b0b9d4fdb0e5cd40beb0dd4143b4', hash_img(vely))

    assert_raise(TypeError) {
      curr.optical_flow_bm(DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      curr.optical_flow_bm(prev, DUMMY_OBJ, prev_vely)
    }
    assert_raise(TypeError) {
      curr.optical_flow_bm(prev, prev_velx, DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      curr.optical_flow_bm(prev, prev_velx, prev_vely, DUMMY_OBJ)
    }
    assert_raise(CvStsUnmatchedFormats) {
      CvMat.new(10, 10, :cv8u, 3).optical_flow_bm(prev)
    }
  end

  def test_extract_surf
    mat0 = CvMat.load(FILENAME_LENA256x256, CV_LOAD_IMAGE_GRAYSCALE)

    # simple
    keypoints1, descriptors1 = mat0.extract_surf(CvSURFParams.new(500, true, 2, 3))
    assert_equal(CvSeq, keypoints1.class)
    assert_equal(254, keypoints1.size)
    assert_equal(Array, descriptors1.class)
    assert_equal(254, descriptors1.size)
    assert_equal(Array, descriptors1[0].class)
    assert_equal(128, descriptors1[0].size)
    
    # use mask
    mask = create_cvmat(mat0.rows, mat0.cols, :cv8u, 1) { |j, i|
      if i < mat0.cols / 2
        CvScalar.new(1)
      else
        CvScalar.new(0)
      end
    }
    keypoints2, descriptors2 = mat0.extract_surf(CvSURFParams.new(500, false), mask)
    assert_equal(CvSeq, keypoints2.class)
    assert_equal(170, keypoints2.size)
    assert_equal(Array, descriptors2.class)
    assert_equal(170, descriptors2.size)
    assert_equal(Array, descriptors2[0].class)
    assert_equal(64, descriptors2[0].size)

    # raise exceptions because of invalid arguments
    assert_raise(TypeError) {
      mat0.extract_surf(DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      mat0.extract_surf(CvSURFParams.new(500), DUMMY_OBJ)
    }

    # Uncomment the following lines to show the result
    # results = []
    # [keypoints1, keypoints2].each { |kpts|
    #   tmp = mat0.GRAY2BGR
    #   kpts.each { |kp|
    #     tmp.circle!(kp.pt, 3, :color => CvColor::Red, :thickness => 1, :line_type => :aa)
    #   }
    #   results << tmp
    # }
    # snap mat0, *results
  end
end

