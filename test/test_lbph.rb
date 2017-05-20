#!/usr/bin/env ruby
# -*- mode: ruby; coding: utf-8-unix -*- 
require 'test/unit'
require 'opencv'
require 'date'
require File.expand_path(File.dirname(__FILE__)) + '/helper'

include OpenCV

# Tests for OpenCV::LBPH
class TestLBPH < OpenCVTestCase
  def setup
    omit_unless(IS_OPENCV2, 'LBPH is not supported in OpenCV 3 or later')
    @lbph = LBPH.new

    @lbph_trained = LBPH.new
    @lbph_update = LBPH.new
    @images = [CvMat.load(FILENAME_LENA256x256, CV_LOAD_IMAGE_GRAYSCALE)] * 2
    @labels = [1, 2]
    @lbph_trained.train(@images, @labels)
  end

  def test_initialize
    [LBPH.new, LBPH.new(1), LBPH.new(1, 2, 3, 4, 5.0)].each { |lbph|
      assert_equal(LBPH, lbph.class)
    }

    assert_raise(TypeError) {
      LBPH.new(DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      LBPH.new(1, DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      LBPH.new(1, 2, DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      LBPH.new(1, 2, 3, DUMMY_OBJ)
    }
    assert_raise(TypeError) {
      LBPH.new(1, 2, 3, 4, DUMMY_OBJ)
    }
  end

  def test_train
    assert_nil(@lbph.train(@images, @labels))

    assert_raise(TypeError) {
      @lbph.train(DUMMY_OBJ, @labels)
    }

    assert_raise(TypeError) {
      @lbph.train(@images, DUMMY_OBJ)
    }
  end

  def test_update
    assert_nil(@lbph_update.train([@images[0]], [@labels[0]]))
    assert_nil(@lbph_update.update([@images[1]], [@labels[1]]))

    assert_raise(TypeError) {
      @lbph_update.update(DUMMY_OBJ, @labels)
    }

    assert_raise(TypeError) {
      @lbph_update.update(@images, DUMMY_OBJ)
    }
  end

  def test_predict
    predicted_label, predicted_confidence = @lbph_trained.predict(@images[0])
    assert_equal(@labels[0], predicted_label)
    assert_in_delta(0.0, predicted_confidence, 0.01)

    assert_raise(TypeError) {
      @lbph_trained.predict(DUMMY_OBJ)
    }
  end

  def test_save
    filename = "lbph_save-#{DateTime.now.strftime('%Y%m%d%H%M%S')}.xml"
    begin
      @lbph_trained.save(filename)
      assert(File.exist? filename)
    ensure
      File.delete filename
    end
    assert_raise(TypeError) {
      @lbph_trained.save(DUMMY_OBJ)
    }
  end

  def test_load
    assert_nothing_raised {
      @lbph.load('lbph_save.xml')
    }
    assert_raise(TypeError) {
      @lbph.load(DUMMY_OBJ)
    }
  end

  def test_name
    assert_equal('FaceRecognizer.LBPH', @lbph.name)
  end

  def test_get_int
    assert_equal(1, @lbph.get_int('radius'))
    assert_equal(8, @lbph.get_int('neighbors'))
    assert_equal(8, @lbph.get_int('grid_x'))
    assert_equal(8, @lbph.get_int('grid_y'))

    assert_raise(TypeError) {
      @lbph.get_int(DUMMY_OBJ)
    }
  end

  def test_get_double
    assert_equal(Float::MAX, @lbph.get_double('threshold'))

    assert_raise(TypeError) {
      @lbph.get_double(DUMMY_OBJ)
    }
  end

  def test_get_matvector
    histgrams = @lbph_trained.get_matvector('histograms')
    assert_equal(Array, histgrams.class)
    assert_equal(2, histgrams.size)
    assert_equal(CvMat, histgrams[0].class)

    assert_raise(TypeError) {
      @lbph.get_matvector(DUMMY_OBJ)
    }
  end

  def test_set_int
    @lbph.set_int('radius', 2)
    @lbph.set_int('neighbors', 3)
    @lbph.set_int('grid_x', 4)
    @lbph.set_int('grid_y', 5)

    assert_equal(2, @lbph.get_int('radius'))
    assert_equal(3, @lbph.get_int('neighbors'))
    assert_equal(4, @lbph.get_int('grid_x'))
    assert_equal(5, @lbph.get_int('grid_y'))

    assert_raise(TypeError) {
      @lbph.set_int(DUMMY_OBJ, 1)
    }
    assert_raise(TypeError) {
      @lbph.set_int('radius', DUMMY_OBJ)
    }
  end

  def test_set_double
    @lbph.set_double('threshold', 1.0)
    assert_in_delta(1.0, @lbph.get_double('threshold'), 0.001)

    assert_raise(TypeError) {
      @lbph.set_double(DUMMY_OBJ, 1.0)
    }
    assert_raise(TypeError) {
      @lbph.set_double('threshold', DUMMY_OBJ)
    }
  end
end

