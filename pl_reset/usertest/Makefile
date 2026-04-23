CC      = gcc
CFLAGS  = -Wall -Wextra -O2 -g -I./lib

TARGET  = test_pl_reset

LIBDIR  = lib
LIBSRC  = $(wildcard $(LIBDIR)/*.c)
LIBHDR  = $(wildcard $(LIBDIR)/*.h)

APP_SRC = $(TARGET).c
SRCS    = $(APP_SRC) $(LIBSRC)
OBJS    = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

%.o: %.c $(LIBHDR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean