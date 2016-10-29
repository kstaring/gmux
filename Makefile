CC	=cc
LD	=cc
CFLAGS	=-g -O2 -Wall
LDFLAGS	=
SRCS	=gmux.c
OBJS	=$(SRCS:.c=.o)
PROG	=gmux

all: $(PROG)

$(PROG): $(OBJS)
	$(LD) -o $(PROG) $(LDFLAGS) $(OBJS)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

install: all
	install -s -o root -g wheel -m 4755 gmux /usr/local/bin

clean:
	rm -f $(OBJS) $(PROG) *.core
