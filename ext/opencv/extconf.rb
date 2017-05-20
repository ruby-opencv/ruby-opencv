#!/usr/bin/env ruby

if RbConfig::CONFIG['CC'] =~ /gcc|clang/
  RbConfig::CONFIG['CPP'] = "#{RbConfig::CONFIG["CXX"]} -E"
elsif RbConfig::CONFIG['arch'] =~ /mswin32/
  RbConfig::CONFIG['CPP'] = "#{CC} /P"
end
RbConfig::MAKEFILE_CONFIG['try_header'] = :try_cpp

require "mkmf"

def cv_version(incdir)
  major, minor, subminor = nil, nil, nil
  src = open("#{incdir}/opencv2/core/version.hpp", 'r') { |f| f.read }
  # FIXME
  if src =~ /^#define\s+CV_VERSION_EPOCH/ # OpenCV 2
    src.lines.each { |line|
      major = $1 if line =~ /\A#define\s+(?:CV_VERSION_EPOCH)\s+(\d+)\s*\Z/
      minor = $1 if line =~ /\A#define\s+(?:CV_VERSION_MAJOR)\s+(\d+)\s*\Z/
      subminor = $1 if line =~ /\A#define\s+(?:CV_VERSION_MINOR)\s+(\d+)\s*\Z/
    }
  else # OpenCV 3
    src.lines.each { |line|
      major = $1 if line =~ /\A#define\s+(?:CV_VERSION_MAJOR)\s+(\d+)\s*\Z/
      minor = $1 if line =~ /\A#define\s+(?:CV_VERSION_MINOR)\s+(\d+)\s*\Z/
      subminor = $1 if line =~ /\A#define\s+(?:CV_VERSION_SUBMINOR)\s+(\d+)\s*\Z/
    }
  end
  { major: major.to_i, minor: minor.to_i, subminor: subminor.to_i }
end

incdir, libdir = dir_config("opencv", "/usr/local/include", "/usr/local/lib")
version = cv_version(incdir)

if version[:major] >= 3
  opencv_headers = ["opencv2/core.hpp", "opencv2/highgui.hpp", "opencv2/imgcodecs.hpp", "opencv2/imgproc.hpp",
                    "opencv2/objdetect.hpp", "opencv2/videoio.hpp", "opencv2/calib3d.hpp", "opencv2/photo.hpp"]
  opencv_libraries = ["opencv_core", "opencv_highgui", "opencv_imgcodecs", "opencv_imgproc", "opencv_objdetect",
                      "opencv_videoio", "opencv_calib3d", "opencv_photo"]
else
  opencv_headers = ["opencv2/core/core_c.h", "opencv2/core/core.hpp", "opencv2/imgproc/imgproc_c.h",
                    "opencv2/imgproc/imgproc.hpp", "opencv2/video/tracking.hpp", "opencv2/features2d/features2d.hpp",
                    "opencv2/flann/flann.hpp", "opencv2/calib3d/calib3d.hpp", "opencv2/objdetect/objdetect.hpp",
                    "opencv2/legacy/compat.hpp", "opencv2/legacy/legacy.hpp", "opencv2/highgui/highgui_c.h",
                    "opencv2/highgui/highgui.hpp", "opencv2/photo/photo.hpp",
                    "opencv2/nonfree/nonfree.hpp"]
  opencv_libraries = ["opencv_calib3d", "opencv_contrib", "opencv_core", "opencv_features2d",
                      "opencv_flann", "opencv_highgui", "opencv_imgproc", "opencv_legacy",
                      "opencv_ml", "opencv_objdetect", "opencv_video", "opencv_photo",
                      "opencv_gpu", "opencv_nonfree"]
  $defs << '-DIS_OPENCV2'
end

puts ">> Check the required libraries..."
if $mswin or $mingw
  suffix = [version[:major], version[:minor], version[:subminor]].map(&:to_s).join
  opencv_libraries.map! { |lib| lib + suffix }
  opencv_libraries_opt.map! { |lib| lib + suffix }
  have_library("msvcrt")
  if $mswin
    $CFLAGS << ' /EHsc'
    CONFIG['CXXFLAGS'] << ' /EHsc'
  end
else
  have_library("stdc++")
end

opencv_libraries.each { |lib| raise "#{lib} not found." unless have_library(lib) }

# Check the required headers
puts ">> Check the required headers..."

# TODO: #77 Check this in some OS/compilers
with_cflags("-x c++") do
  opencv_headers.each { |header| raise "#{header} not found." unless have_header(header) }
end

if $warnflags
  ['-Wdeclaration-after-statement', '-Wimplicit-function-declaration', '-Wimplicit-int',
   '-Wno-self-assign', '-Wno-constant-logical-operand', '-Wno-parentheses-equality',
   '-Wno-tautological-compare'].each { |opt|
    $warnflags.slice!(opt)
  }
  $warnflags.gsub!(/-Wsuggest-attribute=[^\s]+/, '')
end

# Create Makefile
create_makefile('opencv')
