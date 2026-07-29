#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

/* Used for directory creation */
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>

#include <sys/stat.h>
#include <sys/types.h>

#include "../../modules/c-vector/cvector.h"
#include "../../modules/linenoise/linenoise.h"

#include "../color/color.h"
#include "../xerror/xerror.h"
#include "../xmem/xmem.h"

#include "config/config.h"
#include "util/util.h"

#include "forge.h"

/* Project string */
#define FORGE "forge"

/* Internal directories and files */
#define FORGE_DIR ".forge"
#define FORGE_BIN_DIR FORGE_DIR "/bin"
#define FORGE_BIN_MODULES_DIR "/modules"
#define FORGE_BIN_DEBUG_DIR FORGE_BIN_DIR "/debug"
#define FORGE_BIN_DEBUG_OBJ_DIR FORGE_BIN_DEBUG_DIR "/obj"
#define FORGE_BIN_RELEASE_DIR FORGE_BIN_DIR "/release"
#define FORGE_BIN_RELEASE_OBJ_DIR FORGE_BIN_RELEASE_DIR "/obj"

#define FORGE_MAKEFILE (FORGE_DIR "/Makefile")
#define FORGE_MODULES (FORGE_DIR "/modules.mk")
#define FORGE_ADD (FORGE_DIR "/.add.mk")

/* Project directories and files */
#define SRC_DIR "src"
#define MODULES_DIR "modules"
#define MAIN_FILE SRC_DIR "/main.c"

/* Make related arguments */
#define MAKE "make"
#define MAKE_FILE_FLAG "-f"
#define MAKE_RUN "run"
#define MAKE_DEBUG "debug"
#define MAKE_RELEASE "release"
#define MAKE_CLEAN "clean"
#define MAKE_MODULES "modules"

/* Error messages */
#define RUN_ERROR   COLOR(FORGE, BWHITE) ": " COLOR("run error", BRED)
#define BUILD_ERROR COLOR(FORGE, BWHITE) ": " COLOR("build error", BRED)
#define INIT_ERROR  COLOR(FORGE, BWHITE) ": " COLOR("init error", BRED)
#define CLEAN_ERROR COLOR(FORGE, BWHITE) ": " COLOR("clean error", BRED)
#define MODULE_ERROR COLOR(FORGE, BWHITE) ": " COLOR("module error", BRED)


/* Makefile source string
 *
 * NOTE:
 * 	Go towards the bottom to see entire string
 */
static const char *makefile;

/* modules.mk source string
 *
 * NOTE:
 * 	Go towards the bottom to see entire string
 *
 */
static const char *modules;

/* .add.mk source string
 *
 * NOTE:
 * 	Go twoards the bottom to see entire string
 */
static const char *add;

/* Basic source code string
 *
 * NOTE:
 * 	Go towards the bottom to see entire string
 */
static const char *code;

/* Array for easy directory iteration */
static char *directories[] = {
        FORGE_DIR,
	FORGE_BIN_DIR,
	FORGE_BIN_MODULES_DIR,
	FORGE_BIN_DEBUG_DIR,
	FORGE_BIN_DEBUG_OBJ_DIR,
	FORGE_BIN_RELEASE_DIR,
	FORGE_BIN_RELEASE_OBJ_DIR,
	SRC_DIR,
	MODULES_DIR,
};

/* Flag to signify if the project has already been built.
 * This is used when no mode is specified with `build`
 */
static bool built = false;

void forge_run(int argc, char **argv) {
	Config config = config_read();
	cvector(char *) args = NULL;
	char *path = NULL;
	char *exe = strjoin(config.info.name, ".bin");

	forge_build_debug();
	printf("Running...\n");
	path = strjoin(FORGE_BIN_DEBUG_DIR "/", exe);

	if(path != NULL) {
		cvector_push_back(args, path);

		for(size_t i = 1; i != (size_t) argc; i += 1) {
			cvector_push_back(args, argv[i]);
		}

		cvector_push_back(args, NULL);
		shell_execute(args[0], args);
		cvector_free(args);
		free(path);
		free(exe);
	}

	config_delete(&config);
}

