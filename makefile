CFLAGS = -I./include -Wall -Wextra -Wpedantic -Wno-unused-parameter -Wno-unused-variable -DUNICODE

LIBS = -lgdiplus -lgdi32 -luser32 -lcomdlg32

vpath %.h include/sbwnd include/res include/osubg-cmd
vpath %.c src/sbwnd src/main src/osubg-cmd
vpath %.o obj res
vpath %.rc res

objects = osubg.o sb_ds.o sbwnd_init.o sbwnd_create.o sbwnd_general.o sbwnd_proc.o sbwnd_res.o ofile.o

bin/osubg.exe : $(objects) | bin
	gcc -o $@ $^ $(LIBS)

obj/osubg.o : osubg.c sbgui.h | obj
	gcc -c -g -o $@ $< $(CFLAGS)

obj/sb_ds.o : sb_ds.c sb_ds.h | obj
	gcc -c -g -o $@ $< $(CFLAGS)

obj/sbwnd_create.o : sbwnd_create.c sbgui.h | obj
	gcc -c -g -o $@ $< $(CFLAGS)

obj/sbwnd_init.o : sbwnd_init.c sbgui.h | obj
	gcc -c -g -o $@ $< $(CFLAGS)

obj/sbwnd_general.o : sbwnd_general.c sbgui.h | obj
	gcc -c -g -o $@ $< $(CFLAGS)

obj/sbwnd_proc.o : sbwnd_proc.c sbgui.h | obj
	gcc -c -g -o $@ $< $(CFLAGS)

res/sbwnd_res.o : sbwnd_res.rc sbwnd_res.h
	windres -o $@ -I include/res/ $<

obj/ofile.o : ofile.c ofile.h | obj
	gcc -c -g -o $@ $< $(CFLAGS)

.PHONY: clean
clean : obj bin
	echo y | del obj\* bin\osubg.exe res\sbwnd_res.o

bin:
	md bin

obj:
	md obj