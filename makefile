UNAME := $(shell uname)

CC=gcc
ifeq ($(UNAME), Linux)
	LDFLAGS=-lglut -lGLU -lGL -lm
endif
ifeq ($(UNAME), Darwin)
	LDFLAGS=-framework OpenGL -framework GLUT -Wall
endif

all: ; $(CC) hw4.c -o hw4 $(LDFLAGS)

.c: ; $(CC) $@.c -o $@ $(LDFLAGS)

clean: ; rm -f hw4
