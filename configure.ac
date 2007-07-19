dnl Written by Joey Hess <joey@kitenet.net> with some help from autoscan.
dnl Process this file with autoconf to generate the configure script.

AC_INIT(pdmenu)
AC_CONFIG_SRCDIR([src/pdmenu.c])
AC_CONFIG_AUX_DIR(autoconf)

PACKAGE=pdmenu
AC_DEFINE_UNQUOTED(PACKAGE, "$PACKAGE")
AC_SUBST(PACKAGE)

dnl List of all translations.
ALL_LINGUAS=""
AM_GNU_GETTEXT

dnl Checks for programs.
AC_PROG_CC

dnl If we are using gcc for a compiler, I'd like to add some cflags which
dnl are specified in the CFLAGS_FOR_GCC variable in the Makefile.
if test "$GCC" = yes ; then
	gcc_cflags='$(CFLAGS_FOR_GCC)'
fi
AC_SUBST(gcc_cflags)

dnl Check to see if the C compiler can handle the -pipe flag.
dnl To make this work, I set CFLAGS to temporarily include -pipe.
oldcflags="$CFLAGS"
CFLAGS="$CFLAGS -pipe"
AC_MSG_CHECKING(whether cc accepts -pipe)
AC_TRY_COMPILE(,,echo yes,echo no; CFLAGS="$oldcflags")

AC_PROG_LN_S
AC_PROG_INSTALL

AC_MSG_CHECKING(if install -d works)
$ac_cv_path_install -d ./test/test2 2>/dev/null
if test -d ./test/test2 ; then
	AC_MSG_RESULT(yes)
else
	AC_MSG_RESULT(no)
	INSTALL='autoconf/install-sh -c'
fi
dnl Clean up our mess
rmdir ./test/test2 2>/dev/null
rmdir ./test 2>/dev/null

dnl Let the user specify an alternate location for the libs.
dnl I have to add this to CFLAGS, instead of to LIBS, to placate 
dnl some compiler that wanted -L before -l .
if test "$SLANG_LIB_LOC" != "" ; then 
	CFLAGS="-L$SLANG_LIB_LOC $CFLAGS"
fi

