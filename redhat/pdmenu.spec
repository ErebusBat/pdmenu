Summary: A simple text-based menu program
Name: pdmenu
Packager: Joey Hess <joey@kite.ml.org>
Version: 0.6.1
Release: 1
Source: ftp://kite.ml.org/pub/code/pdmenu_0.6.1.tar.gz
Copyright: GPL
Group: Shells/Menus
Buildroot: /tmp

%description
A simple full screen menu program, intended to be comfortable
login shell for inexperienced users.

%prep
%setup
./configure --prefix=/usr --sysconfdir=/etc

%build
make

%install
make INSTALL_PREFIX=$RPM_BUILD_ROOT install

%files
%config /etc/pdmenurc
/usr/bin/pdmenu
/usr/man/man1/pdmenu.1
/usr/man/man5/pdmenurc.5
%doc README CHANGES BUGS TODO pdmenurc.monitor
