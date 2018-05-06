#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <stdbool.h>
#include "fractal.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>




pthread_mutex_t mutex1;
pthread_mutex_t mutex2;
sem_t empty1;
sem_t full1;
sem_t empty2;
sem_t full2;
int le_plus_grand=INT_MIN;



struct fractal *tab1[20];//20? peut etre a changer!!!
//tab1->value=0;//place dans le tableau a laquelle il faut placer la prochaine fractal dont il faut calculer la valeur des pixels.
struct fractal *tab2[20];
//tab2->value=0;//place dans le tableau a laquelle il faut placer la prochaine fractal dont la valeur des pixels est deja calculee.





/*
 * @check: nombre de fichier que l'on met en arguments
 * @flag: true si -d est en argument de la main
 * @output: Si le fichier est -, alors on insere ds le tableau la fractal mise dans l'entree std
 *
 *
 */

void *producer (void * param) {//Si flag est true, alors c'est qu'il y a -d ds la main (int check, int flag)

    int *check=(int *)param;
    int flag=*(check+1);



    int compteur = 0;

    while (compteur != *check) {
        char tiret = '-';
        /*
         * Si le fichier est un tiret
         */
        if ((flag && strcmp(*argv[3 + compteur], &tiret) == 0) || (!flag && strcmp(*argv[2 + compteur], &tiret) == 0)) {
            char lpBuffer[200];


            printf("Entrez une chaine de caracteres : ");
            fgets(lpBuffer, sizeof(lpBuffer), stdin);
            int i = 0;
            char diese = '#';
            if (strcmp(&lpBuffer[i], &diese) == 0) {
                fprintf(stderr, "%s\n", strerror(errno));
            }
            char nom[65];
            char vide = ' ';
            while (strcmp(&lpBuffer[i], &vide) != 0) {
                strcat(nom, lpBuffer + i);//Il faut cheker la valeure de retour??
                i++;
            }

            i++;
            char largeur[32];

            while (strcmp(&lpBuffer[i], &vide) != 0) {
                strcat(largeur, lpBuffer + i);
                i++;
            }
            int largeur2 = atoi(largeur);
            i++;
            char hauteur[32];
            while (strcmp(&lpBuffer[i], &vide) != 0) {
                strcat(hauteur, lpBuffer + i);
                i++;
            }
            int hauteur2 = atoi(hauteur);
            i++;
            char reelle[32];
            while (strcmp(&lpBuffer[i], &vide) != 0) {
                strcat(reelle, lpBuffer + i);
                i++;
            }
            double reelle2 = atof(reelle);
            i++;
            char imaginaire[10];
            char n = '\n';
            while (strcmp(&lpBuffer[i], &n) != 0) {
                strcat(imaginaire, lpBuffer + i);
                i++;
            }
            double imaginaire2 = atof(imaginaire);

            struct fractal *fractal_a_creer;
            int place;

            sem_wait(&empty1);
            pthread_mutex_lock(&mutex1);
            fractal_a_creer = fractal_new(nom, largeur2, hauteur2, reelle2, imaginaire2);
            if (sem_getvalue(&full1, &place) != 0) {
                fprintf(stderr, "%s\n", strerror(errno));
            }


            tab1[place] = fractal_a_creer;
            //tab1->value=tab1->value+1;
            pthread_mutex_unlock(&mutex1);
            sem_post(&full1);
            compteur++;


        }
        else {


            int r;
            if (flag) {
                r = open(*argv[3 + compteur], O_RDONLY);//attention, uniquement si -d
                if (r < 0) {
                    fprintf(stderr, "%s\n", strerror(errno));
                }

            } else {
                r = open(*argv[2 + compteur], O_RDONLY);//attention, uniquement si PAS -d
                if (r < 0) {
                    fprintf(stderr, "%s\n", strerror(errno));
                }
            }

            char caracter;
            ssize_t ret = read(r, &caracter, sizeof(char));
            if (ret < 0) {
                fprintf(stderr, "%s\n", strerror(errno));
            }
            char vide = ' ';
            char fin_de_ligne = '\n';
            char fin_de_doc = '\0';//a vérifier
            while (strcmp(&caracter, &fin_de_doc) != 0) {
                while (strcmp(&caracter, &vide) == 0 &&
                       strcmp(&caracter, &fin_de_doc) != 0) {//il se peut que le doc se termine apres une ligne vide
                    for (int w = 0; strcmp(&caracter, &fin_de_ligne) !=
                                    0; w++) {//Si le premier element de la ligne est vide, il faut aller a la ligne suivante
                        ret = read(r, &caracter, sizeof(char));
                        if (ret < 0) {
                            fprintf(stderr, "%s\n", strerror(errno));
                        }
                        if (ret == 0) {
                            //c'est la fin du fichier
                        }
                    }
                    ret = read(r, &caracter, sizeof(char));
                }
                char diese = '#';
                while (strcmp(&caracter, &diese) == 0 &&
                       strcmp(&caracter, &fin_de_doc) != 0) {//Il se peut qu'un doc se termine apres une diese
                    for (int w = 0; strcmp(&caracter, &fin_de_ligne) !=
                                    0; w++) {//Si la premier element de la ligne est un diese, il faut aller a la ligne suiv
                        ret = read(r, &caracter, sizeof(char));
                        if (ret < 0) {
                            fprintf(stderr, "%s\n", strerror(errno));
                        }
                        if (ret == 0) {
                            //c'est la fin du fichier
                        }
                    }
                    ret = read(r, &caracter, sizeof(char));
                }
                if (strcmp(&caracter, &fin_de_doc) != 0) {//a faire uniquement si c'est pas la fin d'un doc
                    char nom[65];
                    while (strcmp(&caracter, &vide) != 0) {
                        strcpy(nom, &caracter);//A TESTER!!!!
                        ret = read(r, &caracter, sizeof(char));
                        if (ret < 0) {
                            fprintf(stderr, "%s\n", strerror(errno));
                        }
                    }
                    ret = read(r, &caracter, sizeof(char));// a faire car il faut passer au caractère suivant
                    if (ret < 0) {
                        fprintf(stderr, "%s\n", strerror(errno));
                    }
                    char la_largeur[200];//a discuter j'ai mis 200 en ne sachant pas la longeure que pouvait avoir la longeur et la la_largeur mais on parle de 32 bits
                    while (strcmp(&caracter, &vide) != 0) {
                        strcpy(la_largeur, &caracter);
                        ret = read(r, &caracter, sizeof(char));
                        if (ret < 0) {
                            fprintf(stderr, "%s\n", strerror(errno));
                        }
                    }
                    int la_largeur2 = atoi(la_largeur);
                    ret = read(r, &caracter, sizeof(char));// a faire car il faut passer au caractère suivant
                    if (ret < 0) {
                        fprintf(stderr, "%s\n", strerror(errno));
                    }
                    char la_longeur[200];
                    while (strcmp(&caracter, &vide) != 0) {
                        strcpy(la_longeur, &caracter);
                        ret = read(r, &caracter, sizeof(char));
                        if (ret < 0) {
                            fprintf(stderr, "%s\n", strerror(errno));
                        }
                    }
                    int la_longeur2 = atoi(la_longeur);
                    ret = read(r, &caracter, sizeof(char));
                    if (ret < 0) {
                        fprintf(stderr, "%s\n", strerror(errno));
                    }
                    char reel[200];
                    while (strcmp(&caracter, &vide) != 0) {
                        strcpy(reel, &caracter);
                        ret = read(r, &caracter, sizeof(char));
                        if (ret < 0) {
                            fprintf(stderr, "%s\n", strerror(errno));
                        }
                    }
                    double reel2 = atof(reel);
                    ret = read(r, &caracter, sizeof(char));
                    if (ret < 0) {
                        fprintf(stderr, "%s\n", strerror(errno));
                    }
                    char imaginaire[200];
                    while (strcmp(&caracter, &vide) != 0) {
                        strcpy(imaginaire, &caracter);
                        ret = read(r, &caracter, sizeof(char));
                        if (ret < 0) {
                            fprintf(stderr, "%s\n", strerror(errno));
                        }
                    }
                    double imaginaire2 = atof(imaginaire);



                    //fin de la lecture d'une fractal.









                    struct fractal *fractal_a_creer;
                    int place;
                    sem_wait(&empty1);
                    pthread_mutex_lock(&mutex1);
                    fractal_a_creer = fractal_new(nom, la_largeur2, la_longeur2, reel2, imaginaire2);
                    if (sem_getvalue(&full1, &place) != 0) {
                        fprintf(stderr, "%s\n", strerror(errno));
                    }


                    tab1[place] = fractal_a_creer;
                    //tab1->value=tab1->value+1;
                    pthread_mutex_unlock(&mutex1);
                    sem_post(&full1);
                }

            }

        }
        compteur++;
    }
}









