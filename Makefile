BUILD_DIR     ?= _build
CMAKE_OPTIONS ?= 
MAKE_OPTIONS  ?= -j 4
GTEST_ROOT    ?= $(HOME)/.local

default: build

all: clean build test lint

clean:
	@rm -rf $(BUILD_DIR)

prepare:
	@if [ ! -e $(BUILD_DIR) ];			\
 	then													\
		mkdir -p $(BUILD_DIR);			\
		cd $(BUILD_DIR);						\
		cmake $(CMAKE_OPTIONS) ..;	\
	fi

prepare-test:
	@if [ ! -e $(BUILD_DIR) ];																											\
	then																																					\
		mkdir -p $(BUILD_DIR);																											\
 		cd $(BUILD_DIR);																														\
  	cmake $(CMAKE_OPTIONS) -DACE_BUILD_TESTS=ON -DGTEST_ROOT=$(GTEST_ROOT) ..;	\
 	fi

build: prepare
	@cd $(BUILD_DIR) && make $(MAKE_OPTIONS)

test: prepare-test build
	@cd $(BUILD_DIR) && make test

lint: prepare
	@BUILD_DIR=$(BUILD_DIR) .travis/lint.sh

info:
	@echo "         clean: clean the build directory"
	@echo "       prepare: prepare the build directory"
	@echo "         build: build the project"
	@echo "  prepare-test: prepare the build directory with tests enabled"
	@echo "          test: test the project"
	@echo "          lint: lint the project"
