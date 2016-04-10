module Cv
  class IplImage < CvMat
    def initialize(width, height, depth = :cv8u, channel = 3)
      super(height, width, depth, channel)
    end
  end
end
