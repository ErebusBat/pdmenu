#!/usr/bin/perl -n
#
# This is a simplistic program that works the same as cpp. However, it
# only understands #define, #endif, and #ifdef. It cannot handle nested 
# #ifdef statements. And it doesn't mess with any lines not starting with "#".
#
# GNU copyright 1997, Joey Hess.

chomp;
if (/^#/) {
	if (!$skip && /^#define (.*)/) {
		$def{$1}=1;
	}
	elsif (/^#ifdef (.*)/) {
		if (!	$def{$1}) {
			$skip=1;
		}
	}
	elsif (/^#endif/) {
		undef $skip;
	}
}
elsif (!$skip) {
	print "$_\n"
}
