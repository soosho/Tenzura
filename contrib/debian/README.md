
Debian
====================
This directory contains files used to package tenzurad/tenzura-qt
for Debian-based Linux systems. If you compile tenzurad/tenzura-qt yourself, there are some useful files here.

## tenzura: URI support ##


tenzura-qt.desktop  (Gnome / Open Desktop)
To install:

	sudo desktop-file-install tenzura-qt.desktop
	sudo update-desktop-database

If you build yourself, you will either need to modify the paths in
the .desktop file or copy or symlink your tenzura-qt binary to `/usr/bin`
and the `../../share/pixmaps/tenzura128.png` to `/usr/share/pixmaps`

tenzura-qt.protocol (KDE)

