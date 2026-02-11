
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <locale.h>
#include <ctype.h>
#include <time.h>
#include "gererMem.c"


#define TAILLE_HASH 10007   // nombre premier
#define MAX_MOTS_DIFFERENTS 10000
#define TAILLE_MAX_MOT 255


typedef struct {
    char mot[TAILLE_MAX_MOT + 1];
    int occurrences;
} MotOccurence;

//Tableau
typedef struct {
    MotOccurence * tableau;
    int nombreMots;
    int capacite;
} TableMots;


//Cellule
typedef struct Noeud {
    char mot[TAILLE_MAX_MOT + 1];
    int occurrences;
    struct Noeud *suivant;
} Noeud;

//tableau de hashage
typedef struct {
    Noeud **seaux;   // tableau de listes chaînées
    int taille;
    int nombreMots;
} TableHash;


/**
 * @brief Calcule une valeur de hachage simple pour une chaîne de caractères.
 * * @param mot La chaîne de caractères à hacher.
 * @return unsigned long La valeur de hachage calculée (somme ASCII).
 */
unsigned long hashMot(const char *mot) {
    unsigned long h = 0;
    int i = 0;

    while (mot[i] != '\0') {
        // On additionne juste la valeur ASCII de chaque lettre
        h = h + mot[i]; 
        i++;
    }
    return h;
}

 


/**
 * @brief Initialise la structure de la table de hachage.
 * * @param th Pointeur vers la table de hachage à initialiser.
 * @param info Pointeur vers la structure de gestion mémoire.
 */
