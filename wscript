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
        conf.env.CXX = "/usr/local/bin/g++" #specify the cxx compiler.
        #conf.env.CXX = "/usr/bin/g++-5" #specify the cxx compiler.
        conf.load("compiler_cxx")
        #conf.env.append_unique('LIBPATH', ['/usr/lib', '/usr/local/lib', '/usr/local/lib64'])
        #conf.env.append_unique('LIBPATH_ST', ['/usr/lib', '/usr/local/lib', '/usr/local/lib64'])
        #conf.env.append_unique('LIBDIR', ['/usr/lib', '/usr/local/lib', '/usr/local/lib64'])
        #conf.env.append_unique('LIB_ST', ['stdc++'])
        print conf.env
	#conf.recurse('test')



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
