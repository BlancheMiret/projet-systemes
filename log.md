# Journal de bord
---

### Prochain réunion : samedi 4 avril (à 15h ?)

##### À faire :
>- Asmaa : **écrire du code** de test sur xattr et autre
>- Blanche : lire / parcourir / **faire un compte rendu** des 4 liens dans "Liens" du 30 mars + autre documents utiles
>- Louis : **proposition de modélisation** du stockage des tags et de leur hiérarchie

### Réunion 04 : lundi 30 mars

#avancer sur du concret

###### Notes
> - Retard sur le projet ?
> - Différence inline data / extended attributes ?
> - Commencer des tests concrets. Bientôt faire des choix !
> - Proposition d'augmentation de la fréquence des réunions
> - Pas d'essai base de données jusqu'ici

###### Liens
>*Documentation inodes*
>- [How to manipulate the Inode structure?](http://www.cis.syr.edu/~wedu/seed/Documentation/Minix3/Inode.pdf)
>- [The design of the Unix Operating System](http://www.ephotocaption.com/a/45/the_design_of_the_unix_operating_system.pdf)
>
> *Forums / Exemples code*
> - [Configuring inline_data in the ext4 Filesystem](https://harperdbhelp.zendesk.com/hc/en-us/articles/360007127654-Configuring-inline-data-in-the-ext4-Filesystem)
> - [How to use the new Ext4 Inline Data feature ? (storing data directly in the inode)](https://unix.stackexchange.com/questions/197633/how-to-use-the-new-ext4-inline-data-feature-storing-data-directly-in-the-inod)

###### Conclusion / à faire
> Répartition des recherches
>- Asmaa : **écrire du code** de test sur xattr et autre
>- Blanche : lire / parcourir / **faire un compte rendu** des 4 liens dans "Liens" du 30 mars + autre documents utiles
>- Louis : **proposition de modélisation** du stockage des tags et de leur hiérarchie

### Réunion 03 : lundi 23 mars

#Skypeontaime, ça fonctionne mieux

###### Notes
> - Louis : propose un fichier listant à la suite les tags, au format : nom du tag puis nom de son tag parent. Un module du programme servirait à créer un arbre de dépendances à partir de la lecture de ce fichier.
> - Blanche : il est possible de stocker des données de taille inférieure à 60octets directement à l'intérieur des i-nodes, selon un système de fichiers particulier. À voir si possible dans la configuration du projet.
> - Asmaa : peu convaincue par les bibliothèques externes, mais intéressée par les possibilités en base de données.

###### Conclusion / à faire
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
