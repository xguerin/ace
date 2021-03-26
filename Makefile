BUILD_DIR      ?= build
INSTALL_PREFIX ?= /usr/local
CMAKE_OPTIONS  ?= -DCMAKE_INSTALL_PREFIX=$(INSTALL_PREFIX)
MAKE_OPTIONS   ?= -j 4
GTEST_ROOT     ?= $(HOME)/.local

LITE_OPTIONS   ?= -DACE_PLUGIN_HJSON=OFF -DACE_PLUGIN_LUA=OFF -DACE_PLUGIN_PYTHON=OFF -DACE_PLUGIN_YAML=OFF
TEST_OPTIONS   ?= -DCMAKE_BUILD_TYPE=Debug -DACE_BUILD_TESTS=ON -DGTEST_ROOT=$(GTEST_ROOT) 

default: build

all: clean build test lint

clean:
	@rm -rf $(BUILD_DIR) site

prepare:
	@if [ ! -e $(BUILD_DIR) ];		\
 	then													\
		mkdir -p $(BUILD_DIR);			\
		cd $(BUILD_DIR);						\
		cmake $(CMAKE_OPTIONS) ..;	\
	fi

prepare-lite:
	@if [ ! -e $(BUILD_DIR) ];										\
 	then																					\
		mkdir -p $(BUILD_DIR);											\
		cd $(BUILD_DIR);														\
		cmake $(CMAKE_OPTIONS) $(LITE_OPTIONS) ..;	\
	fi

prepare-test:
	@if [ ! -e $(BUILD_DIR) ];										\
	then																					\
		mkdir -p $(BUILD_DIR);											\
 		cd $(BUILD_DIR);														\
  	cmake $(CMAKE_OPTIONS) $(TEST_OPTIONS) ..;	\
 	fi

build: prepare
	@cd $(BUILD_DIR) && make $(MAKE_OPTIONS)

build-lite: prepare-lite
	@cd $(BUILD_DIR) && make $(MAKE_OPTIONS)

test: prepare-test build
	@cd $(BUILD_DIR) && make test

lint: prepare
	@BUILD_DIR=$(BUILD_DIR) .travis/lint.sh

install: build
	@cd $(BUILD_DIR) && make install

info:
	@echo "         clean: clean the build directory"
	@echo "       prepare: prepare the build directory"
	@echo "         build: build the project"
	@echo "  prepare-test: prepare the build directory with tests enabled"
	@echo "          test: test the project"
	@echo "          lint: lint the project"
