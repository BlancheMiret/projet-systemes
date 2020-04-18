CC = gcc
CFLAGS = -Wall -g -std=c11
LDLIBS = -lm
PKGCONFIG = `pkg-config --cflags --libs glib-2.0`
PTHREAD = -lpthread
EXE = tagman tagmanager

all : $(EXE)

tagman : tagman.c
	$(CC) $(CFLAGS) tagman.c $(PKGCONFIG) -o tagman_exe

tagmanager : tags_manager.c
	$(CC) $(CFLAGS) tags_manager.c $(PKGCONFIG) -o tagmanager_exe


cleanall:
	rm -rf *~ $(ALL)
