CC = gcc
TARGET = voogle
OBJECTS = voogle.c functions.o
.PHONY: all debug voogle clean
debug : 
	make CFLAGS="-W -Wall -DDEBUG" $(TARGET)
voogle :
CFLAGS = -W -Wall
$(TARGET) : $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^
	clear
	./voogle

clean:
	rm *.o voogle