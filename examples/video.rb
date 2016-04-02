require 'opencv'

capture = Cv::VideoCapture.new
w = Cv::Window.new('video')

classifier = Cv::CascadeClassifier.new
classifier.load('haarcascade_frontalface_alt.xml')
color = Cv::Scalar.new(0, 255, 255)

loop do
  m = capture.read
  rects = classifier.detect_multi_scale(m)
  rects.each do |r|
    pt1 = Cv::Point.new(r.x, r.y)
    pt2 = Cv::Point.new(r.x + r.width, r.y + r.height)
    m.rectangle!(pt1, pt2, color, thickness: 3, line_type: Cv::CV_AA)
  end

  w.show(m)
  break if Cv::wait_key(10) > 0
end

