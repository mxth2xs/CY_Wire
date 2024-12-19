#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arbre.h"

// Structure to dynamically hold station data
typedef struct {
    int key;
    long capacity;
    double consumption;
    double difference;
} StationDiff;

int compareByDifference(const void* a, const void* b) {
    const StationDiff* stationA = (const StationDiff*)a;
    const StationDiff* stationB = (const StationDiff*)b;

    if (stationA->difference < stationB->difference) return -1;
    if (stationA->difference > stationB->difference) return 1;
    return 0;
}

int compareByCapacity(const void* a, const void* b) {
    const StationDiff* stationA = (const StationDiff*)a;
    const StationDiff* stationB = (const StationDiff*)b;

    if (stationA->capacity < stationB->capacity) return -1;
    if (stationA->capacity > stationB->capacity) return 1;
    return 0;
}

void collectStations(AVLNode* root, StationDiff** stations, int* count, int* size) {
    if (!root) return;

    collectStations(root->left, stations, count, size);

    if (*count >= *size) {
        *size *= 2;
        *stations = realloc(*stations, (*size) * sizeof(StationDiff));
        if (!*stations) {
            perror("Memory reallocation error");
            exit(EXIT_FAILURE);
        }
    }

    (*stations)[*count].key = root->key;
    (*stations)[*count].capacity = root->capacity;
    (*stations)[*count].consumption = root->consumption;
    (*stations)[*count].difference = root->capacity - root->consumption;
    (*count)++;

    collectStations(root->right, stations, count, size);
}

void inorderTraversalToSortedFile(AVLNode* root, FILE* outputFile) {
    if (!root) return;

    int count = 0;
    int size = 1024;
    StationDiff* stations = malloc(size * sizeof(StationDiff));
    if (!stations) {
        perror("Memory allocation error");
        exit(EXIT_FAILURE);
    }

    collectStations(root, &stations, &count, &size);
    qsort(stations, count, sizeof(StationDiff), compareByCapacity);

    for (int i = 0; i < count; i++) {
        fprintf(outputFile, "%d:%ld:%.2f\n", stations[i].key, stations[i].capacity, stations[i].consumption);
    }

    free(stations);
}

void generateTopAndBottom10(AVLNode* root, FILE* topFile, FILE* bottomFile, int limit) {
    if (!root) return;

    int count = 0;
    int size = 1024;
    StationDiff* stations = malloc(size * sizeof(StationDiff));
    if (!stations) {
        perror("Memory allocation error");
        exit(EXIT_FAILURE);
    }

    collectStations(root, &stations, &count, &size);
    qsort(stations, count, sizeof(StationDiff), compareByDifference);

    for (int i = 0; i < limit && i < count; i++) {
        // Top stations: largest differences at the end of the array
        fprintf(topFile, "%d:%ld:%.2f:%.2f\n",
                stations[count - 1 - i].key,
                stations[count - 1 - i].capacity,
                stations[count - 1 - i].consumption,
                stations[count - 1 - i].difference);

        // Bottom stations: smallest differences at the start of the array
        fprintf(bottomFile, "%d:%ld:%.2f:%.2f\n",
                stations[i].key,
                stations[i].capacity,
                stations[i].consumption,
                stations[i].difference);
    }

    free(stations);
}

void generateGnuplotScript(const char* scriptPath, const char* top10Path, const char* bottom10Path, const char* outputPath) {
    FILE* gp = fopen(scriptPath, "w");
    if (!gp) {
        perror("Error opening Gnuplot script file");
        return;
    }

    fprintf(gp,
        "set terminal png size 1200,600\n"
        "set output '%s'\n"
        "set title 'Top 10 and Bottom 10 Stations by Difference'\n"
        "set boxwidth 0.5 relative\n"
        "set style fill solid border -1\n"
        "set ylabel 'Difference (kW)'\n"
        "set xlabel 'Station ID'\n"
        "set xtics rotate by -45\n"
        "set yrange [*:*]\n"
        "set grid ytics\n"
        "set datafile separator \":\"\n"
        "plot '%s' using 4:xtic(1) with boxes title 'Overload (Top 10)' lc rgb 'red',\\\n"
        "     '%s' using 4:xtic(1) with boxes title 'Underutilized (Bottom 10)' lc rgb 'green'\n",
        outputPath, top10Path, bottom10Path);

    fclose(gp);
}

