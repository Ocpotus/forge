#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "../../../modules/c-vector/cvector.h"
#include "../../../modules/tomlc99/toml.h"

#include "../../xmem/xmem.h"
#include "../../xerror/xerror.h"
#include "../../color/color.h"

#include "../util/util.h"

#include "config.h"

#define CONFIG_ERROR COLOR("forge", BWHITE) ": " COLOR("config error", BRED)

#define TEMPORARY_CONFIG_FILE ".config.toml"


/* Reads the project info of TOML file
 */
static void parse_project_info(Config *cfg, toml_table_t *cfgp);
static void parse_modules(Config *cfg, toml_table_t *cfgp);

/* Reads a TOML file.
 *
 * @param path: file path to read
 *
 * RETURNS:
 * 	A pointer to a parse TOML file.
 */
static toml_table_t *read_toml_table(const char *path);

Config config_read() {
	Config result = {
		.info = {
			.name = NULL,
			.version = NULL,
			.author = NULL,
		},
		.modules = NULL,
	};
	toml_table_t *config = read_toml_table(CONFIG_FILE);

	if(config == NULL) {
		xerrorf(CONFIG_ERROR, "failed to parse `config.toml`\n");
	}

	parse_project_info(&result, config);
	parse_modules(&result, config);
	toml_free(config);

	return result;
}

void config_delete(Config *cfg) {
	cfg->info.author = xfree(cfg->info.author);
	cfg->info.version = xfree(cfg->info.version);
	cfg->info.name = xfree(cfg->info.name);

	for(Module *it1 = cvector_begin(cfg->modules); it1 != cvector_end(cfg->modules); it1 += 1) {
		xfreev(it1->name, it1->source, NULL);
	}

	cvector_free(cfg->modules);
}

bool config_write(const char *project) {
	bool result = false;
	FILE *fp = fopen(CONFIG_FILE, "w+");

	if(fp != NULL) {
		fprintf(fp, "[project]\n");

		if(project[0] != '.') {
			fprintf(fp, "\tname = \"%s\"\n", project);
		} else {
			if(project[1] == '\0') {
				char *currDir = get_dir_name();

				if(currDir == NULL) {
					result = false;

					goto EXIT;
				}

				fprintf(fp, "\tname = \"%s\"\n", currDir);
				free(currDir);
			} else {
				fprintf(fp, "\tname = \"%s\"\n", project + 1);
			}
		}

		fprintf(fp, "\tauthor = \"\"\n");
		fprintf(fp, "\tversion = \"0.1.0\"\n\n");
		result = true;
	}

EXIT:
	fclose(fp);

	return result;
}

bool config_update(Config config) {
	bool result = true;
	FILE *fp = fopen(TEMPORARY_CONFIG_FILE, "w");

	if(fp != NULL) {
		fprintf(fp, "[project]\n");
		fprintf(fp, "\tname = \"%s\"\n", config.info.name);
		fprintf(fp, "\tauthor = \"%s\"\n", config.info.author);
		fprintf(fp, "\tversion = \"%s\"\n\n", config.info.version);

		for(Module *it1 = cvector_begin(config.modules); it1 != cvector_end(config.modules); it1 += 1) {
			fprintf(fp, "[[module]]\n");
			fprintf(fp, "\tname = \"%s\"\n", it1->name);
			fprintf(fp, "\tsource = \"%s\"\n", it1->source);
			fprintf(fp, "\n");
		}

		fclose(fp);
	}

	shell_execute("mv", (char *[]) { "mv", TEMPORARY_CONFIG_FILE, CONFIG_FILE, NULL });
	remove(TEMPORARY_CONFIG_FILE);

	return result;
}

static void parse_project_info(Config *cfg, toml_table_t *cfgp) {
	toml_table_t *info = toml_table_in(cfgp, "project");
	toml_datum_t field = { 0 };

	if(info == NULL) {
		xerrorf(CONFIG_ERROR, "failed to find [project] in `config.toml`\n");
	}

	field = toml_string_in(info, "name");

	if(!field.ok) {
		xerrorf(CONFIG_ERROR, "failed to find `name` in [project]\n");
	}

	cfg->info.name = field.u.s;
	field = toml_string_in(info, "author");

	if(!field.ok) {
		xerrorf(CONFIG_ERROR, "failed to find `author` in [project]\n");
	}

	cfg->info.author = field.u.s;
	field = toml_string_in(info, "version");

	if(!field.ok) {
		xerrorf(CONFIG_ERROR, "failed to find `version` in [project]\n");
	}

	cfg->info.version = field.u.s;
}

static void parse_modules(Config *cfg, toml_table_t *cfgp) {
	toml_array_t *modules = toml_array_in(cfgp, "module");

	if(modules == NULL) {
		return;
	}

	for(int idx = 0; true; idx += 1) {
		Module nm = { 0 };
		toml_table_t *module = toml_table_at(modules, idx);
		toml_datum_t field = { 0 };

		if(module == NULL) {
			break;
		}

		field = toml_string_in(module, "name");

		if(!field.ok) {
			xerrorf(CONFIG_ERROR, "failed to find module::%d name!\n", idx);
		}

		nm.name = field.u.s;
		field = toml_string_in(module, "source");

		if(!field.ok) {
			xerrorf(CONFIG_ERROR, "failed to find module::%d source!\n", idx);
		}

		nm.source = field.u.s;
		cvector_push_back(cfg->modules, nm);
	}
}

static toml_table_t *read_toml_table(const char *path) {
	toml_table_t *result = NULL;
	FILE *fp = fopen(path, "r");

	if(fp == NULL) {
		xerror(CONFIG_ERROR);
	}

	result = toml_parse_file(fp, NULL, 0);
	fclose(fp);

	return result;
}
