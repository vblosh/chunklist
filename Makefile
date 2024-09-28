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
LIB_SOURCES = $(wildcard $(SRC_DIR)/*.c)
LIB_OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(LIB_SOURCES))

# Source and object files tests
TEST_SOURCES = $(wildcard $(TEST_DIR)/*.cpp)
TEST_OBJECTS = $(patsubst $(TEST_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(TEST_SOURCES))

# Targets
LIBRARY = $(LIB_DIR)/libchunked_list.a
TEST_EXEC = $(BIN_DIR)/tests

# All target
# Default build target (can be rel, dbg, or san)
all: rel

# Debug build target
dbg: CFLAGS = $(CXXFLAGS_DBG)
dbg: CXXFLAGS = $(CXXFLAGS_DBG)
dbg: LDFLAGS = $(LDFLAGS_DBG)
dbg: $(LIBRARY) $(TEST_EXEC)

# Release build target
rel: CFLAGS = $(CXXFLAGS_REL)
rel: CXXFLAGS = $(CXXFLAGS_REL)
rel: $(LIBRARY)

# AddressSanitizer build target
san: CFLAGS = $(CXXFLAGS_SAN)
san: CXXFLAGS = $(CXXFLAGS_SAN)
san: LDFLAGS = $(LDFLAGS_SAN)
san: $(LIBRARY) $(TEST_EXEC)

# Run tests
test: san
	$(TEST_EXEC)

# Clean up object files, libraries, and executables
clean:
	rm -rf $(OBJ_DIR) $(LIB_DIR) $(BIN_DIR)

.PHONY: all clean test dbg rel

# Compile C source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@
	
$(OBJ_DIR)/%.o: $(TEST_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Create the static library
$(LIBRARY): $(LIB_OBJECTS)
	mkdir -p $(LIB_DIR)
	$(AR) rcs $@ $^

# Rule to build the executable
$(TEST_EXEC): $(TEST_OBJECTS)
	@mkdir -p $(BIN_DIR)   # Create the bin directory if it doesn't exist
	@echo $(TEST_OBJECTS)
	$(CXX) $(TEST_OBJECTS) $(LDFLAGS) $(GTEST_LIBS) -o $@

# Create object directory
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

