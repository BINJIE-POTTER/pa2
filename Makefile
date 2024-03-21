# Define compiler
CC=g++

# Define compiler flags
CFLAGS=-Wall -std=c++11

# Define the directory where the source file is located
SRCDIR=src

# Define the name of the executable output
TARGET=dvr

# Define source files
SOURCES=$(SRCDIR)/distancevector.cpp

# Define the build rule
all:
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET)

# Define a clean rule
clean:
	rm -f $(TARGET)

# Define a run rule (Assuming the executable requires 3 or 4 command line arguments)
run: all
	./$(TARGET) <topologyFile> <messageFile> <changesFile> [<outputFile>]
