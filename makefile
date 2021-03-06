
# find the os
UNAME_S := $(shell uname -s)

# main compiler
CXX := g++
CC := gcc
# CXX := clang --analyze # and comment out the linker last line for sanity

# define the directories
SRCDIR := src
INCDIR := include
BUILDDIR := build
BINDIR := bin
LIBDIR := lib

# define common variables
SRCEXT := c
SOURCES := $(shell find $(SRCDIR) -type f -iname "*.$(SRCEXT)" )
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))


# flags
CFLAGS := -g -lz # -Wall
DEVICE_CLIENT_VERSION := "0.0"
DEVICE_TYPE := "UNKNOWN"
override CFLAGS += -D 'DEVICE_CLIENT_VERSION="$(DEVICE_CLIENT_VERSION)"' -D 'DEVICE_TYPE="$(DEVICE_TYPE)"'

INC := $(shell find $(INCDIR) -maxdepth 1 -type d -exec echo -I {}  \;)
#LIB := $(LIB) -L$(LIBDIR) -loniondebug -lonioni2c

ifeq ($(UNAME_S),Darwin)
	# only add this when compiling on OS X
	INC += -I $(LIBDIR)
endif

# define specific binaries to create
APP0 := device-client
TARGET := $(BINDIR)/$(APP0)



all: info $(TARGET)

$(TARGET): $(OBJECTS) 
	@mkdir -p $(BINDIR)
	@echo " Linking..."
	$(CC) $^ $(CFLAGS) $(LDFLAGS) -o $(TARGET) $(LIB)


# generic: build any object file required
$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(dir $@)
	@echo " $(CC) $(CFLAGS) $(INC) -c -o $@ $<"; $(CC) $(CFLAGS) $(INC) -c -o $@ $<

clean:
	@echo " Cleaning..."; 
	$(RM) -r $(BUILDDIR) $(BINDIR) $(LIB0_TARGET)

info:
	@echo "CC: $(CC)"
	@echo "CFLAGS: $(CFLAGS)"
	@echo "LDFLAGS: $(LDFLAGS)"
	@echo "LIB: $(LIB)"
	@echo "INC: $(INC)"
	@echo "SOURCES: $(SOURCES)"
	@echo "OBJECTS: $(OBJECTS)"


# Tests
tester:
	$(CC) $(CFLAGS) test/tester.cpp $(INC) $(LIB) -o bin/tester

# Spikes
#ticket:
#  $(CXX) $(CXXFLAGS) spikes/ticket.cpp $(INC) $(LIB) -o bin/ticket

.PHONY: clean
