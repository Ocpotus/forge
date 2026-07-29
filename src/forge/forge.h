#ifndef FORGE_H
#define FORGE_H


/* Compiles and runs a debug build of the project */
void forge_run(int argc, char **argv);

/* Compiles a debug version of the project */
void forge_build();

/* Compiles debug version of the project */
void forge_build_debug();

/* Compiles release version of the project */
void forge_build_release();

/* Cleans the compiled binaries */
void forge_clean();

/* Creates a new project */
void forge_new(int argc, char **argv);

void forge_module_build();
void forge_module_verify();
void forge_module_add();


#endif
