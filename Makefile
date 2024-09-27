# Compiler and linker settings
CC = gcc
CXX = g++
AR = ar

# Paths to source files, object files, and library
INC_DIR = include
SRC_DIR = src
OBJ_DIR = obj
LIB_DIR = lib
TEST_DIR = tests
BIN_DIR = bin

# Define different build flags
CXXFLAGS_DBG = -g -O0 -Wall -DDEBUG -I$(INC_DIR) -I$(GTEST_DIR)/include
CXXFLAGS_REL = -O2 -Wall -DNDEBUG -I$(INC_DIR)
CXXFLAGS_SAN = -fsanitize=address,undefined -g -O0 -fno-omit-frame-pointer -Wall -I$(INC_DIR) -I$(GTEST_DIR)/include

LDFLAGS_DBG = -L$(LIB_DIR) -lchunked_list -lpthread
LDFLAGS_SAN = -fsanitize=address,undefined -L$(LIB_DIR) -lchunked_list -lpthread

# GTest library path
GTEST_DIR = /home/pnp/src/vcpkg/installed/x64-linux
GTEST_LIBS = -L$(GTEST_DIR)/lib -lgtest # -lgtest_main

# Source and object files for the chunked list
C_SOURCES = $(wildcard $(SRC_DIR)/*.c)
C_OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(C_SOURCES))

# Test files
C_TEST_SOURCES = $(TEST_DIR)/test_chunked_list_c.cpp
CPP_TEST_SOURCES = $(TEST_DIR)/test_chunked_list_cpp.cpp

# Targets
LIBRARY = $(LIB_DIR)/libchunked_list.a
C_TEST_EXEC = $(BIN_DIR)/test_chunked_list_c
CPP_TEST_EXEC = $(BIN_DIR)/test_chunked_list_cpp

# All target
# Default build target (can be rel, dbg, or san)
all: rel

# Debug build target
dbg: CFLAGS = $(CXXFLAGS_DBG)
dbg: CXXFLAGS = $(CXXFLAGS_DBG)
dbg: LDFLAGS = $(LDFLAGS_DBG)
dbg: $(LIBRARY) $(C_TEST_EXEC) $(CPP_TEST_EXEC)

# Release build target
rel: CFLAGS = $(CXXFLAGS_REL)
rel: CXXFLAGS = $(CXXFLAGS_REL)
rel: $(LIBRARY)

# AddressSanitizer build target
san: CFLAGS = $(CXXFLAGS_SAN)
san: CXXFLAGS = $(CXXFLAGS_SAN)
san: LDFLAGS = $(LDFLAGS_SAN)
san: $(LIBRARY) $(C_TEST_EXEC) $(CPP_TEST_EXEC)

# Run tests
test: san
	$(C_TEST_EXEC)
	$(CPP_TEST_EXEC)

# Clean up object files, libraries, and executables
clean:
	rm -rf $(OBJ_DIR) $(LIB_DIR) $(BIN_DIR)

.PHONY: all clean test dbg rel

# Create the static library
$(LIBRARY): $(C_OBJECTS) | $(LIB_DIR)
	$(AR) rcs $@ $^

# Compile C source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@
	
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile and link the C test executable
$(C_TEST_EXEC): $(C_TEST_SOURCES) $(LIBRARY) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(C_TEST_SOURCES) $(LDFLAGS) $(GTEST_LIBS) -o $@

# Compile and link the C++ test executable
$(CPP_TEST_EXEC): $(CPP_TEST_SOURCES) $(LIBRARY) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(CPP_TEST_SOURCES) $(LDFLAGS) $(GTEST_LIBS) -o $@

# Create object directory
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Create library directory
$(LIB_DIR):
	mkdir -p $(LIB_DIR)
	
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

