/*
 * filesystem.c
 *
 * Ruby extension for 'statvfs' and 'getmntent' system calls
 * Mike Hall www.enteract.com/~mghall 2002-04-24
 * David Palm dvdplm@gmail.com removed all the mounts relative stuff
 * so we can compile under Mac OS X.
 *
 * Tested on Mac OS X and Linux
 */

#include "ruby.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/statvfs.h>


 VALUE mfs, sFSstat;

static VALUE fs_stat(VALUE obj, VALUE dir)
{
	struct statvfs fs;
	char *s = STR2CSTR(dir);

	if ( statvfs(s, &fs) < 0 ) {
		rb_sys_fail(s);
	}
	return rb_struct_new(sFSstat, 
		dir, 
		INT2NUM(fs.f_bsize),
		INT2NUM(fs.f_blocks),
		INT2NUM(fs.f_bfree),
		INT2NUM(fs.f_bavail),
		INT2NUM(fs.f_files),
		INT2NUM(fs.f_ffree),
		INT2NUM(fs.f_favail),
		INT2NUM(fs.f_flag),
		INT2NUM(fs.f_namemax),
    NULL );
}

void Init_filesystem()
{
	mfs = rb_define_module("FileSystem");
	rb_define_module_function(mfs, "stat", fs_stat, 1);
	sFSstat = rb_struct_define("FileSystemStat", "path", "block_size", "blocks", "blocks_free", "blocks_avail", "files", "files_free", "files_avail", "flags", "maxnamelen", NULL);
	rb_global_variable(&sFSstat);
}