void *proconsumer(void * param){
    struct fractal *fractal_a_calculer;
    struct fractal *fractal_a_free;
    int place;
    while(){//a changer en while(le producer a fini && le tableau est vide)
        sem_wait(&full1);
        pthread_mutex_lock(&mutex1);
        if(sem_getvalue(&full1,&place)!=0){
            fprintf(stderr, "%s\n", strerror(errno));
        }
        fractal_a_free=tab1[place];
        fractal_a_calculer=fractal_new(fractal_a_free->name,fractal_a_free->width,fractal_a_free->height,fractal_a_free->a,fractal_a_free->b);
        free(fractal_a_free);
        //tab1->value=tab1->value-1;  //prend une fractal + decremente la valeur du tableau et remplace l'endoit du tableau dont on a pri la fract par NULL
        pthread_mutex_unlock(&mutex1);
        sem_post (&empty1);
        for(int largeur=0;largeur<(fractal_a_calculer->height);largeur++){
            for(int longeur=0;longeur<(fractal_a_calculer->width);longeur++){
                fractal_compute_value (fractal_a_calculer,longeur,largeur);//ATTENTION REVOIE UN INT
            }
        }//fin du calcul des fractal.
        sem_wait(&empty2);
        pthread_mutex_lock(&mutex2);
        if(sem_getvalue(&full2,&place)!=0){
            fprintf(stderr, "%s\n", strerror(errno));
        }
        tab2[place]=fractal_a_calculer;
        //tab2->value=tab2->value+1;
        pthread_mutex_unlock(&mutex2);
        sem_post(&full2);

    }
}




