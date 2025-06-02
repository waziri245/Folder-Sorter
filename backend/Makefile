CC = gcc
SRC = folder_sorter.c
OUTDIR = ../shared
TARGET = $(OUTDIR)/folder_sorter.so

CFLAGS = -Wall -shared -fPIC

all:
	mkdir -p $(OUTDIR)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)
