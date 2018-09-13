SOURCES_DIR := src
INCLUDE_DIR := include
OBJECTS_DIR := build/obj
BIN_DIR := build/out

SOURCE_FILES := main.cpp
SOURCE_FILES += darkhouse.cpp
SOURCE_FILES += window.cpp
SOURCE_FILES += config.cpp
SOURCE_FILES += input.cpp

LIBS := xcb X11 cairo
EXECUTABLE := darkhouse

COMPILE_FLAGS = -std=c++11 -Wall
LINKER_FLAGS = 
CC := g++

####################################################################################

OBJECTS := $(addprefix $(OBJECTS_DIR)/, $(SOURCE_FILES:.cpp=.o))
SOURCES := $(addprefix $(SOURCES_DIR)/, $(SOURCE_FILES))

CFLAGS := -c -I$(INCLUDE_DIR)/
LDFLAGS := $(addprefix -l, $(LIBS))

INCLUDE_FILES := $(shell find $(INCLUDE_DIR)/ -type f -name '*.hpp')
DEPENDENCIES := $(INCLUDE_FILES)

$(OBJECTS_DIR)%.o : $(SOURCES_DIR)%.cpp $(DEPENDENCIES)
	$(CC) $(CFLAGS) $< -o $@

all: $(OBJECTS) $(EXECUTABLE)

$(OBJECTS) : | $(OBJECTS_DIR)

$(OBJECTS_DIR):
	mkdir -p $(OBJECTS_DIR)

$(EXECUTABLE) : $(OBJECTS) | $(BIN_DIR)
	$(CC) $^ -o $(BIN_DIR)/$@ $(LDFLAGS)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)


.PHONY: clean

clean:
	rm -f $(OBJECTS_DIR)/*.o $(BIN_DIR)/*

run: all
	./$(BIN_DIR)/$(EXECUTABLE)
