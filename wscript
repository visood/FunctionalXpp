#! /usr/bin/env python
# encoding: utf-8

APPNAME = "FunctionalXpp"
VERSION = "0.0"

top = "."
out = "build"

#configuration methods

from waflib.Configure import conf

@conf
def hi(ctx):
	print("-> hello, world!, this is an example of a configuration method.")

def printWithContext(message, ctx, withContext = True):
	if not withContext:
		print message
	else:
		print (message + ". Context %d" %id(ctx))



def options(opt):
        opt.load("compiler_cxx")
	opt.recurse('test')


def configure(conf):
        from waflib.Tools.compiler_cxx import cxx_compiler
        conf.env.CXX = "/usr/bin/g++-6" #specify the cxx compiler.
        conf.load("compiler_cxx")
        print conf.env
	conf.check(header_name="stdio.h", features="cxx cxxprogram", mandatory=False)
	conf.recurse('test')


def describe(ctx):
	print "Eventually this project will contain experiments in Modern C++,\n\
with emphasis on  functional programming.\n\
The project will be built with Waf,\n\
which we begin to learn from the Waf book"
	print "environment is a " + str(type(ctx.env))
	try:
		print(ctx.env)
	except:
		print "No environment in Context"
	ctx.recurse('test')

def build(bld):
	bld.recurse('test')
