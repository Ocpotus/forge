#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "../../../modules/tomlc99/toml.h"

#include "../../xmem/xmem.h"
#include "../../xerror/xerror.h"
#include "../../color/color.h"

#include "../util/util.h"

#include "config.h"

#define CONFIG_ERROR COLOR("forge", BWHITE) ": " COLOR("config error", BRED)


/* Reads the project info of TOML file
 */
static void parse_project_info(Config *cfg, toml_table_t *cfgp);

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
	};
	toml_table_t *config = read_toml_table(CONFIG_FILE);

	if(config == NULL) {
		xerrorf(CONFIG_ERROR, "failed to parse `config.toml`\n");
	}

	parse_project_info(&result, config);
	toml_free(config);

	return result;
}

void config_delete(Config *cfg) {
	cfg->info.author = xfree(cfg->info.author);
	cfg->info.version = xfree(cfg->info.version);
	cfg->info.name = xfree(cfg->info.name);
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
