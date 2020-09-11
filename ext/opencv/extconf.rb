#!/usr/bin/env ruby

CC = RbConfig::CONFIG['CC']
if CC =~ /clang/
  RbConfig::MAKEFILE_CONFIG['try_header'] = :try_cpp
  RbConfig::CONFIG['CPP'] = "#{CC} -E"
elsif RbConfig::CONFIG['arch'] =~ /mswin32/
  RbConfig::MAKEFILE_CONFIG['try_header'] = :try_cpp
  RbConfig::CONFIG['CPP'] = "#{CC} /P"
end

require "mkmf"

def _cv_version_suffix(incdir)
  major, minor, subminor = nil, nil, nil
  open("#{incdir}/opencv2/core/version.hpp", 'r') { |f|
    f.read.lines.each { |line|
      major = $1.to_s if line =~ /\A#define\s+(?:CV_VERSION_EPOCH|CV_MAJOR_VERSION)\s+(\d+)\s*\Z/
      minor = $1.to_s if line =~ /\A#define\s+(?:CV_VERSION_MAJOR|CV_MINOR_VERSION)\s+(\d+)\s*\Z/
      subminor = $1.to_s if line =~ /\A#define\s+(?:CV_VERSION_MINOR|CV_SUBMINOR_VERSION)\s+(\d+)\s*\Z/
    }
  }
end

def cv_version_suffix(incdir)
  major, minor, subminor = _cv_version_suffix(incdir)
  major + minor + subminor
end

def cv_version_major(incdir)
  major, minor, subminor = _cv_version_suffix(incdir)
  major
end

# Quick fix for 2.0.0
# @libdir_basename is set to nil and dir_config() sets invalid libdir '${opencv-dir}/' when --with-opencv-dir option passed.
@libdir_basename ||= 'lib'
incdir, libdir = dir_config("opencv", "/usr/local/include", "/usr/local/lib")
dir_config("libxml2", "/usr/include", "/usr/lib")

if cv_version_suffix(incdir) == "3" 
  raise "not ready for ", cv_version_suffix(incdir), "yet\n"
end

opencv_headers = ["opencv2/core/core_c.h", "opencv2/core/core.hpp", "opencv2/imgproc/imgproc_c.h",
                  "opencv2/imgproc/imgproc.hpp", "opencv2/video/tracking.hpp", "opencv2/features2d/features2d.hpp",
                  "opencv2/flann/flann.hpp", "opencv2/calib3d/calib3d.hpp", "opencv2/objdetect/objdetect.hpp",
                  "opencv2/legacy/compat.hpp", "opencv2/legacy/legacy.hpp", "opencv2/highgui/highgui_c.h",
                  "opencv2/highgui/highgui.hpp", "opencv2/photo/photo.hpp"]
opencv_headers_opt = ["opencv2/nonfree/nonfree.hpp"]

opencv_libraries = ["opencv_calib3d", "opencv_contrib", "opencv_core", "opencv_features2d",
                    "opencv_flann", "opencv_highgui", "opencv_imgproc", "opencv_legacy",
                    "opencv_ml", "opencv_objdetect", "opencv_video", "opencv_photo"]
opencv_libraries_opt = ["opencv_gpu", "opencv_nonfree"]

puts ">> Check the required libraries..."
if $mswin or $mingw
  suffix = cv_version_suffix(incdir)
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
opencv_libraries_opt.each { |lib| warn "#{lib} not found." unless have_library(lib) }

# Check the required headers
puts ">> Check the required headers..."
opencv_headers.each { |header| raise "#{header} not found." unless have_header(header) }
opencv_headers_opt.each { |header| warn "#{header} not found." unless have_header(header) }
have_header("stdarg.h")

if $warnflags
  $warnflags.slice!('-Wdeclaration-after-statement')
  $warnflags.slice!('-Wimplicit-function-declaration')
end

# Quick fix for 1.8.7
$CFLAGS << " -I#{File.dirname(__FILE__)}/ext/opencv"

# Create Makefile
create_makefile('opencv')

