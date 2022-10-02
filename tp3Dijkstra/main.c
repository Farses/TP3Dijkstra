#include <stdio.h>
#include "Graphe.h"
#include "fifo.h"

void afficher_successeurs(pSommet * sommet, int num)
{

    printf(" sommet %d (%d adj):\n",num, sommet[num]->nbSommetAdjacent);

    pArc arc=sommet[num]->arc;

    while(arc!=NULL)
    {
        printf("%d(%d) ",arc->sommet, arc->poids);
        arc=arc->arc_suivant;
    }

}

// Ajouter l'arete entre les sommets s1 et s2 du graphe
pSommet* CreerArete(pSommet* sommet,int s1,int s2, int poids)
{
    if(sommet[s1]->arc==NULL)
    {
        pArc Newarc=(pArc)malloc(sizeof(struct Arc));
        Newarc->sommet=s2;
        Newarc->arc_suivant=NULL;
        Newarc->poids = poids ;
        sommet[s1]->arc=Newarc;
        sommet[s1]->nbSommetAdjacent ++ ;
        return sommet;
    }

    else
    {
        pArc temp=sommet[s1]->arc;
        sommet[s1]->nbSommetAdjacent ++ ;
        while(!(temp->arc_suivant==NULL))
        {
            temp=temp->arc_suivant;
        }
        pArc Newarc=(pArc)malloc(sizeof(struct Arc));
        Newarc->sommet=s2;
        Newarc->poids = poids ;
        Newarc->arc_suivant=NULL;

        if(temp->sommet>s2)
        {
            Newarc->arc_suivant=temp->arc_suivant;
            Newarc->sommet=temp->sommet;
            Newarc->poids = temp->poids ;
            temp->sommet=s2;
            temp->poids = poids ;
            temp->arc_suivant=Newarc;

            return sommet;
        }

        temp->arc_suivant=Newarc;
        return sommet;
    }
}

// creer le graphe
Graphe* CreerGraphe(int ordre)
{
    Graphe * Newgraphe=(Graphe*)malloc(sizeof(Graphe));
    Newgraphe->pSommet = (pSommet*)malloc(ordre*sizeof(pSommet));

    for(int i=0; i<ordre; i++)
    {
        Newgraphe->pSommet[i]=(pSommet)malloc(sizeof(struct Sommet));
        Newgraphe->pSommet[i]->predecesseur= -1;
        Newgraphe->pSommet[i]->couleur= 0;
        Newgraphe->pSommet[i]->arc=NULL;
        Newgraphe->pSommet[i]->numeroSommet = i ;
        Newgraphe->pSommet[i]->nbSommetAdjacent = 0 ;

    }
    return Newgraphe;
}


Graphe * lire_graphe(char * nomFichier)
{
    Graphe* graphe;
    FILE * ifs = fopen(nomFichier,"r");
    int taille, orientation, ordre, s1, s2, poids;

    if (!ifs)
    {
        printf("Erreur de lecture fichier\n");
        exit(-1);
    }

    fscanf(ifs,"%d",&ordre);

    graphe=CreerGraphe(ordre); // creer le graphe d'ordre sommets

    fscanf(ifs,"%d",&taille);
    fscanf(ifs,"%d",&orientation);

    graphe->orientation=orientation;
    graphe->ordre=ordre;

    // creer les aretes du graphe
    for (int i=0; i<taille; ++i)
    {
        fscanf(ifs,"%d%d%d",&s1,&s2,&poids);
        graphe->pSommet=CreerArete(graphe->pSommet, s1, s2, poids);

        if(!orientation)
            graphe->pSommet=CreerArete(graphe->pSommet, s2, s1, poids);
    }

    return graphe;
}

/*affichage du graphe avec les successeurs de chaque sommet */
void graphe_afficher(Graphe* graphe)
{
    printf("graphe\n");

    if(graphe->orientation)
        printf("oriente\n");
    else
        printf("non oriente\n");

    printf("ordre = %d\n",graphe->ordre);

    printf("listes d'adjacence :\n");

    for (int i=0; i<graphe->ordre; i++)
    {
        afficher_successeurs(graphe->pSommet, i);
        printf("\n");
    }

}

