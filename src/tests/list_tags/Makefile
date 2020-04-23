CC = gcc
CFLAGS = -Wall -g -std=c11
LDLIBS = -lm
PKGCONFIG = `pkg-config --cflags --libs glib-2.0`
PTHREAD = -lpthread
EXE = test

all : $(EXE)

tagman : tagman.c
	$(CC) $(CFLAGS) tagman.c $(PKGCONFIG) -o tagman_exe


test : tag_manager_test.c tag_manager_test.h test.c
	$(CC) $(CFLAGS) test.c tag_manager_test.c $(PKGCONFIG) -o test_list_tag_exe


cleanall:
	rm -rf *~ $(ALL)