void forge_build() {
	if(built == false) {
		printf("Build mode not specified, defaulting to debug...\n");
		shell_execute(MAKE, (char *[]) { MAKE, MAKE_FILE_FLAG, FORGE_MAKEFILE, MAKE_DEBUG, NULL, });
	}
}

void forge_build_debug() {
	printf("Building a debug version...\n");
	built = true;
	shell_execute(MAKE, (char *[]) { MAKE, MAKE_FILE_FLAG, FORGE_MAKEFILE, MAKE_DEBUG, NULL, });
}

void forge_build_release() {
	printf("Building a release version...\n");
	built = true;
	shell_execute(MAKE, (char *[]) { MAKE, MAKE_FILE_FLAG, FORGE_MAKEFILE, MAKE_RELEASE, NULL, });
}

void forge_clean() {
	printf("Cleaning...\n");
	shell_execute(MAKE, (char *[]) { MAKE, MAKE_FILE_FLAG, FORGE_MAKEFILE, MAKE_CLEAN, NULL, });
}

void forge_new(int argc, char **argv) {
	char *name = NULL;
	FILE *fp = NULL;

	if(argc != 2) {
		exit(EXIT_FAILURE);
	}

	name = argv[1];

        if(strcmp(name, ".") == 0) {
                if(access(CONFIG_FILE, F_OK) == 0) {
                        xerrorf(INIT_ERROR, "project `%s` already exists\n", name);
                }
        } else if(mkdir(name, S_IRWXU) != 0) {
                xerror(INIT_ERROR);
        } else if(chdir(name) == -1) {
                xerror(INIT_ERROR);
        }

        if(config_write(name) == false) {
                xerrorf(INIT_ERROR, "failed initializing a new project\n", NULL);
                exit(EXIT_FAILURE);
        }

	if(mkdir(FORGE_DIR, S_IRWXU) != 0) {
                xerrorf(INIT_ERROR, "failed initializing a new project\n", NULL);
                exit(EXIT_FAILURE);
	}

	for(size_t i = 0; i < sizeof(directories) / sizeof(directories[0]); i++) {
		DIR *dir = opendir(directories[i]);

		if(dir != NULL) {
			closedir(dir);
		} else if(errno == ENOENT) {
			if(mkdir(directories[i], S_IRWXU) != 0) {
				xerror(INIT_ERROR);
			}
		} else {
			xerror(FORGE);
		}
	}
	
	fp = fopen(FORGE_MAKEFILE, "w+");

	if(fp != NULL) {
		fprintf(fp, makefile, name);
		fclose(fp);
		fp = fopen(MAIN_FILE, "w+");

		if(fp != NULL) {
			fprintf(fp, "%s", code);
			fclose(fp);

			fp = fopen(FORGE_MODULES, "w+");

			if(fp != NULL) {
				fprintf(fp, "%s", modules);
				fclose(fp);
			}
		}
	}
}

void forge_module_build() {
	printf("Building modules...\n");
	shell_execute(MAKE, (char *[]) { MAKE, MAKE_FILE_FLAG, FORGE_MAKEFILE, MAKE_MODULES, NULL, });
}

void forge_module_verify() {
	Config config = config_read();

	for(Module *it1 = cvector_begin(config.modules); it1 != cvector_end(config.modules); it1 += 1) {
		printf("Name: '%s', Source: '%s'\n", it1->name, it1->source);
	}

	config_delete(&config);
}

