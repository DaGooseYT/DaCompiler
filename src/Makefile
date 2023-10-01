CFLAGS = -static -fcommon -Wall -ggdb3
SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)

dacompiler: $(OBJS)
	gcc -o $@ $(OBJS) $(CFLAGS)

clean:
	rm -f dacompiler *.o *~

.PHONY: clean