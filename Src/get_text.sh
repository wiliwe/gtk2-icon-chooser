#!/bin/sh

xgettext -d IconChooser -o ./po/IconChooser.pot -k_ -s ./CIconChooser.cpp
cp ./po/IconChooser.pot ./po/IconChooser.po
