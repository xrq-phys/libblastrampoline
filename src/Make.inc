# ARCH detection in Make, used to pick out which trampoline assembly syntax we're gonna use
ARCH := $(shell $(CC) -dumpmachine | sed "s/\([^-]*\).*$$/\1/")

# Normalize ppc64le to powerpc64le
ifeq ($(ARCH),ppc64le)
  ARCH := powerpc64le
endif
# We map amd64 to x86_64 for compatibility with systems that identify 64-bit systems as such
ifeq ($(ARCH),amd64)
  ARCH := x86_64
endif
# We map arm64 (Apple spelling) to aarch64 to avoid having to deal with both spellings everywhere
ifeq ($(ARCH),arm64)
  ARCH := aarch64
endif

# Figure out OS and architecture (used to set `shlib_ext`)
OS := $(shell uname)
ifneq (,$(findstring MINGW,$(OS))$(findstring MSYS,$(OS))$(findstring CYGWIN,$(OS)))
  OS := WINNT
endif

ifeq ($(OS), WINNT)
  SHLIB_EXT := dll
  EXE := .exe
else ifeq ($(OS), Darwin)
  SHLIB_EXT := dylib
else
  SHLIB_EXT := so
endif

CFLAGS := -g -O0 -fPIC -DLIBRARY_EXPORTS -D_GNU_SOURCE
LDFLAGS := -shared

# On linux, we need to link `libdl` to get `dlopen`
ifeq ($(OS),Linux)
LDFLAGS += -ldl
endif

ifeq ($(VERBOSE),0)
ENDCOLOR := "\033[0m"
CCCOLOR := "\033[34m"
SRCCOLOR:="\033[33m"

PRINT_CC = printf '%b %b\n' $(CCCOLOR)CC$(ENDCOLOR) $(SRCCOLOR)$@$(ENDCOLOR); $(1)
else
PRINT_CC = echo $(1); $(1)
endif

# Default installation location; we expect you to be overriding this
prefix ?= prefix
builddir ?= build