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

    puts '-' * 80
    puts "BOUNDING RECT FOUND"
    puts '-' * 80

    # You can detect the "bounding rectangle" which is always oriented horizontally and vertically
    box = contour.bounding_rect
    puts "found external contour with bounding rectangle from #{box.top_left.x},#{box.top_left.y} to #{box.bottom_right.x},#{box.bottom_right.y}"

    # The contour area can be computed:
    puts "that contour encloses an area of #{contour.contour_area} square pixels"

    # .. as can be the length of the contour
    puts "that contour is #{contour.arc_length} pixels long "

    # Draw that bounding rectangle
    cvmat.rectangle! box.top_left, box.bottom_right, :color => OpenCV::CvColor::Black

    # You can also detect the "minimal rectangle" which has an angle, width, height and center coordinates
    # and is not neccessarily horizonally or vertically aligned.
    # The corner of the rectangle with the lowest y and x position is the anchor (see image here: http://bit.ly/lT1XvB)
    # The zero angle position is always straight up. 
    # Positive angle values are clockwise and negative values counter clockwise (so -60 means 60 degree counter clockwise)
    box = contour.min_area_rect
    puts "found minimal rectangle with its center at (#{box.center.x.round},#{box.center.y.round}), width of #{box.size.width.round}px, height of #{box.size.height.round} and an angle of #{box.angle.round} degree"
  end
  contour = contour.h_next
end 

# And save the image
puts "\nSaving image with bounding rectangles"
cvmat.save_image("rotated-boxes-with-detected-bounding-rectangles.jpg")
