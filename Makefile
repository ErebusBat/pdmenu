# Note: this is just a stub Makefile, to make it easy for you to
# run configure and build pdmenu. The full-fledged Makefile is created
# by ./configure, and is called `makeinfo'. You might want to edit it.

sinclude makeinfo

configure: configure.in
	autoconf

config.status: configure
	./configure

makeinfo: autoconf/makeinfo.in config.status
	./config.status
