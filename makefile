UNAME := $(shell uname)

CC=gcc
ifeq ($(UNAME), Linux)
	LDFLAGS=-lglut -lGLU -lGL -lm
endif
ifeq ($(UNAME), Darwin)
	LDFLAGS=-framework OpenGL -framework GLUT -Wall -Wno-deprecated
endif

all: ; $(CC) scene.c -o scene $(LDFLAGS)

.c: ; $(CC) $@.c -o $@ $(LDFLAGS)

clean: ; rm -f scene
