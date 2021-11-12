BUILD_DIR      ?= build
INSTALL_PREFIX ?= /usr/local
CMAKE_OPTIONS  ?= -GNinja -DCMAKE_INSTALL_PREFIX=$(INSTALL_PREFIX)
GTEST_ROOT     ?= $(HOME)/.local

LITE_OPTIONS   ?= -DACE_PLUGIN_HJSON=OFF -DACE_PLUGIN_LUA=OFF -DACE_PLUGIN_PYTHON=OFF -DACE_PLUGIN_YAML=OFF
TEST_OPTIONS   ?= -DCMAKE_BUILD_TYPE=Debug -DACE_BUILD_TESTS=ON -DACE_ENABLE_ASAN=ON -DACE_ENABLE_UBSAN=ON -DGTEST_ROOT=$(GTEST_ROOT) 
EXTR_OPTIONS   ?=

default: build

all: clean build test lint

clean:
	@rm -rf $(BUILD_DIR) site

mkdir:
	@if [ ! -e $(BUILD_DIR) ]; then mkdir -p $(BUILD_DIR); fi

prepare: mkdir
	@cmake -B $(BUILD_DIR) $(CMAKE_OPTIONS) $(EXTR_OPTIONS) .

prepare-lite: mkdir
	@cmake -B $(BUILD_DIR) $(CMAKE_OPTIONS) $(LITE_OPTIONS) $(EXTR_OPTIONS) .;

prepare-test: mkdir
	@cmake -B $(BUILD_DIR) $(CMAKE_OPTIONS) $(TEST_OPTIONS) $(EXTR_OPTIONS) .;

build: prepare
	@ninja -C $(BUILD_DIR)

build-lite: prepare-lite
	@ninja -C $(BUILD_DIR)

test: prepare-test build
	@ninja -C $(BUILD_DIR) test

lint: prepare
	@BUILD_DIR=$(BUILD_DIR) .travis/lint.sh

install: build
	@ninja -C $(BUILD_DIR) install

info:
	@echo "         clean: clean the build directory"
	@echo "       prepare: prepare the build directory"
	@echo "         build: build the project"
	@echo "  prepare-test: prepare the build directory with tests enabled"
	@echo "          test: test the project"
	@echo "          lint: lint the project"
