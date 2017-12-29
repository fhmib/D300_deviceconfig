CC = gcc
OBJECTS = dc_common.o dc_main.o dc_recv.o dc_cfg.o

all: $(OBJECTS)
	$(CC) -o devcfg $(OBJECTS) -lpthread
	$(CC) -o read dc_test_r.c
	$(CC) -o write dc_test_w.c
	$(CC) -o rmmsg dc_rmmsg.c


dc_common.o: dc_common.h dc_common.c
	$(CC) -c dc_common.c

dc_main.o: dc_common.h dc_main.c
	$(CC) -c dc_main.c

dc_recv.o: dc_common.h dc_recv.c
	$(CC) -c dc_recv.c

dc_cfg.o: dc_common.h dc_cfg.c
	$(CC) -c dc_cfg.c

clean:
	rm -f *.o *~ devcfg read write rmmsg
