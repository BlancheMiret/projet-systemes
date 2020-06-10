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
echo "f1 tags :"
tag print f1
echo "f2 tags :"
tag print f2
echo "f3 tags :"
tag print f3
echo "f4 tags :"
tag print f4
echo "f5 tags :"
tag print f5
# Research
echo "Files with color"
tag search couleur
echo "Files with color but not rose"
tag search couleur -not rose
echo "Files with bleu but not turquoise and not jaune"
tag search bleu -not turquoise -not jaune
# Reset
rm f1 f2 f3 f4 f5
tag reset
