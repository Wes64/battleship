#===============================================================
# battleship Makefile
#===============================================================

#===============================================================
# Definitions
#===============================================================
EXECUTABLE := battleship.exe
SOURCE_DIR := src
OBJECT_DIR := build

# Compiler options
CC := gcc
CFLAGS := -g -O3 -Wall -MP -MMD
LDFLAGS := -g

# File locations
CFILES := $(wildcard $(SOURCE_DIR)/*.c)
OBJFILES := $(addprefix $(OBJECT_DIR)/, $(subst $(SOURCE_DIR)/,,$(CFILES:.c=.o)))

#===============================================================
# Dependencies
#===============================================================
INCLUDE := -Isrc
LINK_LIBS :=

#===============================================================
# Rules
#===============================================================
.PHONY: all
all: $(EXECUTABLE)

# Make the executable program
$(EXECUTABLE): $(OBJFILES)
	$(CC) $(LDFLAGS) $(LINK_LIBS) $(OBJFILES) -o $@

# Make build directory
$(OBJECT_DIR):
	-mkdir $@

# Make object files
$(OBJECT_DIR)/%.o: $(SOURCE_DIR)/%.c $(OBJECT_DIR)
	$(CC) $(CFLAGS) $(INCLUDE) -c -o $@ $<

# Clean up
.PHONY: clean
clean:
	-rm -rf $(OBJECT_DIR) $(EXECUTABLE)

#===============================================================