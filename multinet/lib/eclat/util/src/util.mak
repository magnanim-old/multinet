#-----------------------------------------------------------------------
# File    : util.mak
# Contents: build utility modules (on Windows systems)
# Author  : Christian Borgelt
# History : 2003.01.26 file created
#           2003.06.05 module params added
#           2003.08.12 module nstats added
#           2004.12.10 module memsys added
#           2006.07.20 adapted to Visual Studio 8
#           2008.08.01 adapted to name changes of arrays and lists
#           2008.08.18 adapted to main functions of arrays and lists
#           2008.08.22 module escape added, test program tsctest added
#           2016.04.20 completed dependencies on header files
#-----------------------------------------------------------------------
THISDIR = ../../util/src

CC      = cl.exe
DEFS    = /D WIN32 /D NDEBUG /D _CONSOLE /D _CRT_SECURE_NO_WARNINGS
CFLAGS  = /nologo /W3 /O2 /GS- $(DEFS) /c

LD      = link.exe
LDFLAGS = 

PRGS    = sortargs.exe listtest.exe tsctest.exe

#-----------------------------------------------------------------------
# Build Modules
#-----------------------------------------------------------------------
all:          $(PRGS)

sortargs.exe: sortargs.obj util.mak
	$(LD) $(LDFLAGS) $(LIBS) sortargs.obj /out:$@

listtest.exe: listtest.obj util.mak
	$(LD) $(LDFLAGS) $(LIBS) listtest.obj /out:$@

trdtest.exe:  trdtest.obj escape.obj arrays.obj idmap.obj util.mak
	$(LD) $(LDFLAGS) $(LIBS) escape.obj arrays.obj idmap.obj \
              trdtest.obj /out:$@

#-----------------------------------------------------------------------
# Programs
#-----------------------------------------------------------------------
sortargs.obj: fntypes.h arrays.h arrays.c util.mak
	$(CC) $(CFLAGS) /D ARRAYS_MAIN arrays.c /Fo$@

listtest.obj: fntypes.h lists.h lists.c util.mak
	$(CC) $(CFLAGS) /D LISTS_MAIN lists.c /Fo$@

tsctest.obj:  tabread.h tabread.c util.mak
	$(CC) $(CFLAGS) /D TSC_MAIN tabread.c /Fo$@

#-----------------------------------------------------------------------
# Array Operations
#-----------------------------------------------------------------------
arrays.obj:   fntypes.h
arrays.obj:   arrays.h arrays.c util.mak
	$(CC) $(CFLAGS) arrays.c /Fo$@

#-----------------------------------------------------------------------
# List Operations
#-----------------------------------------------------------------------
lists.obj:    fntypes.h
lists.obj:    lists.h lists.c util.mak
	$(CC) $(CFLAGS) lists.c /Fo$@

#-----------------------------------------------------------------------
# Memory Management
#-----------------------------------------------------------------------
memsys.obj:   memsys.h memsys.c util.mak
	$(CC) $(CFLAGS) memsys.c /Fo$@

#-----------------------------------------------------------------------
# Symbol Table Management
#-----------------------------------------------------------------------
symtab.obj:   fntypes.h
symtab.obj:   symtab.h symtab.c util.mak
	$(CC) $(CFLAGS) symtab.c /Fo$@

idmap.obj:    fntypes.h symtab.h arrays.h symtab.c util.mak
	$(CC) $(CFLAGS) /D IDMAPFN symtab.c /Fo$@

#-----------------------------------------------------------------------
# Random Number Generator
#-----------------------------------------------------------------------
random.obj:   random.h random.c util.mak
	$(CC) $(CFLAGS) random.c /Fo$@

#-----------------------------------------------------------------------
# Numerical Statistics
#-----------------------------------------------------------------------
nstats.obj:   nstats.h nstats.c util.mak
	$(CC) $(CFLAGS) nstats.c /Fo$@

nst_pars.obj: scanner.h nstats.h nstats.c util.mak
	$(CC) $(CFLAGS) /D NST_PARSE nstats.c /Fo$@

#-----------------------------------------------------------------------
# Escape Character Handling
#-----------------------------------------------------------------------
escape.obj:   escape.h escape.c util.mak
	$(CC) $(CFLAGS) escape.c /Fo$@

#-----------------------------------------------------------------------
# Table Reader Management
#-----------------------------------------------------------------------
tabread.obj:  escape.h tabread.h tabread.c util.mak
	$(CC) $(CFLAGS) tabread.c /Fo$@

#-----------------------------------------------------------------------
# Scanner
#-----------------------------------------------------------------------
scform.obj:   scanner.h scanner.c util.mak
	$(CC) $(CFLAGS) scanner.c /Fo$@

scanner.obj:  scanner.h scanner.c util.mak
	$(CC) $(CFLAGS) /D SCN_SCAN scanner.c /Fo$@

#-----------------------------------------------------------------------
# Command Line Parameter Retrieval
#-----------------------------------------------------------------------
params.obj:   params.h params.c util.mak
	$(CC) $(CFLAGS) params.c /Fo$@

#-----------------------------------------------------------------------
# Interrupt Signal Handling
#-----------------------------------------------------------------------
sigint.obj:   sigint.h sigint.c util.mak
	$(CC) $(CFLAGS) sigint.c /Fo$@

#-----------------------------------------------------------------------
# Clean up
#-----------------------------------------------------------------------
clean:
	-@erase /Q *~ *.obj *.idb *.pch
