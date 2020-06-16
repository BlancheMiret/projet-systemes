# Tagset
tag create utilisateur film livre genre couleur musique
tag create -f livre roman BD manga essai dictionnaire scolaire biographie
tag create -f musique jazz ballade rock 
tag create -f biographie autobiographie autofiction 
tag create -f roman policier historique science-fiction
tag create -f science-fiction "space opera"
tag create -f policier thriller
tag create -f thriller psychologique
tag create -f couleur rouge vert bleu rose jaune
tag create -f bleu bleu-clair azur turquoise
tag create -f genre western horreur drame
tag create -f jaune ocre
echo "Tagset created :"
tag print