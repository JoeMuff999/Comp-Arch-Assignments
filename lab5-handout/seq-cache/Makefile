# Comment this out if you don't have Tcl/Tk on your system

GUIMODE=-DHAS_GUI

# Modify the following line so that gcc can find the libtcl.so and
# libtk.so libraries on your system. You may need to use the -L option
# to tell gcc which directory to look in. Comment this out if you
# don't have Tcl/Tk.

TKLIBS=-L/usr/lib -ltk -ltcl

# Modify the following line so that gcc can find the tcl.h and tk.h
# header files on your system. Comment this out if you don't have
# Tcl/Tk.

TKINC=-isystem /usr/include/tcl8.6

# Modify these two lines to choose your compiler and compile time
# flags.

CC=gcc
CFLAGS=-Wall -O0 -g -DUSE_INTERP_RESULT -Wno-deprecated-declarations

##################################################
# You shouldn't need to modify anything below here
##################################################

MISCDIR=../misc
INC=$(TKINC) -I$(MISCDIR) $(GUIMODE)
LIBS=$(TKLIBS) -lm
YAS=../misc/yas

all: ssim cache-runner test-cache

cache-runner: cache-runner.c cache.c cache.h cachelab.c cachelab.h $(MISCDIR)/isa.c $(MISCDIR)/isa.h
	$(CC) $(CFLAGS) -I$(MISCDIR) -o cache-runner cache-runner.c cache.c cachelab.c $(MISCDIR)/isa.c -lm

test-cache: test-cache.c $(MISCDIR)/isa.c $(MISCDIR)/isa.h
	$(CC) $(CFLAGS) -I$(MISCDIR) -o test-cache test-cache.c $(MISCDIR)/isa.c

# This rule builds the SEQ simulator (ssim)
ssim: ssim-cache.c sim.h $(MISCDIR)/isa.c $(MISCDIR)/isa.h
	$(CC) $(CFLAGS) $(INC) -o ssim-cache ssim-cache.c cache.c $(MISCDIR)/isa.c $(LIBS)

# These are implicit rules for assembling .yo files from .ys files.
.SUFFIXES: .ys .yo
.ys.yo:
	$(YAS) $*.ys

clean:
	rm -f ssim-cache test-cache cache-runner .csim_results *.o *~ *.exe *.yo *.ys
