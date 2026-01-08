TARGET 	:= main
SRCS		:= $(wildcard *.c)
OBJS		:= $(SRCS:.c=.o)

CC			?= gcc
CFLAGS	?= -g -Wall -Wextra
LDFLAGS ?= -lpthread -lc
.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS)

%.o: $.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
