#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"

// Structure to dynamically hold station data
typedef struct {
    int key;
    long capacity;
    double consumption;
    double difference;
} StationDiff;

int compareByDifference(const void* a, const void* b) {
/**
 * @brief Compares two StationDiff structures by their difference values.
 * 
 * @param a Pointer to the first StationDiff structure.
 * @param b Pointer to the second StationDiff structure.
 * @return An integer less than, equal to, or greater than zero if the difference
 *         of the first structure is less than, equal to, or greater than that of the second.
 */

    const StationDiff* stationA = (const StationDiff*)a;
    const StationDiff* stationB = (const StationDiff*)b;

    if (stationA->difference < stationB->difference) return -1;
    if (stationA->difference > stationB->difference) return 1;
    return 0;
}

int compareByCapacity(const void* a, const void* b) {
/**
 * @brief Compares two StationDiff structures by their capacity values.
 * 
 * @param a Pointer to the first StationDiff structure.
 * @param b Pointer to the second StationDiff structure.
 * @return An integer less than, equal to, or greater than zero if the capacity
 *         of the first structure is less than, equal to, or greater than that of the second.
 */

    const StationDiff* stationA = (const StationDiff*)a;
    const StationDiff* stationB = (const StationDiff*)b;

    if (stationA->capacity < stationB->capacity) return -1;
    if (stationA->capacity > stationB->capacity) return 1;
    return 0;
}

void collectStations(AVLNode* root, StationDiff** stations, int* count, int* size) {
/**
 * @brief Collects all stations from an AVL tree into a dynamic array of StationDiff structures.
 * 
 * @param root The root of the AVL tree.
 * @param stations Pointer to the dynamic array of StationDiff structures.
 * @param count Pointer to the number of stations collected so far.
 * @param size Pointer to the current size of the dynamic array.
 */

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
/**
 * @brief Performs an inorder traversal of the AVL tree and writes sorted stations to a file.
 * 
 * @param root The root of the AVL tree.
 * @param outputFile The file to write the sorted station data.
 */

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
/**
 * @brief Generates the top and bottom 10 stations by difference and writes them to separate files.
 * 
 * @param root The root of the AVL tree.
 * @param topFile The file to write the top stations with the largest differences.
 * @param bottomFile The file to write the bottom stations with the smallest differences.
 * @param limit The number of stations to include in each file.
 */

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
/**
 * @brief Generates a Gnuplot script for visualizing the top and bottom 10 stations.
 * 
 * @param scriptPath The path to save the Gnuplot script file.
 * @param top10Path The path to the file containing the top 10 stations.
 * @param bottom10Path The path to the file containing the bottom 10 stations.
 * @param outputPath The path to save the generated chart image.
 */

    FILE* gp = fopen(scriptPath, "w");
    if (!gp) {
        perror("Error opening Gnuplot script file");
        return;
    }

    fprintf(gp,
    "set logscale y\n"
    "set terminal png size 1200,600\n"
    "set output '%s'\n"
    "set title 'Top 10 and Bottom 10 Stations by Difference'\n"
    "set boxwidth 0.4 relative\n" // Réduit la largeur des barres
    "set style fill solid border -1\n"
    "set ylabel 'Difference (kW)'\n"
    "set xlabel 'Station ID'\n"
    "set xtics rotate by -45\n"
    "set grid ytics\n"
    "set datafile separator \":\"\n"
    "plot '%s' using ($0-0.2):4:xtic(1) with boxes title 'Overload (Top 10)' lc rgb 'red',\\\n"
    "     '%s' using ($0+0.2):($4*(-1)):xtic(1) with boxes title 'Underutilized (Bottom 10)' lc rgb 'green'\n",
    outputPath, top10Path, bottom10Path);
    


    fclose(gp);
}

