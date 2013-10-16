UNAME := $(shell uname)

CC=gcc
ifeq ($(UNAME), Linux)
	LDFLAGS=-lglut -lGLU -lGL -lm
endif
ifeq ($(UNAME), Darwin)
	LDFLAGS=-framework OpenGL -framework GLUT -Wall
endif

all: ; $(CC) hw5.c -o hw5 $(LDFLAGS)

.c: ; $(CC) $@.c -o $@ $(LDFLAGS)

clean: ; rm -f hw5
