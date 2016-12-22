#===================================#
# C Project Makefile
#===================================#

#========= Debug mode setup ========#
DEBUG := -DDEBUG -DVERBOSE -DTRACE
NDEBUG := -UDEBUG -DVERBOSE -UTRACE

#===== Compiler / linker setup =====#
CC := gcc
CFLAGS := -g -O3 -Wall -Wpedantic -Wextra -std=c99
DFLAGS := -MP -MMD
LFLAGS := -s -lm
INCLUDE := 
LIBRARY := 

#======== Source code setup ========#
SRC_DIR := src
INCLUDE += -I$(SRC_DIR)

# Source files
CFILES := $(subst $(SRC_DIR)/main.c,,$(wildcard $(SRC_DIR)/*.c))
HFILES := $(wildcard $(SRC_DIR)/*.h)

# Important files
MAKEFILE := Makefile
IMPORTANT := $(MAKEFILE) README.md

#=========== Build setup ===========#
BUILD_DIR := build
OFILES := $(CFILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
DFILES := $(OFILES:%.o=%.d)

# Main program to create
EXECUTABLE := ./main.exe

# Archive name to create
ARCHIVE := ./project.tar.gz

#========== Documentation ==========#
DOC_DIR := doc
DOXYFILE := Doxyfile
DOXYGEN := $(DOXYFILE) main.html
IMPORTANT += $(DOXYGEN)

#============== Rules ==============#
# Default - make the executable
.PHONY: all
all: $(BUILD_DIR) $(EXECUTABLE) $(TESTS)

# Put all the .o files in the build directory
$(BUILD_DIR):
	-mkdir $@
	-mkdir $(TEST_DIR)/$@

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
$(DOC_DIR): $(DOXYGEN) $(HFILES)
	doxygen Doxyfile

# Make executable for each driver
$(EXECUTABLE): $(OFILES) $(BUILD_DIR)/main.o
	$(CC) $^ $(LIBRARY) $(LFLAGS) -o $@

#============== Clean ==============#
# Clean up build files and executable
.PHONY: clean
clean:
	-rm -rf $(BUILD_DIR) $(TEST_DIR)/$(BUILD_DIR) $(DOC_DIR) *.exe $(ARCHIVE)
	
#============= Archive =============#
# Package all the files into a tar.
.PHONY: archive
archive: $(ARCHIVE)
$(ARCHIVE): $(CFILES) $(HFILES) $(IMPORTANT)
	tar -czvf $@ $^

#===================================#
