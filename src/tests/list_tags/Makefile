CC = gcc
CFLAGS = -Wall -g -std=c11
LDLIBS = -lm
PKGCONFIG = `pkg-config --cflags --libs glib-2.0`
PTHREAD = -lpthread
EXE = tagman

all : $(EXE)

tagman : tags_manager.c
	$(CC) $(CFLAGS) tags_manager.c $(PKGCONFIG) -o tagman


cleanall:
	rm -rf *~ $(ALL)
