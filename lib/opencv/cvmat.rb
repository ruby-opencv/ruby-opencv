module Cv
  class CvMat < Mat
    @@depth_table = {
      cv8u: CV_8U,
      cv8s: CV_8S,
      cv16u: CV_16U,
      cv16s: CV_16S,
      cv32s: CV_32S,
      cv32f: CV_32F,
      cv64f: CV_64F
    }
    @@depth_table2 = @@depth_table.invert

    def initialize(rows, cols, depth = CV_8U, channels = 3)
      depth = @@depth_table[depth] if depth.is_a? Symbol
      type = OpenCV::CV_MAKETYPE(depth, channels)
      super(rows, cols, type)
    end

    def depth
      @@depth_table2[super]
    end

    def channel
      self.channels
    end

    def save_image(filename, params = {})
      raise TypeError unless params.is_a? Hash
      save(filename, params.to_a.flatten)
    end

    def line(p1, p2, options = {})
      color = options[:color] || Scalar.new
      super(p1, p2, color, options)
    end

    def line!(p1, p2, options = {})
      color = options[:color] || Scalar.new
      super(p1, p2, color, options)
    end

    def self.load(filename, iscolor = 1)
      CvMat::imread_as(filename, iscolor, self)
    end

    def encode(ext, options = {})
      raise TypeError unless options.is_a? Hash
      imencode(ext, options.to_a.flatten)
    end

    def self.decode(buf, iscolor = 1)
      Mat::imdecode_as(buf, iscolor, self)
    end

    def sobel(dx, dy, aperture_size = 3)
      depth = (self.depth == :cv8u) ? CV_16S : CV_32F
      super(depth, dx, dy, aperture_size)
    end

    def laplace(aperture_size = 3)
      depth = (self.depth == :cv8u) ? CV_16S : CV_32F
      laplacian(depth, aperture_size)
    end

    def convert_scale_abs(option = {})
      alpha = option[:scale] || 1.0
      beta = option[:shift] || 0
      super(alpha, beta)
    end

    def self.add_weighted(src1, alpha, src2, beta, gamma)
      OpenCV::add_weighted(src1, alpha, src2, beta, gamma)
    end

    # cv::cvtColor
    [:BGR2BGRA, :RGB2RGBA, :BGRA2BGR, :RGBA2RGB, :BGR2RGBA, :RGB2BGRA,
     :RGBA2BGR, :BGRA2RGB, :BGR2RGB, :RGB2BGR, :BGRA2RGBA, :RGBA2BGRA,
     :BGR2GRAY, :RGB2GRAY, :GRAY2BGR, :GRAY2RGB, :GRAY2BGRA, :GRAY2RGBA,
     :BGRA2GRAY, :RGBA2GRAY, :BGR2BGR565, :RGB2BGR565, :BGR5652BGR,
     :BGR5652RGB, :BGRA2BGR565, :RGBA2BGR565, :BGR5652BGRA, :BGR5652RGBA,
     :GRAY2BGR565, :BGR5652GRAY, :BGR2BGR555, :RGB2BGR555, :BGR5552BGR,
     :BGR5552RGB, :BGRA2BGR555, :RGBA2BGR555, :BGR5552BGRA, :BGR5552RGBA,
     :GRAY2BGR555, :BGR5552GRAY, :BGR2XYZ, :RGB2XYZ, :XYZ2BGR, :XYZ2RGB,
     :BGR2YCrCb, :RGB2YCrCb, :YCrCb2BGR, :YCrCb2RGB, :BGR2HSV, :RGB2HSV,
     :BGR2Lab, :RGB2Lab, :BayerBG2BGR, :BayerGB2BGR, :BayerRG2BGR, :BayerGR2BGR,
     :BayerBG2RGB, :BayerGB2RGB, :BayerRG2RGB, :BayerGR2RGB, :BGR2Luv, :RGB2Luv,
     :BGR2HLS, :RGB2HLS, :HSV2BGR, :HSV2RGB, :Lab2BGR, :Lab2RGB, :Luv2BGR, :Luv2RGB,
     :HLS2BGR, :HLS2RGB].each do |method_name|
      define_method method_name do
        cvt_color(eval("COLOR_#{method_name}"), 0)
      end
    end
  end
end
