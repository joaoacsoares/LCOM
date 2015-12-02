COMPILER_TYPE = gnu

CC = gcc

PROG = projeto
SRCS = main.c Numbers.c Graphics.c Utilities.c VBE.c Mouse.c Bitmap.c timer.c keyboard.c MainMenu.c OptionsMenu.c Game.c Whack.c

CCFLAGS= -Wall -O3

DPADD += ${LIBDRIVER} ${LIBSYS}
LDADD += -llm -ldriver -lsys

LDFLAGS += -L.

MAN=
BINDIR? = /usr/sbin

.include <bsd.prog.mk>
.include <bsd.gcc.mk>