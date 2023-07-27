PROYECTO=proc2
DESTDIR=/usr/local/bin
CFLAGS=-Wall
LFLAGS=-lm
CC=gcc
#OBJS:=$(patsubst %.c,%.o,$(wildcard *.c))
SRC:=$(wildcard *.c)

all: $(PROYECTO)

%.o: %.c
	$(CC) -c $< $(CFLAGS) -l bcm2835 -pthread

#$(PROYECTO): $(OBJS)
$(PROYECTO): $(SRC:.c=.o)
	$(CC) $^ -o $@ $(LFLAGS) -l bcm2835 -pthread

.PHONY: clean

install: all
	if [ ! -d $(DESTDIR) ]; then \
		sudo mkdir $(DESTDIR); \
	fi; \
	sudo cp $(PROYECTO) $(DESTDIR)

uninstall:
	sudo rm /usr/local/bin/$(PROYECTO)

clean:
	rm -f *.o *.txt $(PROYECTO)
