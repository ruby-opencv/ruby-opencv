require 'opencv'

w = Cv::Window.new('Paint')
canvas = Cv::Mat.new(500, 500, Cv::CV_8UC3).set_to(Cv::Scalar.new(255, 255, 255))
w.show(canvas)

# Drawing option
opt = {
  color: Cv::Scalar.new(0),
  tickness: 1,
  line_type: Cv::CV_AA
}

point = nil
w.set_mouse_callback { |event, x, y, flags|
  case event
  when Cv::EVENT_MOUSEMOVE
    if flags & Cv::EVENT_FLAG_LBUTTON > 0
      p2 = Cv::Point.new(x, y)
      canvas.line!(point, p2, opt[:color], opt) if point
      point = p2
    end
  when Cv::EVENT_LBUTTONDOWN
    point = Cv::Point.new(x, y)
    canvas.line!(point, point, opt[:color], opt)
  end
  w.show(canvas)
}

Cv::wait_key
