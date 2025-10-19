# Banking System Makefile
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -pedantic
TARGET = banking_system
SOURCE = banking_system.c

# Default target
all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCE)

# Debug build
debug: CFLAGS += -g -DDEBUG
debug: $(TARGET)

# Release build
release: CFLAGS += -O2
release: $(TARGET)

# Install (copy to /usr/local/bin)
install: $(TARGET)
	sudo cp $(TARGET) /usr/local/bin/

# Clean build artifacts
clean:
	rm -f $(TARGET) *.o

# Run the application
run: $(TARGET)
	./$(TARGET)

# Format code (requires clang-format)
format:
	clang-format -i $(SOURCE)

.PHONY: all debug release install clean run format
