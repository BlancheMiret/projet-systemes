# Architecture de Projet

###### Extrait du sujet :
...expliquant la stratégie adoptée pour répondre au
sujet (notamment l'architecture logicielle, les choix faits en terme de où
sont stockés les tags et quand/comment ils sont mis à jours, les structures de
données et les algorithmes implémentés).

#### Architecture globale
Dépendances des modules
- `main.c.` centralise les actions des différents modules selon l'option et les arguments entrée par l'utilisateur
- `tag_manager.c` gère le stockage, la lecture et l'écriture de la hiérarchie des tags
- `tag_file.c` gère l'attribution des tags aux fichiers avec `xattr`
- `path_mangager.c`gère le fichier contenant la liste des fichiers taggés
- `research.c`fournit la fonction de recherche répondant à celle d'un utilisateur

#### Hiérarchie des tags
- Elle est stockée dans le fichier *tag_hierarchy*
- À chaque lecture / écriture, utilisation d'une structure de donnée qui permettant
- Sur le fichier en lui-même chaque nom de tag est précédé du nom de son père, ce qui permet à la lecture de recréer l'arbre de la hiérarchie dans une structure adaptée.
- Utilisation d'une hashmap qui retient pour chaque nom de tag, l'adresse de la structure qui le représente dans l'arbre. Facilite à la fois la création de l'arbre, puis les recherches par nom de tag (des enfants par exemple)
>//Fonctionnalités
>- Il est possible d'afficher la hiérarchie des tags déjà créée, d'ajouter ou de supprimer un tag, mais pas d'en modifier un.
>- Il n'est pas possible d'avoir deux tags de même nom, même à des endroits différents de la hiérarchie
>- Pour préciser une hiérarchie, l'utilisateur. S'il souhaite le placer à la racine : option -n

#### Association des tags aux fichiers
- Les tags sont stockés dans les attributs étendus (*extended attributes*) via les fonctions fournies par la librairie `xattr`. L'idée d'utiliser les attributs étendus nous plaisait en particulier afin de situer les tags au plus près des i-noeuds, c'est à dire des fichiers eux-mêmes.
>//Fonctionnalités
>- Il est possible d'ajouter ou de supprimer un tag d'un fichier.

#### Suivi des fichiers taggés
- Cependant, afin de réduire la complexité en temps de la recherche, il était hors de question de parcourir l'intégralité des fichiers du système de fichier au moment de faire une recherche sur les attributs éttendus où sont inscrits les tags.
- Ainsi, couche supplémentaire pour la conservation de la liste des chemins des fichiers taggés dans le fichiers *nomdufichier*
- Ce qui demandait l'utilisation d'alias sur les commandes `mv` et `cp` afin de suivre les ajouts / modification de fichiers à la liste des fichiers taggés
- L'avantage d'xattr est conservé en simplifiant l'écriture des alias **(???)**

#### Recherche des fichiers taggés
- D'un côté, lecture du fichier de hiérarchie pour faire les bons liens entre les termes de la recherches et les tags associés (enfants)
- Recherche parmi la liste de fichier taggés
>//Fonctionnalités
>- La syntaxe est
>```tag -s tag1 -not tag2 tag3 -not tag4...``` 
