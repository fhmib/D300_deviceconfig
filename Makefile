CC = gcc
OBJECTS = dc_common.o dc_main.o dc_recv.o

devcfg: $(OBJECTS)
	$(CC) -o devcfg $(OBJECTS) -lpthread

dc_common.o: dc_common.h dc_common.c
	$(CC) -c dc_common.c

dc_main.o: dc_common.h dc_main.c
	$(CC) -c dc_main.c

dc_recv.o: dc_common.h dc_recv.c
	$(CC) -c dc_recv.c

clean:
	rm -f *.o *~ devcfg
