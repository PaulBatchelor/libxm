OBJ=context.o load.o play.o xm.o
CFLAGS=-DXM_LINEAR_INTERPOLATION=1 -DXM_DEBUG=0
LDFLAGS=

default: libxm.a

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

libxm.a: $(OBJ)
	ar rcs $@ $(OBJ)

clean:
	rm -rf libxm.a $(OBJ)
