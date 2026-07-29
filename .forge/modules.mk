FORGE_DIR := $(CURDIR)/.forge
FORGE_BIN_DIR := $(FORGE_DIR)/bin
FORGE_BIN_MODULES_DIR := $(FORGE_BIN_DIR)/modules

SRC_DIR := $(CURDIR)/src
MODULES_DIR= $(CURDIR)/modules

# https://github.com/eteran/c-vector.git
c-vector:
	mkdir -p $(MODULES_DIR)/c-vector
	cd /tmp/
	git clone https://github.com/eteran/c-vector.git &> /dev/null
	cd c-vector
	cp cvector.h cvector_utils.h $(MODULES_DIR)/c-vector
	cd /tmp/
	rm -rf c-vector/

# https://github.com/hippie68/optparse99.git
optparse99:
	mkdir -p $(MODULES_DIR)/optparse99
	cd /tmp/
	git clone https://github.com/hippie68/optparse99.git &> /dev/null
	cd optparse99
	gcc *.c -c optparse99.o -Wextra -Werror -Wall -O2 &> /dev/null
	cp optparse99.o $(FORGE_BIN_MODULES_DIR)
	cd /tmp/optparse99/
	cp optparse99.h $(MODULES_DIR)/optparse99
	cd /tmp/
	rm -rf optparse99/

# https://github.com/cktan/tomlc99.git
tomlc99:
	mkdir -p $(MODULES_DIR)/tomlc99
	cd /tmp/
	git clone https://github.com/cktan/tomlc99.git &> /dev/null
	cd tomlc99
	make &> /dev/null
	cp toml.o $(FORGE_BIN_MODULES_DIR)
	cd /tmp/tomlc99/
	cp toml.h $(MODULES_DIR)/tomlc99
	cd /tmp/
	rm -rf tomlc99/
