# Define compiler
CC=g++

# Define compiler flags
CFLAGS=-Wall -std=c++11

# Define the directory where the source file is located
SRCDIR=src

# Define the name of the executable output
TARGET1=dvr
TARGET2=lsr

# Define source files
SOURCES1=$(SRCDIR)/distancevector.cpp
SOURCES2=$(SRCDIR)/lsr.cpp

# Define the build rule
all: $(TARGET1) $(TARGET2)

$(TARGET1): $(SOURCE1)
	$(CC) $(CFLAGS) $(SOURCES1) -o $(TARGET1)

$(TARGET2): $(SOURCE2)
	$(CC) $(CFLAGS) $(SOURCES2) -o $(TARGET2)

# Define a clean rule
clean:
	rm -f $(TARGET1) $(TARGET2)

# Define a run rule (Assuming the executable requires 3 or 4 command line arguments)
run_dvr: $(TARGET1)
	./$(TARGET1) <topologyFile> <messageFile> <changesFile> [<outputFile>]

run_lsr: $(TARGET2)
	./$(TARGET2) <topologyFile> <messageFile> <changesFile> [<outputFile>]
