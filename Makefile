#!/usr/bin/make -f
# Waf Makefile wrapper
WAF_HOME=/home/muchu/work/learn/FunctionalXpp

all:
#@/home/muchu/libs/waf-1.8.20/waf build

all-debug:
	@/home/muchu/libs/waf-1.8.20/waf -v build

all-progress:
	@/home/muchu/libs/waf-1.8.20/waf -p build

install:
	/home/muchu/libs/waf-1.8.20/waf install --yes;

uninstall:
	/home/muchu/libs/waf-1.8.20/waf uninstall

clean:
	@/home/muchu/libs/waf-1.8.20/waf clean

distclean:
	@/home/muchu/libs/waf-1.8.20/waf distclean
	@-rm -rf build
	@-rm -f Makefile

check:
	@/home/muchu/libs/waf-1.8.20/waf check

dist:
	@/home/muchu/libs/waf-1.8.20/waf dist

.PHONY: clean dist distclean check uninstall install all

