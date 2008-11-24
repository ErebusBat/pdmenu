Summary: A simple text-based menu program
Name: pdmenu
Packager: jeff.covey@pobox.com
Version: 1.2.96
Release: 1
Url: http://kitenet.net/~joey/code/pdmenu/
Copyright: GPL
Group: Shells/Menus
Buildroot: /tmp
Requires: gpm, slang >= 0.99.34

%description
A simple full screen menu program, intended to be a comfortable
login shell for inexperienced users.

%prep
%setup -n pdmenu
./configure --prefix=/usr --sysconfdir=/etc --mandir=/usr/share/man

%build
make

%install
make INSTALL_PREFIX=$RPM_BUILD_ROOT install

install examples/newbie/modem-check examples/newbie/rpm-info-on-command \
    /tmp/usr/lib/pdmenu

# What a hack.
cp -a examples redhat/examples
rm -f redhat/examples/*.in
rm -f redhat/examples/newbie/*.in
rm -f redhat/examples/newbie/modem-check 
rm -f redhat/examples/newbie/rpm-info-on-command
rm -f redhat/examples/editdir.pl redhat/examples/showdir.pl

%files
%config /etc/pdmenurc
/usr/bin/pdmenu
/usr/share/man/man1/pdmenu.1
/usr/share/man/man5/pdmenurc.5
/usr/lib/pdmenu
%doc README doc/BUGS doc/TODO doc/ANNOUNCE debian/changelog redhat/examples
