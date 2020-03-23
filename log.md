# Journal de bord
---

### Prochain réunion : lundi 30 mars à 15h


##### À faire
>- Asmaa : recherches sur la possibilité de stocker les tags et leur hiérachie dans une base de données.
>- Blanche : poursuivre l'exploration de modification directe des i-noeuds, si possible quelques tests.
>- Louis : si possible développer l'idée du stockage dans un fichier.


### Réunion 03 : lundi 23 mars

#Skypeontaime

###### Notes
> - Louis : propose un fichier listant à la suite les tags, au format : nom du tag puis nom de son tag parent. Un module du programme servirait à créer un arbre de dépendances à partir de la lecture de ce fichier.
> - Blanche : il est possible de stocker des données de taille inférieure à 60octets directement à l'intérieur des i-nodes, selon un système de fichiers particulier. À voir si possible dans la configuration du projet.
> - Asmaa : peu convaincue par les bibliothèques externes, mais intéressée par les possibilités en base de données. 

###### Conclusion
> Répartition des recherches
>
>- Asmaa : recherches sur la possibilité de stocker les tags et leur hiérachie dans une base de données.
>- Blanche : poursuivre l'exploration de modification directe des i-noeuds, si possible quelques tests.
>- Louis : si possible développer l'idée du stockage dans un fichier.

### Réunion 02 : mardi 17 mars

#Covid-19
Rendez-vous vocal sur Discord.

###### Notes

> Deux axes principaux du sujet :
> - Comment/où stocker la liste des tags et leur hiérarchie :
>   - Hiérarchie : un arbre (non binaire). S'inspirer de la représentation des répertoires dans le SGF ?
> - Comment/Où stocker les tags relatifs à chaque fichiers

###### Liens
>*Modification des inodes*
>- [Is it possible to store data inside an inode](https://unix.stackexchange.com/questions/197570/is-it-possible-to-store-data-directly-inside-an-inode-on-a-unix-linux-filesyst) (forum)
>- [How to manupilate the inode structure](http://www.cis.syr.edu/~wedu/seed/Documentation/Minix3/Inode.pdf) (pdf)
>
>*Bibliothèque xattr*
>- [Attributs étendus](https://fr.wikipedia.org/wiki/Attributs_étendus) (wikipedia)

###### Conclusion

>Répartition des recherches
>- Asmaa : recherches de librairies externes (ex : `xattr`) qui pourraient être utiles.
>- Blanche : explorer la possibilité de modification directe des i-noeuds.
>- Louis : reflexion sur la structure du stockage des tags et de leur hiérarchie.

### Réunion 01 : lundi 9 mars

Réunion à la bibliothèque : lecture du sujet, proposition de recherches personnelles sur le fonctionnement des systèmes de gestion de fichiers.
