#!/usr/bin/env ruby
# face_detect.rb
require "rubygems"
require "opencv"

include OpenCV

window = GUI::Window.new("face detect")
capture = CvCapture.open
detector = CvHaarClassifierCascade::load("../test/samples/haarcascade_frontalface_alt.xml.gz")

loop {
  image = capture.query
  detector.detect_objects(image).each { |rect|
    image.rectangle! rect.top_left, rect.bottom_right, :color => CvColor::Red
  }
  window.show image
  break if GUI::wait_key(100)
}

