CC := cc
CPPFLAGS := -D_XOPEN_SOURCE_EXTENDED=1 -Iinclude
CFLAGS := -Wall -Wextra -pedantic
LDLIBS := -lncurses

BUILD_DIR := build
TARGET := $(BUILD_DIR)/music
SOURCES := main.c src/music.c
HEADERS := include/music.h

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS) | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SOURCES) -o $(TARGET) $(LDLIBS)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
	rmdir $(BUILD_DIR) 2>/dev/null || true
