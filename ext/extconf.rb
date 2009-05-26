require 'mkmf'
have_header 'sys/mnttab.h'	# Solaris
create_makefile 'filesystem'
