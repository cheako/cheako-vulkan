# Makefile targets for cheako-vulkan

.SUFFIXES: .c.in .c      # Add .c.in and .c as suffixes

M4       = m4
M4FLAGS  =
M4SCRIPT =

%.c: %.c.in
	${M4} ${M4FLAGS} ${M4SCRIPT} $< > $*.c

%vert.spv: %vert.glsl
	glslangValidator -S vert -V $< -o $*vert.spv

%frag.spv: %frag.glsl
	glslangValidator -S frag -V $< -o $*frag.spv

vert.spv: vert.glsl
	glslangValidator -S vert -V $< -o vert.spv

frag.spv: frag.glsl
	glslangValidator -S frag -V $< -o frag.spv

LDFLAGS = -Wl,-rpath,\$$ORIGIN -L.
MYLDLIBS = -lglfw -lvulkan -lm

# Include debug symbols and warn on all
CFLAGS = -g -Wall -I../include $(MYCFLAGS)

../lib/libcimgui.a ../lib/imguicolortextedit/libtexteditor.a ../lib/libduktape.a:
	$(MAKE) -C ../lib $(@:../lib/%=%)

../lib/bin/duk_cmdline: ../lib/bin/duk_cmdline.c
	$(MAKE) -C ../lib/bin duk_cmdline

%.duk: %.js ../lib/bin/duk_cmdline
	../lib/bin/duk_cmdline -c $@ $<

%-duk.h: %.duk
	xxd -i $< > $@

%.h: %.js
	xxd -i $< > $@
