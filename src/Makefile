CC = gcc
CFLAGS = -Wall -g -std=c11
PKGCONFIG = `pkg-config --cflags --libs glib-2.0`
EXEC = main_exe

all : $(EXEC)

main_exe : main.c tag_hierarchy.c research.c tag_file.c tag_hierarchy.h research.h tag_file.h
	$(CC) $(CFLAGS) main.c research.c tag_hierarchy.c tag_file.c -o main_exe $(PKGCONFIG)

cleanall:
	rm -rf *~ $(EXEC)