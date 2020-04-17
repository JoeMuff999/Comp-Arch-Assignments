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
CFLAGS=-Wall -O2 -DUSE_INTERP_RESULT -Wno-deprecated-declarations

##################################################
# You shouldn't need to modify anything below here
##################################################

MISCDIR=../misc
INC=$(TKINC) -I$(MISCDIR) $(GUIMODE)
LIBS=$(TKLIBS) -lm
YAS=../misc/yas

all: ssim

# This rule builds the SEQ simulator (ssim)
ssim: ssim.c sim.h $(MISCDIR)/isa.c $(MISCDIR)/isa.h
	$(CC) $(CFLAGS) $(INC) -o ssim ssim.c $(MISCDIR)/isa.c $(LIBS)

# These are implicit rules for assembling .yo files from .ys files.
.SUFFIXES: .ys .yo
.ys.yo:
	$(YAS) $*.ys

clean:
	rm -f ssim *.o *~ *.exe *.yo *.ys
