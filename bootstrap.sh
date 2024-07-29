#!/usr/bin/env bash

make -f .forge/Makefile release
cp .forge/bin/release/forge.bin ~/.local/bin/forge
