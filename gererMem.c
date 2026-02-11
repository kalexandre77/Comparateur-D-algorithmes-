#include <stdio.h>
#include <stdlib.h>


typedef struct {
    size_t cumul_alloc;
    size_t cumul_desalloc;
    size_t max_alloc;
 } InfoMem;



void initInfoMeme(InfoMem * InfoMem){

    InfoMem -> cumul_alloc = 0;
    InfoMem -> cumul_desalloc = 0;
    InfoMem -> max_alloc = 0;

    
}


void * myMalloc(size_t size, InfoMem * InfoMem){
    size_t alloc_actuel;

    void * alloc = malloc(size); //alloue

    if (!alloc){
        fprintf(stderr,"Erreur d'allocation dans MyMalloc"); // comme printf mais renvoie sur la sortie erreur 
        return NULL;
    }

    InfoMem -> cumul_alloc += size; //on attribue la taille au cummul
    alloc_actuel = InfoMem -> cumul_alloc - InfoMem -> cumul_desalloc; //on récup le cummul au moment de l'allocation

    //si le cummul actuel est plus grand que le max alors on le remplace
    if (alloc_actuel > InfoMem -> max_alloc){
        InfoMem -> max_alloc = alloc_actuel;
    }

    return alloc; 

 }


void * myRealloc(void * ptr, size_t new_size, InfoMem* InfoMem, size_t old_size){
    size_t alloc_actuel;

    void * rea = realloc(ptr, new_size);
    if(!rea){
        fprintf(stderr,"Erreur d'allocation dans MyRealloc");
        return NULL;
    }

    //si on modifie l'adresse du pointeur (déplacement dans un nouveau pointeur)
    if(rea != ptr){
        InfoMem -> cumul_alloc += new_size;
        InfoMem -> cumul_desalloc += old_size;
        
    //sinon on est dans le cas d'une réallocation statique (pas de changement de ptr)
    }else{
        //dans le cas ou l'ancienne taille est plus grande
        if (old_size > new_size){
            InfoMem -> cumul_desalloc += old_size - new_size;
        }

        //nouvelle taille plus grande
        if (new_size > old_size){
            InfoMem -> cumul_alloc += new_size - old_size;
        }
    }
    
    alloc_actuel = InfoMem -> cumul_alloc - InfoMem -> cumul_desalloc;

     if (alloc_actuel > InfoMem -> max_alloc){
        InfoMem -> max_alloc = alloc_actuel;
    }

    return rea;


}


void myFree(void * ptr, InfoMem * InfoMem, size_t old_size){

    free(ptr);
    InfoMem -> cumul_desalloc += old_size;

}