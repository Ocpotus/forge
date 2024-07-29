#include "lib/optparse99/optparse99.h"

#include "forge/forge.h"

/* Helper macro for default `--help` option */
#define CLI_HELP_OPTION .short_name = 'h',\
			.long_name = "help",\
			.description = "displays this page",\
			.function = optparse_print_help,


/* Nothing fancy here, just initialize cli, parse, and execute */
int main(int argc, char *argv[]) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
	struct optparse_cmd forge = {
		.name = "forge",
		.about = "forge 0.1.0 - a simple `make` frontend",
		.description = "forge is a `make` frontend that has sane defaults and provides a cohesive project structure."
			       " For more information, pass '--help' to any subcommand.",
		.usage = "forge <SUBCOMMAND> [OPTIONS]", .options = (struct optparse_opt []) {
			{ CLI_HELP_OPTION },
			{ END_OF_OPTIONS },
		},
		.subcommands = (struct optparse_cmd []) {
			{
				.name = "build",
				.about = "builds the project",
				.description = "Builds a forge project with the specified mode."
					       " Builds a debug version if mode is not specified.",
				.usage = "build <MODE>",
				.options = (struct optparse_opt []) {
					{ CLI_HELP_OPTION },
					(struct optparse_opt) {
						.short_name = 'r',
						.long_name = "release",
						.description = "sets the build mode to release",
						.flag_type = FLAG_TYPE_SET_TRUE,
						.function = forge_build_release,
					},
					(struct optparse_opt) {
						.short_name = 'd',
						.long_name = "debug",
						.description = "sets the build mode to debug",
						.flag_type = FLAG_TYPE_SET_FALSE,
						.function = forge_build_debug,
					},
					{ END_OF_OPTIONS },
				},
				.function = forge_build,
			},
			{
				.name = "new",
				.about = "creates a new project",
				.description = "Creates a new project in the directory",
				.usage = "new <PATH>",
				.options = (struct optparse_opt []) {
					{ CLI_HELP_OPTION },
					{ END_OF_OPTIONS },
				},
				.function = forge_new,
			},
			{
				.name = "run",
				.about = "runs a debug build of the project",
				.usage = "run [ARGS]",
				.description = "Running a project consists of 2 steps:\n"
					       "(i) Builds a debug mode if necessary\n"
					       "(ii) executes the debug build with the following arguments\n",
				.options = (struct optparse_opt []) {
					{ CLI_HELP_OPTION },
					{ END_OF_OPTIONS },
				},
				.function = forge_run,
			},
			{
				.name = "clean",
				.about = "cleans the project",
				.description = "Cleaning a project simply deletes all the generated "
					       "object files in in .forge/release/obj and .forge/debug/obj.",
				.options = (struct optparse_opt []) {
					{ CLI_HELP_OPTION },
					{ END_OF_OPTIONS },
				},
				.function = forge_clean,
			},
			{ END_OF_SUBCOMMANDS },
		},
	};
#pragma GCC diagnostic pop

	optparse_parse(&forge, &argc, &argv);
}
