# C Project Makefile

#========= Debug mode setup ========#
# debug.h macros.
DEBUG := -DDEBUG -DVERBOSE -UTRACE
NDEBUG := -UDEBUG -DVERBOSE -UTRACE

#===== Compiler / linker setup =====#
# gcc with MinGW setup.
CC := gcc
CFLAGS := -g -O3 -Wall -Wpedantic -Wextra -std=gnu99
DFLAGS := -MP -MMD
LFLAGS := -s -lm
INCLUDE := 
LIBRARY := 

#======== Source code setup ========#
# Directory for all project files and
# the main.c file.
SRC_DIR := src
INCLUDE += -I$(SRC_DIR)

# Source files
# CFILES excludes MAIN
CFILES := $(subst $(SRC_DIR)/main.c,,$(wildcard $(SRC_DIR)/*.c))
HFILES := $(wildcard $(SRC_DIR)/*.h)

# Important files
MAKEFILE := Makefile

#=========== Build setup ===========#
# Directory for built files.
BUILD_DIR := build
OFILES := $(CFILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
DFILES := $(OFILES:%.o=%.d)

# Main program to create
EXECUTABLE := ./battleship.exe

#========== Documentation ==========#
# Doxygen documentation setup
DOC_DIR := docs
DOXYFILE := Doxyfile
DOXFILES := $(wildcard doxygen/*)

#============== Rules ==============#
# Default - make the executable
.PHONY: all
all: $(BUILD_DIR) $(EXECUTABLE) $(TESTS)

# Put all the .o files in the build directory
$(BUILD_DIR):
	-mkdir $@

# Generate the release build files
.SECONDARY: $(DFILES)
.SECONDARY: $(OFILES)
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(MAKEFILE)
	$(CC) $(CFLAGS) $(DFLAGS) $(DEBUG) $(INCLUDE) -c $< -o $@

# Automatic dependency files
-include $(DFILES)

# Documentation
.PHONY: documentation
documentation: $(DOC_DIR)
$(DOC_DIR): $(DOXFILES) $(CFILES) $(HFILES) $(MCFILES)
	doxygen Doxyfile

# Make executable for each driver
$(EXECUTABLE): $(OFILES) $(BUILD_DIR)/main.o
	$(CC) $^ $(LIBRARY) $(LFLAGS) -o $@

#============== Clean ==============#
# Clean up build files and executable
.PHONY: clean
clean:
	-rm -rf $(BUILD_DIR) $(EXECUTABLE)

#===================================#
