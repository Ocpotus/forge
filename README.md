# Forge
A simple and lightweight `make` front-end/build system for the C/C++ programming language.

## About
Forge provides a simple project structure that I found myself creating over time. It is partly inspired by Rust's Cargo build system.
The aim of this project is to simplify the development process and allow developers/teams not worry about a particular project.
It also does not require one to know/learn a new language (think CMake).

## Getting Started
### Prerequisites
- Linux
- GNU make

### Building & Installing
Run the following in a shell of your choice:
```
$ ./bootstrap.sh
```

### Usage
For usage run the following:
```
$ forge --help
```
### Configuration
Currently configuration is split between `.forge/Makefile` and `config.toml`.
For compilation specific configuration (i.e. compiler, flags, linking) will be done in `.forge/Makefile`.
For project specific configuration see `config.toml`.

### Structure
The following is the project structure created by Forge:
- `.forge/` - Contains Makefile and build files.
- `include/` - Contains header files for a project.
- `lib/` - Currently no use, but in the future foreign code build files (e.g. `.so`, `.o`) will be here.
- `src/` - Contains all the code specific to the project.

## Built With
- [tomlc99](https://github.com/cktan/tomlc99) - For TOML config parsing
- [optparse99](https://github.com/hippie68/optparse99) - For CLI parsing
- [c-vector](https://github.com/eteran/c-vector) - For dynamic arrays

## Roadmap
Right now, Forge suits my needs, however there is still a lot of work to be done. The following is what should be done next:
- Unify configuration into `config.toml`
- Create a plugin-like interface for building and including non-Forge/complex projects. Currently, other projects need to be copied into the Forge project.
