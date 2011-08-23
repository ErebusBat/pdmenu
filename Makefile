CFLAGS_FOR_GCC	= -Wall
SRCDIR		= src
XGETTEXT	= xgettext --keyword=_ --keyword=N_ --add-comments=TRANS:
POTFILE		= po/pdmenu.pot
SRCFILES	= $(wildcard $(SRCDIR)/*.c)
OBJFILES	:= $(SRCFILES:%.c=%.o)
LANGS		= fr ru es
MOS		:= $(addprefix po/, $(addsuffix .mo, $(LANGS)))
LOCALEDIR	= $(INSTALL_PREFIX)/usr/share/locale

all: pdmenu $(POTFILE) $(MOS)

include makeinfo

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
	$(CC) -MM $(SRCFILES) | sed 's!^\(.*\)\.o[ :]!$(SRCDIR)/\1.o:!' \
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
	$(MAKE) $(POTFILE) # make sure translators' input is ready
	rm -f $(OBJFILES) pdmenu $(MOS)

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
	for lang in $(LANGS); do \
		[ ! -d $(LOCALEDIR)/$$lang/LC_MESSAGES/ ] && mkdir -p $(LOCALEDIR)/$$lang/LC_MESSAGES/; \
		install -m 644 po/$$lang.mo $(LOCALEDIR)/$$lang/LC_MESSAGES/pdmenu.mo; \
	done

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

$(POTFILE): $(SRCFILES)
	$(XGETTEXT) -o $(POTFILE)-update $(SRCFILES)
	if test -f $(POTFILE); then \
	  if diff -I '^"POT-Creation-Date:' -I '^Report-Msgid-Bugs-To:' $(POTFILE) $(POTFILE)-update >/dev/null 2>&1; then \
	      rm -f $(POTFILE)-update; \
	  else \
	      rm -f $(POTFILE); \
	      mv $(POTFILE)-update $(POTFILE); \
	  fi; \
	else \
	  mv $(POTFILE)-update $(POTFILE); \
	fi

%.mo: %.po
	msgfmt -o $@ $<

.PHONY: debian test install clean distclean