void *consumer1 (void * param){// pas de -d ds la main
    char *doc_final=(char *) param;
    struct fractal *fractal_moyenne;
    struct fractal *fractal_a_free;
    struct fractal *fractal_a_conserver;
    int premier=1;
    int *b;
    int place;
    while(){//a changer en while(le proconsummer a fini && le tableau est vide)
        sem_wait(&full2);
        pthread_mutex_lock(&mutex2);
        if(sem_getvalue(&full2,&place)!=0){
            fprintf(stderr, "%s\n", strerror(errno));
        }
        fractal_a_free=tab2[place];
        fractal_moyenne=fractal_new(fractal_a_free->name,fractal_a_free->width,fractal_a_free->height,fractal_a_free->a,fractal_a_free->b);
        b=fractal_a_free->value;//obligé de faire ca car fractal_new ne prend pas en compte le tableau value (ici, hyper important)
        fractal_moyenne->value=b;
        free(fractal_a_free);
        //tab2->value=tab2->value-1;//prend une fractal dont il faut calculer la moyenne +decremente et remplace l'endroit du tableau dont on a pris la fract par NULL
        pthread_mutex_unlock(&mutex2);
        sem_post(&empty2);
        b=fractal_moyenne->value;
        int max_tableau=fractal_moyenne->width * fractal_moyenne->height;
        int a_comparer=0;

        for(int i=0;i<max_tableau;i++){
            a_comparer=a_comparer+*(b+i);
        }
        a_comparer=a_comparer/(max_tableau);//a cheker si on mettrait pas plutot un double et une division plus précise.
        if(a_comparer>le_plus_grand){
            if(premier){
                fractal_a_conserver=fractal_moyenne;
                premier=0;
            }
            else {
                le_plus_grand = a_comparer;
                free(fractal_a_conserver);//normalement c'est ok nn?
                fractal_a_conserver = fractal_moyenne;
            }
        }
        else{
            free(fractal_moyenne);
        }

    }
    int fin=write_bitmap_sdl(fractal_a_conserver,doc_final);

}


