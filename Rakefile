# -*- ruby -*-

require 'rubygems'
require 'bundler'
require 'hoe'

require 'rake/extensiontask'
require './ext/opencv/lib/opencv/version'

Hoe.plugin :gemspec

hoespec = Hoe.spec 'opencv'  do |p|
  p.version = OpenCV::VERSION
  p.changes = p.paragraphs_of('History.txt', 0..1).join("\n\n")
  p.description = <<EOF
OpenCV wrapper for Ruby
EOF
  p.rubyforge_name = 'opencv'
  p.developer('lsxi', 'masakazu.yonekura@gmail.com')
  p.developer('ser1zw', '')
  p.developer('pcting', 'pcting@gmail.com')

  p.need_tar = false
  p.need_zip = false
  p.readme_file  = 'README.rdoc'
  p.history_file = 'History.txt'
  p.spec_extras = {
    :extensions => %w{extconf.rb}
  }
  p.summary = 'OpenCV wrapper for Ruby.'
  p.test_globs = ['test/test_*.rb']
  p.clean_globs << 'lib/*.so' << 'tmp'

  p.url = 'http://blueruby.mydns.jp/opencv'

  p.extra_dev_deps << ['rake-compiler', '>= 0'] << ['hoe-gemspec'] << ['rspec']

  Rake::ExtensionTask.new('opencv', spec) do |ext|
    ext.lib_dir = File.join('lib', 'opencv')
  end

end

hoespec.spec.files.delete('.gemtest')
hoespec.spec.files.delete('ruby-opencv.gemspec')
hoespec.spec.files.delete('opencv.gemspec')
hoespec.spec.cert_chain = []
hoespec.spec.signing_key = nil

Rake::Task[:test].prerequisites << :compile

# vim: syntax=Ruby
