CFLAGS = -I../../include

all: duk_cmdline

../libduktape.a:
	$(MAKE) -C .. libduktape.a

duk_cmdline: CFLAGS = -I../../include -DDUK_CMDLINE_PRINTALERT_SUPPORT
duk_cmdline: ../libduktape.a duk_print_alert.o -lm

clean:
	rm -f duk_cmdline

.PHONY: clean ../libduktape.a
