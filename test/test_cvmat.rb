#!/usr/bin/env ruby
# -*- mode: ruby; coding: utf-8 -*-
require 'test/unit'
require 'opencv'
require File.expand_path(File.dirname(__FILE__)) + '/helper'

include OpenCV

# Tests for OpenCV::CvMat
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
    assert_equal('ebc0b85d3ac44ea60181c997f35d13df', hash_img(mat))

    mat = CvMat.load(FILENAME_CAT, CV_LOAD_IMAGE_GRAYSCALE)
    assert_equal(CvMat, mat.class)
    assert_equal(375, mat.cols)
    assert_equal(500, mat.rows)
    assert_equal(:cv8u, mat.depth)
    assert_equal(1, mat.channel)
    assert_equal('f0ae1d7f2d6b3a64d093e3181361f3a4', hash_img(mat))

    mat = CvMat.load(FILENAME_CAT, CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_ANYCOLOR)
    assert_equal(CvMat, mat.class)
    assert_equal(375, mat.cols)
    assert_equal(500, mat.rows)
    assert_equal(:cv8u, mat.depth)
    assert_equal(3, mat.channel)
    assert_equal('ebc0b85d3ac44ea60181c997f35d13df', hash_img(mat))

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

    # Uncomment the following lines to see the result images
    #
    # open('test-jpeg.jpg', 'wb') { |f|
    #   f.write jpg.pack("c*")
    # }
    # open('test-png.png', 'wb') { |f|
    #   f.write png.pack("c*")
    # }
  end

  def test_decode
    data = nil
    open(FILENAME_CAT, 'rb') { |f|
      data = f.read
    }
    data_ary = data.unpack("c*")
    data_mat = CvMat.new(1, data_ary.size).set_data(data_ary)
    expected = CvMat.load(FILENAME_CAT)

    mat1 = CvMat.decode(data)
    mat2 = CvMat.decode(data_ary)
    mat3 = CvMat.decode(data_mat)
    mat4 = CvMat.decode(data, CV_LOAD_IMAGE_COLOR)
    mat5 = CvMat.decode(data_ary, CV_LOAD_IMAGE_COLOR)
    mat6 = CvMat.decode(data_mat, CV_LOAD_IMAGE_COLOR)
    expected_hash = hash_img(expected)

    [mat1, mat2, mat3, mat4, mat5, mat6].each { |mat|
      assert_equal(CvMat, mat.class)
      assert_equal(expected.rows, mat.rows)
      assert_equal(expected.cols, mat.cols)
      assert_equal(expected.channel, mat.channel)
      assert_equal(expected_hash, hash_img(mat))
    }

    expected_c1 = CvMat.load(FILENAME_CAT, CV_LOAD_IMAGE_GRAYSCALE)
    mat1c1 = CvMat.decode(data, CV_LOAD_IMAGE_GRAYSCALE)
    mat2c1 = CvMat.decode(data_ary, CV_LOAD_IMAGE_GRAYSCALE)
    mat3c1 = CvMat.decode(data_mat, CV_LOAD_IMAGE_GRAYSCALE)
    expected_hash_c1 = hash_img(expected_c1)

    [mat1c1, mat2c1, mat3c1].each { |mat|
      assert_equal(CvMat, mat.class)
      assert_equal(expected_c1.rows, mat.rows)
      assert_equal(expected_c1.cols, mat.cols)
      assert_equal(expected_c1.channel, mat.channel)
      assert_equal(expected_hash_c1, hash_img(mat))
    }

    assert_raise(TypeError) {
      CvMat.decode(DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      CvMat.decode(data, DUMMY_OBJ)
    }

    # Uncomment the following line to show the result images
    # snap mat1, mat2, mat3
  end

  def test_GOOD_FEATURES_TO_TRACK_OPTION
    assert_equal(0xff, CvMat::GOOD_FEATURES_TO_TRACK_OPTION[:max])
    assert_nil(CvMat::GOOD_FEATURES_TO_TRACK_OPTION[:mask])
    assert_equal(3, CvMat::GOOD_FEATURES_TO_TRACK_OPTION[:block_size])
    assert((not CvMat::GOOD_FEATURES_TO_TRACK_OPTION[:use_harris]))
    assert_in_delta(0.04, CvMat::GOOD_FEATURES_TO_TRACK_OPTION[:k], 0.01)
  end

  def test_FIND_CONTOURS_OPTION
    assert_equal(1, CvMat::FIND_CONTOURS_OPTION[:mode])
    assert_equal(2, CvMat::FIND_CONTOURS_OPTION[:method])
    assert_equal(0, CvMat::FIND_CONTOURS_OPTION[:offset].x)
    assert_equal(0, CvMat::FIND_CONTOURS_OPTION[:offset].y)
  end

  def test_FIND_FUNDAMENTAL_MAT_OPTION
    assert((not CvMat::FIND_FUNDAMENTAL_MAT_OPTION[:with_status]))
    assert_in_delta(1.0, CvMat::FIND_FUNDAMENTAL_MAT_OPTION[:maximum_distance], 0.01)
    assert_in_delta(0.99, CvMat::FIND_FUNDAMENTAL_MAT_OPTION[:desirable_level], 0.01)
  end

  def test_to_s
    m = CvMat.new(10, 20)
    assert_equal('<OpenCV::CvMat:20x10,depth=cv8u,channel=3>', m.to_s)
    m = CvMat.new(10, 20, :cv16s)
    assert_equal('<OpenCV::CvMat:20x10,depth=cv16s,channel=3>', m.to_s)
    m = CvMat.new(10, 20, :cv32f, 1)
    assert_equal('<OpenCV::CvMat:20x10,depth=cv32f,channel=1>', m.to_s)
  end

  def test_inside
    m = CvMat.new(20, 10)
    assert(m.inside? CvPoint.new(0, 0))
    assert(m.inside? CvPoint.new(9, 19))
    assert((not m.inside? CvPoint.new(10, 0)))
    assert((not m.inside? CvPoint.new(0, 20)))
    assert((not m.inside? CvPoint.new(10, 20)))
  end

  def test_to_IplConvKernel
    kernel = CvMat.new(10, 20).to_IplConvKernel(CvPoint.new(2, 3))
    assert_equal(10, kernel.rows)
    assert_equal(20, kernel.cols)
    assert_equal(2, kernel.anchor.x)
    assert_equal(3, kernel.anchor.y)
    assert_equal(2, kernel.anchor_x)
    assert_equal(3, kernel.anchor_y)
  end

  def test_create_mask
    mask = CvMat.new(10, 20).create_mask
    assert_equal(20, mask.width)
    assert_equal(10, mask.height)
    assert_equal(:cv8u, mask.depth)
    assert_equal(1, mask.channel)
  end

  def test_fields
    m = CvMat.new(20, 10)
    assert_equal(10, m.width)
    assert_equal(10, m.columns)
    assert_equal(10, m.cols)
    assert_equal(20, m.height)
    assert_equal(20, m.rows)
    assert_equal(:cv8u, m.depth)
    assert_equal(3, m.channel)

    m = CvMat.new(20, 10, :cv16s, 1)
    assert_equal(10, m.width)
    assert_equal(10, m.columns)
    assert_equal(10, m.cols)
    assert_equal(20, m.height)
    assert_equal(20, m.rows)
    assert_equal(:cv16s, m.depth)
    assert_equal(1, m.channel)
  end

  def test_clone
    m1 = create_cvmat(10, 20)
    m2 = m1.clone
    assert_equal(m1.data, m2.data)
  end

  def test_copy
    m1 = create_cvmat(10, 20, CV_32F, 1) { |j, i, c| CvScalar.new(c) }

    m2 = m1.copy
    assert_equal(m1.data, m2.data)
    
    m2 = create_cvmat(10, 20, CV_32F, 1).zero
    m3 = m1.copy(m2)
    assert_equal(m1.data, m2.data)
    assert_equal(m1.data, m3.data)

    rows, cols = m1.rows, m1.cols
    mask = create_cvmat(rows, cols, CV_8U, 1) { |j, i, c|
      val = (i > cols / 2) ? 0 : 255
      CvScalar.new(val)
    }

    m2_orig = m2.copy
    m3 = m1.copy(m2, mask)
    rows.times { |j|
      cols.times { |i|
        expected = (mask[j, i][0] == 0) ? m2_orig[j, i] : m1[j, i]
        assert_cvscalar_equal(expected, m2[j, i])
        assert_cvscalar_equal(expected, m3[j, i])
      }
    }
    
    assert_raise(TypeError) {
      m1.copy(DUMMY_OBJ)
    }
  end

  def test_convert_depth
    m = CvMat.new(10, 20, :cv32f)
    assert_equal(:cv8u, m.to_8u.depth)
    assert_equal(:cv8s, m.to_8s.depth)
    assert_equal(:cv16u, m.to_16u.depth)
    assert_equal(:cv16s, m.to_16s.depth)
    assert_equal(:cv32s, m.to_32s.depth)
    assert_equal(:cv32f, m.to_32f.depth)
    assert_equal(:cv64f, m.to_64f.depth)
  end

  def test_vector
    m = CvMat.new(1, 2)
    assert(m.vector?)

    m = CvMat.new(2, 2)
    assert((not m.vector?))
  end

  def test_square
    m = CvMat.new(2, 2)
    assert(m.square?)
    m = CvMat.new(1, 2)
    assert((not m.square?))
  end

  def test_to_CvMat
    m1 = CvMat.new(2, 3, :cv32f, 4)
    m2 = m1.to_CvMat
    assert_equal(CvMat, m2.class)
    assert_equal(m1.rows, m2.rows)
    assert_equal(m1.cols, m2.cols)
    assert_equal(m1.depth, m2.depth)
    assert_equal(m1.channel, m2.channel)
    assert_equal(m1.data, m2.data)
  end

  def test_sub_rect
    m1 = create_cvmat(10, 10)

    assert_raise(ArgumentError) {
      m1.sub_rect
    }

    m2 = m1.sub_rect(CvRect.new(0, 0, 2, 3))
    assert_equal(2, m2.width)
    assert_equal(3, m2.height)
    m2.height.times { |j|
      m2.width.times { |i|
        assert_cvscalar_equal(m1[j, i], m2[j, i])
      }
    }
    
    topleft = CvPoint.new(2, 3)
    m2 = m1.sub_rect(topleft, CvSize.new(4, 5))
    assert_equal(4, m2.width)
    assert_equal(5, m2.height)
    m2.height.times { |j|
      m2.width.times { |i|
        assert_cvscalar_equal(m1[topleft.y + j, topleft.x + i], m2[j, i])
      }
    }

    topleft = CvPoint.new(1, 2)
    m2 = m1.sub_rect(topleft.x, topleft.y, 3, 4)
    assert_equal(3, m2.width)
    assert_equal(4, m2.height)
    m2.height.times { |j|
      m2.width.times { |i|
        assert_cvscalar_equal(m1[topleft.y + j, topleft.x + i], m2[j, i])
      }
    }

    # Alias
    m2 = m1.subrect(CvRect.new(0, 0, 2, 3))
    assert_equal(2, m2.width)
    assert_equal(3, m2.height)
    m2.height.times { |j|
      m2.width.times { |i|
        assert_cvscalar_equal(m1[j, i], m2[j, i])
      }
    }

    assert_raise(TypeError) {
      m1.sub_rect(DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      m1.sub_rect(DUMMY_OBJ, CvSize.new(1, 2))
    }
    assert_raise(TypeError) {
      m1.sub_rect(CvPoint.new(1, 2), DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      m1.sub_rect(DUMMY_OBJ, 2, 3, 4)
    }
    assert_raise(TypeError) {
      m1.sub_rect(1, DUMMY_OBJ, 3, 4)
    }
    assert_raise(TypeError) {
      m1.sub_rect(1, 2, DUMMY_OBJ, 4)
    }
    assert_raise(TypeError) {
      m1.sub_rect(1, 2, 3, DUMMY_OBJ)
    }
  end

  def test_get_rows
    m1 = create_cvmat(10, 20) { |j, i, c| CvScalar.new(c) }

    row = 2
    m2 = m1.get_rows(row)
    assert_equal(1, m2.rows)
    assert_equal(m1.cols, m2.cols)
    m1.cols.times { |i|
      assert_cvscalar_equal(m1[row, i], m2[i])
    }

    row1 = 3..7
    row2 = 2...8
    [row1, row2].each { |row|
      m3 = m1.get_rows(row)
      w = (row.exclude_end?) ? row.last - row.begin : row.last - row.begin + 1
      assert_equal(w, m3.rows)
      assert_equal(m1.cols, m3.cols)

      m3.rows.times { |j|
        m3.cols.times { |i|
          assert_cvscalar_equal(m1[row.begin + j, i], m3[j, i])
        }
      }
    }

    [row1, row2].each { |row|
      delta = 2
      m3 = m1.get_rows(row, 2)
      w = (((row.exclude_end?) ? row.last - row.begin : row.last - row.begin + 1).to_f / delta).ceil
      assert_equal(w, m3.rows)
      assert_equal(m1.cols, m3.cols)

      m3.rows.times { |j|
        m3.cols.times { |i|
          assert_cvscalar_equal(m1[row.begin + j * delta, i], m3[j, i])
        }
      }
    }

    assert_raise(TypeError) {
      m1.get_rows(DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      m1.get_rows(1, DUMMY_OBJ)
    }
  end

  def test_get_cols
    m1 = create_cvmat(10, 20) { |j, i, c| CvScalar.new(c) }

    col = 2
    m2 = m1.get_cols(col)
    assert_equal(m1.rows, m2.rows)
    assert_equal(1, m2.cols)
    m1.height.times { |j|
      assert_cvscalar_equal(m1[j, col], m2[j])
    }

    col1 = 3..7
    col2 = 2...8
    [col1, col2].each { |col|
      m3 = m1.get_cols(col)
      w = (col.exclude_end?) ? col.last - col.begin : col.last - col.begin + 1
      assert_equal(m1.rows, m3.rows)
      assert_equal(w, m3.cols)

      m3.rows.times { |j|
        m3.cols.times { |i|
          assert_cvscalar_equal(m1[j, col.begin + i], m3[j, i])
        }
      }
    }

    assert_raise(TypeError) {
      m1.get_cols(DUMMY_OBJ)
    }
  end

  def test_each_row
    m1 = create_cvmat(2, 3)
    a = [[1, 2, 3], [4, 5, 6]]
    a.map! { |a1|
      a1.map! { |a2|
        CvScalar.new(a2, a2, a2, a2).to_ary
      }
    }

    j = 0
    m1.each_row { |r|
      a[j].size.times { |i|
        assert_cvscalar_equal(a[j][i], r[i])
      }
      j += 1
    }
  end

  def test_each_col
    m1 = create_cvmat(2, 3)
    a = [[1, 4], [2, 5], [3, 6]]
    a.map! { |a1|
      a1.map! { |a2|
        CvScalar.new(a2, a2, a2, a2).to_ary
      }
    }

    j = 0
    m1.each_col { |c|
      a[j].size.times { |i|
        assert_cvscalar_equal(a[j][i], c[i])
      }
      j += 1
    }

    # Alias
    j = 0
    m1.each_column { |c|
      a[j].size.times { |i|
        assert_cvscalar_equal(a[j][i], c[i])
      }
      j += 1
    }
  end

  def test_diag
    m = create_cvmat(5, 5)
    a = [1, 7, 13, 19, 25].map { |x| CvScalar.new(x, x, x, x) }
    d = m.diag

    a.each_with_index { |s, i|
      assert_cvscalar_equal(s, d[i])
    }

    a = [2, 8, 14, 20].map { |x| CvScalar.new(x, x, x, x) }
    d = m.diag(1)
    a.each_with_index { |s, i|
      assert_cvscalar_equal(s, d[i])
    }

    a = [6, 12, 18, 24].map { |x| CvScalar.new(x, x, x, x) }
    d = m.diag(-1)
    a.each_with_index { |s, i|
      assert_cvscalar_equal(s, d[i])
    }

    # Alias
    a = [1, 7, 13, 19, 25].map { |x| CvScalar.new(x, x, x, x) }
    d = m.diagonal
    a.each_with_index { |s, i|
      assert_cvscalar_equal(s, d[i])
    }

    [m.rows, m.cols, -m.rows, -m.cols].each { |d|
      assert_raise(CvStsOutOfRange) {
        m.diag(d)
      }
    }
  end

  def test_size
    m = CvMat.new(2, 3)
    assert_equal(3, m.size.width)
    assert_equal(2, m.size.height)
  end

  def test_dims
    m = CvMat.new(2, 3)
    assert_equal([2, 3], m.dims)
  end

  def test_dim_size
    m = CvMat.new(2, 3)
    assert_equal(2, m.dim_size(0))
    assert_equal(3, m.dim_size(1))

    assert_raise(TypeError) {
      m.dim_size(DUMMY_OBJ)
    }
  end

  def test_aref
    m = create_cvmat(2, 3)
    assert_cvscalar_equal(CvScalar.new(1, 1, 1, 1), m[0])
    assert_cvscalar_equal(CvScalar.new(5, 5, 5, 5), m[4])
    assert_cvscalar_equal(CvScalar.new(2, 2, 2, 2), m[0, 1])
    assert_cvscalar_equal(CvScalar.new(4, 4, 4, 4), m[1, 0])
    assert_cvscalar_equal(CvScalar.new(2, 2, 2, 2), m[0, 1, 2])
    assert_cvscalar_equal(CvScalar.new(4, 4, 4, 4), m[1, 0, 3, 4])

    # Alias
    assert_cvscalar_equal(CvScalar.new(1, 1, 1, 1), m.at(0))

    assert_raise(TypeError) {
      m[DUMMY_OBJ]
    }
    
    assert_raise(CvStsOutOfRange) {
      m[-1]
    }
    assert_raise(CvStsOutOfRange) {
      m[6]
    }
    assert_raise(CvStsOutOfRange) {
      m[2, 2]
    }
    assert_raise(CvStsOutOfRange) {
      m[1, 3]
    }
    assert_raise(CvStsOutOfRange) {
      m[2, 2, 1]
    }
    assert_raise(CvStsOutOfRange) {
      m[1, 3, 1]
    }
  end

  def test_aset
    m = create_cvmat(2, 3)
    m[0] = CvScalar.new(10, 10, 10, 10)
    assert_cvscalar_equal(CvScalar.new(10, 10, 10, 10), m[0])
    m[1, 0] = CvScalar.new(20, 20, 20, 20)
    assert_cvscalar_equal(CvScalar.new(20, 20, 20, 20), m[1, 0])
    m[1, 0, 2] = CvScalar.new(4, 4, 4, 4)
    assert_cvscalar_equal(CvScalar.new(4, 4, 4, 4), m[1, 0])
    m[1, 0, 2, 4] = CvScalar.new(5, 5, 5, 5)
    assert_cvscalar_equal(CvScalar.new(5, 5, 5, 5), m[1, 0])

    assert_raise(TypeError) {
      m[DUMMY_OBJ] = CvScalar.new(10, 10, 10, 10)
    }
    assert_raise(TypeError) {
      m[0] = DUMMY_OBJ
    }

    assert_raise(CvStsOutOfRange) {
      m[-1]
    }
    assert_raise(CvStsOutOfRange) {
      m[6]
    }
    assert_raise(CvStsOutOfRange) {
      m[2, 2]
    }
    assert_raise(CvStsOutOfRange) {
      m[1, 3]
    }
    assert_raise(CvStsOutOfRange) {
      m[2, 2, 1]
    }
    assert_raise(CvStsOutOfRange) {
      m[1, 3, 1]
    }
  end

  def test_set_data
    [CV_8U, CV_8S, CV_16U, CV_16S, CV_32S].each { |depth|
      a = [10, 20, 30, 40, 50, 60]
      m = CvMat.new(2, 3, depth, 1)
      m.set_data(a)
      (m.rows * m.cols).times { |i|
        assert_equal(a[i], m[i][0])
      }
    }

    [CV_32F, CV_64F].each { |depth|
      a = [0.1, 0.2, 0.3, 0.4, 0.5, 0.6]
      m = CvMat.new(2, 3, depth, 1)
      m.set_data(a)
      (m.rows * m.cols).times { |i|
        assert_in_delta(a[i], m[i][0], 1.0e-5)
      }
    }

    a = [[10, 20, 30], [40, 50, 60]]
    m = CvMat.new(2, 3, CV_8U, 1)
    m.set_data(a)
    (m.rows * m.cols).times { |i|
      assert_equal(a.flatten[i], m[i][0])
    }

    [CV_8U, CV_8S, CV_16U, CV_16S, CV_32S, CV_32F, CV_64F].each { |depth|
      m = CvMat.new(2, 3, depth, 1)
      assert_raise(TypeError) {
        a = [DUMMY_OBJ] * 6
        m.set_data(a)
      }
    }
  end

  def test_fill
    m1 = create_cvmat(2, 3)
    m2 = m1.fill(CvScalar.new(1, 2, 3, 4))
    m1.fill!(CvScalar.new(1, 2, 3, 4))
    m2.height.times { |j|
      m2.width.times { |i|
        assert_cvscalar_equal(CvScalar.new(1, 2, 3, 4), m1[j, i])
        assert_cvscalar_equal(CvScalar.new(1, 2, 3, 4), m2[j, i])
      }
    }

    m1 = create_cvmat(5, 5)
    m0 = m1.clone
    mask = CvMat.new(m1.height, m1.width, :cv8u, 1).clear
    2.times { |j|
      2.times { |i|
        mask[j, i] = CvScalar.new(1, 1, 1, 1)
      }
    }

    m2 = m1.fill(CvScalar.new(1, 2, 3, 4), mask)
    m1.fill!(CvScalar.new(1, 2, 3, 4), mask)
    m2.height.times { |j|
      m2.width.times { |i|
        if i < 2 and j < 2
          assert_cvscalar_equal(CvScalar.new(1, 2, 3, 4), m1[j, i])
          assert_cvscalar_equal(CvScalar.new(1, 2, 3, 4), m2[j, i])
        else
          assert_cvscalar_equal(m0[j, i], m1[j, i])
          assert_cvscalar_equal(m0[j, i], m2[j, i])
        end
      }
    }
    
    # Alias
    m1 = create_cvmat(2, 3)
    m2 = m1.set(CvScalar.new(1, 2, 3, 4))
    m1.set!(CvScalar.new(1, 2, 3, 4))
    m2.height.times { |j|
      m2.width.times { |i|
        assert_cvscalar_equal(CvScalar.new(1, 2, 3, 4), m1[j, i])
        assert_cvscalar_equal(CvScalar.new(1, 2, 3, 4), m2[j, i])
      }
    }

    m1 = create_cvmat(5, 5)
    m0 = m1.clone
    mask = CvMat.new(m1.height, m1.width, CV_8U, 1).clear
    2.times { |j|
      2.times { |i|
        mask[j, i] = CvScalar.new(1, 1, 1, 1)
      }
    }

    m2 = m1.set(CvScalar.new(1, 2, 3, 4), mask)
    m1.set!(CvScalar.new(1, 2, 3, 4), mask)
    m2.height.times { |j|
      m2.width.times { |i|
        if i < 2 and j < 2
          assert_cvscalar_equal(CvScalar.new(1, 2, 3, 4), m1[j, i])
          assert_cvscalar_equal(CvScalar.new(1, 2, 3, 4), m2[j, i])
        else
          assert_cvscalar_equal(m0[j, i], m1[j, i])
          assert_cvscalar_equal(m0[j, i], m2[j, i])
        end
      }
    }

    assert_raise(TypeError) {
      m1.fill(DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      m1.fill(CvScalar.new(1), DUMMY_OBJ)
    }
  end

  def test_clear
    m1 = create_cvmat(2, 3)
    m2 = m1.clear
    m1.clear!
    m2.height.times { |j|
      m2.width.times { |i|
        assert_cvscalar_equal(CvScalar.new(0, 0, 0, 0), m1[j, i])
        assert_cvscalar_equal(CvScalar.new(0, 0, 0, 0), m2[j, i])
      }
    }

    # Alias
    m1 = create_cvmat(2, 3)
    m2 = m1.set_zero
    m1.set_zero!
    m3 = create_cvmat(2, 3)
    m4 = m3.zero
    m3.zero!
    m2.height.times { |j|
      m2.width.times { |i|
        assert_cvscalar_equal(CvScalar.new(0, 0, 0, 0), m1[j, i])
        assert_cvscalar_equal(CvScalar.new(0, 0, 0, 0), m2[j, i])
        assert_cvscalar_equal(CvScalar.new(0, 0, 0, 0), m3[j, i])
        assert_cvscalar_equal(CvScalar.new(0, 0, 0, 0), m4[j, i])
      }
    }
  end

  def test_identity
    m1 = create_cvmat(5, 5)
    m2 = m1.identity
    m1.identity!
    m2.height.times { |j|
      m2.width.times { |i|
        if i == j
          assert_cvscalar_equal(CvScalar.new(1, 0, 0, 0), m1[j, i])
          assert_cvscalar_equal(CvScalar.new(1, 0, 0, 0), m2[j, i])
        else
          assert_cvscalar_equal(CvScalar.new(0, 0, 0, 0), m1[j, i])
          assert_cvscalar_equal(CvScalar.new(0, 0, 0, 0), m2[j, i])
        end
      }
    }

    m1 = CvMat.new(5, 5, :cv8u, 4)
    s = CvScalar.new(1, 2, 3, 4)
    m2 = m1.identity(s)
    m1.identity!(s)
    m2.height.times { |j|
      m2.width.times { |i|
        if i == j
          assert_cvscalar_equal(s, m1[j, i])
          assert_cvscalar_equal(s, m2[j, i])
        else
          assert_cvscalar_equal(CvScalar.new(0, 0, 0, 0), m1[j, i])
          assert_cvscalar_equal(CvScalar.new(0, 0, 0, 0), m2[j, i])
        end
      }
    }

    assert_raise(TypeError) {
      m1.identity(DUMMY_OBJ)
    }
  end

  def test_range
    m1 = CvMat.new(1, 10, CV_32S, 1)
    m2 = m1.range(0, m1.cols)
    m1.range!(0, m1.cols)
    m2.width.times { |i|
      assert_cvscalar_equal(CvScalar.new(i, 0, 0, 0), m1[0, i])
      assert_cvscalar_equal(CvScalar.new(i, 0, 0, 0), m2[0, i])
    }

    assert_raise(TypeError) {
      m1.range(DUMMY_OBJ, 2)
    }
    assert_raise(TypeError) {
      m1.range(1, DUMMY_OBJ)
    }
  end

  def test_reshape
    m = create_cvmat(2, 3, CV_8U, 3)

    vec = m.reshape(0, 1)
    assert_equal(6, vec.width)
    assert_equal(1, vec.height)
    size = m.width * m.height
    size.times { |i|
      assert_cvscalar_equal(m[i], vec[i])
    }

    ch1 = m.reshape(1)
    assert_equal(9, ch1.width)
    assert_equal(2, ch1.height)

    m.height.times { |j|
      m.width.times { |i|
        s1 = ch1[j, i * 3][0]
        s2 = ch1[j, i * 3 + 1][0]
        s3 = ch1[j, i * 3 + 2][0]
        assert_cvscalar_equal(m[j, i], CvScalar.new(s1, s2, s3, 0))
      }
    }
    
    assert_raise(TypeError) {
      m.reshape(DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      m.reshape(0, DUMMY_OBJ)
    }
  end

  def test_repeat
    m1 = create_cvmat(2, 3, :cv8u, 3)
    m2 = CvMat.new(6, 9, :cv8u, 3)
    m2 = m1.repeat(m2)
    m2.height.times { |j|
      m2.width.times { |i|
        a = m1[j % m1.height, i % m1.width]
        assert_cvscalar_equal(m2[j, i], a)
      }
    }
    assert_raise(TypeError) {
      m1.repeat(DUMMY_OBJ)
    }
  end

  def test_flip
    m0 = create_cvmat(2, 3)

    m1 = m0.clone
    m1.flip!(:x)
    m2 = m0.flip(:x)
    m3 = m0.clone
    m3.flip!(:y)
    m4 = m0.flip(:y)
    m5 = m0.clone
    m5.flip!(:xy)
    m6 = m0.flip(:xy)
    m7 = m0.clone
    m7.flip!
    m8 = m0.flip
    
    [m1, m2, m3, m4, m5, m6, m7, m8].each { |m|
      assert_equal(m0.height, m.height)
      assert_equal(m0.width, m.width)
    }
    m0.height.times { |j|
      m0.width.times { |i|
        ri = m0.width - i - 1
        rj = m0.height - j - 1
        assert_cvscalar_equal(m0[j, ri], m1[j, i])
        assert_cvscalar_equal(m0[j, ri], m2[j, i])
        assert_cvscalar_equal(m0[rj, i], m3[j, i])
        assert_cvscalar_equal(m0[rj, i], m4[j, i])
        assert_cvscalar_equal(m0[rj, ri], m5[j, i])
        assert_cvscalar_equal(m0[rj, ri], m6[j, i])
        assert_cvscalar_equal(m0[j, ri], m7[j, i])
        assert_cvscalar_equal(m0[j, ri], m8[j, i])
      }
    }

    assert_raise(TypeError) {
      m0.flip(DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      m0.flip!(DUMMY_OBJ)
    }
  end

  def test_split
    m0 = create_cvmat(2, 3, :cv8u, 3) { |j, i, c|
      CvScalar.new(c * 10, c * 20, c * 30)
    }

    splitted = m0.split
    assert_equal(m0.channel, splitted.size)
    splitted.each_with_index { |m, idx|
      assert_equal(CvMat, m.class)
      assert_equal(m0.height, m.height)
      assert_equal(m0.width, m.width)
      assert_equal(1, m.channel)

      c = 0
      m0.height.times { |j|
        m0.width.times { |i|
          val = c * 10 * (idx + 1)
          assert_cvscalar_equal(CvScalar.new(val), m[j, i])
          c += 1
        }
      }
    }

    # IplImage#split should return Array<IplImage>
    image = create_iplimage(2, 3, :cv8u, 3) { |j, i, c|
      CvScalar.new(c * 10, c * 20, c * 30)
    }

    splitted = image.split
    assert_equal(3, splitted.size)
    splitted.each_with_index { |img, channel|
      assert_equal(IplImage, img.class)
      assert_equal(image.height, img.height)
      assert_equal(image.width, img.width)
      assert_equal(1, img.channel)

      img.height.times { |j|
        img.width.times { |i|
          val = image[j, i][channel]
          assert_cvscalar_equal(CvScalar.new(val), img[j, i])
        }
      }
    }
  end

  def test_merge
    m0 = create_cvmat(2, 3, :cv8u, 4) { |j, i, c|
      CvScalar.new(c * 10, c * 20, c * 30, c * 40)
    }
    m1 = create_cvmat(2, 3, :cv8u, 1) { |j, i, c|
      CvScalar.new(c * 10)
    }
    m2 = create_cvmat(2, 3, :cv8u, 1) { |j, i, c|
      CvScalar.new(c * 20)
    }
    m3 = create_cvmat(2, 3, :cv8u, 1) { |j, i, c|
      CvScalar.new(c * 30)
    }
    m4 = create_cvmat(2, 3, :cv8u, 1) { |j, i, c|
      CvScalar.new(c * 40)
    }

    m = CvMat.merge(m1, m2, m3, m4)

    assert_equal(m0.height, m.height)
    assert_equal(m0.width, m.width)
    m0.height.times { |j|
      m0.width.times { |i|
        assert_cvscalar_equal(m0[j, i], m[j, i])
      }
    }

    m5 = create_cvmat(2, 3, :cv8u, 1) { |j, i, c|
      CvScalar.new(c * 50)
    }

    assert_raise(TypeError) {
      CvMat.merge(DUMMY_OBJ)
    }
    assert_raise(ArgumentError) {
      CvMat.merge
    }
    assert_raise(ArgumentError) {
      CvMat.merge(m1, m2, m3, m4, m5)
    }
    assert_raise(ArgumentError) {
      CvMat.merge(CvMat.new(1, 2, :cv8u, 2))
    }
    assert_raise(ArgumentError) {
      CvMat.merge(CvMat.new(1, 2, :cv8u, 1),
                  CvMat.new(2, 2, :cv8u, 1))
    }
    assert_raise(ArgumentError) {
      CvMat.merge(CvMat.new(1, 2, :cv8u, 1),
                  CvMat.new(1, 2, :cv32f, 1))
    }
  end

  def test_rand_shuffle
    m0 = create_cvmat(2, 3)
    m1 = m0.clone
    m1.rand_shuffle!
    m2 = m0.rand_shuffle
    m3 = m0.clone
    m3.rand_shuffle!(123, 234)
    m4 = m0.rand_shuffle(123, 234)
    
    assert_shuffled_equal = lambda { |src, shuffled|
      assert_equal(src.width, shuffled.width)
      assert_equal(src.height, shuffled.height)
      mat0, mat1 = [], []
      src.height { |j|
        src.width { |i|
          mat0 << src[j, i].to_s
          mat1 << shuffled[j, i].to_s
        }
      }
      assert_equal(0, (mat0 - mat1).size)
    }

    [m1, m2, m3, m4].each { |m|
      assert_shuffled_equal.call(m0, m)
    }

    assert_raise(TypeError) {
      m0.rand_shuffle(DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      m0.rand_shuffle(123, DUMMY_OBJ)
    }
  end

  def test_lut
    m0 = create_cvmat(2, 3, :cv8u, 3)
    lut_mat = create_cvmat(1, 256, :cv8u, 3) { |j, i, c|
      CvScalar.new(255 - c, 255 - c, 255 - c)
    }

    m = m0.lut(lut_mat)
    assert_equal(m0.height, m.height)
    assert_equal(m0.width, m.width)
    m0.height.times { |j|
      m0.width.times { |i|
        r, g, b = m0[j, i].to_ary.map { |c| 255 - c }
        assert_cvscalar_equal(CvScalar.new(r, g, b, 0), m[j, i])
      }
    }
    
    assert_raise(TypeError) {
      m0.lut(DUMMY_OBJ)
    }
  end

  def test_convert_scale
    m0 = create_cvmat(2, 3, :cv32f, 4) { |j, i, c|
      CvScalar.new(-c, -c, -c, -c)
    }

    m1 = m0.convert_scale(:depth => :cv8u)
    m2 = m0.convert_scale(:scale => 1.5)
    m3 = m0.convert_scale(:shift => 10.0)
    m4 = m0.convert_scale(:depth => CV_16U)

    [m1, m2, m3, m4].each { |m|
      assert_equal(m0.height, m.height)
      assert_equal(m0.width, m.width)
    }
    m0.height.times { |j|
      m0.width.times { |i|
        assert_cvscalar_equal(CvScalar.new(0, 0, 0, 0), m1[j, i])
        a = m0[j, i].to_ary.map { |x| x * 1.5 }
        assert_in_delta(a, m2[j, i], 0.001)
        a = m0[j, i].to_ary.map { |x| x + 10.0 }
        assert_in_delta(a, m3[j, i], 0.001)
        assert_cvscalar_equal(CvScalar.new(0, 0, 0, 0), m4[j, i])
      }
    }

    assert_raise(TypeError) {
      m0.convert_scale(DUMMY_OBJ)
    }
  end

  def test_convert_scale_abs
    m0 = create_cvmat(2, 3, :cv8u, 4) { |j, i, c|
      CvScalar.new(c, c, c, c)
    }
    
    m1 = m0.convert_scale_abs(:depth => :cv64f)
    m2 = m0.convert_scale_abs(:scale => 2)
    m3 = m0.convert_scale_abs(:shift => 10.0)
    m4 = m0.convert_scale_abs(:depth => CV_64F)

    [m1, m2, m3, m4].each { |m|
      assert_equal(m0.height, m.height)
      assert_equal(m0.width, m.width)
    }
    m0.height.times { |j|
      m0.width.times { |i|
        assert_cvscalar_equal(m0[j, i], m1[j, i])
        a = m0[j, i].to_ary.map { |x| (x * 2).abs }
        assert_in_delta(a, m2[j, i], 0.001)
        a = m0[j, i].to_ary.map { |x| (x + 10.0).abs }
        assert_in_delta(a, m3[j, i], 0.001)
        assert_cvscalar_equal(m0[j, i], m4[j, i])
      }
    }

    assert_raise(TypeError) {
      m0.convert_scale(DUMMY_OBJ)
    }
  end

  def test_add
    m1 = create_cvmat(6, 4, :cv32f, 4) { |j, i, c|
      CvScalar.new(c * 0.1, c * 0.2, c * 0.3, c * 0.4)
    }
    m2 = create_cvmat(6, 4, :cv32f, 4) { |j, i, c|
      CvScalar.new(c * 1, c * 2, c * 3, c * 4)
    }

    # CvMat + CvMat
    m3 = m1.add(m2)
    assert_equal(m1.height, m3.height)
    assert_equal(m1.width, m3.width)
    n = 0
    m1.height.times { |j|
      m1.width.times { |i|
        s = CvScalar.new(n * 1.1, n * 2.2, n * 3.3, n * 4.4)
        assert_in_delta(s, m3[j, i], 0.001)
        n += 1
      }
    }

    # CvMat + CvScalar
    s1 = CvScalar.new(1, 2, 3, 4)
    m3 = m1.add(s1)
    assert_equal(m1.height, m3.height)
    assert_equal(m1.width, m3.width)
    n = 0
    m1.height.times { |j|
      m1.width.times { |i|
        s = CvScalar.new(n * 0.1 + 1, n * 0.2 + 2, n * 0.3 + 3, n * 0.4 + 4)
        assert_in_delta(s, m3[j, i], 0.001)
        n += 1
      }
    }

    # Alias
    m3 = m1 + m2
    assert_equal(m1.height, m3.height)
    assert_equal(m1.width, m3.width)
    n = 0
    m1.height.times { |j|
      m1.width.times { |i|
        s = CvScalar.new(n * 1.1, n * 2.2, n * 3.3, n * 4.4)
        assert_in_delta(s, m3[j, i], 0.001)
        n += 1
      }
    }

    # CvMat + CvMat with Mask
    mask = create_cvmat(6, 4, :cv8u, 1) { |j, i, c|
      (i < 3 and j < 2) ? 1 : 0
    }

    m4 = m1.add(m2, mask)
    assert_equal(m1.height, m4.height)
    assert_equal(m1.width, m4.width)
    n = 0
    m1.height.times { |j|
      m1.width.times { |i|
        if i < 3 and j < 2
          s = CvScalar.new(n * 1.1, n * 2.2, n * 3.3, n * 4.4)
        else
          s = m1[j, i]
        end
        assert_in_delta(s, m4[j, i], 0.001)
        n += 1
      }
    }

    # CvMat + CvScalar with Mask
    m4 = m1.add(s1, mask)
    assert_equal(m1.height, m4.height)
    assert_equal(m1.width, m4.width)
    n = 0
    m1.height.times { |j|
      m1.width.times { |i|
        if i < 3 and j < 2
          s = CvScalar.new(n * 0.1 + 1, n * 0.2 + 2, n * 0.3 + 3, n * 0.4 + 4)          
        else
          s = m1[j, i]
        end
        assert_in_delta(s, m4[j, i], 0.001)
        n += 1
      }
    }

    assert_raise(TypeError) {
      m1.add(DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      m1.add(CvScalar.new(1), DUMMY_OBJ)
    }
  end

  def test_sub
    m1 = create_cvmat(6, 4, :cv32f, 4) { |j, i, c|
      CvScalar.new(c * 0.1, c * 0.2, c * 0.3, c * 0.4)
    }
    m2 = create_cvmat(6, 4, :cv32f, 4) { |j, i, c|
      CvScalar.new(c * 1, c * 2, c * 3, c * 4)
    }

    # CvMat - CvMat
    m3 = m1.sub(m2)
    assert_equal(m1.height, m3.height)
    assert_equal(m1.width, m3.width)
    n = 0
    m1.height.times { |j|
      m1.width.times { |i|
        s = CvScalar.new(-n * 0.9, -n * 1.8, -n * 2.7, -n * 3.6)
        assert_in_delta(s, m3[j, i], 0.001)
        n += 1
      }
    }

    # CvMat - CvScalar
    s1 = CvScalar.new(1, 2, 3, 4)
    m3 = m1.sub(s1)
    assert_equal(m1.height, m3.height)
    assert_equal(m1.width, m3.width)
    n = 0
    m1.height.times { |j|
      m1.width.times { |i|
        s = CvScalar.new(n * 0.1 - 1, n * 0.2 - 2, n * 0.3 - 3, n * 0.4 - 4)
        assert_in_delta(s, m3[j, i], 0.001)
        n += 1
      }
    }

    # Alias
    m3 = m1 - m2
    assert_equal(m1.height, m3.height)
    assert_equal(m1.width, m3.width)
    n = 0
    m1.height.times { |j|
      m1.width.times { |i|
        s = CvScalar.new(-n * 0.9, -n * 1.8, -n * 2.7, -n * 3.6)
        assert_in_delta(s, m3[j, i], 0.001)
        n += 1
      }
    }

    mask = create_cvmat(6, 4, :cv8u, 1) { |j, i, c|
      (i < 3 and j < 2) ? 1 : 0
    }

    # CvMat - CvMat with Mask
    m4 = m1.sub(m2, mask)
    assert_equal(m1.height, m4.height)
    assert_equal(m1.width, m4.width)
    n = 0
    m1.height.times { |j|
      m1.width.times { |i|
        if i < 3 and j < 2
          s = CvScalar.new(-n * 0.9, -n * 1.8, -n * 2.7, -n * 3.6)
        else
          s = m1[j, i]
        end
        assert_in_delta(s, m4[j, i], 0.001)
        n += 1
      }
    }

    # CvMat - CvScalar with Mask
    m4 = m1.sub(s1, mask)
    assert_equal(m1.height, m4.height)
    assert_equal(m1.width, m4.width)
    n = 0
    m1.height.times { |j|
      m1.width.times { |i|
        if i < 3 and j < 2
          s = CvScalar.new(n * 0.1 - 1, n * 0.2 - 2, n * 0.3 - 3, n * 0.4 - 4)
        else
          s = m1[j, i]
        end
        assert_in_delta(s, m4[j, i], 0.001)
        n += 1
      }
    }

    assert_raise(TypeError) {
      m1.sub(DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      m1.sub(CvScalar.new(1), DUMMY_OBJ)
    }
  end

  def test_mul
    m1 = create_cvmat(3, 3, :cv32f)
    s1 = CvScalar.new(0.1, 0.2, 0.3, 0.4)
    m2 = create_cvmat(3, 3, :cv32f) { s1 }

    # CvMat * CvMat
    m3 = m1.mul(m2)
    assert_equal(m1.height, m3.height)
    assert_equal(m1.width, m3.width)
    assert_each_cvscalar(m3, 0.001) { |j, i, c|
      n = c + 1
      CvScalar.new(n * 0.1, n * 0.2, n * 0.3, n * 0.4)
    }
    
    # CvMat * CvMat * scale
    scale = 2.5
    m3 = m1.mul(m2, scale)
    assert_equal(m1.height, m3.height)
    assert_equal(m1.width, m3.width)
    assert_each_cvscalar(m3, 0.001) { |j, i, c|
      n = (c + 1) * scale
      CvScalar.new(n * 0.1, n * 0.2, n * 0.3, n * 0.4)
    }

    # CvMat * CvScalar
    scale = 2.5
    m3 = m1.mul(s1)
    assert_equal(m1.height, m3.height)
    assert_equal(m1.width, m3.width)
    assert_each_cvscalar(m3, 0.001) { |j, i, c|
      n = c + 1
      CvScalar.new(n * 0.1, n * 0.2, n * 0.3, n * 0.4)
    }

    # CvMat * CvScalar * scale
    m3 = m1.mul(s1, scale)
    assert_equal(m1.height, m3.height)
    assert_equal(m1.width, m3.width)
    assert_each_cvscalar(m3, 0.001) { |j, i, c|
      n = (c + 1) * scale
      CvScalar.new(n * 0.1, n * 0.2, n * 0.3, n * 0.4)
    }

    assert_raise(TypeError) {
      m1.mul(DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      m1.mul(m2, DUMMY_OBJ)
    }
  end

  def test_mat_mul
    m0 = create_cvmat(3, 3, :cv32f, 1) { |j, i, c|
      CvScalar.new(c * 0.1)
    }
    m1 = create_cvmat(3, 3, :cv32f, 1) { |j, i, c|
      CvScalar.new(c)
    }
    m2 = create_cvmat(3, 3, :cv32f, 1) { |j, i, c|
      CvScalar.new(c + 1)
    }

    m3 = m0.mat_mul(m1)
    m4 = m0 * m1

    [m3, m4].each { |m|
      assert_equal(m1.width, m.width)
      assert_equal(m1.height, m.height)
      assert_in_delta(1.5, m[0, 0][0], 0.001)
      assert_in_delta(1.8, m[0, 1][0], 0.001)
      assert_in_delta(2.1, m[0, 2][0], 0.001)
      assert_in_delta(4.2, m[1, 0][0], 0.001)
      assert_in_delta(5.4, m[1, 1][0], 0.001)
      assert_in_delta(6.6, m[1, 2][0], 0.001)
      assert_in_delta(6.9, m[2, 0][0], 0.001)
      assert_in_delta(9, m[2, 1][0], 0.001)
      assert_in_delta(11.1, m[2, 2][0], 0.001)
    }

    m5 = m0.mat_mul(m1, m2)
    [m5].each { |m|
      assert_equal(m1.width, m.width)
      assert_equal(m1.height, m.height)
      assert_in_delta(2.5, m[0, 0][0], 0.001)
      assert_in_delta(3.8, m[0, 1][0], 0.001)
      assert_in_delta(5.1, m[0, 2][0], 0.001)
      assert_in_delta(8.2, m[1, 0][0], 0.001)
      assert_in_delta(10.4, m[1, 1][0], 0.001)
      assert_in_delta(12.6, m[1, 2][0], 0.001)
      assert_in_delta(13.9, m[2, 0][0], 0.001)
      assert_in_delta(17, m[2, 1][0], 0.001)
      assert_in_delta(20.1, m[2, 2][0], 0.001)
    }

    assert_raise(TypeError) {
      m0.mat_mul(DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      m0.mat_mul(m1, DUMMY_OBJ)
    }
  end

  def test_div
    m1 = create_cvmat(3, 3, :cv32f)
    s1 = CvScalar.new(0.1, 0.2, 0.3, 0.4)
    m2 = create_cvmat(3, 3, :cv32f) { s1 }

    # CvMat / CvMat
    m3 = m1.div(m2)
    assert_equal(m1.height, m3.height)
    assert_equal(m1.width, m3.width)
    assert_each_cvscalar(m3, 0.001) { |j, i, c|
      n = c + 1
      CvScalar.new(n / 0.1, n / 0.2, n / 0.3, n / 0.4)
    }
    
    # scale * CvMat / CvMat
    scale = 2.5
    m3 = m1.div(m2, scale)
    assert_equal(m1.height, m3.height)
    assert_equal(m1.width, m3.width)
    assert_each_cvscalar(m3, 0.001) { |j, i, c|
      n = (c + 1) * scale
      CvScalar.new(n / 0.1, n / 0.2, n / 0.3, n / 0.4)
    }

    # CvMat / CvScalar
    scale = 2.5
    m3 = m1.div(s1)
    assert_equal(m1.height, m3.height)
    assert_equal(m1.width, m3.width)
    assert_each_cvscalar(m3, 0.001) { |j, i, c|
      n = c + 1
      CvScalar.new(n / 0.1, n / 0.2, n / 0.3, n / 0.4)
    }

    # scale * CvMat / CvScalar
    m3 = m1.div(s1, scale)
    assert_equal(m1.height, m3.height)
    assert_equal(m1.width, m3.width)
    assert_each_cvscalar(m3, 0.001) { |j, i, c|
      n = (c + 1) * scale
      CvScalar.new(n / 0.1, n / 0.2, n / 0.3, n / 0.4)
    }

    # Alias
    m3 = m1 / m2
    assert_equal(m1.height, m3.height)
    assert_equal(m1.width, m3.width)
    assert_each_cvscalar(m3, 0.001) { |j, i, c|
      n = c + 1
      CvScalar.new(n / 0.1, n / 0.2, n / 0.3, n / 0.4)
    }

    assert_raise(TypeError) {
      m1.div(DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      m1.div(m2, DUMMY_OBJ)
    }
  end

  def test_add_weighted
    m1 = create_cvmat(3, 2, :cv8u) { |j, i, c| c + 1 }
    m2 = create_cvmat(3, 2, :cv8u) { |j, i, c| (c + 1) * 10 }
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

  def test_and
    m1 = create_cvmat(6, 4)
    s1 = CvScalar.new(1, 2, 3, 4)
    m2 = create_cvmat(6, 4) { s1 }
    mask = create_cvmat(6, 4, :cv8u, 1) { |j, i, c|
      s = (i < 3 and j < 2) ? 1 : 0
      CvScalar.new(s)
    }

    # CvMat & CvMat
    m3 = m1.and(m2)
    assert_equal(m1.height, m3.height)
    assert_equal(m1.width, m3.width)
    assert_each_cvscalar(m3) { |j, i, c|
      n = c + 1
      CvScalar.new(n & 1, n & 2, n & 3, n & 4)
    }

    # CvMat & CvMat with mask
    m3 = m1.and(m2, mask)
    assert_equal(m1.height, m3.height)
    assert_equal(m1.width, m3.width)
    assert_each_cvscalar(m3) { |j, i, c|
      n = c + 1
      if i < 3 and j < 2
        CvScalar.new(n & 1, n & 2, n & 3, n & 4)
      else
        CvScalar.new(n, n, n, n)
      end
    }
    
    # CvMat & CvScalar
    m3 = m1.and(s1)
    assert_equal(m1.height, m3.height)
    assert_equal(m1.width, m3.width)
    assert_each_cvscalar(m3) { |j, i, c|
      n = c + 1
      CvScalar.new(n & 1, n & 2, n & 3, n & 4)
    }

    # CvMat & CvScalar with mask
    m3 = m1.and(s1, mask)
    assert_equal(m1.height, m3.height)
    assert_equal(m1.width, m3.width)
    assert_each_cvscalar(m3) { |j, i, c|
      n = c + 1
      if i < 3 and j < 2
        CvScalar.new(n & 1, n & 2, n & 3, n & 4)
      else
        CvScalar.new(n, n, n, n)
      end
    }

    # Alias
    m3 = m1 & m2
    assert_equal(m1.height, m3.height)
    assert_equal(m1.width, m3.width)
    assert_each_cvscalar(m3) { |j, i, c|
      n = c + 1
      CvScalar.new(n & 1, n & 2, n & 3, n & 4)
    }

    m3 = m1 & s1
    assert_equal(m1.height, m3.height)
    assert_equal(m1.width, m3.width)
    assert_each_cvscalar(m3) { |j, i, c|
      n = c + 1
      CvScalar.new(n & 1, n & 2, n & 3, n & 4)
    }

    assert_raise(TypeError) {
      m1.and(DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      m1.and(m2, DUMMY_OBJ)
    }
  end

  def test_or
    m1 = create_cvmat(6, 4)
    s1 = CvScalar.new(1, 2, 3, 4)
    m2 = create_cvmat(6, 4) { s1 }
    mask = create_cvmat(6, 4, :cv8u, 1) { |j, i, c|
      s = (i < 3 and j < 2) ? 1 : 0
      CvScalar.new(s)
    }
    
    # CvMat | CvMat
    m3 = m1.or(m2)
    assert_equal(m1.height, m3.height)
    assert_equal(m1.width, m3.width)
    assert_each_cvscalar(m3) { |j, i, c|
      n = c + 1
      CvScalar.new(n | 1, n | 2, n | 3, n | 4)
    }

    # CvMat | CvMat with mask
    m3 = m1.or(m2, mask)
    assert_equal(m1.height, m3.height)
    assert_equal(m1.width, m3.width)
    assert_each_cvscalar(m3) { |j, i, c|
      n = c + 1
      if i < 3 and j < 2
        CvScalar.new(n | 1, n | 2, n | 3, n | 4)
      else
        CvScalar.new(n, n, n, n)
      end
    }
    
    # CvMat | CvScalar
    m3 = m1.or(s1)
    assert_equal(m1.height, m3.height)
    assert_equal(m1.width, m3.width)
    assert_each_cvscalar(m3) { |j, i, c|
      n = c + 1
      CvScalar.new(n | 1, n | 2, n | 3, n | 4)
    }

    # CvMat | CvScalar with mask
    m3 = m1.or(s1, mask)
    assert_equal(m1.height, m3.height)
    assert_equal(m1.width, m3.width)
    assert_each_cvscalar(m3) { |j, i, c|
      n = c + 1
      if i < 3 and j < 2
        CvScalar.new(n | 1, n | 2, n | 3, n | 4)
      else
        CvScalar.new(n, n, n, n)
      end
    }

    # Alias
    m3 = m1 | m2
    assert_equal(m1.height, m3.height)
    assert_equal(m1.width, m3.width)
    assert_each_cvscalar(m3) { |j, i, c|
      n = c + 1
      CvScalar.new(n | 1, n | 2, n | 3, n | 4)
    }

    m3 = m1 | s1
    assert_equal(m1.height, m3.height)
    assert_equal(m1.width, m3.width)
    assert_each_cvscalar(m3) { |j, i, c|
      n = c + 1
      CvScalar.new(n | 1, n | 2, n | 3, n | 4)
    }

    assert_raise(TypeError) {
      m1.or(DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      m1.or(m2, DUMMY_OBJ)
    }
  end

  def test_xor
    m1 = create_cvmat(6, 4)
    s1 = CvScalar.new(1, 2, 3, 4)
    m2 = create_cvmat(6, 4) { s1 }
    mask = create_cvmat(6, 4, :cv8u, 1) { |j, i, c|
      s = (i < 3 and j < 2) ? 1 : 0
      CvScalar.new(s)
    }
    
    # CvMat ^ CvMat
    m3 = m1.xor(m2)
    assert_equal(m1.height, m3.height)
    assert_equal(m1.width, m3.width)
    assert_each_cvscalar(m3) { |j, i, c|
      n = c + 1
      CvScalar.new(n ^ 1, n ^ 2, n ^ 3, n ^ 4)
    }

    # CvMat ^ CvMat with mask
    m3 = m1.xor(m2, mask)
    assert_equal(m1.height, m3.height)
    assert_equal(m1.width, m3.width)
    assert_each_cvscalar(m3) { |j, i, c|
      n = c + 1
      if i < 3 and j < 2
        CvScalar.new(n ^ 1, n ^ 2, n ^ 3, n ^ 4)
      else
        CvScalar.new(n, n, n, n)
      end
    }
    
    # CvMat ^ CvScalar
    m3 = m1.xor(s1)
    assert_equal(m1.height, m3.height)
    assert_equal(m1.width, m3.width)
    assert_each_cvscalar(m3) { |j, i, c|
      n = c + 1
      CvScalar.new(n ^ 1, n ^ 2, n ^ 3, n ^ 4)
    }

    # CvMat ^ CvScalar with mask
    m3 = m1.xor(s1, mask)
    assert_equal(m1.height, m3.height)
    assert_equal(m1.width, m3.width)
    assert_each_cvscalar(m3) { |j, i, c|
      n = c + 1
      if i < 3 and j < 2
        CvScalar.new(n ^ 1, n ^ 2, n ^ 3, n ^ 4)
      else
        CvScalar.new(n, n, n, n)
      end
    }

    # Alias
    m3 = m1 ^ m2
    assert_equal(m1.height, m3.height)
    assert_equal(m1.width, m3.width)
    assert_each_cvscalar(m3) { |j, i, c|
      n = c + 1
      CvScalar.new(n ^ 1, n ^ 2, n ^ 3, n ^ 4)
    }

    m3 = m1 ^ s1
    assert_equal(m1.height, m3.height)
    assert_equal(m1.width, m3.width)
    assert_each_cvscalar(m3) { |j, i, c|
      n = c + 1
      CvScalar.new(n ^ 1, n ^ 2, n ^ 3, n ^ 4)
    }

    assert_raise(TypeError) {
      m1.xor(DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      m1.xor(m2, DUMMY_OBJ)
    }
  end

  def test_not
    m1 = create_cvmat(6, 4, :cv8s)
    m2 = m1.not;
    m3 = m1.clone
    m3.not!
    [m2, m3].each { |m|
      assert_equal(m1.height, m.height)
      assert_equal(m1.width, m.width)
      assert_each_cvscalar(m) { |j, i, c|
        n = c + 1
        CvScalar.new(~n, ~n, ~n, ~n)
      }
    }
  end

  def test_eq
    m1 = create_cvmat(6, 4, :cv8u, 1) { |j, i, c|
      n = (c.even?) ? 10 : c
      CvScalar.new(n, 0, 0, 0)
    }
    m2 = create_cvmat(6, 4, :cv8u, 1) { |j, i, c|
      CvScalar.new(10, 0, 0, 0)
    }
    s1 = CvScalar.new(10, 0, 0, 0)
    m3 = m1.eq(m2)
    m4 = m1.eq(s1)
    m5 = m1.eq(10)

    [m3, m4, m5].each { |m|
      assert_equal(m1.height, m.height)
      assert_equal(m1.width, m.width)
      assert_each_cvscalar(m) { |j, i, c|
        n = (c.even?) ? 0xff : 0
        CvScalar.new(n, 0, 0, 0)
      }
    }

    assert_raise(TypeError) {
      m1.eq(DUMMY_OBJ)
    }
  end

  def test_gt
    m1 = create_cvmat(6, 4, :cv8u, 1) { |j, i, c|
      CvScalar.new(c, 0, 0, 0)
    }
    m2 = create_cvmat(6, 4, :cv8u, 1) { |j, i, c|
      CvScalar.new(10, 0, 0, 0)
    }
    s1 = CvScalar.new(10, 0, 0, 0)
    m3 = m1.gt(m2)
    m4 = m1.gt(s1)
    m5 = m1.gt(10)

    [m3, m4, m5].each { |m|
      assert_equal(m1.height, m.height)
      assert_equal(m1.width, m.width)
      assert_each_cvscalar(m) { |j, i, c|
        n = (c > 10) ? 0xff : 0
        CvScalar.new(n, 0, 0, 0)
      }
    }

    assert_raise(TypeError) {
      m1.gt(DUMMY_OBJ)
    }
  end

  def test_ge
    m1 = create_cvmat(6, 4, :cv8u, 1) { |j, i, c|
      CvScalar.new(c, 0, 0, 0)
    }
    m2 = create_cvmat(6, 4, :cv8u, 1) { |j, i, c|
      CvScalar.new(10, 0, 0, 0)
    }
    s1 = CvScalar.new(10, 0, 0, 0)
    m3 = m1.ge(m2)
    m4 = m1.ge(s1)
    m5 = m1.ge(10)

    [m3, m4, m5].each { |m|
      assert_equal(m1.height, m.height)
      assert_equal(m1.width, m.width)
      assert_each_cvscalar(m) { |j, i, c|
        n = (c >= 10) ? 0xff : 0
        CvScalar.new(n, 0, 0, 0)
      }
    }

    assert_raise(TypeError) {
      m1.ge(DUMMY_OBJ)
    }
  end

  def test_lt
    m1 = create_cvmat(6, 4, :cv8u, 1) { |j, i, c|
      CvScalar.new(c, 0, 0, 0)
    }
    m2 = create_cvmat(6, 4, :cv8u, 1) { |j, i, c|
      CvScalar.new(10, 0, 0, 0)
    }
    s1 = CvScalar.new(10, 0, 0, 0)
    m3 = m1.lt(m2)
    m4 = m1.lt(s1)
    m5 = m1.lt(10)

    [m3, m4, m5].each { |m|
      assert_equal(m1.height, m.height)
      assert_equal(m1.width, m.width)
      assert_each_cvscalar(m) { |j, i, c|
        n = (c < 10) ? 0xff : 0
        CvScalar.new(n, 0, 0, 0)
      }
    }

    assert_raise(TypeError) {
      m1.lt(DUMMY_OBJ)
    }
  end

  def test_le
    m1 = create_cvmat(6, 4, :cv8u, 1) { |j, i, c|
      CvScalar.new(c, 0, 0, 0)
    }
    m2 = create_cvmat(6, 4, :cv8u, 1) { |j, i, c|
      CvScalar.new(10, 0, 0, 0)
    }
    s1 = CvScalar.new(10, 0, 0, 0)
    m3 = m1.le(m2)
    m4 = m1.le(s1)
    m5 = m1.le(10)

    [m3, m4, m5].each { |m|
      assert_equal(m1.height, m.height)
      assert_equal(m1.width, m.width)
      assert_each_cvscalar(m) { |j, i, c|
        n = (c <= 10) ? 0xff : 0
        CvScalar.new(n, 0, 0, 0)
      }
    }

    assert_raise(TypeError) {
      m1.le(DUMMY_OBJ)
    }
  end

  def test_in_range
    lower, upper = 10, 20
    m0 = create_cvmat(6, 4, :cv8u, 1) { |j, i, c|
      CvScalar.new(c + 5, 0, 0, 0)
    }
    m1 = create_cvmat(6, 4, :cv8u, 1) { |j, i, c|
      CvScalar.new(lower, 0, 0, 0)
    }
    m2 = create_cvmat(6, 4, :cv8u, 1) { |j, i, c|
      CvScalar.new(upper, 0, 0, 0)
    }
    s1 = CvScalar.new(lower, 0, 0, 0)
    s2 = CvScalar.new(upper, 0, 0, 0)

    m3 = m0.in_range(m1, m2)
    m4 = m0.in_range(s1, s2)
    m5 = m0.in_range(lower, upper)

    [m3, m4, m5].each { |m|
      assert_equal(m0.height, m.height)
      assert_equal(m0.width, m.width)
      assert_each_cvscalar(m) { |j, i, c|
        val = m0[j, i][0]
        n = ((lower..upper).include? val) ? 0xff : 0
        CvScalar.new(n, 0, 0, 0)
      }
    }

    assert_raise(TypeError) {
      m0.in_range(DUMMY_OBJ, m2)
    }
    assert_raise(TypeError) {
      m0.in_range(m1, DUMMY_OBJ)
    }
  end

  def test_abs_diff
    m0 = create_cvmat(6, 4, :cv32f, 4) { |j, i, c|
      CvScalar.new(-10 + 10.5, 20 + 10.5, -30 + 10.5, 40 - 10.5)
    }
    m1 = create_cvmat(6, 4, :cv32f, 4) { |j, i, c|
      CvScalar.new(c + 10.5, c - 10.5, c + 10.5, c - 10.5)
    }
    m2 = create_cvmat(6, 4, :cv32f, 4) { |j, i, c|
      CvScalar.new(c, c, c, c)
    }

    s1 = CvScalar.new(-10, 20, -30, 40)
    m3 = m1.abs_diff(m2)
    m4 = m0.abs_diff(s1)

    [m3, m4].each { |m|
      assert_equal(m1.width, m.width)
      assert_equal(m1.height, m.height)
      assert_each_cvscalar(m, 0.001) {
        CvScalar.new(10.5, 10.5, 10.5, 10.5)
      }
    }

    assert_raise(TypeError) {
      m0.abs_diff(DUMMY_OBJ)
    }
  end

  def test_normalize
    mat = create_cvmat(2, 2, :cv32f, 1) { |j, i, c|
      CvScalar.new(c, 0, 0, 0)
    }

    m = mat.normalize
    expected = [0.0, 0.267, 0.534, 0.801]
    expected.each_with_index { |x, i|
      assert_in_delta(x, m[i][0], 0.001)
    }

    minf = mat.normalize(1, 0, CV_NORM_INF)
    expected = [0.0, 0.333, 0.666, 1.0]
    expected.each_with_index { |x, i|
      assert_in_delta(x, minf[i][0], 0.001)
    }

    ml1 = mat.normalize(1, 0, CV_NORM_L1)
    expected = [0.0, 0.166, 0.333, 0.5]
    expected.each_with_index { |x, i|
      assert_in_delta(x, ml1[i][0], 0.001)
    }

    ml2 = mat.normalize(1, 0, CV_NORM_L2)
    expected = [0.0, 0.267, 0.534, 0.801]
    expected.each_with_index { |x, i|
      assert_in_delta(x, ml2[i][0], 0.001)
    }

    mminmax = mat.normalize(10, 5, CV_NORM_MINMAX)
    expected = [5.0, 6.666, 8.333, 10.0]
    expected.each_with_index { |x, i|
      assert_in_delta(x, mminmax[i][0], 0.001)
    }

    minf = mat.normalize(1, 0, CV_NORM_INF, CV_32FC3)
    expected = [0.0, 0.333, 0.666, 1.0]
    expected.each_with_index { |x, i|
      assert_in_delta(x, minf[i][0], 0.001)
    }

    mask = mat.to_8u.zero
    mask[0, 0] = CvScalar.new(255, 0, 0)
    mask[1, 0] = CvScalar.new(255, 0, 0)
    minf = mat.normalize(1, 0, CV_NORM_INF, -1, mask)
    expected = [0.0, 0.0, 1.0, 0.0]
    expected.each_with_index { |x, i|
      assert_in_delta(x, minf[i][0], 0.001)
    }

    assert_raise(TypeError) {
      mat.normalize(DUMMY_OBJ, 0, CV_NORM_INF)
    }
    assert_raise(TypeError) {
      mat.normalize(1, DUMMY_OBJ, CV_NORM_INF)
    }
    assert_raise(TypeError) {
      mat.normalize(1, 0, DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      mat.normalize(1, 0, CV_NORM_INF, DUMMY_OBJ)
    }
  end

  def test_count_non_zero
    m0 = create_cvmat(6, 4, :cv32f, 1) { |j, i, c|
      n = 0
      n = 1 if i == 0
      CvScalar.new(n, 0, 0, 0)
    }
    assert_equal(6, m0.count_non_zero)
  end

  def test_sum
    m0 = create_cvmat(6, 4, :cv32f, 1) { |j, i, c|
      CvScalar.new(c, c, c, c)
    }
    assert_cvscalar_equal(CvScalar.new(276, 0, 0, 0), m0.sum)

    m0 = create_cvmat(6, 4, :cv32f, 1) { |j, i, c|
      CvScalar.new(-c)
    }
    assert_cvscalar_equal(CvScalar.new(-276, 0, 0, 0), m0.sum)
  end

  def test_avg_sdv
    m0 = create_cvmat(6, 4, :cv32f, 4) { |j, i, c|
      CvScalar.new(c * 0.1, -c * 0.1, c, -c)
    }
    # CvMat#avg
    assert_in_delta(CvScalar.new(1.15, -1.15, 11.5, -11.5), m0.avg, 0.001)
    # CvMat#sdv
    assert_in_delta(CvScalar.new(0.69221, 0.69221, 6.9221, 6.9221), m0.sdv, 0.001)
    # CvMat#avg_sdv
    avg, sdv = m0.avg_sdv
    assert_in_delta(CvScalar.new(1.15, -1.15, 11.5, -11.5), avg, 0.001)
    assert_in_delta(CvScalar.new(0.69221, 0.69221, 6.9221, 6.9221), sdv, 0.001)

    mask = create_cvmat(6, 4, :cv8u, 1) { |j, i, c|
      n = (i == j) ? 1 : 0
      CvScalar.new(n)
    }
    # CvMat#avg
    assert_in_delta(CvScalar.new(0.75, -0.75, 7.5, -7.5), m0.avg(mask), 0.001)
    # CvMat#sdv
    assert_in_delta(CvScalar.new(0.55901, 0.55901, 5.5901, 5.5901), m0.sdv(mask), 0.001)
    # CvMat#avg_sdv
    avg, sdv = m0.avg_sdv(mask)
    assert_in_delta(CvScalar.new(0.75, -0.75, 7.5, -7.5), avg, 0.001)
    assert_in_delta(CvScalar.new(0.55901, 0.55901, 5.5901, 5.5901), sdv, 0.001)

    assert_raise(TypeError) {
      m0.avg(DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      m0.sdv(DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      m0.avg_sdv(DUMMY_OBJ)
    }
  end

  def test_min_max_loc
    m0 = create_cvmat(6, 4, :cv32f, 1) { |j, i, c|
      CvScalar.new(c * 0.5)
    }
    m0[2, 3] = CvScalar.new(100.5) # Max
    m0[5, 1] = CvScalar.new(-100.5) # Min

    min_val, max_val, min_loc, max_loc = m0.min_max_loc
    assert_equal(-100.5, min_val)
    assert_equal(5, min_loc.y)
    assert_equal(1, min_loc.x)
    assert_equal(100.5, max_val)
    assert_equal(2, max_loc.y)
    assert_equal(3, max_loc.x)

    assert_raise(TypeError) {
      m0.min_max_loc(DUMMY_OBJ)
    }
  end

  def test_norm
    src1 = CvMat.new(3, 3, :cv32f, 1).set_data([1, 2, 3, 4, 5, 6, 7, 8, 9])
    src2 = CvMat.new(3, 3, :cv32f, 1).set_data([2, 3, 4, 5, 6, 7, 8, 9, 1])
    mask = CvMat.new(3, 3, :cv8u, 1).set_data([1, 1, 0, 1, 1, 0, 0, 0, 0])

    assert_in_delta(CvMat.norm(src1), 16.88, 0.01)

    assert_in_delta(CvMat.norm(src1, nil, CV_NORM_L1), 45.0, 0.01)
    assert_in_delta(CvMat.norm(src1, nil, CV_NORM_L2), 16.88, 0.01)
    assert_in_delta(CvMat.norm(src1, nil, CV_NORM_INF), 9.0, 0.01)

    assert_in_delta(CvMat.norm(src1, src2, CV_NORM_L1), 16.0, 0.01)
    assert_in_delta(CvMat.norm(src1, src2, CV_NORM_L2), 8.49, 0.01)
    assert_in_delta(CvMat.norm(src1, src2, CV_NORM_INF), 8.0, 0.01)

    assert_in_delta(CvMat.norm(src1, src2, CV_NORM_L1, mask), 4.0, 0.01)
    assert_in_delta(CvMat.norm(src1, src2, CV_NORM_L2, mask), 2.0, 0.01)
    assert_in_delta(CvMat.norm(src1, src2, CV_NORM_INF, mask), 1.0, 0.01)

    assert_raise(TypeError) {
      CvMat.norm(DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      CvMat.norm(src1, DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      CvMat.norm(src1, src2, DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      CvMat.norm(src1, src2, CV_NORM_L1, DUMMY_OBJ)
    }
  end

  def test_dot_product
    m1 = create_cvmat(2, 2, :cv32f, 1) { |j, i, c|
      CvScalar.new(c * 0.5)
    }
    m2 = create_cvmat(2, 2, :cv32f, 1) { |j, i, c|
      CvScalar.new(c * 1.5)
    }
    assert_in_delta(10.5, m1.dot_product(m2), 0.001)

    m1 = create_cvmat(2, 2, :cv32f) { |j, i, c|
      CvScalar.new(c * 0.5, c * 0.6, c * 0.7, c * 0.8)
    }
    m2 = create_cvmat(2, 2, :cv32f) { |j, i, c|
      CvScalar.new(c * 1.5, c * 2.0, c * 2.5, c * 3.0)
    }
    assert_in_delta(85.39999, m1.dot_product(m2), 0.001)

    assert_raise(TypeError) {
      m1.dot_product(DUMMY_OBJ)
    }
  end

  def test_cross_product
    m1 = create_cvmat(1, 3, :cv32f, 1) { |j, i, c|
      CvScalar.new(c * 0.5)
    }
    m2 = create_cvmat(1, 3, :cv32f, 1) { |j, i, c|
      CvScalar.new(c + 1)
    }
    m3 = m1.cross_product(m2)

    assert_in_delta(CvScalar.new(-0.5), m3[0, 0], 0.001)
    assert_in_delta(CvScalar.new(1), m3[0, 1], 0.001)
    assert_in_delta(CvScalar.new(-0.5), m3[0, 2], 0.001)

    assert_raise(TypeError) {
      m1.cross_product(DUMMY_OBJ)
    }
  end

  def test_transform
    m0 = create_cvmat(5, 5, :cv32f, 3) { |j, i, c|
      CvScalar.new(c * 0.5, c * 1.0, c * 1.5)
    }
    transmat = CvMat.new(3, 3, :cv32f, 1);
    transmat[0, 0] = CvScalar.new(0.0)
    transmat[1, 0] = CvScalar.new(0.0)
    transmat[2, 0] = CvScalar.new(0.0)

    transmat[0, 1] = CvScalar.new(0.0)
    transmat[1, 1] = CvScalar.new(0.0)
    transmat[2, 1] = CvScalar.new(1.0)

    transmat[0, 2] = CvScalar.new(1.0)
    transmat[1, 2] = CvScalar.new(0.0)
    transmat[2, 2] = CvScalar.new(0.0)

    m1 = m0.transform(transmat)
    assert_each_cvscalar(m1, 0.01) { |j, i, c|
      CvScalar.new(c * 1.5, 0, c, 0)
    }

    stf = CvMat.new(3, 1, :cv32f, 1)
    stf[0, 0] = CvScalar.new(-10)
    stf[1, 0] = CvScalar.new(0.0)
    stf[2, 0] = CvScalar.new(5)

    m1 = m0.transform(transmat, stf)
    assert_each_cvscalar(m1, 0.01) { |j, i, c|
      CvScalar.new(c * 1.5 - 10, 0, c + 5, 0)
    }

    assert_raise(TypeError) {
      m0.transform(DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      m0.transform(transmat, DUMMY_OBJ)
    }
  end

  def test_perspective_transform
    mat = CvMat.new(1, 1, :cv32f, 2)
    mat[0] = CvScalar.new(2, 3)
    transmat = CvMat.new(3, 3, :cv32f, 1).clear
    mat.channel.times { |c|
      transmat[c, c] = CvScalar.new(1.0)
    }
    transmat[2, 2] = CvScalar.new(0.5)

    m = mat.perspective_transform(transmat)
    assert_equal(1, m.height)
    assert_equal(1, m.width)
    assert_equal(:cv32f, m.depth)
    assert_equal(2, m.channel)
    assert_in_delta(CvScalar.new(4, 6), m[0], 0.001);

    mat = CvMat.new(1, 1, :cv32f, 3)
    mat[0] = CvScalar.new(2, 3, 4)
    transmat = CvMat.new(4, 4, :cv32f, 1).clear
    mat.channel.times { |c|
      transmat[c, c] = CvScalar.new(1.0)
    }
    transmat[3, 3] = CvScalar.new(0.5)

    m = mat.perspective_transform(transmat)
    assert_equal(1, m.height)
    assert_equal(1, m.width)
    assert_equal(:cv32f, m.depth)
    assert_equal(3, m.channel)
    assert_in_delta(CvScalar.new(4, 6, 8), m[0], 0.001);

    assert_raise(TypeError) {
      mat.perspective_transform(DUMMY_OBJ)
    }
    assert_raise(CvStsAssert) {
      mat.perspective_transform(CvMat.new(3, 3, :cv32f, 3))
    }
  end

  def test_mul_transposed
    mat0 = create_cvmat(2, 2, :cv32f, 1) { |j, i, c|
      CvScalar.new((c + 1) * 2)
    }
    delta = create_cvmat(2, 2, :cv32f, 1) { |j, i, c|
      CvScalar.new(c + 1)
    }

    [mat0.mul_transposed,
     mat0.mul_transposed(:delta => nil),
     mat0.mul_transposed(:order => 0),
     mat0.mul_transposed(:scale => 1.0)].each { |m|
      expected = [20, 44,
                  44, 100]
      assert_equal(2, m.rows)
      assert_equal(2, m.cols)
      assert_equal(:cv32f, m.depth)
      expected.each_with_index { |x, i|
        assert_in_delta(x, m[i][0], 0.1)
      }
    }

    m = mat0.mul_transposed(:delta => delta)
    expected = [5, 11,
                11, 25]
    assert_equal(2, m.rows)
    assert_equal(2, m.cols)
    assert_equal(:cv32f, m.depth)
    expected.each_with_index { |x, i|
      assert_in_delta(x, m[i][0], 0.1)
    }

    m = mat0.mul_transposed(:delta => delta, :order => 1, :scale => 2.0)
    expected = [20, 28,
                28, 40]
    assert_equal(2, m.rows)
    assert_equal(2, m.cols)
    assert_equal(:cv32f, m.depth)
    expected.each_with_index { |x, i|
      assert_in_delta(x, m[i][0], 0.1)
    }
  end

  def test_trace
    m0 = create_cvmat(5, 5, :cv32f, 4) { |j, i, c|
      CvScalar.new(c * 0.5, c * 1.0, c * 1.5, c * 2.0)
    }
    assert_in_delta(CvScalar.new(30, 60, 90, 120), m0.trace, 0.001)
  end

  def test_transpose
    m0 = create_cvmat(2, 3, :cv32f, 4) { |j, i, c|
      CvScalar.new(c * 0.5, c * 1.0, c * 1.5, c * 2.0)
    }
    m1 = m0.transpose
    m2 = m0.t

    [m1, m2].each { |m|
      assert_equal(m0.rows, m.cols)
      assert_equal(m0.cols, m.rows)
      assert_each_cvscalar(m, 0.001) { |j, i, c|
        m0[i, j]
      }
    }
  end

  def test_det
    elems = [2.5, 4.5, 2.0,
             3.0, 2.5, -0.5,
             1.0, 0.5, 1.5] 
    m0 = create_cvmat(3, 3, :cv32f, 1) { |j, i, c|
      CvScalar.new(elems[c])
    }
    assert_in_delta(-14.5, m0.det, 0.001)
  end

  def test_invert
    elems = [1, 2, 3,
             2, 6, 9,
             1, 4, 7]
    m0 = create_cvmat(3, 3, :cv32f, 1) { |j, i, c|
      CvScalar.new(elems[c])
    }
    m1 = m0.invert
    m2 = m0.invert(:lu)
    m3 = m0.invert(:svd)
    m4 = m0.invert(:svd_sym)
    m5 = m0.invert(:svd_symmetric)

    expected = [3, -1, 0, -2.5, 2, -1.5, 1, -1, 1]
    [m1, m2, m3].each { |m|
      assert_equal(m0.width, m.width)
      assert_equal(m0.height, m.height)
      assert_each_cvscalar(m, 0.001) { |j, i, c|
        CvScalar.new(expected[c])
      }
    }

    expected = [3, -1, 0, -1.0, 0.15, 0.23, 0, 0.23, -0.15]
    [m4, m5].each { |m|
      assert_equal(m0.width, m.width)
      assert_equal(m0.height, m.height)
      assert_each_cvscalar(m, 0.1) { |j, i, c|
        CvScalar.new(expected[c])
      }
    }

    assert_raise(TypeError) {
      m0.invert(DUMMY_OBJ)
    }
  end

  def test_solve
    elems1 = [3, 4, 5,
              8, 9, 6,
              3, 5, 9]
    elems2 = [3,
              4,
              5]
    a = create_cvmat(3, 3, :cv32f, 1) { |j, i, c|
      CvScalar.new(elems1[c])
    }
    b = create_cvmat(3, 1, :cv32f, 1) { |j, i, c|
      CvScalar.new(elems2[c])
    }

    m1 = CvMat.solve(a, b)
    m2 = CvMat.solve(a, b, :lu)
    m3 = CvMat.solve(a, b, :svd)
    m4 = CvMat.solve(a, b, :svd_sym)
    m5 = CvMat.solve(a, b, :svd_symmetric)
    expected = [2, -2, 1]
    [m1, m2, m3].each { |m|
      assert_equal(b.width, m.width)
      assert_equal(a.height, m.height)
      assert_each_cvscalar(m, 0.001) { |j, i, c|
        CvScalar.new(expected[c])
      }
    }

    assert_raise(TypeError) {
      CvMat.solve(DUMMY_OBJ, b)
    }
    assert_raise(TypeError) {
      CvMat.solve(a, DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      CvMat.solve(a, b, DUMMY_OBJ)
    }
  end

  def test_svd
    rows = 2
    cols = 3
    m0 = create_cvmat(rows, cols, :cv32f, 1) { |j, i, c|
      CvScalar.new(c + 1)
    }

    [m0.svd, m0.clone.svd(CV_SVD_MODIFY_A)].each { |w, u, v|
      expected = [0.38632, -0.92237,
                  0.92237, 0.38632]
      assert_equal(rows, u.rows)
      assert_equal(rows, u.cols)
      expected.each_with_index { |x, i|
        assert_in_delta(x, u[i][0], 0.0001)
      }

      assert_equal(rows, w.rows)
      assert_equal(cols, w.cols)
      expected = [9.50803, 0, 0,
                  0, 0.77287, 0]
      expected.each_with_index { |x, i|
        assert_in_delta(x, w[i][0], 0.0001)
      }

      assert_equal(cols, v.rows)
      assert_equal(rows, v.cols)
      expected = [0.42867, 0.80596,
                  0.56631, 0.11238,
                  0.70395, -0.58120]
      
      expected.each_with_index { |x, i|
        assert_in_delta(x, v[i][0], 0.0001)
      }
    }

    w, ut, v = m0.svd(CV_SVD_U_T)
    expected = [0.38632, 0.92237,
                -0.92237, 0.38632]
    assert_equal(rows, ut.rows)
    assert_equal(rows, ut.cols)
    expected.each_with_index { |x, i|
      assert_in_delta(x, ut[i][0], 0.0001)
    }

    assert_equal(rows, w.rows)
    assert_equal(cols, w.cols)
    expected = [9.50803, 0, 0,
                0, 0.77287, 0]
    expected.each_with_index { |x, i|
      assert_in_delta(x, w[i][0], 0.0001)
    }

    assert_equal(cols, v.rows)
    assert_equal(rows, v.cols)
    expected = [0.42867, 0.80596,
                0.56631, 0.11238,
                0.70395, -0.58120]
    
    expected.each_with_index { |x, i|
      assert_in_delta(x, v[i][0], 0.0001)
    }

    w, u, vt = m0.svd(CV_SVD_V_T)
    expected = [0.38632, -0.92237,
                0.92237, 0.38632]
    assert_equal(rows, u.rows)
    assert_equal(rows, u.cols)
    expected.each_with_index { |x, i|
      assert_in_delta(x, u[i][0], 0.0001)
    }

    assert_equal(rows, w.rows)
    assert_equal(cols, w.cols)
    expected = [9.50803, 0, 0,
                0, 0.77287, 0]
    expected.each_with_index { |x, i|
      assert_in_delta(x, w[i][0], 0.0001)
    }

    assert_equal(rows, vt.rows)
    assert_equal(cols, vt.cols)
    expected = [0.42867, 0.56631, 0.70395,
                0.80596, 0.11238, -0.58120]
    expected.each_with_index { |x, i|
      assert_in_delta(x, vt[i][0], 0.0001)
    }
  end

  def test_eigenvv
    elems = [6, -2, -3, 7]
    m0 = create_cvmat(2, 2, :cv32f, 1) { |j, i, c|
      CvScalar.new(elems[c])
    }

    v1 = m0.eigenvv
    v2 = m0.eigenvv(10 ** -15)
    v3 = m0.eigenvv(10 ** -15, 1, 1)

    [v1, v2].each { |vec, val|
      assert_in_delta(-0.615, vec[0, 0][0], 0.01)
      assert_in_delta(0.788, vec[0, 1][0], 0.01)
      assert_in_delta(0.788, vec[1, 0][0], 0.01)
      assert_in_delta(0.615, vec[1, 1][0], 0.01)
      assert_in_delta(8.562, val[0][0], 0.01)
      assert_in_delta(4.438, val[1][0], 0.01)
    }

    vec3, val3 = v3
    assert_in_delta(-0.615, vec3[0, 0][0], 0.01)
    assert_in_delta(0.788, vec3[0, 1][0], 0.01)
    assert_in_delta(8.562, val3[0][0], 0.01)

    assert_raise(TypeError) {
      m0.eigenvv(DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      m0.eigenvv(nil, DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      m0.eigenvv(nil, nil, DUMMY_OBJ)
    }
  end

  def test_find_homography
    # Nx2
    src = CvMat.new(4, 2, :cv32f, 1)
    dst = CvMat.new(4, 2, :cv32f, 1)

    # Nx3 (Homogeneous coordinates)
    src2 = CvMat.new(4, 3, :cv32f, 1)
    dst2 = CvMat.new(4, 3, :cv32f, 1)
    
    # Homography
    #   <src>     =>    <dst>
    # (0, 0)      =>  (50, 0)
    # (255, 0)    =>  (205, 0)
    # (255, 255)  =>  (255, 220)
    # (0, 255)    =>  (0, 275)
    [[0, 0], [255, 0], [255, 255], [0, 255]].each_with_index { |coord, i|
      src[i, 0] = coord[0]
      src[i, 1] = coord[1]

      src2[i, 0] = coord[0] * 2
      src2[i, 1] = coord[1] * 2
      src2[i, 2] = 2
    }
    [[50, 0], [205, 0], [255, 220], [0, 275]].each_with_index { |coord, i|
      dst[i, 0] = coord[0]
      dst[i, 1] = coord[1]

      dst2[i, 0] = coord[0] * 2
      dst2[i, 1] = coord[1] * 2
      dst2[i, 2] = 2
    }

    mat1 = CvMat.find_homography(src, dst)
    mat2 = CvMat.find_homography(src, dst, :all)
    mat3 = CvMat.find_homography(src, dst, :ransac)
    mat4 = CvMat.find_homography(src, dst, :lmeds)
    mat5, status5 = CvMat.find_homography(src, dst, :ransac, 5, true)
    mat6, status6 = CvMat.find_homography(src, dst, :ransac, 5, true)
    mat7 = CvMat.find_homography(src, dst, :ransac, 5, false)
    mat8 = CvMat.find_homography(src, dst, :ransac, 5, nil)
    mat9 = CvMat.find_homography(src, dst, :all, 5, true)
    mat10, status10 = CvMat.find_homography(src2, dst2, :ransac, 5, true)

    [mat1, mat2, mat3, mat4, mat5, mat6, mat7, mat8, mat9, mat10].each { |mat|
      assert_equal(3, mat.rows)
      assert_equal(3, mat.cols)
      assert_equal(:cv32f, mat.depth)
      assert_equal(1, mat.channel)
      [0.72430, -0.19608, 50.0,
       0.0, 0.62489, 0.0,
       0.00057, -0.00165, 1.0].each_with_index { |x, i|
        assert_in_delta(x, mat[i][0], 0.0001)
      }
    }
    
    [status5, status6, status10].each { |status|
      assert_equal(1, status.rows)
      assert_equal(4, status.cols)
      assert_equal(:cv8u, status.depth)      
      assert_equal(1, status.channel)
      4.times { |i|
        assert_in_delta(1.0, status[i][0], 0.0001)
      }
    }

    assert_raise(TypeError) {
      CvMat.find_homography(DUMMY_OBJ, dst, :ransac, 5, true)
    }
    assert_raise(TypeError) {
      CvMat.find_homography(src, DUMMY_OBJ, :ransac, 5, true)
    }
    assert_raise(TypeError) {
      CvMat.find_homography(src, dst, DUMMY_OBJ, 5, true)
    }
    assert_raise(TypeError) {
      CvMat.find_homography(src, dst, :ransac, DUMMY_OBJ, true)
    }
    CvMat.find_homography(src, dst, :ransac, 5, DUMMY_OBJ)
  end

  def test_find_fundamental_mat
    points1 = [[488.362, 169.911],
               [449.488, 174.44],
               [408.565, 179.669],
               [364.512, 184.56],
               [491.483, 122.366],
               [451.512, 126.56],
               [409.502, 130.342],
               [365.5, 134.0],
               [494.335, 74.544],
               [453.5, 76.5],
               [411.646, 79.5901],
               [366.498, 81.6577],
               [453.5, 76.5],
               [411.646, 79.5901],
               [366.498, 81.6577]]
    
    points2 =  [[526.605, 213.332],
                [470.485, 207.632],
                [417.5, 201.0],
                [367.485, 195.632],
                [530.673, 156.417],
                [473.749, 151.39],
                [419.503, 146.656],
                [368.669, 142.565],
                [534.632, 97.5152],
                [475.84, 94.6777],
                [421.16, 90.3223],
                [368.5, 87.5],
                [475.84, 94.6777],
                [421.16, 90.3223],
                [368.5, 87.5]]

    # 7 point
    num_points = 7
    mat1 = CvMat.new(num_points, 2, :cv64f, 1)
    mat2 = CvMat.new(num_points, 2, :cv64f, 1)

    points1[0...num_points].each_with_index { |pt, i|
      mat1[i, 0] = CvScalar.new(pt[0])
      mat1[i, 1] = CvScalar.new(pt[1])
    }
    points2[0...num_points].each_with_index { |pt, i|
      mat2[i, 0] = CvScalar.new(pt[0])
      mat2[i, 1] = CvScalar.new(pt[1])
    }
    f_mat1 = CvMat.find_fundamental_mat(mat1, mat2, CV_FM_7POINT)
    f_mat2, status = CvMat.find_fundamental_mat(mat1, mat2, CV_FM_7POINT, :with_status => true)

    expected = [0.000009, 0.000029, -0.010343,
                -0.000033, 0.000000, 0.014590,
                0.004415, -0.013420, 1.000000,
                0.000000, 0.000001, -0.000223,
                -0.000001, 0.000036, -0.005309,
                -0.000097, -0.006463, 1.000000,
                0.000002, 0.000005, -0.001621,
                -0.000005, 0.000031, -0.002559,
                0.000527, -0.007424, 1.000000]
    [f_mat1, f_mat2].each { |f_mat|
      assert_equal(9, f_mat.rows)
      assert_equal(3, f_mat.cols)
      expected.each_with_index { |val, i|
        assert_in_delta(val, f_mat[i][0], 1.0e-5)
      }
    }
    assert_equal(num_points, status.cols)
    num_points.times { |i|
      assert_in_delta(1, status[i][0], 1.0e-5)
    }

    # 8 point
    num_points = 8
    mat1 = CvMat.new(num_points, 2, :cv64f, 1)
    mat2 = CvMat.new(num_points, 2, :cv64f, 1)

    points1[0...num_points].each_with_index { |pt, i|
      mat1[i, 0] = CvScalar.new(pt[0])
      mat1[i, 1] = CvScalar.new(pt[1])
    }
    points2[0...num_points].each_with_index { |pt, i|
      mat2[i, 0] = CvScalar.new(pt[0])
      mat2[i, 1] = CvScalar.new(pt[1])
    }

    f_mat1 = CvMat.find_fundamental_mat(mat1, mat2, CV_FM_8POINT)
    f_mat2, status = CvMat.find_fundamental_mat(mat1, mat2, CV_FM_8POINT, :with_status => true)

    expected = [0.000001, 0.000004, -0.001127,
                -0.000005, 0.000038, -0.003778,
                0.000819, -0.008325, 1.000000]
    [f_mat1, f_mat2].each { |f_mat|
      assert_equal(3, f_mat.rows)
      assert_equal(3, f_mat.cols)
      expected.each_with_index { |val, i|
        assert_in_delta(val, f_mat[i][0], 1.0e-5)
      }
    }
    assert_equal(num_points, status.cols)
    num_points.times { |i|
      assert_in_delta(1, status[i][0], 1.0e-5)
    }

    # RANSAC default
    num_points = points1.size
    mat1 = CvMat.new(num_points, 2, :cv64f, 1)
    mat2 = CvMat.new(num_points, 2, :cv64f, 1)

    points1[0...num_points].each_with_index { |pt, i|
      mat1[i, 0] = CvScalar.new(pt[0])
      mat1[i, 1] = CvScalar.new(pt[1])
    }
    points2[0...num_points].each_with_index { |pt, i|
      mat2[i, 0] = CvScalar.new(pt[0])
      mat2[i, 1] = CvScalar.new(pt[1])
    }

    [CvMat.find_fundamental_mat(mat1, mat2, CV_FM_RANSAC, :with_status => false,
                                :maximum_distance => 1.0, :desirable_level => 0.99),
     CvMat.find_fundamental_mat(mat1, mat2, CV_FM_RANSAC)].each { |f_mat|
      assert_equal(3, f_mat.rows)
      assert_equal(3, f_mat.cols)
      expected = [0.000010, 0.000039, -0.011141,
                  -0.000045, -0.000001, 0.019631,
                  0.004873, -0.017604, 1.000000]
      expected.each_with_index { |val, i|
        assert_in_delta(val, f_mat[i][0], 1.0e-5)
      }
    }
    
    # RANSAC with options
    f_mat, status = CvMat.find_fundamental_mat(mat1, mat2, CV_FM_RANSAC, :with_status => true,
                                               :maximum_distance => 2.0, :desirable_level => 0.8)
    assert_equal(3, f_mat.rows)
    assert_equal(3, f_mat.cols)
    assert_equal(1, status.rows)
    assert_equal(num_points, status.cols)

    expected_f_mat = [0.000009, 0.000030, -0.010692,
                      -0.000039, 0.000000, 0.020567,
                      0.004779, -0.018064, 1.000000]
    expected_f_mat.each_with_index { |val, i|
      assert_in_delta(val, f_mat[i][0], 1.0e-5)
    }
    expected_status = [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1]
    expected_status.each_with_index { |val, i|
      assert_in_delta(val, status[i][0], 1.0e-5)
    }

    # LMedS default
    num_points = 12
    mat1 = CvMat.new(num_points, 2, :cv64f, 1)
    mat2 = CvMat.new(num_points, 2, :cv64f, 1)

    points1[0...num_points].each_with_index { |pt, i|
      mat1[i, 0] = CvScalar.new(pt[0])
      mat1[i, 1] = CvScalar.new(pt[1])
    }
    points2[0...num_points].each_with_index { |pt, i|
      mat2[i, 0] = CvScalar.new(pt[0])
      mat2[i, 1] = CvScalar.new(pt[1])
    }

    [CvMat.find_fundamental_mat(mat1, mat2, CV_FM_LMEDS, :with_status => false,
                                :maximum_distance => 1.0, :desirable_level => 0.99),
     CvMat.find_fundamental_mat(mat1, mat2, CV_FM_LMEDS)].each { |f_mat|
      assert_equal(3, f_mat.rows)
      assert_equal(3, f_mat.cols)
      expected = [0.0, 0.0, 0.0,
                  0.0, 0.0, 0.0,
                  0.0, 0.0, 1.0]
      expected.each_with_index { |val, i|
        assert_in_delta(val, f_mat[i][0], 0.1)
      }
    }
    
    # LMedS with options
    f_mat, status = CvMat.find_fundamental_mat(mat1, mat2, CV_FM_LMEDS, :with_status => true,
                                               :desirable_level => 0.8)
    assert_equal(3, f_mat.rows)
    assert_equal(3, f_mat.cols)
    assert_equal(1, status.rows)
    assert_equal(num_points, status.cols)

    expected_fmat = [0.0, 0.0, 0.0,
                     0.0, 0.0, 0.0,
                     0.0, 0.0, 1.0]
    expected_f_mat.each_with_index { |val, i|
      assert_in_delta(val, f_mat[i][0], 0.1)
    }
    expected_status = [0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1]
    expected_status.each_with_index { |val, i|
      assert_equal(val, status[i][0].to_i)
    }

    [CV_FM_7POINT, CV_FM_8POINT, CV_FM_RANSAC, CV_FM_LMEDS].each { |method|
      assert_raise(TypeError) {
        CvMat.find_fundamental_mat(DUMMY_OBJ, mat2, method, :with_status => true)
      }
      assert_raise(TypeError) {
        CvMat.find_fundamental_mat(mat1, DUMMY_OBJ, method, :with_status => true)
      }
      assert_raise(TypeError) {
        CvMat.find_fundamental_mat(mat1, mat2, method, DUMMY_OBJ)
      }
    }
    assert_raise(TypeError) {
      CvMat.find_fundamental_mat(mat1, mat2, DUMMY_OBJ, :with_status => true)
    }
  end

  def test_compute_correspond_epilines
    test_func = lambda { |mat1, mat2, f_mat_arr, num_points|
      f_mat = CvMat.new(3, 3, CV_64F, 1)
      f_mat_arr.each_with_index { |a, i|
        f_mat[i] = CvScalar.new(a)
      }
      
      line = CvMat.compute_correspond_epilines(mat1, 1, f_mat)
      assert_equal(num_points, line.rows)
      assert_equal(3, line.cols)
      
      expected = [[-0.221257, -0.975215, 6.03758],
                  [0.359337, -0.933208, -3.61419],
                  [0.958304, -0.28575, -15.0573],
                  [0.73415, -0.678987, -10.4037],
                  [0.0208539, -0.999783, 2.11625],
                  [0.284451, -0.958691, -2.31993],
                  [0.624647, -0.780907, -8.35208],
                  [0.618494, -0.785789, -8.23888],
                  [0.766694, -0.642012, -11.0298],
                  [0.700293, -0.713855, -9.76109]]
      
      expected.size.times { |i|
        assert_in_delta(expected[i][0], line[i, 0][0], 1.0e-3)
        assert_in_delta(expected[i][1], line[i, 1][0], 1.0e-3)
        assert_in_delta(expected[i][2], line[i, 2][0], 1.0e-3)
      }

      assert_raise(ArgumentError) {
        m = CvMat.new(10, 10, CV_32F, 1)
        CvMat.compute_correspond_epilines(m, 1, f_mat)
      }
    }

    num_points = 10
    # input points are Nx2 matrix
    points1 =[[17, 175],
              [370, 24],
              [192, 456],
              [614, 202],
              [116, 111],
              [305, 32],
              [249, 268],
              [464, 157],
              [259, 333],
              [460, 224]]

    points2 = [[295, 28],
               [584, 221],
               [67, 172],
               [400, 443],
               [330, 9],
               [480, 140],
               [181, 140],
               [350, 265],
               [176, 193],
               [333, 313]]

    mat1 = CvMat.new(num_points, 2, CV_64F, 1)
    mat2 = CvMat.new(num_points, 2, CV_64F, 1)
    points1.flatten.each_with_index { |pt, i|
      mat1[i] = CvScalar.new(pt)
    }
    points2.flatten.each_with_index { |pt, i|
      mat2[i] = CvScalar.new(pt)
    }

    # pre computed f matrix from points1, points2
    # f_mat = CvMat.find_fundamental_mat(mat1, mat2, CV_FM_LMEDS)
    f_mat_arr = [0.000266883, 0.000140277, -0.0445223,
                 -0.00012592, 0.000245543, -0.108868,
                 -0.00407942, -0.00291097, 1]
    test_func.call(mat1, mat2, f_mat_arr, num_points)

    # input points are 2xN matrix
    points1 = [[17, 370, 192, 614, 116, 305, 249, 464, 259, 460],
               [175, 24, 456, 202, 111, 32, 268, 157, 333, 224]]

    points2 = [[295, 584, 67, 400, 330, 480, 181, 350, 176, 333],
               [28, 221, 172, 443, 9, 140, 140, 265, 193, 313]]

    mat1 = CvMat.new(2, num_points, CV_64F, 1)
    mat2 = CvMat.new(2, num_points, CV_64F, 1)
    points1.flatten.each_with_index { |pt, i|
      mat1[i] = CvScalar.new(pt)
    }
    points2.flatten.each_with_index { |pt, i|
      mat2[i] = CvScalar.new(pt)
    }
    test_func.call(mat1, mat2, f_mat_arr, num_points)


    f_mat = CvMat.new(3, 3, CV_64F, 1)
    f_mat_arr.each_with_index { |a, i|
      f_mat[i] = CvScalar.new(a)
    }
    assert_raise(TypeError) {
      CvMat.compute_correspond_epilines(DUMMY_OBJ, 1, f_mat)
    }
    assert_raise(TypeError) {
      CvMat.compute_correspond_epilines(mat1, DUMMY_OBJ, f_mat)
    }
    assert_raise(TypeError) {
      CvMat.compute_correspond_epilines(mat1, 1, DUMMY_OBJ)
    }
  end

  def test_apply_color_map
    mat = CvMat.new(64, 256, :cv8u, 1)
    mat.cols.times { |c|
      mat.rows.times { |r|
        mat[r, c] = c
      }
    }

    results = []
    [COLORMAP_AUTUMN, COLORMAP_BONE, COLORMAP_JET, COLORMAP_WINTER,
     COLORMAP_RAINBOW, COLORMAP_OCEAN, COLORMAP_SUMMER, COLORMAP_SPRING,
     COLORMAP_COOL, COLORMAP_HSV, COLORMAP_PINK, COLORMAP_HOT].each { |colormap|
      cmap = mat.apply_color_map(colormap)
      assert_equal(CvMat, cmap.class)
      assert_equal(mat.rows, cmap.rows)
      assert_equal(mat.cols, cmap.cols)
      results << cmap
    }

    assert_raise(TypeError) {
      mat.apply_color_map(DUMMY_OBJ)
    }

    # Uncomment the following line to show the result
    # snap *results
  end

  def test_subspace_project
    w = CvMat.new(10, 20, :cv32f, 1)
    mean = CvMat.new(w.rows, 1, :cv32f, 1)
    mat = CvMat.new(w.cols, w.rows, :cv32f, 1)
    result = mat.subspace_project(w, mean)

    assert_equal(CvMat, result.class)
    assert_equal(w.cols, result.rows)
    assert_equal(w.cols, result.cols)
  end

  def test_subspace_reconstruct
    w = CvMat.new(10, 20, :cv32f, 1)
    mean = CvMat.new(w.rows, 1, :cv32f, 1)
    mat = CvMat.new(w.cols, w.cols, :cv32f, 1)
    result = mat.subspace_reconstruct(w, mean)

    assert_equal(CvMat, result.class)
    assert_equal(w.cols, result.rows)
    assert_equal(w.rows, result.cols)
  end
end

