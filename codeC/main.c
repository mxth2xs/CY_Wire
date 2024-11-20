#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "arbre.h"

void generateTopAndBottom10Data(AVLNode* root, FILE* dataFile, int* count, int limit, int isTop) {
    if (!root || *count >= limit) {
        return;
    }

    if (isTop) {
        generateTopAndBottom10Data(root->right, dataFile, count, limit, isTop); // Parcours inversé pour les plus chargés
    } else {
        generateTopAndBottom10Data(root->left, dataFile, count, limit, isTop); // Parcours normal pour les moins chargés
    }

    if (*count < limit) {
        fprintf(dataFile, "%d %ld %.2f\n", root->key, root->capacity, root->consumption);
        (*count)++;
    }

    if (isTop) {
        generateTopAndBottom10Data(root->left, dataFile, count, limit, isTop);
    } else {
        generateTopAndBottom10Data(root->right, dataFile, count, limit, isTop);
    }
}

void generateGnuplotScript(const char* scriptPath) {
    FILE* gp = fopen(scriptPath, "w");
    if (!gp) {
        perror("Erreur d'ouverture du fichier Gnuplot");
        return;
    }

    fprintf(gp,
            "set terminal png size 1200,600\n"
            "set output '/home/cytech/CY_Wire/output/lv_chart.png'\n"
            "set title 'Top 10 and Bottom 10 LV Stations by Load'\n"
            "set boxwidth 0.5 relative\n"
            "set style fill solid\n"
            "set ylabel 'Load (kW)'\n"
            "set xlabel 'Station ID'\n"
            "set xtics rotate by -45\n"
            "plot '/home/cytech/CY_Wire/output/lv_top10.dat' using 0:3:2:3 with boxes lc rgb 'red' title 'Top 10', \\\n"
            "     '/home/cytech/CY_Wire/output/lv_bottom10.dat' using 0:3:2:3 with boxes lc rgb 'green' title 'Bottom 10'\n");
    fclose(gp);
}

void readline(int columnequal, AVLNode** arbre, FILE* fichier) {
    char ligne[1024];
    int ligne_num = 0;

    while (fgets(ligne, sizeof(ligne), fichier)) {
        ligne_num++;

        char *colonnes[8] = {NULL};
        char *token = strtok(ligne, ";");
        int i = 0;

        while (token != NULL && i < 8) {
            colonnes[i] = token;
            i++;
            token = strtok(NULL, ";");
        }

        if (!colonnes[columnequal] || colonnes[columnequal][0] == '\0') {
            fprintf(stderr, "Ligne %d ignorée : clé vide ou nulle.\n", ligne_num);
            continue;
        }

        if (!colonnes[6] || colonnes[6][0] == '\0') {
            fprintf(stderr, "Ligne %d ignorée : capacité vide ou nulle.\n", ligne_num);
            continue;
        }

        int key = atoi(colonnes[columnequal]);
        long capacity = atol(colonnes[6]);
        double consumption = (colonnes[7] != NULL && colonnes[7][0] != '-') ? atof(colonnes[7]) : 0.0;

        *arbre = insertNode(*arbre, key, capacity, consumption);
    }

    printf("Lecture du fichier terminée. Total de lignes lues : %d\n", ligne_num);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <station_type> <consumer_type> <plant_id>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *station_type = argv[1];
    char *consumer_type = argv[2];
    char *plant_id = argv[3];

    char nomFichier[256];

    if (strcmp(plant_id, "-1") == 0) {
        sprintf(nomFichier, "tmp/filter_%s_%s.csv", station_type, consumer_type);
    } else {
        sprintf(nomFichier, "tmp/filter_%s_%s_%s.csv", station_type, consumer_type, plant_id);
    }

    FILE *fichier = fopen(nomFichier, "r");
    if (fichier == NULL) {
        perror("Erreur d'ouverture du fichier");
        return EXIT_FAILURE;
    }

    AVLNode *arbre = NULL;

    if (strcmp(station_type, "hvb") == 0) {
        readline(1, &arbre, fichier);
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

    char outputFichier[256];
    sprintf(outputFichier, "/home/cytech/CY_Wire/output/output_%s.csv", station_type);
    FILE *outputFile = fopen(outputFichier, "w");
    if (outputFile == NULL) {
        perror("Erreur d'ouverture du fichier de sortie");
        freeTree(arbre);
        return EXIT_FAILURE;
    }

    fprintf(outputFile, "ID%s:Capacity:Consumption\n", station_type);
    inorderTraversalToCSV(arbre, outputFile);

    printf("Parcours In-Order enregistré dans le fichier : %s\n", outputFichier);
    fclose(outputFile);

    if (strcmp(station_type, "lv") == 0 && strcmp(consumer_type, "all") == 0) {
        printf("Génération des graphiques pour les postes LV les plus et moins chargés...\n");

        FILE* top10Data = fopen("/home/cytech/CY_Wire/output/lv_top10.dat", "w");
        if (!top10Data) {
            perror("Erreur d'ouverture du fichier pour les 10 postes les plus chargés");
            freeTree(arbre);
            return EXIT_FAILURE;
        }
        int count = 0;
        generateTopAndBottom10Data(arbre, top10Data, &count, 10, 1);
        fclose(top10Data);

        FILE* bottom10Data = fopen("/home/cytech/CY_Wire/output/lv_bottom10.dat", "w");
        if (!bottom10Data) {
            perror("Erreur d'ouverture du fichier pour les 10 postes les moins chargés");
            freeTree(arbre);
            return EXIT_FAILURE;
        }
        count = 0;
        generateTopAndBottom10Data(arbre, bottom10Data, &count, 10, 0);
        fclose(bottom10Data);

        generateGnuplotScript("/home/cytech/CY_Wire/output/lv_plot.gp");
        system("gnuplot /home/cytech/CY_Wire/output/lv_plot.gp");
    }

    freeTree(arbre);
    return EXIT_SUCCESS;
}
