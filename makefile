headers = $(SBWND_H)/sb_ds.h $(SBWND_H)/sbwnd_init.h $(SBWND_H)/sbwnd_create.h $(SBWND_H)/sbgui.h \
			$(SBWND_H)/sbwnd_general.h $(SBWND_H)/sbwnd_proc.h include/res/sbwnd_res.h

SBWND_H = include/sbwnd

CFLAGS = -I./include -Wall -Wextra -Wpedantic -Wno-unused-parameter -Wno-unused-variable

LIBS = -lgdiplus -lgdi32 -luser32 -lcomdlg32

bin/osubg.exe : obj/osubg.o obj/sb_ds.o obj/sbwnd_init.o obj/sbwnd_create.o obj/sbwnd_general.o obj/sbwnd_proc.o res/sbwnd_res.o
	md bin & gcc -o bin/osubg.exe obj/osubg.o obj/sb_ds.o obj/sbwnd_init.o obj/sbwnd_create.o obj/sbwnd_general.o obj/sbwnd_proc.o res/sbwnd_res.o $(LIBS)

obj/osubg.o : src/main/osubg.c obj $(headers)
	md obj & gcc -c -g -o obj/osubg.o src/main/osubg.c $(CFLAGS)

obj/sb_ds.o : src/sbwnd/sb_ds.c $(headers)
	gcc -c -g -o obj/sb_ds.o src/sbwnd/sb_ds.c $(CFLAGS)

obj/sbwnd_create.o : src/sbwnd/sbwnd_create.c $(headers)
	gcc -c -g -o obj/sbwnd_create.o src/sbwnd/sbwnd_create.c $(CFLAGS)

obj/sbwnd_init.o : src/sbwnd/sbwnd_init.c $(headers)
	gcc -c -g -o obj/sbwnd_init.o src/sbwnd/sbwnd_init.c $(CFLAGS)

obj/sbwnd_general.o : src/sbwnd/sbwnd_general.c $(headers)
	gcc -c -g -o obj/sbwnd_general.o src/sbwnd/sbwnd_general.c $(CFLAGS)

obj/sbwnd_proc.o : src/sbwnd/sbwnd_proc.c $(headers)
	gcc -c -g -o obj/sbwnd_proc.o src/sbwnd/sbwnd_proc.c $(CFLAGS)

res/sbwnd_res.o : res/sbwnd_res.rc include/res/sbwnd_res.h
	windres -o res/sbwnd_res.o -I include/res/ res/sbwnd_res.rc