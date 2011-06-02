#!/usr/bin/env ruby
#
# Detects contours in an image and
# their boundingboxes
#
require "opencv"

# Load image
cvmat = OpenCV::CvMat.load("rotated-boxes.jpg")

# The "canny" edge-detector does only work with grayscale images
# so to be sure, convert the image
# otherwise you will get something like:
# terminate called after throwing an instance of 'cv::Exception'
#  what():  /opt/local/var/macports/build/_opt_local_var_macports_sources_rsync.macports.org_release_ports_graphics_opencv/work/OpenCV-2.2.0/modules/imgproc/src/canny.cpp:67: error: (-210)  in function cvCanny
cvmat = cvmat.BGR2GRAY

# Use the "canny" edge detection algorithm (http://en.wikipedia.org/wiki/Canny_edge_detector)
# Parameters are two colors that are then used to determine possible corners
canny = cvmat.canny(50, 150)

# Look for contours
# We want them to be returned as a flat list (CV_RETR_LIST) and simplified (CV_CHAIN_APPROX_SIMPLE)
# Both are the defaults but included here for clarity
contour = canny.find_contours(:mode => OpenCV::CV_RETR_LIST, :method => OpenCV::CV_CHAIN_APPROX_SIMPLE)

# The Canny Algorithm returns two matches for every contour (see O'Reilly: Learning OpenCV Page 235)
# We need only the external edges so we ignore holes.
# When there are no more contours, contours.h_next will return nil
while contour
  # No "holes" please (aka. internal contours)
  unless contour.hole?
    box = contour.bounding_rect
    puts "found external contour from #{box.top_left.x},#{box.top_left.y} to #{box.bottom_right.x},#{box.bottom_right.y}"

    # Draw that bounding rectangle
    cvmat.rectangle! box.top_left, box.bottom_right, :color => OpenCV::CvColor::Black
  end
  contour = contour.h_next
end 

# And save the image
puts "Saving image with bounding rectangles"
cvmat.save_image("rotated-boxes-with-detected-bounding-rectangles.jpg")
