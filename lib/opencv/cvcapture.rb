module Cv
  class CvCapture < VideoCapture
    alias_method :query, :read
    alias_method :close, :release

    def self.open(value = nil)
      self.new(value)
    end
  end
end
