OBJECTS_DIR=build/obj
SOURCES_DIR=src
EXE_DIR=build/out

EXECUTABLE=darkhouse

SOURCES = main.cpp
SOURCES += darkhouse.cpp
SOURCES += window.cpp
SOURCES += config.cpp
SOURCES += input.cpp


CC=g++
INCLUDES=-Iinclude/
LIB=-lxcb
OBJECTS=$(addprefix $(OBJECTS_DIR)/, $(SOURCES:.cpp=.o))
CFLAGS=-c -std=c++14 -Wall $(INCLUDES)
LDFLAGS=$(LIB)

$(OBJECTS_DIR)%.o : $(SOURCES_DIR)%.cpp
	$(CC) $(CFLAGS) $< -o $@

all: $(OBJECTS) $(EXECUTABLE)

$(OBJECTS) : | $(OBJECTS_DIR)

$(OBJECTS_DIR):
	mkdir -p $(OBJECTS_DIR)

$(EXECUTABLE) : $(OBJECTS) | $(EXE_DIR)
	$(CC) $^ -o $(EXE_DIR)/$@ $(LDFLAGS)

$(EXE_DIR):
	mkdir -p $(EXE_DIR)


.PHONY: clean

clean:
	rm -f $(OBJECTS_DIR)/*.o $(EXE_DIR)/*

run: all
	./$(EXE_DIR)/$(EXECUTABLE)
