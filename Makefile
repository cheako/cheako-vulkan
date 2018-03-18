# Makefile for cheako-vulkan

TOPTARGETS := all clean

SUBDIRS := $(shell dirname $(sort $(wildcard 0*/.)))

$(TOPTARGETS): lib $(SUBDIRS)
lib $(SUBDIRS):
	$(MAKE) -C $@ $(filter $(TOPTARGETS),$(MAKECMDGOALS))

DISTDIFFS := $(addsuffix .diff,$(filter-out $(firstword $(SUBDIRS)),$(SUBDIRS)))
dist:
	$(MAKE) clean
	rm -f */*.orig */*.rej */data_dynamic.c */imgui.ini */default_js.h
	rm -f 0*.diff
	$(MAKE) $(DISTDIFFS)
$(DISTDIFFS):
	diff -bNur $(shell echo $(basename $@) $(SUBDIRS) | tr \\n \  | awk -vRS=' ' '!f { f=$$0; next; }; $$0 == f { print l; exit; }; { l=$$0; }') $(basename $@) > $@; [ $$? -eq 1 ]

clean:

.PHONY: $(TOPTARGETS) lib $(SUBDIRS)
