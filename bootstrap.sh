#!/usr/bin/env bash

make -f .forge/Makefile release
mkdir -p ~/.local/bin
cp .forge/bin/release/forge.bin ~/.local/bin/forge
