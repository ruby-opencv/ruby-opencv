# -*- mode: ruby; coding: utf-8 -*-
require 'bundler/gem_tasks'
require 'rake/extensiontask'
require 'yard'
require_relative 'yard_extension'

task :default => :spec

Rake::ExtensionTask.new('opencv') do |ext|
  ext.lib_dir = 'lib'
end

YARD::Rake::YardocTask.new do |t|
  t.files   = ['lib/**/*.rb', 'ext/opencv/*.cpp']
end
