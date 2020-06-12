# Create a hierarchy
echo "creating hierarchy..."
tag create couleur film livre musique
tag create -f musique jazz ballade rock 
tag create -f livre roman BD manga essai dictionnaire scolaire biographie
tag create -f biographie autobiographie autofiction 
tag create -f roman policier historique science-fiction
tag create -f science-fiction "space opera"
tag create -f policier thriller
tag create -f thriller psychologique
echo "Hierarchy created :"
tag print
# Tagging files
touch test1
touch test2
touch test3
touch test4
touch test5
echo "tagging files..."
tag link test1 thriller roman policier ballade
tag link test2 film science-fiction "space opera"
tag link test3 historique jazz
tag link test4 autofiction roman
tag link test5 film thriller psychologique
echo "test1 tags :"
tag print test1
echo "test2 tags :"
tag print test2
echo "test3 tags :"
tag print test3
echo "test4 tags :"
tag print test4
echo "test5 tags :"
tag print test5
# Searching tags
echo "searching tags..."
echo "Files with psychologique thriller but not film"
tag search psychologique -not policier
echo "Files with film thriller"
tag search thriller film thriller
