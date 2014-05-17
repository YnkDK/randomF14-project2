# The following flags are a direct copy from the Makefile
# for the SFMT pseudorandom number generator used
WARN = -Wmissing-prototypes -Wall
OPTI = -O3 -finline-functions -fomit-frame-pointer -DNDEBUG \
-fno-strict-aliasing --param max-inline-insns-single=1800
STD = -std=c99
CC = gcc
CCFLAGS = $(OPTI) $(WARN) $(STD)
CCFLAGS += --param inline-unit-growth=500 \
--param large-function-growth=900
SSE2FLAGS = -msse2 -DHAVE_SSE2

# Target ouput(s)
MYS_TARGET = mysMultisetEquality

mys: mysMultisetEquality.c SFMT.c SFMT.h SFMT-sse2.h \
	SFMT-params19937.h
	$(CC) $(CCFLAGS) $(SSE2FLAGS) -DSFMT_MEXP=19937 -o $@ SFMT.c mysMultisetEquality.c
