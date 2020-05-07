CC = gcc
CFLAGS = -Wall -g -std=c11
LDLIBS = -lm
PKGCONFIG = `pkg-config --cflags --libs glib-2.0`
EXEC = main_exe

all : $(EXEC)

main_exe : main.c tag_manager.c
	$(CC) $(CFLAGS) main.c tag_manager.c -o main_exe $(PKGCONFIG)

cleanall:
	rm -rf *~ $(EXEC)