CFLAGS=-shared -fPIC -I../src
LDFLAGS+=-lsporth -lsoundpipe -lsndfile -lm -ljack -lpthread 
LDFLAGS += ../src/libxm.a
CONFIG ?= 

include $(CONFIG)

xm.so: xm.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) 

clean:
	rm -rf xm.so
