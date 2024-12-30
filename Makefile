CC := gcc

# Get build type from cmdline
# Default to debug @TODO
BUILD_TYPE ?= test

CFLAGS_COMMON := -Wall -Werror -Isrc
CFLAGS_DEBUG := -g -O0

# set compile flags
ifeq ($(BUILD_TYPE), debug)
	CFLAGS := $(CFLAGS_COMMON) $(CFLAGS_DEBUG)
else
	CFLAGS := $(CFLAGS_COMMON)
endif

# Source directory
SRC_DIR := src
# Objects directory
OBJ_DIR := build/obj
#Binary directory
BIN_DIR := build/bin

# Source files
SRC_FILES := $(wildcard $(SRC_DIR)/*.c)
# Object files
SRC_OBJS := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/src/%.o, $(SRC_FILES))
# Exclude main entry for test runs
SRC_OBJS_EXCLUDE_MAIN_FUNC := $(filter-out $(OBJ_DIR)/src/main.o, $(SRC_OBJS))

# Test directory
TEST_DIR := tests
# Test files
TEST_FILES := $(wildcard $(TEST_DIR)/*.c)
# Place Test binaries in a single dir
TEST_OBJS := $(patsubst $(TEST_DIR)/%.c, $(OBJ_DIR)/tests/%.o, $(TEST_FILES))
TEST_MAIN := $(TEST_DIR)/test.c

# =====================================
.PHONY: test clean

$(OBJ_DIR)/src/%.o: $(SRC_DIR)/%.c
	mkdir -p $(OBJ_DIR)/src
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/tests/%.o: $(TEST_DIR)/%.c
	mkdir -p $(OBJ_DIR)/tests
	$(CC) $(CFLAGS) -c $< -o $@

# create a static lib for testing without main.c
$(OBJ_DIR)/wizzydb.a: $(SRC_OBJS_EXCLUDE_MAIN_FUNC)
	mkdir -p $(OBJ_DIR)
	ar rcs $@ $^

# Link lib in test
$(OBJ_DIR)/testBin: $(OBJ_DIR)/wizzydb.a $(TEST_OBJS)
	@echo $(TEST_OBJS)
	$(CC) $^ -o $@

test: $(OBJ_DIR)/testBin
	@./$(OBJ_DIR)/testBin

clean:
	rm -rf build
