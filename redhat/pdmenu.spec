Summary: A simple text-based menu program
Name: pdmenu
Packager: jeff.covey@pobox.com
Version: 1.2.42
Release: 1
Url: http://kitenet.net/programs/pdmenu/
Source: ftp://kitenet.net/pub/code/pdmenu/pdmenu_1.2.42.tar.gz
Copyright: GPL
Group: Shells/Menus
Buildroot: /tmp
Requires: gpm, slang >= 0.99.34

%description
A simple full screen menu program, intended to be a comfortable
login shell for inexperienced users.

%prep
%setup
./configure --prefix=/usr --sysconfdir=/etc

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
