#!/usr/bin/perl
#
# Syntax: showdir.pl <directory> <old-directory>
#
# Where <directory> is the directory to display, and <old-directory>
# is the optional name of the directory they were on previously.
# Doen't specify <old-directory> yourself, it is only used internally by the
# program.
#
# Generates a pdmenurc file at stdout that lets the user view files in the 
# directory, and change to other directories.

$dir=shift;
$dir.='/' if $dir=~m#/$# eq undef; # make sure there is a trailing / on $dir.
$olddir=shift;
$olddir.='/' if $olddir=~m#/$# eq undef; # make sure there is a trailing / on $olddir.

# Make sure to remove the menu we are about to show, so we do not inaverdently
# add on one with the same name that already exists.
print "remove:show_directory_$dir\n";
print "menu:show_directory_$dir:Files in $dir:";
print "Select a file to display it or select a directory to move into it.\n";

opendir(PWD,$dir);
while ($fn=readdir(PWD)) {
	# Make a version of $fn that has ':' and '\' and '_' characters
	# escaped out.
	$fne=$fn;
	$fne=~s/\\/\\\\/g;
	$fne=~s/:/\\:/g;
	$fne=~s/_/\\_/g;

	if (-d "$dir/$fn" && $fn ne '.' && $fn ne '..') {
		# See if the directory the selected is the directory we were just on 
		# previously.
		if ("$dir$fn/" ne $olddir) {
			# Run this program again for a submenu.
			print "exec:$fne/:m:$0 $dir$fn/ $dir\n"
		}
		else {
			print "exit:$fne/\n";
		}
	}
	elsif ($fn eq '..' && $dir ne '/') {
		# Go down one directory.
		($parent)=$dir=~m:^(.*)/.*?/:;
		$parent.='/';
		# If they select to go down one directory, and the result is the directory
		# we were just on previously, then exit the menu instead of displaying
		# a new one.
		if ($parent ne $olddir) {
			print "exec:$fne/:m:$0 $parent $dir\n";
		}
		else {
			print "exit:$fne/\n";
		}
	}
	elsif ($fn ne '.' && $fn ne '..') {
		# View a file.
		print "exec:$fne:d:cat $dir$fn\n";
	}
}
closedir(PWD);
