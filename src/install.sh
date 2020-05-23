echo 'installation xattr...'
sudo apt-get install xattr
echo 'installation glib....'
sudo apt-get install libglib2.0-dev
echo 'création répertoire ~/.tag...'
mkdir ~/.tag
echo 'création fichiers tag_hierarchy et paths.txt...'
touch ~/.tag/tag_hierarchy
touch ~/.tag/paths.txt
mv tag ~/.tag
gcc update-mv.c -o mvt
mv mvt ~/.tag
echo 'export PATH=$PATH:~/.tag' >> ~/.bashrc
echo 'relance le terminal...'
source ~/.bashrc
echo 'fin installation...'
