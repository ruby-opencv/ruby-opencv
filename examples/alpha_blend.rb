require 'opencv'

img1 = Cv::imread('images/lenna.jpg', -1)
img2 = Cv::imread('images/tiffany.jpg', -1)

window = Cv::Window.new('Alpha blend')
max = 100.0
val = max / 2.0
window.set_trackbar("Alpha", max, val) { |v|
  a = v.to_f / max
  window.show(Cv::add_weighted(img1, a, img2, 1.0 - a, 0))
}

window.show(Cv::add_weighted(img1, val / max, img2, 1.0 - val / max, 0))
Cv.wait_key
