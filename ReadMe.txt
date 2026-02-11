Pour compiler le projet mettre dans l'invite de commande :

gcc -Wall Projet.c -o Projet

Pour executer mettre dans l'invite de commande:

./Projet [-a] [-c algo1 | algo2 | algo3] [-n int] [-t int] [-p fichierperformance.txt] [-l fichierresultat.txt]
fichier de données

Options :

-a : permet d'afficher le résultat des algorithmes.
-l [fichiersortie.txt] : Permet d'écrire le résultat dans un fichier texte.
-n [int] : Permettre le choix du nombre de mots à afficher.
-c [algo1 | algo2 | algo3] : Permet le choix entre trois algorithmes.
-p [fichierperf.txt]: Permet d'écrire les performances dans un fichier.txt.
-t [k] : Permet à l’utilisateur de ne demander que les mots d’au moins k lettres.

Pour executer le script python:

python 3 graph.py fichierperformance.txt [a][t]

Options:

t = génère un graphique de performance par rapport au temps
a = génère un graphique de performance par rapport à l'allocation maximale


