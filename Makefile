CC = gcc
CFLAGS = -Wall -Wextra -g -I.
LDFLAGS = 
TARGET = bin/server

# Find all .c files in the project
SOURCES = $(wildcard *.c) \
          $(wildcard http/*.c) \
          $(wildcard methods/get/*.c) \
          $(wildcard socket/*.c) \
          $(wildcard utils/*.c)

# Replace .c with .o to get the object file names
OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@mkdir -p $(dir $@)
	@echo "  LD $@"
	@$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS) $(LDFLAGS)
	@echo "✓ Build successful! Executable at ./bin/server"

%.o: %.c
	@echo "  CC $< "
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "  CLEAN"
	@rm -f $(OBJECTS) $(TARGET)
	@echo "✓ Clean complete."

.PHONY: all clean
