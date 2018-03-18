ARCHIVES = libcimgui.a imguicolortextedit/libtexteditor.a \
	libduktape.a libwhereami.a vk_mem_alloc.a

all: $(ARCHIVES)

OBJSCIMGUI = cimgui/cimgui/cimgui.o cimgui/cimgui/fontAtlas.o cimgui/cimgui/drawList.o cimgui/cimgui/listClipper.o cimgui/imgui/imgui.o cimgui/imgui/imgui_draw.o cimgui/imgui/imgui_demo.o cimgui/imgui/examples/vulkan_example/imgui_impl_glfw_vulkan.o

cimgui/imgui/examples/vulkan_example/imgui_impl_glfw_vulkan.o: CPPFLAGS= -Icimgui/imgui -I../include

%.a:
	$(AR) rcUu $@ $+
	ranlib $@

libcimgui.a: $(OBJSCIMGUI)

override CPPFLAGS += -I../include
libduktape.a: duktape/src/duktape.o duktape/extras/module-duktape/duk_module_duktape.o
libwhereami.a: whereami/src/whereami.o

imguicolortextedit/libtexteditor.a: export CPPFLAGS += -I../cimgui/imgui -I../../include
imguicolortextedit/libtexteditor.a: imguicolortextedit/Makefile imguicolortextedit/TextEditor.h imguicolortextedit/TextEditor.cpp imguicolortextedit/ctexteditor.cpp imguicolortextedit/ctexteditor.h
	$(MAKE) -C imguicolortextedit libtexteditor.a

bin/dukdump: bin/Makefile bin/dindukdump.c libduktape.a
	$(MAKE) -C bin dukdump

define VK_MEM_ALLOC_SOURCE
#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

endef
export VK_MEM_ALLOC_SOURCE
vk_mem_alloc.o:
	printf "$$VK_MEM_ALLOC_SOURCE" | $(COMPILE.cpp) $(OUTPUT_OPTION) -x c++ -

vk_mem_alloc.a: vk_mem_alloc.o

clean:
	rm -f $(OBJSCIMGUI) duktape/src/duktape.o whereami/src/whereami.o \
		vk_mem_alloc.o
	rm -f $(ARCHIVES)
	$(MAKE) -C imguicolortextedit clean
	$(MAKE) -C bin clean

.PHONY: all clean