void forge_module_add() {
	char *name = NULL;
	char *source = NULL;
	Config config = config_read();
	char *editor = NULL;
	FILE *fp = NULL;

	while(true) {
		bool found = false;

		while(((name = linenoise("Name: ")) == NULL) || (name[0] == '\0')) {
			fprintf(stderr, "Module must have a name!");
		}

		for(Module *it1 = cvector_begin(config.modules); it1 != cvector_end(config.modules); it1 += 1) {
			if(!strcmp(name, it1->name)) {
				fprintf(stderr, "module '%s' is already defined!\n", name);
				xfree(name);
				found = true;
				break;
			}
		}

		if(!found) {
			break;
		}
	}

	while((source = linenoise("Source: ")) == NULL) {
		fprintf(stderr, "Module must have a source!\n");
	}

	for(Module *it1 = cvector_begin(config.modules); it1 != cvector_end(config.modules); it1 += 1) {
		if(!strcmp(source, it1->source)) {
			fprintf(stderr, "module source '%s' is already defined, this may cause issues!\n", source);
			break;
		}
	}

	fp = fopen(FORGE_ADD, "w+");

	if(fp == NULL) {
		xerror(MODULE_ERROR);
	}

	fprintf(fp, add, source, name);
	fclose(fp);

	// Get editor
	editor = getenv("FORGE_EDITOR");

	if(editor == NULL) {
		editor = getenv("EDITOR");

		if(editor == NULL) {
			editor = getenv("VISUAL");

			if(editor == NULL) {
				editor = "vi";
			}
		}
	}

	shell_execute(editor, (char *[]) { editor, FORGE_ADD, NULL });
	//shell_execute("tail", (char *[]) { "tail", "-n", "+2", FORGE_ADD, ">>", FORGE_MODULES, NULL });
	cvector_push_back(config.modules, ((Module) { .name = name, .source = source}));
	config_update(config);
	config_delete(&config);
	xfreev(name, source, NULL);
	remove(FORGE_ADD);
}

static const char *code =
"#include <stdio.h>\n"
"\n"
"\n"
"int main() {\n"
"\tprintf(\"Hello world!\\n\");\n"
"\n"
"\treturn 0;\n"
"}\n";

