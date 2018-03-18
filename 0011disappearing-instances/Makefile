# Makefile for vulkan

# Our single executable
OBJS = vulkan

all: $(OBJS)

include ../misc-targets.mk

DEPENDS = libdata.so
vulkan: -lpthread $(MYLDLIBS) $(DEPENDS)
LIBDEPENDS_DYNAMIC = 0_vert.spv 0_frag.spv
data_dynamic.o: $(LIBDEPENDS_DYNAMIC) data.h
LIBDEPENDS = data_bulk.o data_dynamic.o
libdata.so: libdata.c $(LIBDEPENDS)
	$(LINK.c) -shared -fPIC $(OUTPUT_OPTION) $^

clean:
	rm -f $(OBJS) $(DEPENDS) $(LIBDEPENDS) $(LIBDEPENDS_DYNAMIC)


# valgrind discovers bugs, google it.
test: clean all
	valgrind ./vulkan

.PHONY: all clean test
