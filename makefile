cc=gcc
cflags=-w -g -c

objs=main.o scan.o parse.o util.o

debug.exe: $(objs)
	$(cc) $(cflag) $(objs) -o debug.exe

main.o: main.c globals.h
	$(cc) $(cflags) main.c

scan.o: scan.c scan.h util.h globals.h
	$(cc) $(cflags) scan.c
parse.o: parse.c parse.h scan.h globals.h
	$(cc) $(cflags) parse.c
util.o: util.c globals.h
	$(cc) $(cflags) util.c