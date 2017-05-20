#!/usr/bin/env ruby
# -*- mode: ruby; coding: utf-8-unix -*- 
require 'test/unit'
require 'opencv'
require 'date'
require File.expand_path(File.dirname(__FILE__)) + '/helper'

include OpenCV

# Tests for OpenCV::EigenFaces
class TestEigenFaces < OpenCVTestCase
  def setup
    omit_unless(IS_OPENCV2, 'EigenFaces is not supported in OpenCV 3 or later')
    @eigenfaces = EigenFaces.new

    @eigenfaces_trained = EigenFaces.new
    @images = [CvMat.load(FILENAME_LENA256x256, CV_LOAD_IMAGE_GRAYSCALE)] * 2
    @labels = [1, 2]
    @eigenfaces_trained.train(@images, @labels)
  end

  def test_initialize
    [EigenFaces.new, EigenFaces.new(1), EigenFaces.new(1, 99999)].each { |ef|
      assert_equal(EigenFaces, ef.class)
    }

    assert_raise(TypeError) {
      EigenFaces.new(DUMMY_OBJ)
    }

    assert_raise(TypeError) {
      EigenFaces.new(1, DUMMY_OBJ)
    }
  end

  def test_train
    assert_nil(@eigenfaces.train(@images, @labels))

    assert_raise(TypeError) {
      @eigenfaces.train(DUMMY_OBJ, @labels)
    }

    assert_raise(TypeError) {
      @eigenfaces.train(@images, DUMMY_OBJ)
    }
  end

  def test_predict
    predicted_label, predicted_confidence = @eigenfaces_trained.predict(@images[0])
    assert_equal(@labels[0], predicted_label)
    assert_in_delta(0.0, predicted_confidence, 0.01)

    assert_raise(TypeError) {
      @eigenfaces_trained.predict(DUMMY_OBJ)
    }
  end

  def test_save
    filename = "eigenfaces_save-#{DateTime.now.strftime('%Y%m%d%H%M%S')}.xml"
    begin
      @eigenfaces_trained.save(filename)
      assert(File.exist? filename)
    ensure
      File.delete filename
    end
    assert_raise(TypeError) {
      @eigenfaces_trained.save(DUMMY_OBJ)
    }
  end

  def test_load
    assert_nothing_raised {
      @eigenfaces_trained.load('eigenfaces_save.xml')
    }
    assert_raise(TypeError) {
      @eigenfaces_trained.load(DUMMY_OBJ)
    }
  end

  def test_name
    assert_equal('FaceRecognizer.Eigenfaces', @eigenfaces.name)
  end

  def test_get_mat
    mat = @eigenfaces_trained.get_mat('eigenvalues')
    assert_not_nil(mat)
    assert_equal(CvMat, mat.class)

    assert_raise(TypeError) {
      @eigenfaces_trained.get_mat(DUMMY_OBJ)
    }
  end
end

