#!/usr/bin/env ruby
#
# This file shows the different retrieval modes for contour detection
#
require "opencv"

# Load image
# The structure of the image is "explained" in bitmap-contours-with-labels.png
cvmat = OpenCV::CvMat.load("bitmap-contours.png")

# "find_contours" does only operate on bitmap images (black/white)
mat = OpenCV::CvMat.new(cvmat.rows, cvmat.cols, :cv8u, 1)
(cvmat.rows * cvmat.cols).times do |i|
  mat[i] = (cvmat[i][0] <= 128) ? OpenCV::CvScalar.new(0) : OpenCV::CvScalar.new(255)
end

# find_contours takes two parameters:
# 1. Retrieval mode (:mode, defines the structure of the contour sequence returned)
#    - CV_RETR_LIST (default)
#    - CV_RETR_EXTERNAL
#    - CV_RETR_CCOMP
#    - CV_RETR_TREE
# 2. Retrieval Method (:method, how the contours are approximated)
#    - CV_CHAIN_CODE
#    - CV_CHAIN_APPROX_NONE
#    - CV_CHAIN_APPROX_SIMPLE (default)
#    - CV_CHAIN_APPROX_TC89_L1
#    - CV_CHAIN_APPROX_T89_KCOS
#    - CV_LINK_RUNS

#
# The default: CV_RETR_LIST and CV_CHAIN_APPROX_SIMPLE
# This produces a flat list of contours that can be traversed with .h_next and .h_prev
#
puts "Detecting using CV_RETR_LIST and CV_CHAIN_APPROX_SIMPLE"
contour = mat.find_contours(:mode => OpenCV::CV_RETR_LIST, :method => OpenCV::CV_CHAIN_APPROX_SIMPLE)
cindex=1

while contour
  puts "Contour ##{cindex} is #{contour.contour_area} px^2 (width: #{contour.bounding_rect.width}, height: #{contour.bounding_rect.height}, type: #{(contour.hole?)?"hole":"contour"})"
  contour = contour.h_next
  cindex+=1
end 

#
# CV_RETR_EXTERNAL retrieves only the outer most non "hole" contour
#
puts '-'*80
puts "Detecting using CV_RETR_EXTERNAL and CV_CHAIN_APPROX_SIMPLE"
contour = mat.find_contours(:mode => OpenCV::CV_RETR_EXTERNAL, :method => OpenCV::CV_CHAIN_APPROX_SIMPLE)
cindex=1

while contour
  puts "Contour ##{cindex} is #{contour.contour_area} px^2 (width: #{contour.bounding_rect.width}, height: #{contour.bounding_rect.height}, type: #{(contour.hole?)?"hole":"contour"})"
  contour = contour.h_next
  cindex+=1
end 

#
# CV_RETR_CCOMP organizes the contours in a two level deep stack
# The first level holds the contours
# The second level contains the holes of the contours in level 1
#
# C00001 <-> C00000 <-> C000 <-> C0
#                         |     |
#                         V     V
#                       H0000   H00
#
puts '-'*80
puts "Detecting using CV_RETR_CCOMP and CV_CHAIN_APPROX_SIMPLE"
contour = mat.find_contours(:mode => OpenCV::CV_RETR_CCOMP, :method => OpenCV::CV_CHAIN_APPROX_SIMPLE)

# C00001
puts "Contour #1 is #{contour.contour_area} px^2 (width: #{contour.bounding_rect.width}, height: #{contour.bounding_rect.height}, type: #{(contour.hole?)?"hole":"contour"})"
contour = contour.h_next

# C00000
puts "Contour #2 is #{contour.contour_area} px^2 (width: #{contour.bounding_rect.width}, height: #{contour.bounding_rect.height}, type: #{(contour.hole?)?"hole":"contour"})"
contour = contour.h_next

# C000
puts "Contour #3 is #{contour.contour_area} px^2 (width: #{contour.bounding_rect.width}, height: #{contour.bounding_rect.height}, type: #{(contour.hole?)?"hole":"contour"})"
contour_down = contour.v_next

# H0000
puts "Contour #4 is #{contour_down.contour_area} px^2 (width: #{contour_down.bounding_rect.width}, height: #{contour_down.bounding_rect.height}, type: #{(contour_down.hole?)?"hole":"contour"})"
contour = contour.h_next

# C0
puts "Contour #5 is #{contour.contour_area} px^2 (width: #{contour.bounding_rect.width}, height: #{contour.bounding_rect.height}, type: #{(contour.hole?)?"hole":"contour"})"
contour_down = contour.v_next

# H00
puts "Contour #6 is #{contour_down.contour_area} px^2 (width: #{contour_down.bounding_rect.width}, height: #{contour_down.bounding_rect.height}, type: #{(contour_down.hole?)?"hole":"contour"})"

#
# CV_RETR_TREE manages the contours in a tree structure
# This reconstructs the complete hierarchy of contours and holes that the image displayed
#
# C0
#  |
#  V
# H00
#  |
#  V
# C000
#  |
#  V
# H0000-------+
#  |          |
#  V          V
# C00000   C00001
#
puts '-'*80
puts "Detecting using CV_RETR_TREE and CV_CHAIN_APPROX_SIMPLE"
contour = mat.find_contours(:mode => OpenCV::CV_RETR_TREE, :method => OpenCV::CV_CHAIN_APPROX_SIMPLE)

# C0
puts "Contour #1 is #{contour.contour_area} px^2 (width: #{contour.bounding_rect.width}, height: #{contour.bounding_rect.height}, type: #{(contour.hole?)?"hole":"contour"})"
contour = contour.v_next

# H00
puts "Contour #2 is #{contour.contour_area} px^2 (width: #{contour.bounding_rect.width}, height: #{contour.bounding_rect.height}, type: #{(contour.hole?)?"hole":"contour"})"
contour = contour.v_next

# C000
puts "Contour #3 is #{contour.contour_area} px^2 (width: #{contour.bounding_rect.width}, height: #{contour.bounding_rect.height}, type: #{(contour.hole?)?"hole":"contour"})"
contour = contour.v_next

# H0000
puts "Contour #4 is #{contour.contour_area} px^2 (width: #{contour.bounding_rect.width}, height: #{contour.bounding_rect.height}, type: #{(contour.hole?)?"hole":"contour"})"
contour = contour.v_next

# C00000
puts "Contour #5 is #{contour.contour_area} px^2 (width: #{contour.bounding_rect.width}, height: #{contour.bounding_rect.height}, type: #{(contour.hole?)?"hole":"contour"})"
contour_right = contour.h_next

# C00001
puts "Contour #6 is #{contour_right.contour_area} px^2 (width: #{contour_right.bounding_rect.width}, height: #{contour_right.bounding_rect.height}, type: #{(contour_right.hole?)?"hole":"contour"})"
