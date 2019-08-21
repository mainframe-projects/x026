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

sudo cp x026 /usr/local/bin

# Install manpage
sudo cp x026.man /usr/local/share/man/man1/x026.1


