NAME=meCh
CC=gcc
CFLAGS=-O3 -Wall -Werror -Wextra -ansi -D_POSIX_C_SOURCE=200112L -pedantic -g
LDFLAGS=
LIBS=-lm

MODDIR = mod
_MODOBJ = utopia.o tell.o seen.o lastseen.o on.o fortune.o 1337.o autorejoin.o \
    sed.o ask.o
MODOBJ = $(patsubst %,$(MODDIR)/%,$(_MODOBJ))

DEPS = irc.h config.h module.h utils.h
OBJ = irc.o config.o module.o utils.o
MAIN = main.o


%.o: %.c $(DEPS)
	$(CC) -c -o $@ $(CFLAGS) $<

$(MODDIR)/%.o: $(MODDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $(CFLAGS) $<

all: $(OBJ) $(MODOBJ) $(MAIN)
	$(CC) -o $(NAME) $(LDFLAGS) $(OBJ) $(MODOBJ) $(MAIN) $(LIBS)

clean:
	rm *.o
	rm $(MODDIR)/*.o
	rm $(NAME)
.PHONY: clean
