# Compiler and linker settings
CC = gcc
CXX = g++
AR = ar
CFLAGS = -Wall -Werror -g -I./include
CXXFLAGS = -Wall -Werror -g -I./include -I$(GTEST_DIR)/include
LDFLAGS = -L./lib -lchunked_list -lpthread

# Paths to source files, object files, and library
SRC_DIR = ./src
OBJ_DIR = ./obj
LIB_DIR = ./lib
TEST_DIR = ./tests

# GTest library path
GTEST_DIR = /home/pnp/src/vcpkg/installed/x64-linux
GTEST_LIBS = -L$(GTEST_DIR)/lib -lgtest # -lgtest_main

# Source and object files for the chunked list
C_SOURCES = $(SRC_DIR)/chunked_list.c
C_OBJECTS = $(OBJ_DIR)/chunked_list.o

# Test files
C_TEST_SOURCES = $(TEST_DIR)/test_chunked_list_c.cpp
CPP_TEST_SOURCES = $(TEST_DIR)/test_chunked_list_cpp.cpp

# Targets
LIBRARY = $(LIB_DIR)/libchunked_list.a
C_TEST_EXEC = test_chunked_list_c
CPP_TEST_EXEC = test_chunked_list_cpp

# All target
all: $(LIBRARY) $(C_TEST_EXEC) $(CPP_TEST_EXEC)

# Create the static library
$(LIBRARY): $(C_OBJECTS) | $(LIB_DIR)
	$(AR) rcs $@ $^

# Compile C source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile and link the C test executable
$(C_TEST_EXEC): $(C_TEST_SOURCES) $(LIBRARY)
	$(CXX) $(CXXFLAGS) $(C_TEST_SOURCES) $(LDFLAGS) $(GTEST_LIBS) -o $@

# Compile and link the C++ test executable
$(CPP_TEST_EXEC): $(CPP_TEST_SOURCES) $(LIBRARY)
	$(CXX) $(CXXFLAGS) $(CPP_TEST_SOURCES) $(LDFLAGS) $(GTEST_LIBS) -o $@

# Create object directory
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Create library directory
$(LIB_DIR):
	mkdir -p $(LIB_DIR)

# Run tests
test: all
	./$(C_TEST_EXEC)
	./$(CPP_TEST_EXEC)

# Clean up object files, libraries, and executables
clean:
	rm -rf $(OBJ_DIR) $(LIB_DIR) $(C_TEST_EXEC) $(CPP_TEST_EXEC)

.PHONY: all clean test
