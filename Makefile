# C Project Makefile

#========= Debug mode setup ========#
# debug.h macros.
DEBUG := -DDEBUG -DVERBOSE -UTRACE -DGLEW
NDEBUG := -UDEBUG -DVERBOSE -UTRACE -DGLEW

UNAME := $(shell uname)

#===== Compiler / linker setup =====#
# gcc with MinGW setup.
CC := gcc
CFLAGS := -g -O3 -Wall -Wpedantic -Wextra -std=c99
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
# CFILES excluses MAIN
CFILES := $(subst $(SRC_DIR)/main.c,,$(wildcard $(SRC_DIR)/*.c))
HFILES := $(wildcard $(SRC_DIR)/*.h)
IFILES := $(wildcard $(SRC_DIR)/*.inc)

# Important files
MAKEFILE := Makefile
IMPORTANT := $(MAKEFILE) README.md

#=========== Build setup ===========#
# Directory for built files.
BUILD_DIR := build
OFILES := $(CFILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
DFILES := $(OFILES:%.o=%.d)

# Main program to create
EXECUTABLE := ./main.exe

# Archive name to create
ARCHIVE := ./project.tar.gz

#============ Test files ===========#
# Standalone text executable sources
# to link with the rest of the code.
TEST_DIR := test
TCFILES := $(wildcard $(TEST_DIR)/*.c)
TOFILES := $(TCFILES:$(TEST_DIR)/%.c=$(BUILD_DIR)/$(TEST_DIR)/%.o)
TDFILES := $(TOFILES:%.o=%.d)
TESTS := $(TCFILES:$(TEST_DIR)/%.c=%.exe)

#========== Documentation ==========#
# Doxygen documentation setup
DOC_DIR := doc
WEB_DIR := web
DOXYFILE := Doxyfile
IMPORTANT += $(DOXYFILE) doc/main.html doc/main.dox

#============== Rules ==============#
# Default - make the executable
.PHONY: all
all: $(BUILD_DIR) $(EXECUTABLE) $(TESTS)

# Put all the .o files in the build directory
$(BUILD_DIR):
	-mkdir $@
	-mkdir $@/$(TEST_DIR)

# Generate the release build files
.SECONDARY: $(DFILES)
.SECONDARY: $(OFILES)
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(MAKEFILE)
	$(CC) $(CFLAGS) $(DFLAGS) $(DEBUG) $(INCLUDE) -c $< -o $@

.SECONDARY: $(TDFILES)
.SECONDARY: $(TOFILES)
$(TEST_DIR)/$(BUILD_DIR)/%.o: $(TEST_DIR)/%.c $(MAKEFILE)
	$(CC) $(CFLAGS) $(DFLAGS) $(DEBUG) $(INCLUDE) -c $< -o $@

# Automatic dependency files
-include $(DFILES)
-include $(TDFILES)

# Documentation
.PHONY: documentation
documentation: $(WEB_DIR)
$(WEB_DIR): $(DOXYGEN) $(HFILES)
	doxygen Doxyfile

# Make executable for each driver
$(EXECUTABLE): $(OFILES) $(BUILD_DIR)/main.o
	$(CC) $^ $(LIBRARY) $(LFLAGS) -o $@

%.exe: $(OFILES) $(BUILD_DIR)/$(TEST_DIR)/%.o
	$(CC) $^ $(LIBRARY) $(LFLAGS) -o $@

#============== Clean ==============#
# Clean up build files and executable
.PHONY: clean
clean:
	-rm -rf $(BUILD_DIR) $(TEST_DIR)/$(BUILD_DIR) $(WEB_DIR) *.exe $(ARCHIVE)
	
#============= Archive =============#
# Package all the files into a tar.
.PHONY: archive
archive: $(ARCHIVE)
$(ARCHIVE): $(CFILES) $(HFILES) $(IFILES) $(IMPORTANT)
	tar -czvf $@ $^

#===================================#
