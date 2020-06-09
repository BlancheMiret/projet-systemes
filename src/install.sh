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
gcc update-rm.c -o uprm
cp mvt ~/.tag/
cp mvt.sh ~/.tag
chmod u=wrx mvt.sh
mv upmv ~/.tag
mv uprm ~/.tag
mv upmv2 ~/.tag
echo 'export PATH=$PATH:~/.tag' >> ~/.bashrc
echo 'relance le terminal...'
source ~/.bashrc
echo 'fin installation...'
