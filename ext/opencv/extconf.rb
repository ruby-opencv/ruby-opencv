#!/usr/bin/env ruby

if RbConfig::CONFIG['CC'] =~ /gcc|clang/
  RbConfig::CONFIG['CPP'] = "#{RbConfig::CONFIG["CXX"]} -E"
elsif RbConfig::CONFIG['arch'] =~ /mswin32/
  RbConfig::CONFIG['CPP'] = "#{CC} /P"
end
RbConfig::MAKEFILE_CONFIG['try_header'] = :try_cpp

require "mkmf"

def cv_version_suffix(incdir)
  major, minor, subminor = nil, nil, nil
  open("#{incdir}/opencv2/core/version.hpp", 'r') { |f|
    f.read.lines.each { |line|
      major = $1.to_s if line =~ /\A#define\s+(?:CV_VERSION_EPOCH|CV_MAJOR_VERSION)\s+(\d+)\s*\Z/
      minor = $1.to_s if line =~ /\A#define\s+(?:CV_VERSION_MAJOR|CV_MINOR_VERSION)\s+(\d+)\s*\Z/
      subminor = $1.to_s if line =~ /\A#define\s+(?:CV_VERSION_MINOR|CV_SUBMINOR_VERSION)\s+(\d+)\s*\Z/
    }
  }
  major + minor + subminor
end

incdir, libdir = dir_config("opencv", "/usr/local/include", "/usr/local/lib")

opencv_headers = ["opencv2/core.hpp", "opencv2/highgui.hpp", "opencv2/imgcodecs.hpp", "opencv2/imgproc.hpp", "opencv2/objdetect.hpp", "opencv2/videoio.hpp"]
opencv_libraries = ["opencv_core", "opencv_highgui", "opencv_imgcodecs", "opencv_imgproc", "opencv_objdetect", "opencv_videoio"]

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

# Check the required headers
puts ">> Check the required headers..."
opencv_headers.each { |header| raise "#{header} not found." unless have_header(header) }

if $warnflags
  $warnflags.slice!('-Wdeclaration-after-statement')
  $warnflags.slice!('-Wimplicit-function-declaration')
end

# Create Makefile
create_makefile('opencv')
