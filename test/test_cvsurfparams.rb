#!/usr/bin/env ruby
# -*- mode: ruby; coding: utf-8 -*-
require 'test/unit'
require 'opencv'
require File.expand_path(File.dirname(__FILE__)) + '/helper'

include OpenCV

# Tests for OpenCV::CvSURFParams
class TestCvSURFParams < OpenCVTestCase
  def setup
    omit_unless(IS_OPENCV2, 'CvSURFParams is not supported in OpenCV 3 or later')
    @surf_param1 = CvSURFParams.new(345.6)
  end

  def test_initialize
    sp1 = CvSURFParams.new(345.6)
    assert_equal(false, sp1.extended)
    assert_in_delta(345.6, sp1.hessian_threshold, 0.001)
    assert_equal(3, sp1.n_octaves)
    assert_equal(4, sp1.n_octave_layers)

    sp2 = CvSURFParams.new(456.7, true, 4, 5)
    assert_equal(true, sp2.extended)
    assert_in_delta(456.7, sp2.hessian_threshold, 0.001)
    assert_equal(4, sp2.n_octaves)
    assert_equal(5, sp2.n_octave_layers)
  end

  def test_extended
    assert_equal(false, @surf_param1.extended)

    @surf_param1.extended = true
    assert_equal(true, @surf_param1.extended)
  end

  def test_hessian_threshold
    assert_in_delta(345.6, @surf_param1.hessian_threshold, 0.001)

    @surf_param1.hessian_threshold = 456.7
    assert_in_delta(456.7, @surf_param1.hessian_threshold, 0.001)
  end

  def test_n_octaves
    assert_equal(3, @surf_param1.n_octaves)

    @surf_param1.n_octaves = 4
    assert_equal(4, @surf_param1.n_octaves)
  end

  def test_n_octave_layers
    assert_equal(4, @surf_param1.n_octave_layers)

    @surf_param1.n_octave_layers = 5
    assert_equal(5, @surf_param1.n_octave_layers)
  end
end

