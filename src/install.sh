echo 'installation xattr...'
sudo apt-get install xattr
echo 'installation glib....'
sudo apt-get install libglib2.0-dev
echo 'création répertoire ~/.tag...'
mkdir ~/.tag
echo 'création fichiers tag_hierarchy et paths.txt...'
touch ~/.tag/tag_hierarchy
touch ~/.tag/paths.txt
make
mv tag ~/.tag
gcc update-mv2.c -o upmv
gcc update-mv3.c -o upmv2
gcc update-rm2.c -o uprm2
gcc update-cp.c -o upcp
gcc update-cp2.c -o upcp2
cp mvt ~/.tag/
cp mvt.sh ~/.tag
cp rmta ~/.tag/
cp rmta.sh ~/.tag/
cp cpta ~/.tag/
cp cpt.sh ~/.tag
chmod u=wrx ~/.tag/mvt.sh
chmod u=wrx ~/.tag/rmta.sh
chmod u=wrx ~/.tag/mvt.sh
chmod u=wrx ~/.tag/cpt.sh
chmod u=wrx ~/.tag/cpta
mv upmv ~/.tag
mv uprm2 ~/.tag
mv upmv2 ~/.tag
mv upcp ~/.tag
mv upcp2 ~/.tag
echo 'export PATH=$PATH:~/.tag' >> ~/.bashrc
echo 'relance le terminal...'
echo "alias mv='mvt.sh'" >> ~/.bashrc
echo "alias cp='cpt.sh'" >> ~/.bashrc
echo "alias rm='rmta.sh'" >> ~/.bashrc
source ~/.bashrc
echo 'fin installation...'
