# Whether to turn compiler warnings into errors
export WERROR ?= true
export BUILD_DIR ?= build

default: release

release:
	mkdir -p ./$(BUILD_DIR) && cd ./$(BUILD_DIR) && cmake ../ -DCMAKE_BUILD_TYPE=Release -DWERROR=$(WERROR) && VERBOSE=1 cmake --build .

debug:
	mkdir -p ./$(BUILD_DIR) && cd ./$(BUILD_DIR) && cmake ../ -DCMAKE_BUILD_TYPE=Debug -DWERROR=$(WERROR) && VERBOSE=1 cmake --build .

clean:
	rm -rf ./$(BUILD_DIR)
	# remove remains from running 'make coverage'
	rm -f *.profraw
	rm -f *.profdata

format:
	./scripts/format.sh

.PHONY: format