static const char *makefile = 
".ONESHELL:\n"
".SILENT:\n"
"\n"
"# Project name\n"
"PROJECT := %s\n"
"\n"
"# Final executable\n"
"EXE := $(PROJECT).bin\n"
"\n"
"# Utilities\n"
"MKDIR := mkdir\n"
"RM := rm\n"
"ECHO := echo\n"
"\n"
"# Directories\n"
"ROOT_DIR := \n"
"\n"
"# Internal forge directories\n"
"FORGE_DIR := .forge\n"
"FORGE_BIN_DIR := $(FORGE_DIR)/bin\n"
"FOREGE_BIN_MODULES_DIR := $(FORGE_BIN_DIR)/modules\n"
"FORGE_BIN_DEBUG_DIR := $(FORGE_BIN_DIR)/debug\n"
"FORGE_BIN_DEBUG_OBJ_DIR := $(FORGE_BIN_DEBUG_DIR)/obj\n"
"FORGE_BIN_RELEASE_DIR := $(FORGE_BIN_DIR)/release\n"
"FORGE_BIN_RELEASE_OBJ_DIR := $(FORGE_BIN_RELEASE_DIR)/obj\n"
"\n"
"SRC_DIR := src\n"
"MODULES_DIR := modules\n"
"\n"
"# Compiler\n"
"CC := clang\n"
"\n"
"# Source file type\n"
"SRC_TYPE := c\n"
"\n"
"# Source files\n"
"SRCS := $(shell find $(SRC_DIR) -type f -name \"*.$(SRC_TYPE)\")\n"
"DEBUG_OBJS := $(addprefix $(FORGE_BIN_DEBUG_OBJ_DIR)/,$(SRCS:src/%%.$(SRC_TYPE)=%%.o))\n"
"RELEASE_OBJS := $(addprefix $(FORGE_BIN_RELEASE_OBJ_DIR)/,$(SRCS:src/%%.$(SRC_TYPE)=%%.o))\n"
"\n"
"# Compilation flags\n"
"LINKER_FLAGS :=\n"
"INCLUDE_FLAGS := -I$(MODULES_DIR)\n"
"RELEASE_FLAGS := -Wextra -Wall -Werror -Wno-vla -std=c99 -O2 $(LINKER_FLAGS) $(INCLUDE_FLAGS) -DNDEBUG\n"
"DEBUG_FLAGS := -Wextra -Wall -Wno-vla -std=c99 -g3 $(LINKER_FLAGS) $(INCLUDE_FLAGS)\n"
"\n"
"# Arguments to be passed when running \"make run Args=\"<arguments>\"\n"
"ARGS :=\n"
"\n"
".PHONY: all run debug release clean directories clean-release clean-debug\n"
"\n"
"all: debug\n"
"\n"
"run: debug\n"
"\t./$(FORGE_BIN_DEBUG_DIR)/$(EXE) $(ARGS)\n"
"\n"
"debug: directories $(DEBUG_OBJS)\n"
"\t$(CC) $(DEBUG_FLAGS) $(DEBUG_OBJS) $(FORGE_BIN_MODULES_DIR)/*.o -o $(FORGE_BIN_DEBUG_DIR)/$(EXE)\n"
"\n"
"release: directories clean-release $(RELEASE_OBJS)\n"
"\t$(CC) $(RELEASE_FLAGS) $(RELEASE_OBJS) $(FORGE_BIN_MODULES_DIR)/*.o -o $(FORGE_BIN_RELEASE_DIR)/$(EXE)\n"
"\n"
"clean: clean-release clean-debug\n"
"\n"
"directories:\n"
"\t$(MKDIR) -p $(FORGE_DIR)\n"
"\t$(MKDIR) -p $(FORGE_BIN_DIR)\n"
"\t$(MKDIR) -p $(FORGE_BIN_MODULES_DIR)\n"
"\t$(MKDIR) -p $(FORGE_BIN_DEBUG_DIR)\n"
"\t$(MKDIR) -p $(FORGE_BIN_DEBUG_OBJ_DIR)\n"
"\t$(MKDIR) -p $(FORGE_BIN_RELEASE_DIR)\n"
"\t$(MKDIR) -p $(FORGE_BIN_RELEASE_OBJ_DIR)\n"
"\t$(MKDIR) -p $(SRC_DIR)\n"
"\t$(MKDIR) -p $(MODULES_DIR)\n"
"\n"
"clean-release:\n"
"\t$(RM) -f $(RELEASE_OBJS)\n"
"\t$(RM) -f $(FORGE_BIN_RELEASE_DIR)/$(EXE)\n"
"\t$(RM) -rf $(FORGE_BIN_RELEASE_OBJ_DIR)/*\n"
"\n"
"clean-debug:\n"
"\t$(RM) -f $(DEBUG_OBJS)\n"
"\t$(RM) -f $(FORGE_BIN_DEBUG_DIR)/$(EXE)\n"
"\t$(CC) $(DEBUG_FLAGS) -c $< -o $@\n"
"\n"
"$(RELEASE_OBJS): $(FORGE_BIN_RELEASE_OBJ_DIR)/%%.o: $(SRC_DIR)/%%.$(SRC_TYPE)\n"
"\t$(MKDIR) -p $(dir $@)\n"
"\t$(CC) $(RELEASE_FLAGS) -c $< -o $@\n"
"\n"
"# Modules\n"
"include modules\n"
"build-modules:";

static const char *modules =
"FORGE_DIR := $(CURDIR)/.forge\n"
"FORGE_BIN_DIR := $(FORGE_DIR)/bin\n"
"FORGE_BIN_MODULES_DIR := $(FORGE_BIN_DIR)/modules\n"
"\n"
"SRC_DIR := $(CURDIR)/src\n"
"MODULES_DIR= $(CURDIR)/modules\n"
"\n";

/*
FORGE_DIR := $(CURDIR)/.forge
FORGE_BIN_DIR := $(FORGE_DIR)/bin
FORGE_BIN_MODULES_DIR := $(FORGE_BIN_DIR)/modules

SRC_DIR := $(CURDIR)/src
MODULES_DIR= $(CURDIR)/modules
*/
static const char *add =
"# $(FORGE_DIR) = .forge/, $(FORGE_BIN_MODULES_DIR) = .forge/bin/modules/, $(MODULES_DIR) = modules/\n"
"\n"
"# %s\n"
"%s:\n"
"";
