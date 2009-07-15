# Copyright © 2009 Tyler Hayes
# ALL RIGHTS RESERVED
#
# [This program is licensed under the GPL version 3 or later.]
# Please see the file COPYING in the source
# distribution of this software for license terms.
#
# Thanks to Tom Szilagyi for his Makefile help.
#
# NOTE: since LADSPA_PATH varies depending on your distro,
# YOU MUST CHANGE THE VARIABLE TO MATCH YOUR LADSPA_PATH
# ENVIRONMENT VARIABLE!
#
# This Makefile also assumes that you already have the ladspa_sdk
# installed and thus have the ladspa.h header file required for
# inclusion in the source files and that it is located in an
# appropriate inclusion path directory.
#

#-----------------------------------------------------

CC	=	gcc
CFLAGS	=	-I -Wall -O3 -fPIC
LINKER  =	ld
LDFLAGS =	-shared
LADSPA_PATH =	/usr/lib/ladspa		# change this variable to match
					# your LADSPA_PATH environment
					# variable (type 'echo $LADSPA_PATH'
					# at your shell prompt)
PLUGINS	=	sb_revolution.so

# ----------------------------------------------------

all: $(PLUGINS)

sb_revolution.so: sb_revolution.c
	$(CC) $(CFLAGS) -c sb_revolution.c
	$(LINKER) $(LDFLAGS) -o sb_revolution.so sb_revolution.o

install: sb_revolution.so
	mv sb_revolution.so $(LADSPA_PATH)

uninstall:
	rm -f $(LADSPA_PATH)/sb_*

clean:
	rm -f *.o *.so
