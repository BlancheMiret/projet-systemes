echo "TEST - ERROR MANAGING"

# Hierarchy
tag create utilisateur film livre genre couleur
tag create -f couleur rouge vert bleu rose jaune
tag create -f bleu bleu-clair azur turquoise
tag create musique
tag create -f genre western horreur drame
tag create -f jaune ocre
echo "Hierarchy created :"
tag print

# Tag
touch f1 f2 f3 f4 f5
tag link f1 vert rouge couleur
tag link f2 film rose western azur rouge
tag link f3 film azur ocre
tag link f4 turquoise 
tag link f5 jaune

#Errors
echo "***** Missing arguments *****"
echo "-- tag create"
tag create
echo "-- tag delete"
tag delete
echo "-- tag link f1"
tag link f1

echo "******** Syntax error *******"
echo "-- tag lsqjfl"
tag lsqjfl
echo "-- tag search couleur rouge qoskd"
tag search couleur rouge qoskd
echo "-- tag link f1 lksjqdf"
tag link f1 lksjqdf
echo "-- tag unlink blablabla"
tag unlink blablabla

echo "******* Inexisting tag ******"
echo "-- tag create -f user Louis Blanche Asmaa" 
tag create -f user Louis Blanche Asmaa
echo "-- tag link f1 western user" #A REVOIR
tag link f1 western user 
echo "-- tag search wester rouge user"
tag search wester rouge user
echo "-- tag delete user"
tag delete user
echo "-- tag unlink f5 couleur jaune" # Comportement : en enlève aucun ? 
tag unlink f5 couleur jaune
echo "-- tag unlink f1 vert rouge couleur user" # Comportement : en enlève aucun ? 
tag unlink f1 vert rouge couleur user

echo "******** Existing tag *******"
echo "--tag create couleur"
tag create couleur
echo "--tag link f2 film" # Tagger avec un tag déja présent ? 
tag link f2 film

echo "****** Inexisting file ******"
echo "--tag print f8"
tag print f8
echo "--tag link f8 couleur"
tag link f8 couleur
echo "--tag unlink f8 couleur"
tag unlink f8 couleur

echo "****** Double research ******"
echo "--tag search couleur couleur"
tag search couleur couleur

# Reset
tag reset
rm f1 f2 f3 f4 f5
