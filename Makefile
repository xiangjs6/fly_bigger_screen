CC=gcc
SRCS=$(wildcard *.c */*.c)
OBJS=$(patsubst %.c, %.o, $(SRCS))
FLAG=-g
NAME=$(wildcard *.c)
TARGET_NAME=fly_bigger_screen
LIB_PATH=.
LIB_NAME_LIST=-levdi -lSDL
TARGET=$(patsubst %.c, %, $(TARGET_NAME))

$(TARGET):$(OBJS)
	$(CC) -o $@ $^ $(FLAG) -L $(LIB_PATH) $(LIB_NAME_LIST)

%.o:%.c
	$(CC) -o $@ -c $< -g

clean:
	rm -rf $(TARGET) $(OBJS)

