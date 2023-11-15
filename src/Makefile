.PHONY: default lrtmidi
default: lrtmidi

BUILD_DATE  := $(shell date "+%Y-%m-%dT%H:%M:%S")

LNX_GCC_RUN := g++ -shared -fPIC -O2 -g  -Werror=return-type
WIN_GCC_RUN := gcc -shared -fPIC -O2
MAC_GCC_RUN := g++ -O2 -bundle -undefined dynamic_lookup -all_load

LNX_COPTS   := -I/usr/include/rtaudio
WIN_COPTS   := -I/mingw64/include/lua5.1 
MAC_COPTS   := -std=c++11 -I/usr/local/include/lua

LNX_LOPTS   := -lrtmidi -g
WIN_LOPTS   := -lkernel32
MAC_LOPTS   := -lrtmidi

LNX_SO_EXT  := so
WIN_SO_EXT  := dll
MAC_SO_EXT  := so

GCC_RUN     :=
SO_EXT      :=
COPTS       :=
LOPTS       :=

# platforms: LNX, WIN, MAC
# (may be set in sandbox.mk)

PLATFORM    := LNX
LUA_VERSION := 5.4

-include sandbox.mk

GCC_RUN       := $(or $(GCC_RUN),       $($(PLATFORM)_GCC_RUN))
SO_EXT        := $(or $(SO_EXT),        $($(PLATFORM)_SO_EXT))
COPTS         := $(or $(COPTS),         $($(PLATFORM)_COPTS))
LOPTS         := $(or $(LOPTS),         $($(PLATFORM)_LOPTS))

lrtmidi:
	@mkdir -p build/lua$(LUA_VERSION)/
	$(GCC_RUN) $(COPTS) \
	    -D LRTMIDI_VERSION=Makefile"-$(BUILD_DATE)" \
	    main.cpp  \
	    midibase.cpp midiin.cpp midiout.cpp \
	    async_util.cpp error.cpp \
	    lrtmidi_compat.c \
	    $(LOPTS) \
	    -o build/lua$(LUA_VERSION)/lrtmidi.$(SO_EXT)
	    

