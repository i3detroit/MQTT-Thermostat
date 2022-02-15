CC=gcc
CFLAGS=-I. -I../src
TARGET=test

$(TARGET): $(TARGET).o doControl.o
	$(CC) -o $@ $^ $(CFLAGS)

doControl.o: ../src/doControl.cpp
	$(CC) -c -o $@ $< $(CFLAGS)

test.o: test.c
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm *.o $(TARGET)
