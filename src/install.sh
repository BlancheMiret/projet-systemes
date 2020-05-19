#!/bin/bash
sudo apt-get install xattr
mkdir ~/.tag
touch ~/.tag/tag_hierarchy
touch ~/.tag/paths.txt
make
mv main_exe ~/.tag
gcc update-mv.c -o mvt
mv mvt ~/.tag
echo "export PATH=$PATH:~/.tag" >> ~/.bashrc
echo "alias mvt='mvt'" >> ./bashrc
source bashrc