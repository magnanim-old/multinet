#-----------------------------------------------------------------------
# File    : eclat.mak
# Contents: build eclat program (on Windows systems)
# Author  : Christian Borgelt
# History : 2003.01.26 file created
#           2003.08.22 talset removed, external tract (apriori) added
#           2006.07.20 adapted to Visual Studio 8
#           2010.02.12 module pfxtree replaced by module clomax
#           2010.08.22 module escape added (for module tabread)
#           2011.08.31 external module fim16 added (16 items machine)
#           2014.08.21 extended by module istree from apriori source
#           2016.04.20 completed dependencies on header files
#-----------------------------------------------------------------------
THISDIR  = ..\..\eclat\src
UTILDIR  = ..\..\util\src
MATHDIR  = ..\..\math\src
TRACTDIR = ..\..\tract\src
APRIDIR  = ..\..\apriori\src

CC       = cl.exe
DEFS     = /D WIN32 /D NDEBUG /D _CONSOLE /D _CRT_SECURE_NO_WARNINGS
CFLAGS   = /nologo /W3 /O2 /GS- $(DEFS) /c $(ADDFLAGS)
INCS     = /I $(UTILDIR) /I $(MATHDIR) /I $(TRACTDIR) /I $(APRIDIR)

LD       = link.exe
LDFLAGS  = /nologo /subsystem:console /incremental:no
LIBS     = 

HDRS     = $(UTILDIR)\fntypes.h    $(UTILDIR)\arrays.h     \
           $(UTILDIR)\memsys.h     $(UTILDIR)\symtab.h     \
           $(UTILDIR)\tabread.h    $(UTILDIR)\tabwrite.h   \
           $(UTILDIR)\error.h      $(MATHDIR)\ruleval.h    \
           $(TRACTDIR)\tract.h     $(TRACTDIR)\patspec.h   \
           $(TRACTDIR)\clomax.h    $(TRACTDIR)\report.h    \
           $(TRACTDIR)\fim16.h     $(APRIDIR)\istree.h     \
           eclat.h
OBJS     = $(UTILDIR)\arrays.obj   $(UTILDIR)\memsys.obj   \
           $(UTILDIR)\idmap.obj    $(UTILDIR)\escape.obj   \
           $(UTILDIR)\tabread.obj  $(UTILDIR)\tabwrite.obj \
           $(UTILDIR)\scform.obj   $(MATHDIR)\gamma.obj    \
           $(MATHDIR)\chi2.obj     $(MATHDIR)\ruleval.obj  \
           $(TRACTDIR)\taread.obj  $(TRACTDIR)\patspec.obj \
           $(TRACTDIR)\clomax.obj  $(TRACTDIR)\repcm.obj   \
           $(TRACTDIR)\fim16.obj   $(APRIDIR)\istree.obj   \
           eclat.obj
PRGS     = eclat.exe

#-----------------------------------------------------------------------
# Build Program
#-----------------------------------------------------------------------
all:          $(PRGS)

eclat.exe:    $(OBJS) eclat.mak
	$(LD) $(LDFLAGS) $(OBJS) $(LIBS) /out:$@

#-----------------------------------------------------------------------
# Main Program
#-----------------------------------------------------------------------
eclat.obj:    $(HDRS) eclat.mak
	$(CC) $(CFLAGS) $(INCS) /D ECL_MAIN eclat.c /Fo$@

#-----------------------------------------------------------------------
# External Modules
#-----------------------------------------------------------------------
$(UTILDIR)\arrays.obj:
	cd $(UTILDIR)
	$(MAKE) /f util.mak arrays.obj   ADDFLAGS="$(ADDFLAGS)"
	cd $(THISDIR)
$(UTILDIR)\memsys.obj:
	cd $(UTILDIR)
	$(MAKE) /f util.mak memsys.obj   ADDFLAGS="$(ADDFLAGS)"
	cd $(THISDIR)
$(UTILDIR)\idmap.obj:
	cd $(UTILDIR)
	$(MAKE) /f util.mak idmap.obj    ADDFLAGS="$(ADDFLAGS)"
	cd $(THISDIR)
$(UTILDIR)\escape.obj:
	cd $(UTILDIR)
	$(MAKE) /f util.mak escape.obj   ADDFLAGS="$(ADDFLAGS)"
	cd $(THISDIR)
$(UTILDIR)\tabread.obj:
	cd $(UTILDIR)
	$(MAKE) /f util.mak tabread.obj  ADDFLAGS="$(ADDFLAGS)"
	cd $(THISDIR)
$(UTILDIR)\tabwrite.obj:
	cd $(UTILDIR)
	$(MAKE) /f util.mak tabwrite.obj ADDFLAGS="$(ADDFLAGS)"
	cd $(THISDIR)
$(UTILDIR)\scform.obj:
	cd $(UTILDIR)
	$(MAKE) /f util.mak scform.obj   ADDFLAGS="$(ADDFLAGS)"
	cd $(THISDIR)
$(MATHDIR)\ruleval.obj:
	cd $(MATHDIR)
        $(MAKE) /f math.mak ruleval.obj  ADDFLAGS="$(ADDFLAGS)"
	cd $(THISDIR)
$(MATHDIR)\gamma.obj:
	cd $(MATHDIR)
        $(MAKE) /f math.mak gamma.obj    ADDFLAGS="$(ADDFLAGS)"
	cd $(THISDIR)
$(MATHDIR)\chi2.obj:
	cd $(MATHDIR)
        $(MAKE) /f math.mak gamma.obj    ADDFLAGS="$(ADDFLAGS)"
	cd $(THISDIR)
$(TRACTDIR)\taread.obj:
	cd $(TRACTDIR)
	$(MAKE) /f tract.mak taread.obj  ADDFLAGS="$(ADDFLAGS)"
	cd $(THISDIR)
$(TRACTDIR)\patspec.obj:
	cd $(TRACTDIR)
	$(MAKE) /f tract.mak patspec.obj ADDFLAGS="$(ADDFLAGS)"
	cd $(THISDIR)
$(TRACTDIR)\clomax.obj:
	cd $(TRACTDIR)
	$(MAKE) /f tract.mak clomax.obj  ADDFLAGS="$(ADDFLAGS)"
	cd $(THISDIR)
$(TRACTDIR)\repcm.obj:
	cd $(TRACTDIR)
	$(MAKE) /f tract.mak repcm.obj   ADDFLAGS="$(ADDFLAGS)"
	cd $(THISDIR)
$(TRACTDIR)\fim16.obj:
	cd $(TRACTDIR)
	$(MAKE) /f tract.mak fim16.obj   ADDFLAGS="$(ADDFLAGS)"
	cd $(THISDIR)
$(APRIDIR)\istree.obj:
	cd $(APRIDIR)
	$(MAKE) /f apriori.mak istree.obj   ADDFLAGS="$(ADDFLAGS)"
	cd $(THISDIR)

#-----------------------------------------------------------------------
# Install
#-----------------------------------------------------------------------
install:
	-@copy $(PRGS) ..\..\..\bin

#-----------------------------------------------------------------------
# Clean up
#-----------------------------------------------------------------------
localclean:
	-@erase /Q *~ *.obj *.idb *.pch $(PRGS)

clean:
	$(MAKE) /f eclat.mak localclean
	cd $(APRIDIR)
	$(MAKE) /f apriori.mak localclean
	cd $(TRACTDIR)
	$(MAKE) /f tract.mak localclean
	cd $(MATHDIR)
	$(MAKE) /f math.mak clean
	cd $(UTILDIR)
	$(MAKE) /f util.mak clean
	cd $(THISDIR)
