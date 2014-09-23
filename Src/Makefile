# Makefile for Icon Chooser GTK+ 2.0 application.
# Date: 2008-09-26
# Author: William.L

#CC = gcc
PROG = IconChooser
HEADERS = CIconChooser.h

CC = g++
STRIP = strip
CFLAGS = `pkg-config --cflags gtk+-2.0 gdk-pixbuf-2.0`
LIBS = `pkg-config --libs gtk+-2.0 gdk-pixbuf-2.0`
       #Add "-lstdc++" parameter if using "gcc" to compile
# For 64-bit CPU architecture
CPU64 = -m64
GDB = -g

DEFINES = -DUSE_FILECHOOSER
DEFINES += -DTEST
DEFINES += -DDEBUG_MENU_ICONCHOOSER

iconchooser_OBJS = CIconChooser.o main.o

all: $(PROG)

$(PROG): $(iconchooser_OBJS)
	$(CC) -o $(PROG) $(iconchooser_OBJS) $(CFLAGS) $(LIBS)
#	$(CC) -o $(PROG) $(iconchooser_OBJS) $(CFLAGS) $(CPU64) $(LIBS)
# Add "-Xlinker --verbose" to gcc's command-line arguments to have it pass this option to ld.
	$(STRIP) $@

%.o: %.cpp $(HEADERS)
	echo Compiling $@...
	$(CC) $(DEFINES) $(CFLAGS) -c $< -o $@
#	$(CC) $(DEFINES) $(CFLAGS) $(CPU64) -c $< -o $@

.PHONY: clean
clean:
	rm -f *.o *.bak *~ *.~cpp *.~h $(PROG)

