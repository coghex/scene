CC=gcc
LDFLAGS=-framework OpenGL -framework GLUT

.c: ; $(CC) $@.c -o $@.o $(LDFLAGS)

clean: ; rm -f *.o