void readLines(int columnIndex, AVLNode** tree, FILE* file) {
/**
 * @brief Reads lines from a CSV file and inserts them as nodes into an AVL tree.
 * 
 * @param columnIndex The column index in the CSV file that represents the key.
 * @param tree Pointer to the root of the AVL tree.
 * @param file The input file to read data from.
 */

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
/**
 * @brief The main entry point of the program. Parses command-line arguments, processes input data,
 *        performs AVL tree operations, generates files, and optionally visualizes data.
 * 
 * @param argc The number of command-line arguments.
 * @param argv The array of command-line arguments.
 * @return EXIT_SUCCESS on successful execution, or EXIT_FAILURE on error.
 */

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
        // Open top10 and bottom10 files for writing
        FILE* top10File = fopen("output/top10_lv_all.csv", "w");
        FILE* bottom10File = fopen("output/bottom10_lv_all.csv", "w");
        if (!top10File || !bottom10File) {
            perror("Error opening top/bottom 10 files");
            freeTree(tree);
            return EXIT_FAILURE;
        }

        // Generate top and bottom 10 stations
        generateTopAndBottom10(tree, top10File, bottom10File, 10);

        // Close the top and bottom files before processing them
        fclose(top10File);
        fclose(bottom10File);

        // Now, open the same files for reading
        top10File = fopen("output/top10_lv_all.csv", "r");
        bottom10File = fopen("output/bottom10_lv_all.csv", "r");
        if (!top10File || !bottom10File) {
            perror("Error opening top/bottom 10 files");
            freeTree(tree);
            return EXIT_FAILURE;
        }

        // Allocate space for merged data
        int size = 1024;
        int count = 0;
        StationDiff* stations = malloc(size * sizeof(StationDiff));
        if (!stations) {
            perror("Memory allocation error");
            fclose(top10File);
            fclose(bottom10File);
            return EXIT_FAILURE;
        }

        // Read from the top file and add to stations array
        char line[1024];
        while (fgets(line, sizeof(line), top10File)) {
            if (count >= size) {
                size *= 2;
                stations = realloc(stations, size * sizeof(StationDiff));
                if (!stations) {
                    perror("Memory reallocation error");
                    fclose(top10File);
                    fclose(bottom10File);
                    return EXIT_FAILURE;
                }
            }

            // Parse line
            sscanf(line, "%d:%ld:%lf:%lf", &stations[count].key, &stations[count].capacity,
                   &stations[count].consumption, &stations[count].difference);
            count++;
        }

        // Read from the bottom file and add to stations array
        while (fgets(line, sizeof(line), bottom10File)) {
            if (count >= size) {
                size *= 2;
                stations = realloc(stations, size * sizeof(StationDiff));
                if (!stations) {
                    perror("Memory reallocation error");
                    fclose(top10File);
                    fclose(bottom10File);
                    return EXIT_FAILURE;
                }
            }

            // Parse line
            sscanf(line, "%d:%ld:%lf:%lf", &stations[count].key, &stations[count].capacity,
                   &stations[count].consumption, &stations[count].difference);
            count++;
        }

        // Sort the stations array by the difference column
        qsort(stations, count, sizeof(StationDiff), compareByDifference);

        // Write the sorted data to lv_all_minmax.csv
        FILE* outputFile = fopen("output/lv_all_minmax.csv", "w");
        if (!outputFile) {
            perror("Error opening output file");
            fclose(top10File);
            fclose(bottom10File);
            free(stations);
            return EXIT_FAILURE;
        }

        for (int i = 0; i < count; i++) {
            fprintf(outputFile, "%d:%ld:%.2f:%.2f\n", stations[i].key,
                    stations[i].capacity, stations[i].consumption, stations[i].difference);
        }

        // Clean up
        fclose(top10File);
        fclose(bottom10File);
        fclose(outputFile);
        free(stations);

        char scriptPath[256];
        sprintf(scriptPath, "output/plot_%s_%s.gp", station_type, consumer_type);
        generateGnuplotScript(scriptPath, "output/top10_lv_all.csv", "output/bottom10_lv_all.csv", "output/chart_lv_all.png");

        system("gnuplot output/plot_lv_all.gp");
    }

    freeTree(tree);
    return EXIT_SUCCESS;
}
