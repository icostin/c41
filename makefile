# [c41] makefile for Unix-like systems
# Changelog:
# * 2013/01/04 Costin Ionescu: initial commit

N := c41
D := C41

ifeq ($(CC),)
CC := gcc
endif

ifeq ($(LINK),)
LINK := gcc
endif

ifeq ($(PREFIX_DIR),)
PREFIX_DIR:=$(HOME)/.local
endif

src_list := src/misc.c src/ma.c src/pmvops.c src/array.c src/str.c src/io.c src/smt.c src/fsi.c src/fsp.c src/ilc.c src/ucw8.c src/rbtree.c src/esm.c
hdr_list := $(wildcard include/*.h) $(wildcard include/$(N)/*.h)
sl_obj_list := $(patsubst src/%.c,out/sl/%.o,$(src_list))
dl_obj_list := $(patsubst src/%.c,out/dl/%.o,$(src_list))

.PHONY: all libs install uninstall dlib slib tests sltest dltest clean tags arc

CFLAGS := -O3 -fvisibility=hidden -fomit-frame-pointer -Iinclude -Wall -Wextra -Werror -DNDEBUG -DHAVE_STRING_H
LIB_CFLAGS := $(CFLAGS) -ffreestanding -nostartfiles -nostdlib
SL_CFLAGS := $(LIB_CFLAGS) -D$(D)_STATIC
DL_CFLAGS := $(LIB_CFLAGS) -D$(D)_DLIB_BUILD -fpic

all: libs tests

libs: dlib slib

tests: sltest dltest

arc:
	cd .. && tar -Jcvf $(N).txz $(N)/src $(N)/include $(N)/make* $(N)/README* $(N)/LICENCE

install: libs tests
	mkdir -p $(PREFIX_DIR)/lib
	mkdir -p $(PREFIX_DIR)/include
	cp -v out/dl/lib$(N).so $(PREFIX_DIR)/lib/
	[ `whoami` != root ] || ldconfig
	cp -v out/sl/lib$(N).a $(PREFIX_DIR)/lib/
	-rm -rf $(PREFIX_DIR)/include/c41
	cp -vr include $(PREFIX_DIR)/

uninstall:
	-rm -f $(PREFIX_DIR)/lib/lib$(N).so
	-rm -f $(PREFIX_DIR)/lib/lib$(N).a
	-rm -rf $(PREFIX_DIR)/include/c41.h $(PREFIX_DIR)/include/c41
	[ `whoami` != root ] || ldconfig

dlib: out/dl/lib$(N).so

slib: out/sl/lib$(N).a

dltest: out/dl/test dlib
	LD_LIBRARY_PATH=out/dl nice -n 19 $<

sltest: out/sl/test slib
	nice -n 19 $<

clean:
	-rm -rf out tags

tags:
	ctags -R --fields=+iaS --extra=+q --exclude='.git' .

out out/dl out/sl:
	mkdir -p $@

out/dl/lib$(N).so: $(dl_obj_list) | out/dl
	$(LINK) -shared -ffreestanding -nostartfiles -nostdlib -Wl,--entry,c41_lib_entry -Wl,-soname,lib$(N).so -o$@ $^
	strip $@

$(dl_obj_list): out/dl/%.o: src/%.c $(hdr_list) | out/dl
	$(CC) -c $(DL_CFLAGS) $< -o $@

out/dl/test: src/test.c $(hdr_list) | out/dl
	$(CC) -o$@ $(CFLAGS) src/test.c -Lout/dl -l$(N)

out/sl/lib$(N).a: $(sl_obj_list) | out/sl
	ar rcs $@ $^

$(sl_obj_list): out/sl/%.o: src/%.c $(hdr_list) | out/sl
	$(CC) -c $(SL_CFLAGS) $< -o $@

out/sl/test: src/test.c $(hdr_list) out/sl/lib$(N).a | out/sl
	$(CC) -o$@ $(CFLAGS) -D$(D)_STATIC src/test.c -static -Lout/sl -l$(N)

