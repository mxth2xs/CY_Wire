#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "arbre.h"

// Fonction pour lire le fichier CSV et construire l'arbre AVL
void readline(int columnequal, AVLNode** arbre, FILE* fichier) {
    char ligne[1024]; // Buffer pour lire chaque ligne du fichier
    int ligne_num = 0; // Compteur de lignes

    while (fgets(ligne, sizeof(ligne), fichier)) {
        ligne_num++; // Incrémenter le compteur de lignes

        // Diviser la ligne en colonnes
        char *colonnes[8] = {NULL};
        char *token = strtok(ligne, ";");
        int i = 0;

        while (token != NULL && i < 8) {
            colonnes[i] = token;
            i++;
            token = strtok(NULL, ";");
        }

        // Vérifier que les colonnes nécessaires sont bien remplies
        if (!colonnes[columnequal] || colonnes[columnequal][0] == '\0') {
            fprintf(stderr, "Ligne %d ignorée : clé vide ou nulle.\n", ligne_num);
            continue;
        }

        if (!colonnes[6] || colonnes[6][0] == '\0') {
            fprintf(stderr, "Ligne %d ignorée : capacité vide ou nulle.\n", ligne_num);
            continue;
        }

        // Convertir les colonnes en valeurs
        int key = atoi(colonnes[columnequal]);
        long capacity = atol(colonnes[6]);
        double consumption = (colonnes[7] != NULL && colonnes[7][0] != '-') ? atof(colonnes[7]) : 0.0;

        // Insérer dans l'arbre
        *arbre = insertNode(*arbre, key, capacity, consumption);
    }

    printf("Lecture du fichier terminée. Total de lignes lues : %d\n", ligne_num);
}

// Fonction principale
int main(int argc, char *argv[]) {
    // Vérifier que 3 arguments sont fournis
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <station_type> <consumer_type> <plant_id>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *station_type = argv[1];
    char *consumer_type = argv[2];
    char *plant_id = argv[3];

    char nomFichier[256]; // Taille suffisante pour stocker le chemin complet

    // Construire le nom de fichier dynamique
    if (strcmp(plant_id, "-1") == 0) {
        sprintf(nomFichier, "tmp/filter_%s_%s.csv", station_type, consumer_type);
    } else {
        sprintf(nomFichier, "tmp/filter_%s_%s_%s.csv", station_type, consumer_type, plant_id);
    }

    // Ouvrir le fichier en mode lecture
    FILE *fichier = fopen(nomFichier, "r");
    if (fichier == NULL) {
        perror("Erreur d'ouverture du fichier");
        return EXIT_FAILURE;
    }

    AVLNode *arbre = NULL; // Initialiser l'arbre à NULL

    // Lire les données en fonction du type de station
    if (strcmp(station_type, "hvb") == 0) {
        readline(1, &arbre, fichier); // Passer l'adresse de l'arbre
    } else if (strcmp(station_type, "hva") == 0) {
        readline(2, &arbre, fichier);
    } else if (strcmp(station_type, "lv") == 0) {
        readline(3, &arbre, fichier);
    } else {
        fprintf(stderr, "Type de station inconnu : %s\n", station_type);
        fclose(fichier);
        return EXIT_FAILURE;
    }

    fclose(fichier);

        struct stat st = {0};
    if (stat("/home/cytech/CY_Wire/output", &st) == -1) {
        mkdir("/home/cytech/CY_Wire/output", 0755);
    }
    // Créer le fichier CSV de sortie
    char outputFichier[256];
    sprintf(outputFichier, "/home/cytech/CY_Wire/output/output_%s.csv", station_type); // Chemin absolu pour le fichier de sortie
    FILE *outputFile = fopen(outputFichier, "w");
    if (outputFile == NULL) {
        perror("Erreur d'ouverture du fichier de sortie");
        freeTree(arbre);
        return EXIT_FAILURE;
    }

    // Écrire l'en-tête du fichier CSV
    fprintf(outputFile, "ID%s:Capacity:Consumption\n", station_type);

    // Écrire les données triées par capacité dans le fichier CSV
    inorderTraversalToCSV(arbre, outputFile);

    printf("Parcours In-Order enregistré dans le fichier : %s\n", outputFichier);

    // Fermer le fichier de sortie
    fclose(outputFile);

    // Libérer la mémoire
    freeTree(arbre);

    return EXIT_SUCCESS;
}