void initTableHash(TableHash *th, InfoMem *info){
    th->taille = TAILLE_HASH;
    th->nombreMots = 0;

    th->seaux = (Noeud **)myMalloc(sizeof(Noeud*) * th->taille, info);
    if (!th->seaux) {
        fprintf(stderr, "Erreur alloc table hash\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < th->taille; i++) {
        th->seaux[i] = NULL;
    }
}



/**
 * @brief Ajoute un mot ou incrémente son nombre d'occurrences dans la table de hachage.
 * * @param th Pointeur vers la table de hachage.
 * @param mot Le mot à ajouter.
 * @param info Pointeur vers la structure de gestion mémoire.
 * @return int 1 si succès, 0 si échec d'allocation.
 */

int ajoutHash(TableHash *th, const char *mot, InfoMem *info){
    unsigned long h = hashMot(mot) % th->taille;
    Noeud *courant = th->seaux[h];

    while (courant) {
        if (strcmp(courant->mot, mot) == 0) {
            courant->occurrences++;
            return 1;
        }
        courant = courant->suivant;
    }

    // pas trouvé → nouveau noeud
    Noeud *n = (Noeud *)myMalloc(sizeof(Noeud), info);
    if (!n) return 0;

    strncpy(n->mot, mot, TAILLE_MAX_MOT);
    n->mot[TAILLE_MAX_MOT] = '\0';
    n->occurrences = 1;

    n->suivant = th->seaux[h];
    th->seaux[h] = n;
    th->nombreMots++;

    return 1;
}






/**
 * @brief Transfère le contenu de la table de hachage vers un tableau linéaire.
 * * @param table Pointeur vers le tableau de destination.
 * @param hash Pointeur vers la table de hachage source.
 * @param info Pointeur vers la structure de gestion mémoire (pour réallocation éventuelle).
 */
void tableDepuisHash(TableMots *table, TableHash *hash, InfoMem *info){

    table->nombreMots = 0;

    if (table->capacite < hash->nombreMots) {
        int ancienne = table->capacite;
        int nouvelle = hash->nombreMots;

        size_t old_size = (size_t)ancienne * sizeof(MotOccurence);
        size_t new_size = (size_t)nouvelle * sizeof(MotOccurence);

        MotOccurence *tmp = (MotOccurence*)myRealloc(table->tableau, new_size, info, old_size);
        if (!tmp) {
            fprintf(stderr, "Erreur realloc tableDepuisHash\n");
            exit(EXIT_FAILURE);
        }
        table->tableau = tmp;
        table->capacite = nouvelle;
    }

    for (int i = 0; i < hash->taille; i++) {
        Noeud *cur = hash->seaux[i];
        while (cur) {
            strncpy(table->tableau[table->nombreMots].mot, cur->mot, TAILLE_MAX_MOT);
            table->tableau[table->nombreMots].mot[TAILLE_MAX_MOT] = '\0';
            table->tableau[table->nombreMots].occurrences = cur->occurrences;
            table->nombreMots++;
            cur = cur->suivant;
        }
    }
}

/**
 * @brief Libère toute la mémoire associée à la table de hachage (buckets et nœuds).
 * * @param hash Pointeur vers la table de hachage.
 * @param info Pointeur vers la structure de gestion mémoire.
 */
void libererTableHash(TableHash *hash, InfoMem *info){
    if (!hash || !hash->seaux) return;

    for (int i = 0; i < hash->taille; i++) {
        Noeud *cur = hash->seaux[i];
        while (cur) {
            Noeud *suiv = cur->suivant;
            myFree(cur, info, sizeof(Noeud));
            cur = suiv;
        }
    }

    myFree(hash->seaux, info, sizeof(Noeud*) * hash->taille);
    hash->seaux = NULL;
}


/**
 * @brief Initialise le tableau dynamique de mots.
 * * @param table Pointeur vers la structure TableMots.
 * @param info Pointeur vers la structure de gestion mémoire.
 */
void initialiserTableMots(TableMots *table, InfoMem *info){
    table->nombreMots = 0;
    table -> capacite = 10;

        table -> tableau = NULL;
        table -> tableau = (MotOccurence*)myMalloc(sizeof(MotOccurence) * table -> capacite, info);
    if(!table -> tableau){
        fprintf(stderr, "Erreur d'allocation tableau");
        return;
    }
}



/**
 * @brief Affiche le contenu brut d'un fichier sur la sortie standard.
 * * @param nomFichier Chemin du fichier à lire.
 * @return int EXIT_SUCCESS ou EXIT_FAILURE.
 */
int afficherFichier(const char *nomFichier){
    FILE *fichier = fopen(nomFichier, "r");;
    char ligne[256];

    if(fichier == NULL){
        printf("Erreur\n");
        return EXIT_FAILURE;
    }

    while (fgets(ligne, sizeof(ligne), fichier) != NULL) {
        printf("%s", ligne);
    }

    fclose(fichier);

    return EXIT_SUCCESS;
}



/**
 * @brief Ajoute un mot ou incrémente son compteur via une recherche linéaire (non triée).
 * * @param table Pointeur vers le tableau de mots.
 * @param mot Le mot à traiter.
 * @param info Pointeur vers la structure de gestion mémoire.
 * @return int 1 si succès, 0 si erreur d'allocation.
 */
int ajoutOuPlus(TableMots *table, const char *mot,InfoMem * info){
    for (int i = 0; i < table->nombreMots; i++) {
        if (strcmp(table->tableau[i].mot, mot) == 0) {
            table->tableau[i].occurrences++;
            return 1;
        }
    }

    //Si on a plus de place alors on double la capacité d'ou le ((table -> capacite)*2)
    if (table->nombreMots >= table->capacite) {
        int ancienne_capacite = table->capacite;
        int nouvelle_capacite = ancienne_capacite * 2;
        
        size_t old_size = ancienne_capacite * sizeof(MotOccurence);
        size_t new_size = nouvelle_capacite * sizeof(MotOccurence);

        MotOccurence *tmp = (MotOccurence*) myRealloc(table->tableau, new_size, info, old_size);
        if (!tmp) {
            fprintf(stderr, "Erreur alloc ajoutTrie\n");
            return 0;
        }
        
        table->tableau = tmp;
        table->capacite = nouvelle_capacite;
    }


    //on insère

    strncpy(table->tableau[table->nombreMots].mot, mot, TAILLE_MAX_MOT);
    table->tableau[table->nombreMots].mot[TAILLE_MAX_MOT] = '\0';
    table->tableau[table->nombreMots].occurrences = 1;

    table->nombreMots++;
    return 1;
}


/**
 * @brief Effectue une recherche dichotomique pour trouver l'index d'un mot.
 * * @param table Pointeur vers le tableau de mots.
 * @param mot Le mot à chercher.
 * @param[out] trouve Pointeur mis à 1 si le mot existe, 0 sinon.
 * @return int L'index du mot s'il existe, ou l'index d'insertion sinon.
 */
int recherche(TableMots *table, const char *mot, int * trouve){
    int debut = 0;
    int fin = table->nombreMots - 1;
    
    while (debut <= fin){
        int mid = (debut + fin) / 2;
        int cmp = strcmp(mot, table->tableau[mid].mot);
        //si on trouve on renvoie le milieu
        if (cmp == 0){
            *trouve = 1;
            return mid; 
        }
        
        // Si mot > tableau[mid] à droite
        if (cmp > 0){
            debut = mid + 1;
        } 
        // Si mot < tableau[mid] à gauche
        else {
            fin = mid - 1;
        }
    }
    
    *trouve = 0;
    return debut; // C'est ici qu'on devra l'insérer
}



/**
 * @brief Insère un mot en maintenant le tableau trié alphabétiquement (insertion triée).
 * * @param table Pointeur vers le tableau de mots.
 * @param mot Le mot à insérer.
 * @param info Pointeur vers la structure de gestion mémoire.
 * @return int 1 si succès, 0 si erreur d'allocation.
 */
int ajoutTrie(TableMots *table, const char *mot, InfoMem *info){
    int trouve = 0;
    int index = recherche(table, mot, &trouve);

    //on a trouvé le mot
    if (trouve == 1) {
        table->tableau[index].occurrences++;
        return 1;
    }

    //plus de place on double
    if (table->nombreMots >= table->capacite) {
        int ancienne_capacite = table->capacite;
        int nouvelle_capacite = ancienne_capacite * 2;
        
        size_t old_size = ancienne_capacite * sizeof(MotOccurence);
        size_t new_size = nouvelle_capacite * sizeof(MotOccurence);

        MotOccurence *tmp = (MotOccurence*) myRealloc(table->tableau, new_size, info, old_size);
        if (!tmp) {
            fprintf(stderr, "Erreur alloc ajoutTrie\n");
            return 0;
        }
        
        table->tableau = tmp;
        table->capacite = nouvelle_capacite;
    }

    
    // On pousse vers la droite pour libérer la case 'index'
    for (int i = table->nombreMots; i > index; i--) {
        table->tableau[i] = table->tableau[i - 1];
    }

    //insertion
    strncpy(table->tableau[index].mot, mot, TAILLE_MAX_MOT);
    table->tableau[index].mot[TAILLE_MAX_MOT] = '\0'; // Sécurité
    table->tableau[index].occurrences = 1;
    table->nombreMots++;

    return 1;
}



/**
 * @brief Fonction de comparaison pour qsort (tri décroissant par occurrences).
 * * @param a Pointeur vers le premier élément MotOccurence.
 * @param b Pointeur vers le deuxième élément MotOccurence.
 * @return int Différence des occurrences (b - a).
 */
int comparerParOccurrences(const void *a, const void *b) {
    const MotOccurence *elemA = (const MotOccurence *)a;
    const MotOccurence *elemB = (const MotOccurence *)b;
    
    return (elemB->occurrences - elemA->occurrences);
}



/**
 * @brief Aiguille le traitement du mot vers la fonction appropriée selon l'algorithme choisi.
 * * @param table Pointeur vers le tableau de mots.
 * @param hash Pointeur vers la table de hachage (si algo3).
 * @param mot Le mot à traiter.
 * @param info Pointeur vers la structure de gestion mémoire.
 * @param algo Chaîne identifiant l'algorithme ("algo1", "algo2", "algo3").
 */
static void compterMot(TableMots *table, TableHash *hash, const char *mot, InfoMem *info, const char *algo){
    if (strcmp(algo, "algo1") == 0) {
        ajoutOuPlus(table, mot, info);
    }
    else if (strcmp(algo, "algo2") == 0) {
        ajoutTrie(table, mot, info);
    }
    else if (strcmp(algo, "algo3") == 0) {
        if (hash == NULL) return;
        ajoutHash(hash, mot, info);
    }

}



/**
 * @brief Lit un fichier, découpe les mots et lance le comptage.
 * * @param nomFichier Chemin du fichier à analyser.
 * @param table Pointeur vers le tableau de résultats.
 * @param hash Pointeur vers la table de hachage (optionnel).
 * @param info Pointeur vers la structure de gestion mémoire.
 * @param algo Nom de l'algorithme à utiliser.
 * @param min_taille Taille minimale des mots à prendre en compte.
 * @return int EXIT_SUCCESS ou EXIT_FAILURE.
 */
int lireFichierEtCompter(const char *nomFichier, TableMots *table, TableHash *hash, InfoMem *info, const char *algo, int min_taille){
    FILE *fichier = fopen(nomFichier, "r");
    if (fichier == NULL) return EXIT_FAILURE;

    char ligne[1024];
    char mot[256];

    while (fgets(ligne, sizeof(ligne), fichier) != NULL) {

        int i = 0;
        int j = 0;

        while (ligne[i] != '\0') {
            unsigned char c = (unsigned char)ligne[i];

            if (isalpha(c) || c > 127 || (c == '-' && j > 0 && ligne[i + 1] != '\0' && (isalpha((unsigned char)ligne[i + 1]) || (unsigned char)ligne[i+1] > 127))) {

                if (j < (int)sizeof(mot) - 1) { 
                    if (c < 128){mot[j++] = (char)tolower(c);}
                    else {mot[j++] = (char)c;}
                }

            } else {
                if (j > 0) {
                    mot[j] = '\0';

                    if(j >= min_taille){
                        compterMot(table, hash, mot, info, algo);
                    }

                    j = 0;
                }
            }

            i++;
        }

        if (j > 0) {
            mot[j] = '\0';
            if (j >= min_taille){
                compterMot(table, hash, mot, info, algo);
            }
        }
    }

    fclose(fichier);
    return EXIT_SUCCESS;
}


/**
 * @brief Écrit le résultat du comptage (mot : occurrences) dans un fichier.
 * * @param table La structure contenant les données triées.
 * @param nom_fichier Nom du fichier de sortie.
 * @param limite Nombre maximum de mots à écrire.
 * @return int EXIT_SUCCESS ou EXIT_FAILURE.
 */
int Ecrire_fichier_res(TableMots table, char * nom_fichier, int limite){
    FILE *f;

    if (!nom_fichier || strlen(nom_fichier) == 0){
        fprintf(stderr, "Aucun fichier mentionne pour ecriture\n");
        return EXIT_FAILURE;
    }

    f = fopen(nom_fichier, "w");
    if(!f){
        fprintf(stderr,"Erreur d'ouverture du fichier res");
        return EXIT_FAILURE;
    }

    if (limite < 0 || limite > table.nombreMots) {
        limite = table.nombreMots;
    }

    for(int i = 0; i  < limite; i++){
        fprintf(f,"%s : %d\n", table.tableau[i].mot, table.tableau[i].occurrences);
    }

    fclose(f);
    return EXIT_SUCCESS;
}


/**
 * @brief Enregistre les métriques de performance dans un fichier dédié.
 * * @param nomFichierPerf Nom du fichier de logs.
 * @param algo Algorithme utilisé.
 * @param fichierAnalyse Fichier source analysé.
 * @param temps_execution Durée du traitement.
 * @param info Structure contenant les stats mémoire.
 * @param nb_mots Nombre total de mots uniques trouvés.
 */

void ecrirePerformances(const char *nomFichierPerf, const char *algo, const char *fichierAnalyse, double temps_execution, const InfoMem *info, int nb_mots){
    FILE *fp = fopen(nomFichierPerf, "a");
    if (!fp) {
        fprintf(stderr, "Erreur ouverture fichier performances %s\n", nomFichierPerf);
        return;
    }

    fprintf(fp, "algo=%s fichier=%s nbmots=%d temps=%.6f max_alloc=%zu cumul_alloc=%zu cumul_desalloc=%zu\n", algo, fichierAnalyse, nb_mots,temps_execution, info->max_alloc, info->cumul_alloc, info->cumul_desalloc);
    fclose(fp);
}


/**
 * @brief Retourne le temps CPU écoulé depuis le lancement du programme.
 * * @return double Temps en secondes.
 */
double tempsSecondes(void) {
    return (double)clock() / CLOCKS_PER_SEC;
}


/**
 * @brief Point d'entrée principal. Gère les arguments CLI et orchestre le comptage.
 * * @param argc Nombre d'arguments.
 * @param argv Tableau des arguments.
 * @return int Code de sortie du programme.
 */

int main(int argc, char *argv[]){
    setlocale(LC_ALL, "");

    if (argc < 2) 
        return EXIT_FAILURE;

    InfoMem info;
    initInfoMeme(&info);

    TableMots table;
    initialiserTableMots(&table, &info);
    if (!table.tableau) 
        return EXIT_FAILURE;

    TableHash hash;
    TableHash *pHash = NULL;

    int taille_mot = 0;

    int opt;
    char *fichier = NULL;

    int opt_a = 0;

    int opt_l = 0; 
    char *optarg_l = NULL;

    int opt_n = 0; 
    int optarg_n = 0;

    int opt_c = 0; 
    char *optarg_c = NULL;

    int opt_p = 0; 
    char *optarg_p = NULL;

    int opt_t = 0;
    int optarg_t = 0;

    while ((opt = getopt(argc, argv, "al:n:c:p:t:")) != -1) {
        switch (opt) {
            case 'a': //-a = afficher res
                opt_a = 1;
                break;
                
            case 'l': //-l [fichiersortie.txt]= écrire le résultat dans un fichier texte
                opt_l = 1; 
                optarg_l = optarg;
                break;

            case 'n': //-n [int] = affiche le nombre de mot choisi
                opt_n = 1;
                optarg_n = atoi(optarg);
                break;

            case 'c': //-c [algo1 | algo2 | algo3] = permet le choix d'algorithme de tri
                opt_c = 1;
                optarg_c = optarg;
                break;

            case 'p': //-p [fichierperf.txt] = écrire les performances d'un fichier.txt
                opt_p = 1; 
                optarg_p = optarg;
                break;
            
            case 't': //-t [k] =  permet à l’utilisateur de ne demander que les mots d’au moins k lettres
                opt_t = 1;
                optarg_t = atoi(optarg);
                break;

            default: abort();
        }
    }

    if (optind >= argc) {
        fprintf(stderr, "Aucun fichier spécifié.\n");
        return EXIT_FAILURE;
    }
    fichier = argv[optind];

    
    if (opt_c) {

        //Vérification algorithme
        if (strcmp(optarg_c, "algo1") && strcmp(optarg_c, "algo2") && strcmp(optarg_c, "algo3")) {
            fprintf(stderr, "Algorithme inconnu : %s\n", optarg_c);
            return EXIT_FAILURE;
        }

        

        // Initialisation algo3
        if (strcmp(optarg_c, "algo3") == 0) {
            initTableHash(&hash, &info);
            pHash = &hash;
        }

    

        // Début chrono
        double debut = 0.0, fin = 0.0;

        if (opt_p) 
            debut = tempsSecondes();

        // Lecture + comptage
        if(opt_t){
            taille_mot = optarg_t;
        }

        if (lireFichierEtCompter(fichier, &table, pHash, &info, optarg_c, taille_mot) != EXIT_SUCCESS) {
                return EXIT_FAILURE;
        }
        //Conversion hash → tableau
        if (pHash) {
            tableDepuisHash(&table, pHash, &info);
        }

        // Tri par occurrences
        qsort(table.tableau, table.nombreMots, sizeof(MotOccurence), comparerParOccurrences);

        // Limite d'affichage
        int limite = table.nombreMots;
        if (opt_n && optarg_n > 0 && optarg_n < limite) {
            limite = optarg_n;
        }

        
        // Fin chrono + écriture perfs
        if (opt_p) {
            fin = tempsSecondes();
            ecrirePerformances(optarg_p, optarg_c,fichier, fin - debut, &info, table.nombreMots);
            
        }

        //Sortie 
        if (opt_l) {
            Ecrire_fichier_res(table, optarg_l, limite);
        }

        // Libération algo3
        if (pHash) {
            libererTableHash(pHash, &info);
        }
    }
    
    if (opt_a) {
        int limite = table.nombreMots;
        if (opt_n && optarg_n > 0 && optarg_n < limite) {
            limite = optarg_n;
        }

        for (int i = 0; i < limite; i++) {
                printf("%s: %d\n",table.tableau[i].mot,table.tableau[i].occurrences);
            }
    }

    //on libère l'espace
    myFree(table.tableau, &info, table.capacite * sizeof(MotOccurence));
    
    return EXIT_SUCCESS;
}

