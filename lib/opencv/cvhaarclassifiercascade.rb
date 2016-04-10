module Cv
  CV_HAAR_DO_CANNY_PRUNING = 1

  class CvHaarClassifierCascade < CascadeClassifier
    def self.load(filename)
      new(filename)
    end

    def detect_objects(image, options = {})
      rects = detect_multi_scale(image, options)
      rects.each { |rect| yield rect } if block_given?
      rects
    end
  end
end
