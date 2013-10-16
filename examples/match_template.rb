#!/usr/bin/env ruby

# A demo of Ruby/OpenCV's match_template function

require 'opencv'
include OpenCV

puts 'This program demonstrates the match_template function'
puts 'Usage:'
puts "ruby #{__FILE__} <template_filename> <match_filename>"
puts

template_filename = (ARGV.size == 2) ? ARGV[0] : File.expand_path(File.dirname(__FILE__) + '/../test/samples/lena-eyes.jpg')
match_image_filename = (ARGV.size == 2) ? ARGV[1] : File.expand_path(File.dirname(__FILE__) + '/../test/samples/lena-inpaint.jpg')

template = CvMat.load(template_filename)
match_image = CvMat.load(match_image_filename)
result = match_image.match_template(template)

pt1 = result.min_max_loc[2] # minimum location
pt2 = CvPoint.new(pt1.x + template.width, pt1.y + template.height)
match_image.rectangle!(pt1, pt2, :color => CvColor::Black, :thickness => 3)

window = GUI::Window.new('Display window') # Create a window for display.
window.show(match_image) # Show our image inside it.
GUI::wait_key # Wait for a keystroke in the window.
