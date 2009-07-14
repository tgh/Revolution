CFLAGS	=	-Wall -O3 -fPIC
CC	=	gcc
LINKER  =	ld

# ----------------------------------------------------

Revolution.so: Revolution.c ladspa.h
	$(CC) $(CFLAGS) -c Revolution.c
	$(LINKER) -o Revolution.so Revolution.o -shared

clean:
	rm *.o *.so
