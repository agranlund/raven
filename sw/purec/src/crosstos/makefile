#
# Generic makefile
#  - doesn't rely on any shell dependencies
#  - somewhat successfully tested on macOS, Linux and MingW32
#

CC =			gcc
AS =			gcc
LD =			gcc
STRIP = 		strip

ifeq ($(OS),Windows_NT)
    UNAME = Windows
else
    UNAME = $(shell uname -s)
endif

CFLAGS =		-Wall -pedantic -std=gnu99 -O3
LIBS =
LDFLAGS = 

ifeq ($(UNAME), Windows)
PREFIX = 		m68k-atari-tos-
SUFFIX =   		.exe
INCBIN = 		incbin.exe
BUILDDIR=		dist
ABIN = 			abin/
else
PREFIX = 		m68k-atari-tos-
SUFFIX =   		
INCBIN = 		./incbin
BUILDDIR=		dist
ABIN = 			abin/
endif

#
# Convert to lower case (kludgy, but afaik it's the only platform independent way to do this)
lc = $(subst A,a,$(subst B,b,$(subst C,c,$(subst D,d,$(subst E,e,$(subst F,f,$(subst G,g,$(subst H,h,$(subst I,i,$(subst J,j,$(subst K,k,$(subst L,l,$(subst M,m,$(subst N,n,$(subst O,o,$(subst P,p,$(subst Q,q,$(subst R,r,$(subst S,s,$(subst T,t,$(subst U,u,$(subst V,v,$(subst W,w,$(subst X,x,$(subst Y,y,$(subst Z,z,$1))))))))))))))))))))))))))

# Recursive wildcards
rwildcard =		$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

# How to recursively find all files that match a pattern
BINARIES = \
				$(foreach f, $(call rwildcard,$(ABIN),*.TTP), $(subst $(ABIN),,$(f))) \
				$(foreach f, $(call rwildcard,$(ABIN),*.TOS), $(subst $(ABIN),,$(f))) \
				$(foreach f, $(call rwildcard,$(ABIN),*.PRG), $(subst $(ABIN),,$(f))) \
				$(foreach f, $(call rwildcard,$(ABIN),*.ttp), $(subst $(ABIN),,$(f))) \
				$(foreach f, $(call rwildcard,$(ABIN),*.tos), $(subst $(ABIN),,$(f))) \
				$(foreach f, $(call rwildcard,$(ABIN),*.prg), $(subst $(ABIN),,$(f)))

OBJECTS	= \
				./main.o 		\
				./cpu.o  		\
				./gemdos.o  	\
				./bios.o  		\
				./xbios.o  		\
				./vt52.o 		\
				./paths.o
OBJECTS	+= \
				./musashi/m68kcpu.o 	\
				./musashi/m68kops.o 	\
				./musashi/softfloat/softfloat.o 	\
				./musashi/m68kdasm.o

OBJECTS += \
				./tinyalloc/tinyalloc.o

DEFINES = 

INCLUDES =		-I. -I./mushashi -I./tinyalloc

#
# Misc. filename mangling macros
targname =		$(firstword $(subst /, ,$1))
filename =		$(call lc,$(PREFIX)$(call targname, $1)-$(notdir $(basename $1))$(SUFFIX))
fullname =		$(BUILDDIR)/$(call targname,$1)/$(call filename,$1)

bobject  =		$(addsuffix .o, bin-$(call targname, $1)-$(notdir $(basename $1)))
bsource  =		$(addsuffix .c, bin-$(call targname, $1)-$(notdir $(basename $1)))

targets  =		$(foreach f, $(BINARIES), $(call fullname, $f))

#
# Target definition (binary -> C source code)
define target_object
$(call bsource,$f): $(ABIN)$f $(INCBIN)
				$(INCBIN) $(ABIN)$(f) >$(call bsource, $(f))
endef

#
# Target definition (final binaries)
define target_binary
$(call fullname, $f) : $(call bobject,$f) $(OBJECTS)
				mkdir -p $(BUILDDIR)/$(call targname,$f)
				$(LD) $(LDFLAGS) $(OBJECTS) $(call bobject,$f) $(LIBS) -o $(call fullname, $f)
endef

all: $(targets)
				$(info $(targets))

# Generates actual targets (see definitions above)
$(foreach f, $(BINARIES), $(eval $(target_binary)))
$(foreach f, $(BINARIES), $(eval $(target_object)))

# Conversion utility (binary -> C source code)
$(INCBIN):	incbin.c
				$(CC) -o $(INCBIN) incbin.c

# Generic compilation target
.c.o:
				$(CC) -c $(CFLAGS) $(INCLUDES) $(DEFINES) $< -o $@

# GTFO
clean:
				rm -f $(INCBIN) $(OBJECTS) \
				$(foreach f, $(BINARIES), \
				rm -f $(call bobject, $f); \
				rm -f $(call bsource, $f); \
				rm -f $(call fullname, $f); \
				)
