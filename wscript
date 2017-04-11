#! /usr/bin/env python
# encoding: utf-8

APPNAME = "FunctionalXpp"
VERSION = "0.0"

top = "."
out = "build"

DIRS = 'source test'

import glob, os

from waflib.Configure import conf

def printProcess(msg, l = 70):
    lmsg = (l - len(msg))/2
    msgToPrint = lmsg * "-" + msg + lmsg * "-"
    if len(msgToPrint) < l:
        msgToPrint = msgToPrint.split(" ")[0] + "  " + msgToPrint.split(" ")[1]

    print l * "="
    print msgToPrint
    print l * "="

def global_env(ctx):
    ctx.env.appname = APPNAME
    ctx.env.append_unique('LDFLAGS_N',
                          [ 'bz2',
                            'gsl',
                            'gslcblas',
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
    conf.find_program('g++', var = 'CXX', mandatory = True)
    conf.load('g++')
    conf.find_program('gcc', var = 'C', mandatory = True)
    #conf.load('gcc')
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
    #print ("environment release\n")
    #print(conf.all_envs['release'])
    #print "-----------------------------------------------------------------------------------------"

    conf.setenv('debug', env=conf.env.derive())
    conf.env.CXXFLAGS = ['-DDEBUG',
                         '-D_GLIBCXX_DEBUG',
                         '-D_GLIBCXX_DEBUG_PEDANTIC',
                         '-g', '-std=c++17']
    conf.define('DEBUG', 1)
    #print ("environment debug\n")
    #print(conf.all_envs['debug'])
    #print "-----------------------------------------------------------------------------------------"

def configure_gcc_5(conf):
    conf.find_program('g++-5', var = 'CXX', mandatory = True)
    conf.load('g++')
    conf.find_program('gcc-5', var = 'C', mandatory = True)
    #conf.load('gcc')
    global_env(conf)
    conf.env.append_unique('STLIB', 'stdc++')
    conf.env.append_unique('LDFLAGS_N', 'stdc++')
    conf.setenv('alternative-gcc-5', env=conf.env.derive())
    conf.env.CXXFLAGS = ['-Wall',
                         '-Wno-unknown-pragmas',
                         '-Wextra',
                         '-Wconversion',
                         '-O3',
                         '-std=c++14']
    conf.define('RELEASE', 1)
    #print ("environment release\n")
    #print(conf.all_envs['release'])
    #print "-----------------------------------------------------------------------------------------"

    conf.setenv('alternative-gcc-5-debug', env=conf.env.derive())
    conf.env.CXXFLAGS = ['-DDEBUG',
                         '-D_GLIBCXX_DEBUG',
                         '-D_GLIBCXX_DEBUG_PEDANTIC',
                         '-g', '-std=c++17']
    conf.define('DEBUG', 1)
    #print ("environment debug\n")
    #print(conf.all_envs['debug'])
    #print "-----------------------------------------------------------------------------------------"


def configure_clang(conf):
    conf.find_program('clang', var='CXX', mandatory = True)
    #conf.CXX = 'clang'
    conf.load("clang++")
    #conf.find_program('clang', var='CC', mandatory = True)
    #conf.load("clang")
    global_env(conf)
    conf.env.append_unique('LDFLAGS_N',
                          [ "pthread",
                            "util", "m"]
    )
    conf.setenv('alternative-clang', env=conf.env.derive())
    conf.env.CXXFLAGS = ['-Wall',
                         '-Wno-unknown-pragmas',
                         '-Wextra',
                         '-Wconversion',
                         '-fno-strict-aliasing',
                         '-D_FORTIFY_SOURCE=2',
                         '-fstack-protector',
                         '--param=ssp-buffer-size=4',
                         '-Wformat',
                         '-Werror=format-security',
                         '-fwrapv',
                         '-O3',
                         '-std=c++1z'
    ]
    conf.env.append_unique('LIBS',
                           ['c++', 'c++abi'])
    conf.define('RELEASE', 1)

    #print ("environment release\n")
    #print(conf.all_envs['release'])
    #print "-----------------------------------------------------------------------------------------"

    conf.setenv('alternative-clang-debug', env=conf.env.derive())
    conf.env.CXXFLAGS = ['-g',
                         '-glldb',
                         '-Wdocumentation']
    conf.define('DEBUG', 1)
    #print ("environment debug\n")
    #print(conf.all_envs['debug'])
    #print "-----------------------------------------------------------------------------------------"


def options(opt):
    opt.load("compiler_cxx")
    opt.load("compiler_c")

    print("read options")
    opt.add_option(
        '--clang',
        action = 'store_true',
        default = False,
    )


def configure_bak(conf):
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

def configure(conf):
    printProcess("Configuring gcc")
    configure_gcc(conf)
    conf.setenv("bebo") #seemingly, env name can be anything here --- used as a seperator
    printProcess("Configuring gcc-5")
    configure_gcc_5(conf)
    conf.setenv("buba")
    printProcess("Configuring clang")
    configure_clang(conf)

from waflib.Build import BuildContext

class release(BuildContext):
    cmd = 'build_release'
    variant = 'release'

class debug(BuildContext):
    cmd = 'build_debug'
    variant = 'debug'

class alt_release(BuildContext):
    cmd = 'build_gcc5_release'
    variant = 'alternative-gcc-5'

class alt_debug(BuildContext):
    cmd = 'build_gcc5_debug'
    variant = 'alternative-gcc-5-debug'

class alt_release(BuildContext):
    cmd = 'build_clang_release'
    variant = 'alternative-clang'

class alt_debug(BuildContext):
    cmd = 'build_clang_debug'
    variant = 'alternative-gcc-5-debug'

def build(bld):
    if not bld.variant:
        bld.fatal("""Please invoke a variant to build: \n
        1. waf build_release \n
        2. waf build_debug \n
        3. waf build_clang_release\n
        4. waf build_clang_debug\n
        5. waf build_gcc5_release \n
        6. waf build_gcc5_debug\n""")

    printProcess("Building " + bld.variant)
    bld.recurse(DIRS)