void readLines(int columnIndex, AVLNode** tree, FILE* file) {
    char line[1024];
    int lineNumber = 0;

    while (fgets(line, sizeof(line), file)) {
        lineNumber++;
        char* columns[8] = {NULL};
        char* token = strtok(line, ";");
        int i = 0;

        while (token != NULL && i < 8) {
            columns[i] = token;
            i++;
            token = strtok(NULL, ";");
        }

        if (!columns[columnIndex] || columns[columnIndex][0] == '\0') {
            fprintf(stderr, "Line %d ignored: empty or null key.\n", lineNumber);
            continue;
        }

        if (!columns[6] || columns[6][0] == '\0') {
            fprintf(stderr, "Line %d ignored: empty or null capacity.\n", lineNumber);
            continue;
        }

        int key = atoi(columns[columnIndex]);
        long capacity = atol(columns[6]);
        double consumption = (columns[7] != NULL && columns[7][0] != '-') ? atof(columns[7]) : 0.0;

        *tree = insertNode(*tree, key, capacity, consumption);
    }

    printf("File reading completed. Total lines read: %d\n", lineNumber);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <station_type> <consumer_type> <plant_id>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *station_type = argv[1];
    char *consumer_type = argv[2];
    char *plant_id = argv[3];

    char inputFileName[256];
    if (strcmp(plant_id, "-1") == 0) {
        sprintf(inputFileName, "tmp/filter_%s_%s.csv", station_type, consumer_type);
    } else {
        sprintf(inputFileName, "tmp/filter_%s_%s_%s.csv", station_type, consumer_type, plant_id);
    }

    FILE *inputFile = fopen(inputFileName, "r");
    if (inputFile == NULL) {
        perror("Error opening input file");
        return EXIT_FAILURE;
    }

    AVLNode *tree = NULL;
    if (strcmp(station_type, "hvb") == 0) {
        readLines(1, &tree, inputFile);
    } else if (strcmp(station_type, "hva") == 0) {
        readLines(2, &tree, inputFile);
    } else if (strcmp(station_type, "lv") == 0) {
        readLines(3, &tree, inputFile);
    } else {
        fprintf(stderr, "Unknown station type: %s\n", station_type);
        fclose(inputFile);
        return EXIT_FAILURE;
    }

    fclose(inputFile);

    char outputFileName[256];
    sprintf(outputFileName, "output/sorted_%s_%s.csv", station_type, consumer_type);
    FILE *outputFile = fopen(outputFileName, "w");
    if (!outputFile) {
        perror("Error opening output file");
        freeTree(tree);
        return EXIT_FAILURE;
    }

    fprintf(outputFile, "ID%s:Capacity:Consumption\n", station_type);
    inorderTraversalToSortedFile(tree, outputFile);
    fclose(outputFile);

    // Only generate top/bottom 10 and plot if station_type=lv and consumer_type=all
    if (strcmp(station_type, "lv") == 0 && strcmp(consumer_type, "all") == 0) {
        FILE* top10File = fopen("output/top10_lv_all.csv", "w");
        FILE* bottom10File = fopen("output/bottom10_lv_all.csv", "w");
        if (!top10File || !bottom10File) {
            perror("Error opening top/bottom 10 files");
            freeTree(tree);
            return EXIT_FAILURE;
        }

        generateTopAndBottom10(tree, top10File, bottom10File, 10);

        fclose(top10File);
        fclose(bottom10File);

        char scriptPath[256];
        sprintf(scriptPath, "output/plot_%s_%s.gp", station_type, consumer_type);
        generateGnuplotScript(scriptPath, "output/top10_lv_all.csv", "output/bottom10_lv_all.csv", "output/chart_lv_all.png");

        system("gnuplot output/plot_lv_all.gp");
    }

    freeTree(tree);
    return EXIT_SUCCESS;
}
