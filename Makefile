CC = gcc
CFLAGS = -Wall -Wextra -I/usr/local/include
LDFLAGS = -L/usr/local/lib -lraylib
TARGET = cam_sim 
SRCS = main.c

motion = false
color = red 

all: build
	
build:
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET) $(LDFLAGS)
	./$(TARGET) $(color) $(motion)

clean:
	rm -rf $(TARGET)
