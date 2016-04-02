require 'opencv'

m = Cv::imread('images/lenna.jpg', -1)
classifier = Cv::CascadeClassifier.new('haarcascade_frontalface_alt.xml')
rects = classifier.detect_multi_scale(m)
color = Cv::Scalar.new(0, 255, 255)
rects.each do |r|
  pt1 = Cv::Point.new(r.x, r.y)
  pt2 = Cv::Point.new(r.x + r.width, r.y + r.height)
  m.rectangle!(pt1, pt2, color, thickness: 3, line_type: Cv::CV_AA)
end

w = Cv::Window.new('Face detect')
w.show(m)
Cv::wait_key
