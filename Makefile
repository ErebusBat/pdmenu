all: pdmenu

include makeinfo

CFLAGS_FOR_GCC  = -Wall
SRCDIR          = src

OBJFILES=$(SRCDIR)/pdmenu.o $(SRCDIR)/screen.o $(SRCDIR)/rc.o \
	$(SRCDIR)/menu.o $(SRCDIR)/inputbox.o $(SRCDIR)/mouse.o \
	$(SRCDIR)/keyboard.o $(SRCDIR)/pdstring.o $(SRCDIR)/actions.o \
	$(SRCDIR)/window.o $(SRCDIR)/error.o $(SRCDIR)/pdgetline.o

pdmenu: .dep $(OBJFILES)
	${CC} -o pdmenu $(OBJFILES) $(CFLAGS) $(LIBS)

depend: .dep
dep: .dep
.dep: src/*.h src/*.c
	mv makeinfo makeinfo.bak
	awk '$$0 ~ /^# DO NOT REMOVE THIS LINE/ { exit } { print }'  \
		< makeinfo.bak > makeinfo
	-rm -f makeinfo.bak
	echo "# DO NOT REMOVE THIS LINE" >> makeinfo
	$(CC) -MM $(SRCDIR)/*.c | sed 's!^\(.*\)\.o[ :]!$(SRCDIR)/\1.o:!' \
		>> makeinfo
	touch .dep

distclean: clean
	-$(MAKE) -C po distclean
	find . -name '\#*\#' -o -name '*.bak' -o -name '.??*' -o \
		-name '*~' -o -name '.gdb_history' -exec rm {} \;
	rm -f examples/pdmenurc examples/pdmenurc.monitor \
		examples/pdmenurc.complex examples/newbie/pdmenurc.newbie \
		src/slang.h config.cache config.log config.status makeinfo \
		.dep gmon.out doc/pdmenu.man doc/pdmenurc.man

clean:
	$(MAKE) -C po clean
	rm -f src/*.o pdmenu

install: all
	$(INSTALL) -d $(INSTALL_PREFIX)/$(BINDIR) \
		$(INSTALL_PREFIX)/$(MANDIR)/man1 \
		$(INSTALL_PREFIX)/$(MANDIR)/man5 \
		$(INSTALL_PREFIX)/$(SYSCONFDIR) \
		$(INSTALL_PREFIX)/$(DATADIR)/pdmenu
	$(INSTALL_PROGRAM) pdmenu $(INSTALL_PREFIX)/$(BINDIR)
	$(INSTALL) doc/pdmenu.man $(INSTALL_PREFIX)/$(MANDIR)/man1/pdmenu.1 -m 0644
	$(INSTALL) doc/pdmenurc.man $(INSTALL_PREFIX)/$(MANDIR)/man5/pdmenurc.5 -m 0644
	if [ -e $(INSTALL_PREFIX)/$(SYSCONFDIR)/pdmenurc ]; then \
		echo "Backing up $(INSTALL_PREFIX)/$(SYSCONFDIR)/pdmenurc to $(INSTALL_PREFIX)/$(SYSCONFDIR)/pdmenurc.old .." ; \
		cp $(INSTALL_PREFIX)/$(SYSCONFDIR)/pdmenurc $(INSTALL_PREFIX)/$(SYSCONFDIR)/pdmenurc.old ; \
	fi
	$(INSTALL) examples/pdmenurc $(INSTALL_PREFIX)/$(SYSCONFDIR)/pdmenurc -m 0644
	$(INSTALL) examples/showdir.pl $(INSTALL_PREFIX)/$(DATADIR)/pdmenu
	cd $(INSTALL_PREFIX)/$(DATADIR)/pdmenu && $(LN_S) -f showdir.pl editdir.pl
	$(MAKE) -C po install

test: pdmenu
	./pdmenu examples/pdmenurc

debian:
	dpkg-buildpackage -tc -rfakeroot

configure: configure.ac
	autoconf

config.status: configure
	./configure

makeinfo: autoconf/makeinfo.in config.status
	./config.status

.c.o:
	cd $(SRCDIR) && $(CC) -c ../$< $(CFLAGS)

.PHONY: debian test install clean distclean
