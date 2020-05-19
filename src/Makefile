CC = gcc
CFLAGS = -Wall -g -std=c11
PKGCONFIG = `pkg-config --cflags --libs glib-2.0`
EXEC = main_exe

all : $(EXEC)

main_exe : main.c tag_hierarchy.c research.c tag_hierarchy.h research.h
	$(CC) $(CFLAGS) main.c tag_hierarchy.c research.c -o main_exe $(PKGCONFIG)

cleanall:
	rm -rf *~ $(EXEC)