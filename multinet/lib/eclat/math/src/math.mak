#-----------------------------------------------------------------------
# File    : math.mak
# Contents: build math utility modules (on Windows systems)
# Author  : Christian Borgelt
# History : 2003.01.26 file created
#           2003.05.19 intexp, choose, zeta, quantile, and normd added
#           2003.08.15 module radfn added
#           2016.04.20 completed dependencies on header files
#-----------------------------------------------------------------------
THISDIR  = ..\..\math\src
UTILDIR  = ..\..\util\src
TRACTDIR = ..\..\tract\src

CC      = cl.exe
DEFS    = /D WIN32 /D NDEBUG /D _CONSOLE /D _CRT_SECURE_NO_WARNINGS
CFLAGS  = /nologo /W3 /O2 /GS- $(DEFS) /c
INCS    = /I $(UTILDIR) /I $(TRACTDIR)

#-----------------------------------------------------------------------
# Build Modules
#-----------------------------------------------------------------------
all:        intexp.obj choose.obj zeta.obj gamma.obj gammall.obj \
            normal.obj chi2.obj quantile.obj radfn.obj ruleval.obj

#-----------------------------------------------------------------------
# Mathematical Functions
#-----------------------------------------------------------------------
intexp.obj:   intexp.h intexp.c math.mak
	$(CC) $(CFLAGS) intexp.c /Fo$@

choose.obj:   choose.h choose.c math.mak
	$(CC) $(CFLAGS) choose.c /Fo$@

zeta.obj:     zeta.h zeta.c math.mak
	$(CC) $(CFLAGS) zeta.c /Fo$@

gamma.obj:    gamma.h gamma.c math.mak
	$(CC) $(CFLAGS) gamma.c /Fo$@

gammall.obj:  gamma.h gamma.c math.mak
	$(CC) $(CFLAGS) /D GAMMAQTL gamma.c /Fo$@

normal.obj:   normal.h normal.c math.mak
	$(CC) $(CFLAGS) normal.c /Fo$@

chi2.obj:     gamma.h
chi2.obj:     chi2.h chi2.c math.mak
	$(CC) $(CFLAGS) chi2.c /Fo$@

chi2all.obj:  gamma.h normal.h
chi2all.obj:  chi2.h chi2.c math.mak
	$(CC) $(CFLAGS) /D CHI2QTL chi2.c /Fo$@

radfn.obj:    gamma.h
radfn.obj:    radfn.h radfn.c math.mak
	$(CC) $(CFLAGS) radfn.c /Fo$@

ruleval.obj:  $(UTILDIR)\fntypes.h  $(UTILDIR)\arrays.h \
              $(UTILDIR)\arrays.h   $(UTILDIR)\symtab.h \
              $(TRACTDIR)\tract.h   gamma.h chi2.h
ruleval.obj:  ruleval.h ruleval.c math.mak
	$(CC) $(CFLAGS) $(INCS) ruleval.c /Fo$@

#-----------------------------------------------------------------------
# Clean up
#-----------------------------------------------------------------------
clean:
	-@erase /Q *~ *.obj *.idb *.pch
