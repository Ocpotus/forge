#ifndef FORGE_CONFIG_H
#define FORGE_CONFIG_H

#include <stdlib.h>
#include <stdbool.h>

#define CONFIG_FILE "config.toml"


typedef struct Config {
	/*
	 * @member name: project name
	 * @member author: package author
	 * @member version: release version
         * @member language: programming language used
	 */
	struct {
		char *name;
		char *author;
		char *version;
	} info;
} Config;

/* Parses a "config.toml" file.
 *
 * @return: a parsed Config struct
 */
Config config_read();

/* Deletes the data stored.
 *
 * @param *cfg: Config to clear data.
 */
void config_delete(Config *cfg);

/* Writes a config file
 *
 * @return: if the write was successful
 */
bool config_write(const char *project);


#endif
