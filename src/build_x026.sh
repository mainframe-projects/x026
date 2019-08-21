#!/bin/bash

# Install Deps
./ install_dependancies.sh

# Build makefile

xmkmf

# Make dependancies

make depend

# Build

make

# Install executables

sudo cp x026 ../x026-1.0/usr/local/bin

# Install manpage
sudo cp x026.man ../x026-1.0/usr/local/share/man/man1/x026.1