void *consumer2 (void * param){// -d ds la main

    char *doc_final;
    struct fractal *fractal_moyenne;
    struct fractal *fractal_a_free;
    //struct fractal *fractal_a_conserver;
    //int premier=1;
    int *b;
    int place;
    while(){//a changer en while(le proconsummer a fini && le tableau est vide)
        sem_wait(&full2);
        pthread_mutex_lock(&mutex2);
        if(sem_getvalue(&full2,&place)!=0){
            fprintf(stderr, "%s\n", strerror(errno));
        }
        fractal_a_free=tab2[place];
        fractal_moyenne=fractal_new(fractal_a_free->name,fractal_a_free->width,fractal_a_free->height,fractal_a_free->a,fractal_a_free->b);
        b=fractal_a_free->value;//obligé de faire ca car fractal_new ne prend pas en compte le tableau value (ici, hyper important)
        fractal_moyenne->value=b;
        free(fractal_a_free);
        //tab2->value=tab2->value-1;//prend une fractal dont il faut calculer la moyenne +decremente et remplace l'endroit du tableau dont on a pris la fract par NULL
        pthread_mutex_unlock(&mutex2);
        sem_post(&empty2);
        doc_final=fractal_moyenne->name;

        int fin=write_bitmap_sdl(fractal_moyenne,doc_final);
        free(fractal_moyenne);

    }

}








int main (int argc, char *argv[]) {
    int nthread;
    int compt;
    char *c = argv[1];


    int error;
    error = pthread_mutex_init(&mutex1, NULL);
    if (error != 0) {
        return (EXIT_FAILURE);
    }
    error = pthread_mutex_init(&mutex2, NULL);
    if (error != 0) {
        return (EXIT_FAILURE);
    }
    error = sem_init(&empty1, 0, 20);// N a discuter
    if (error != 0) {
        return (EXIT_FAILURE);
    }
    error = sem_init(&empty2, 0, 20);//N a discuter
    if (error != 0) {
        return (EXIT_FAILURE);
    }
    error = sem_init(&full1, 0, 0);
    if (error != 0) {
        return (EXIT_FAILURE);
    }
    error = sem_init(&full2, 0, 0);
    if (error != 0) {
        return (EXIT_FAILURE);
    }

    if (strcmp(c, "-d") == 0) {//si -d est présent ds la main



        compt = argc - 4;
        nthread = *argv[2];





        pthread_t thread_pro;
        pthread_t thread_co;
        int arg[2];
        arg[0] = compt;
        arg[1] = 1;//-d ds la main
        char *doc_fin=argv[argc-1];
        int err=pthread_create(&(thread_pro),NULL,&producer, arg);
        if (err != 0) {
            return (EXIT_FAILURE);
        }
        err = pthread_create(&(thread_co), NULL, &consumer2, NULL);
        if (err != 0) {
            return (EXIT_FAILURE);
        }
        int pro=nthread-2;
        int i=0;
        pthread_t thread_proco[pro];
        while(i<pro){
            err = pthread_create(&(thread_proco[i]), NULL, &proconsumer, NULL);
            if (err != 0) {
                return (EXIT_FAILURE);
            }
            i++;
        }
    }

    else {
        compt = argc - 3;
        nthread = *argv[2];

        pthread_t thread_pro;//producteur qui lit les fractal et les met dans un tableau
        pthread_t thread_co;//consommateur final qui compare les valeur calculee
        int arg[2];
        arg[0] = compt;
        arg[1] = 0;//pas de -d ds la main
        int err=pthread_create(&(thread_pro),NULL,&producer,arg);
        if (err != 0) {
            return (EXIT_FAILURE);
        }
        err = pthread_create(&(thread_co), NULL, &consumer1, NULL);
        if (err != 0) {
            return (EXIT_FAILURE);
        }
        int pro=nthread-2;
        int i=0;
        pthread_t thread_proco[pro];
        while(i<pro){
            err = pthread_create(&(thread_proco[i]), NULL, &proconsumer, NULL);
            if (err != 0) {
                return (EXIT_FAILURE);
            }
            i++;
        }






        return 0;
    }
}



























