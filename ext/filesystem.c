/*
 * filesystem.c
 *
 * Ruby extension for 'statvfs' and 'getmntent' system calls
 * Mike Hall www.enteract.com/~mghall 2002-04-24
 *
 * Tested on Linux and Solaris
 */

#include "ruby.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/statvfs.h>

#ifdef HAVE_SYS_MNTTAB_H

/* Solaris */
#include <sys/mnttab.h>
#define MNTENT		mnttab
#define START_MNT(F,M)	fopen(F,M)
#define GET_MNT(FP,MP)	(getmntent(FP,MP) == 0)
#define END_MNT(F)	fclose(F)
#define	MOUNTLIST	"/etc/mnttab"

#else

#ifdef HAVE_MNTENT
  #include <mntent.h>

/* GNU, Linux, (and BSDs?) */
// #include <mntent.h>
#define MNTENT		mntent
#define	START_MNT(F,M)	setmntent(F,M)
#define GET_MNT(FP,MP)	((MP = getmntent(FP)) != NULL)
#define END_MNT(F)	endmntent(F)
#define	MOUNTLIST	"/etc/mtab"
#endif /* HAVE_MNTENT */

#endif

 VALUE mfs, sFSstat, sFSstat64;
// , sFSmount;

// static VALUE setup_mount(struct MNTENT *mp)
// {
//  return rb_struct_new(sFSmount, 
// #ifdef HAVE_SYS_MNTTAB_H
//    rb_tainted_str_new2(mp->mnt_special),
//    rb_tainted_str_new2(mp->mnt_mountp),
//    rb_tainted_str_new2(mp->mnt_fstype),
//    rb_tainted_str_new2(mp->mnt_mntopts),
//    rb_tainted_str_new2(mp->mnt_time)
// #else
//    rb_tainted_str_new2(mp->mnt_fsname),
//    rb_tainted_str_new2(mp->mnt_dir),
//    rb_tainted_str_new2(mp->mnt_type),
//    rb_tainted_str_new2(mp->mnt_opts),
//    INT2NUM(mp->mnt_freq),
//    INT2NUM(mp->mnt_passno)
// #endif
//   );
// }

// static VALUE fs_mounts(int argc, VALUE *argv, VALUE obj)
// {
//  VALUE x, res;
//  FILE *fp;
//  char *tab;
//  struct MNTENT *mp;
// #ifdef HAVE_SYS_MNTTAB_H
//  struct MNTENT mt;
//  mp = &mt;
// #endif
// 
//  if ( rb_scan_args(argc, argv, "01", &x) == 1 ) {
//    tab = STR2CSTR(x);
//  } else {
//    tab = MOUNTLIST;
//  }
// 
//         fp = START_MNT(tab, "r");
//  if ( fp == NULL ) {
//    rb_sys_fail(tab);
//  }
// 
//  if ( rb_block_given_p() ) {
//    res = obj;
//    while ( GET_MNT(fp, mp) ) {
//      rb_yield( setup_mount(mp) );
//    }
//  } else {
//    res = rb_ary_new();
//    while ( GET_MNT(fp, mp) ) {
//      rb_ary_push(res, setup_mount(mp));
//    }
//  }
//  END_MNT(fp);
//  return res;
// }
// 

static VALUE fs_stat64(VALUE obj, VALUE dir)
{
	struct statvfs fs;
	char *s = STR2CSTR(dir);

	if ( statfs64(s, &fs) < 0 ) {
		rb_sys_fail(s);
	}
	return rb_struct_new(sFSstat64, 
		dir, 
		INT2NUM(fs.f_bsize)
    // INT2NUM(fs.f.iosize),
    // INT2NUM(fs.f_blocks),
    // INT2NUM(fs.f_bfree),
    // INT2NUM(fs.f_bavail),
    // INT2NUM(fs.f_owner),
    //     INT2NUM(fs.f_type)
    );
}


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
 	rb_define_module_function(mfs, "stat64", fs_stat64, 1);
 // rb_define_module_function(mfs, "mounts", fs_mounts, -1);

	sFSstat = rb_struct_define("FileSystemStat",
		"path", "block_size", "blocks", "blocks_free", "blocks_avail", 
		"files", "files_free", "files_avail", "flags", "maxnamelen", NULL);
	rb_global_variable(&sFSstat);

  sFSstat64 = rb_struct_define("FileSystemStat64","path", "bsize"); //, "iosize", "blocks", "bfree", "bavail", "owner", "type");
	rb_global_variable(&sFSstat64);

//  sFSmount = rb_struct_define("FileSystemMount",
//    "device", "mount", "fstype", "options", 
// #ifdef HAVE_SYS_MNTTAB_H
//    "time",
// #else
//    "dump_interval", "check_pass",
// #endif
//          0);
//  rb_global_variable(&sFSmount);


#ifdef ST_RDONLY
	rb_define_const(mfs, "RDONLY",	INT2FIX(ST_RDONLY));
#endif
#ifdef ST_NOSUID
	rb_define_const(mfs, "NOSUID",	INT2FIX(ST_NOSUID));
#endif
#ifdef ST_NOTRUNC
	rb_define_const(mfs, "NOTRUNC",	INT2FIX(ST_NOTRUNC));
#endif
#ifdef ST_NODEV
	rb_define_const(mfs, "NODEV",	INT2FIX(ST_NODEV));
#endif
#ifdef ST_NOEXEC
	rb_define_const(mfs, "NOEXEC",	INT2FIX(ST_NOEXEC));
#endif
#ifdef ST_SYNCHRONOUS
	rb_define_const(mfs, "SYNC",	INT2FIX(ST_SYNCHRONOUS));
#endif
#ifdef ST_MANDLOCK
	rb_define_const(mfs, "MANDLOCK", INT2FIX(ST_MANDLOCK));
#endif
#ifdef ST_WRITE
	rb_define_const(mfs, "WRITE",	INT2FIX(ST_WRITE));
#endif
#ifdef ST_APPEND
	rb_define_const(mfs, "APPEND",	INT2FIX(ST_APPEND));
#endif
#ifdef ST_IMMUTABLE
	rb_define_const(mfs, "IMMUTABLE", INT2FIX(ST_IMMUTABLE));
#endif
#ifdef ST_NOATIME
	rb_define_const(mfs, "NOATIME",	INT2FIX(ST_NOATIME));
#endif
#ifdef ST_NODIRATIME
	rb_define_const(mfs, "NODIRATIME", INT2FIX(ST_NODIRATIME));
#endif
}

