#!/bin/bash

dpkg-deb --build x026-1.0/ x026-1.0-armhf.deb
lintian x026-1.0-armhf.deb 