int determinerSommetPrioritaire(Dijkstra* algo, Graphe* g) {
    int distance = 999, sommet = 0 ; //Ici, on initialise la variable 'distance' à 999 pour qu'elle soit forcément au-dessus de la distance du 1er sommet au sommet initial qu'on va verifier dans la boucle for.
    for(int k = 0 ; k < g->ordre ; k++) {
        if(distance > algo[k].distance && algo[k].distance != 0 && algo[k].marque == 0) {
            distance = algo[k].distance ;
            sommet = k ;
        }
    }
    return sommet ;
}

void algoDijk(Graphe* graphe, Dijkstra algo[], int sommetInitiale) {
    int marqueTotal = 0 ;

    ///Initialisation du tableau contenant les informations relatives à l'algorithme de Dijkstra
    for(int i = 0 ; i < graphe->ordre ; i++) { // i correspond aux sommets
        algo[i].predecesseur = -1 ;
        algo[i].distance = 0 ;
        algo[i].marque = 0 ;
    }
    int sommetPrioritaire = sommetInitiale ;
    while(marqueTotal != graphe->ordre){ //On continue la boucle tant qu'il reste encore des sommets à marquer (fonctionne donc qu'avec un graphe connexe)
        marqueTotal = 0 ;
        pArc temp = graphe->pSommet[sommetPrioritaire]->arc;
        while (temp != NULL) { //On explore les sommets adjacents du sommet qu'on etudie
            if (algo[temp->sommet].marque == 0) {
                if(algo[temp->sommet].distance == 0 ||
                algo[temp->sommet].distance > algo[sommetPrioritaire].distance + temp->poids) { // Pour metter à jour la distance, il faut que l'ancienne soit plus grand que celle où l'on est.
                    algo[temp->sommet].distance = algo[sommetPrioritaire].distance + temp->poids ;
                    algo[temp->sommet].predecesseur = sommetPrioritaire; //Mettre les predecesseurs
                }
            }
            ///POUR PASSER AU SOMMET ADJACENT SUIVANT
            temp = temp->arc_suivant;
        }
        algo[sommetPrioritaire].marque = 1 ;
        for(int i = 0 ; i < graphe->ordre ; i++) { //Compte le nombre de sommets marqués
            if(algo[i].marque == 1) {
                marqueTotal++ ;
            }
        }
        ///Après avoir explorer tous les sommets adjacents d'un sommet n, on determine la distance la plus courte parmi tous les sommets afin qu'il devienne à son tour prioritaire
        if(marqueTotal != graphe->ordre) {
            sommetPrioritaire = determinerSommetPrioritaire(algo, graphe);
        }
    }
}

void afficherCheminPlusCourt(Dijkstra algo[], int sommetInitiale, int sommetFinale) {
    int i = sommetFinale, k = sommetFinale;
    do{
        printf("%d <-- ", i) ;
        i = algo[i].predecesseur ;
    }while(i != sommetInitiale) ; //On parcour notre tableau de l'algorithme
    printf("%d  (distance = %d)", sommetInitiale, algo[k].distance) ;
}


int main() {
    Graphe * g=lire_graphe("graphe1_TP3.txt");
    Dijkstra algo[g->ordre] ;
    int sommetInitial, sommetFinal ;


    printf("numero du sommet initial : \n");

    /// afficher le graphe
    graphe_afficher(g);

    printf("Quel est le sommet de depart ?\n") ;
    scanf("%d", &sommetInitial) ;
    printf("Quel est le sommet de d'arrive ?\n") ;
    scanf("%d", &sommetFinal) ;
    algoDijk(g, algo, sommetInitial) ;
    afficherCheminPlusCourt(algo, sommetInitial, sommetFinal) ;

    return 0;
}
