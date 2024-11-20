#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arbre.h"

void readline(int columnequal, AVLNode* arbre, FILE* fichier, char* ligne){
    while (fgets(ligne, sizeof(ligne), fichier))
    {

        // Diviser la ligne en colonnes
        char *colonnes[8];
        char *token = strtok(ligne, ";");
        int i = 0;
        while (token != NULL && i < 8)
        {
            colonnes[i] = token;
            i++;
            token = strtok(NULL, ";");
        }

        // Vérifier si la ligne correspond au type de station spécifié
        if (colonnes[6][0] != '-' && colonnes[columnequal][0] == '-')
        {
            // Lire la consommation
            long consommation = atol(colonnes[6]); // Colonne 6 = capacité ou charge
            insertNode(arbre, colonnes[columnequal], colonnes[6], 0);
        }
        else{
            AVLNode* noeud = searchNode(arbre, colonnes[columnequal]);
            if(colonnes[7][0] != '-'){
                noeud->consumption+=atol(colonnes[7]);
            }
            else{
                noeud->consumption+=atol(colonnes[6]);
            }
            
        }
    }
}

int main(int argc, char *argv[])
{
    // Vérifier que 3 arguments sont fournis
    if (argc != 4)
    {
        return -1; // Retourner une erreur
    }

    char *station_type = argv[1];
    char *consumer_type = argv[2];
    char *plant_id = argv[3];

    char nomFichier[256]; // Taille suffisante pour stocker le chemin complet

    // Construire le nom de fichier dynamique
    sprintf(nomFichier, "tmp/filter_%s_%s_%s.csv", station_type, consumer_type, plant_id);

    // Ouvrir le fichier en mode lecture
    FILE *fichier = fopen(nomFichier, "r");
    if (fichier == NULL)
    {
        perror("Erreur d'ouverture du fichier");
        return EXIT_FAILURE;
    }

    char ligne[1024];     // Buffer pour lire chaque ligne
    AVLNode *arbre = NULL; // Liste des nœuds

    if (strcmp(station_type, "hvb") == 0)
    {
        // read(colonne_diff, colonne_equal)
        readline(2, arbre, fichier, ligne);
        // Lire chaque ligne jusqu'à la fin du fichier
    }
    if (strcmp(station_type, "hva") == 0)
    {
        // read(colonne_diff, colonne_equal)
        readline(3, arbre, fichier, ligne);
        // Lire chaque ligne jusqu'à la fin du fichier
    }
    if (strcmp(station_type, "lv") == 0)
    {
        // read(colonne_diff, colonne_equal)
        readline(1, arbre, fichier, ligne);
        // Lire chaque ligne jusqu'à la fin du fichier
    }

    


    fclose(fichier);

    return EXIT_SUCCESS;
}