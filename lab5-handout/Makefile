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

##################################################
# You shouldn't need to modify anything below here
##################################################

# Use this rule (make all) to build the Y86-64 tools. The variables you've
# assigned to GUIMODE, TKLIBS, and TKINC will override the values that
# are currently assigned in seq/Makefile and pipe/Makefile.
all: tar
	(cd misc; make all)
	(cd pipe; make all GUIMODE=$(GUIMODE) TKLIBS="$(TKLIBS)" TKINC="$(TKINC)")
	(cd seq; make all GUIMODE=$(GUIMODE) TKLIBS="$(TKLIBS)" TKINC="$(TKINC)")
	(cd seq-cache; make all GUIMODE=$(GUIMODE) TKLIBS="$(TKLIBS)" TKINC="$(TKINC)")
	(cd y86-code; make all)

tar: seq/ssim.c pipe/psim.c y86-code/myprog.ys seq-cache/ssim-cache.c seq-cache/cache.c
	rm -f handin.tar
	tar cf handin.tar seq/ssim.c pipe/psim.c y86-code/myprog.ys seq-cache/ssim-cache.c seq-cache/cache.c

clean:
	rm -f *~ core
	(cd misc; make clean)
	(cd pipe; make clean)
	(cd seq; make clean)
	(cd y86-code; make clean)
	(cd ptest; make clean)
	(cd seq-cache; make clean)
	rm -f handin.tar

