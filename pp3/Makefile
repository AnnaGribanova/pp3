CC = g++
CFLAGS = -Wall -g
EXECUTABLE = Main

all: $(EXECUTABLE)

$(EXECUTABLE): pp3.cpp ThreadPool.cpp ThreadPool.h
	$(CC) $(CFLAGS) pp3.cpp ThreadPool.cpp -o $(EXECUTABLE)

clean:
	rm -f $(EXECUTABLE)

.PHONY: all clean
