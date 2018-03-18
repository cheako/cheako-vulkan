all: libcimgui.a imguicolortextedit/libtexteditor.a libduktape.a libwhereami.a

OBJSCIMGUI = cimgui/cimgui/cimgui.o cimgui/cimgui/fontAtlas.o cimgui/cimgui/drawList.o cimgui/cimgui/listClipper.o cimgui/imgui/imgui.o cimgui/imgui/imgui_draw.o cimgui/imgui/imgui_demo.o cimgui/imgui/examples/vulkan_example/imgui_impl_glfw_vulkan.o

cimgui/imgui/examples/vulkan_example/imgui_impl_glfw_vulkan.o: CXXFLAGS= -Icimgui/imgui -I../include

%.a:
	$(AR) rcUu $@ $+
	ranlib $@

libcimgui.a: $(OBJSCIMGUI)

override CFLAGS += -I../include
libduktape.a: duktape/src/duktape.o duktape/extras/module-duktape/duk_module_duktape.o
libwhereami.a: whereami/src/whereami.o

imguicolortextedit/libtexteditor.a: export CXXFLAGS= -I../cimgui/imgui -I../../include
imguicolortextedit/libtexteditor.a: imguicolortextedit/Makefile imguicolortextedit/TextEditor.h imguicolortextedit/TextEditor.cpp imguicolortextedit/ctexteditor.cpp imguicolortextedit/ctexteditor.h
	$(MAKE) -C imguicolortextedit libtexteditor.a

bin/dukdump: bin/Makefile bin/dindukdump.c libduktape.a
	$(MAKE) -C bin dukdump

clean:
	rm -f $(OBJSCIMGUI) libcimgui.a duktape/src/duktape.o libduktape.a libwhereami.a
	rm -f duktape/src/duktape.o whereami/src/whereami.o
	$(MAKE) -C imguicolortextedit clean
	$(MAKE) -C bin clean

.PHONY: all clean