dnl The function we check for in libslang is important, because
dnl we need to make sure we have the right version of slang.
AC_CHECK_LIB(slang, SLsig_block_signals,, 
	AC_MSG_ERROR(can't find required slang library or library is obsolete. Try setting the SLANG_LIB_LOC environment variable to point to the directory containing the slang library.))

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(getopt.h)

dnl The slang headers can be in several places.
AC_CHECK_HEADERS($SLANG_H_LOC, slang_h_ok=yes)
if test "$slang_h_ok" = yes ; then
	SLANG_H=$SLANG_H_LOC
else
	AC_CHECK_HEADERS(slang.h slang/slang.h, break)
	if test "$ac_cv_header_slang_h" = yes ; then
	 	SLANG_H=slang.h
	elif test "$ac_cv_header_slang_slang_h" = yes ; then
		SLANG_H=slang/slang.h
	else
		AC_MSG_ERROR(can't find required slang.h header file. Try setting the SLANG_H_LOC environment variable to point to it.)
	fi
fi
AC_SUBST(SLANG_H)

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
dnl AC_HEADER_TIME

dnl Checks for library functions.
AC_TYPE_SIGNAL
AC_CHECK_FUNCS(setenv)
AC_CHECK_FUNCS(getopt_long, , [
	AC_MSG_WARN(long command line switches are disabled.)
	AC_CHECK_FUNCS(getopt, ,
		AC_MSG_WARN(all command line switches are disabled!)
		)
	])
AC_CHECK_FUNCS(asprintf)

# Allow user to control whether gpm is linked in.
AC_ARG_WITH([gpm],
	    AC_HELP_STRING([--with-gpm], [link in gpm (default is YES if available)]),
	    [ac_cv_use_gpm=$withval], [ac_cv_use_gpm=yes])
AC_CACHE_CHECK([whether to use gpm],
	       [ac_cv_use_gpm], [ac_cv_use_gpm=yes])

dnl Checks for gpm, first the headers, then functions needed, then the library.
if test "$ac_cv_use_gpm" = yes ; then
	AC_CHECK_HEADERS(gpm.h)
	dnl Mouse support requires the select function.
	AC_CHECK_FUNCS(select)
	if test "$ac_cv_header_gpm_h" = yes && test "$ac_cv_func_select" = yes ; then
		AC_CHECK_LIB(gpm, Gpm_Close)
		if test "$ac_cv_lib_gpm_Gpm_Close" != yes ; then 
			AC_MSG_WARN(pdmenu will be built without GPM mouse support.)
		else
			AC_DEFINE(GPM_SUPPORT)
		fi
	else
		AC_MSG_WARN(pdmenu will be built without GPM mouse support.)
	fi
fi

AC_CHECK_PROG(DATE_PROG,date,yes,no)
if test "$DATE_PROG" = yes ; then
	AC_MSG_CHECKING(for GNU compatable date utility)
	GNU_DATE_TEST=`date +"%B %d %Y %d %b %y" 2>/dev/null`
	if test "$GNU_DATE_TEST" = "" ; then
		AC_MSG_RESULT(not found)
	else
		AC_MSG_RESULT(found)
		AC_MSG_CHECKING(the date)
		BUILD_DATE=`date +"%B %d %Y"`
		AC_SUBST(BUILD_DATE)
		AC_MSG_RESULT($BUILD_DATE)
	
		AC_MSG_CHECKING(the date for lsm files)
		LSM_BUILD_DATE=`date +%d%b%y | tr a-z A-Z` 2>/dev/null
		AC_SUBST(LSM_BUILD_DATE)
		AC_MSG_RESULT($LSM_BUILD_DATE)
	fi
fi

AC_PROG_AWK
if test "$AWK" != "" ; then 
	AC_MSG_CHECKING(pdmenu's version)
	VER=`$AWK -F "[[()]]" '/pdmenu \(.*\) / {print $2 ; exit}' <debian/changelog`
	AC_SUBST(VER)
	AC_MSG_RESULT($VER)
	OLD_VER=`$AWK -F "[[()]]" '\
		/^pdmenu \(.*\) / { \
			if (++c==2) \
				{ print $2 ; exit } \
			}' <debian/changelog`
	AC_SUBST(OLD_VER)
else
	AC_MSG_WARN(cannot determine pdmenu version without awk.)
	AC_MSG_WARN(setting version to UNKNOWN - edit Makefile and fix!)
	VER=UNKNOWN
	OLD_VER=UNKNOWN
fi

AC_CHECK_PROGS(PERL_PROG,perl perl4 perl5)
if test "$PERL_PROG" != "" ; then
	AC_MSG_CHECKING(the size of the tar file)
	TAR_FILE_SIZE_GUESS=`perl -e '\
		print int(qx{tar cf - . |gzip| wc -c}/1024)'`
	AC_SUBST(TAR_FILE_SIZE_GUESS)
	AC_MSG_RESULT(about $TAR_FILE_SIZE_GUESS kb)
fi

dnl Have to calculate the bindir without any ${..} expansions in it for use
dnl in pdmenurc.in.
if test "$bindir" != '${exec_prefix}/bin' ; then
	PDMENU_BINDIR=$bindir
elif test "$exec_prefix" != NONE ; then
	PDMENU_BINDIR=$exec_prefix/bin
elif test "$prefix" != NONE ; then
	PDMENU_BINDIR=$prefix/bin
else
	PDMENU_BINDIR=$ac_default_prefix/bin
fi
AC_SUBST(PDMENU_BINDIR)

dnl Have to calculate the sysconfdir without any ${..} expansions in it for
dnl use in man pages.
if test "$sysconfdir" != '${prefix}/etc' ; then
	PDMENU_SYSCONFDIR=$sysconfdir
elif test "$prefix" != NONE ; then
	PDMENU_SYSCONFDIR=$prefix/etc
else
	PDMENU_SYSCONFDIR=$ac_default_prefix/etc
fi
AC_SUBST(PDMENU_SYSCONFDIR)

if test "$datadir" != '${prefix}/share' ; then
        PDMENU_LIBDIR=$datadir
elif test "$datadir" != NONE ; then
        PDMENU_LIBDIR=$prefix/share
else
        PDMENU_LIBDIR=$ac_default_prefix/share
fi
AC_SUBST(PDMENU_LIBDIR)

dnl Since the next step will make a new lsm file, remove all old 
dnl lsm files.
rm -f doc/pdmenu-*.lsm

AC_CONFIG_FILES([makeinfo:autoconf/makeinfo.in
	doc/pdmenu.man
	doc/pdmenurc.man
	examples/pdmenurc
	examples/pdmenurc.monitor
	examples/pdmenurc.complex
	examples/newbie/pdmenurc.newbie
	src/slang.h
	redhat/pdmenu.spec 
	doc/pdmenu-$VER.lsm:doc/pdmenu.lsm.in
])
AC_OUTPUT
