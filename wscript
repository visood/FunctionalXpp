#! /usr/bin/env python
# encoding: utf-8

APPNAME = "FunctionalXpp"
VERSION = "0.0"

top = "."
out = "build"

DIRS = 'source test'

import glob, os

from waflib.Configure import conf

def global_env(ctx):
    ctx.env.appname = APPNAME
    ctx.env.append_unique('LDFLAGS_N',
                          ['rt', 'pthread', 'm', 'z',
                           'bz2', 'gsl', 'gslcblas',
                           'dl']
    )
    ctx.env.append_unique('CATCH_PATH', '/usr/local/include/Catch')
    ctx.env.append_unique("INCLUDES_REL",
                          ['include',
                           'include/containers',
                           'include/containers/iterators',
                           'include/database',
                           'include/utils'
                          ]
    )
    #ctx.env.INCLUDES_N = ["../../../" + x for x in ctx.env.INCLUDES_REL]
    ctx.env.append_unique('INCLUDES_ABS', 
                           ctx.env.CATCH_PATH[0]
    )

def configure_gcc(conf):
    #conf.find_program('g++', var = 'CXX', mandator = True)
    #conf.CXX = "g++"
    #conf.load('compiler_cxx')
    #conf.find_program('gcc', var = 'C', mandator = True)
    conf.C = "gcc"
    conf.load('compiler_c')
    global_env(conf)
    conf.env.append_unique('STLIB', 'stdc++')
    conf.env.append_unique('LDFLAGS_N', 'stdc++')
    conf.setenv('release', env=conf.env.derive())
    conf.env.CXXFLAGS = ['-Wall',
                         '-Wno-unknown-pragmas',
                         '-Wextra',
                         '-Wconversion',
                         '-O3',
                         '-std=c++17']
    conf.define('RELEASE', 1)
    print ("environment release\n")
    print(conf.all_envs['release'])
    print "-----------------------------------------------------------------------------------------"

    conf.setenv('debug', env=conf.env.derive())
    conf.env.CXXFLAGS = ['-DDEBUG',
                         '-D_GLIBCXX_DEBUG',
                         '-D_GLIBCXX_DEBUG_PEDANTIC',
                         '-g', '-std=c++17']
    conf.define('DEBUG', 1)
    print ("environment debug\n")
    print(conf.all_envs['debug'])
    print "-----------------------------------------------------------------------------------------"

def configure_clang(conf):
    #conf.find_program('clang++', var='CXX', mandatory = True)
    conf.CXX = 'clang++'
    conf.load("compiler_cxx")
    #conf.find_program('clang', var='C', mandatory = True)
    conf.C = 'clang'
    conf.load("compiler_c")
    global_env(conf)
    conf.env.append_unique('LDFLAGS_N',
                           ['stdlib=libc++']
    )
    conf.env.append_unique('STLIB', 'libc++')
    conf.setenv('release', env=conf.env.derive())
    conf.env.CXXFLAGS = ['-Wall',
                         '-Wextra',
                         '-std=c++1z',
                         '-stdlib=libc++',
                         '-O3']
    conf.env.append_unique('LIBS',
                           ['c++', 'c++abi'])
    conf.define('RELEASE', 1)

    print ("environment release\n")
    print(conf.all_envs['release'])
    print "-----------------------------------------------------------------------------------------"

    conf.setenv('debug', env=conf.env.derive())
    conf.env.CXXFLAGS = ['-g',
                         '-glldb',
                         '-Wdocumentation']
    conf.define('DEBUG', 1)
    print ("environment debug\n")
    print(conf.all_envs['debug'])
    print "-----------------------------------------------------------------------------------------"


def options(opt):
    opt.load("compiler_cxx")
    opt.load("compiler_c")

    opt.add_option(
        '--clang',
        action = 'store_true',
        default = False,
    )


def configure(conf):
    from waflib.Tools.compiler_cxx import cxx_compiler
    cxx_compiler['linux'] = ['gxx', 'clangxx']
    conf.load('compiler_cxx')
    print (conf.env.COMPILER_CXX)
    #from waflib.Tools.compiler_c import c_compiler
    #c_compiler['linux'] = ['clang',  'gcc', 'gcc-5', 'gcc-4.9', 'gcc-4.8']
    #conf.load('compiler_c')
    #if conf.options.clang:
    #    configure_clang(conf)
    #else:
    #    configure_gcc(conf)
    if conf.env.COMPILER_CXX == "clangxx":
        configure_clang(conf)
    else:
        configure_gcc(conf)
    #print(conf.env)


def build(bld):
    if not bld.variant:
        print "Build a variant: build_release or build_debug"
    bld.recurse(DIRS)

from waflib.Build import BuildContext
class release(BuildContext):
    cmd = 'build_release'
    variant = 'release'

class debug(BuildContext):
    cmd = 'build_debug'
    variant = 'debug'
