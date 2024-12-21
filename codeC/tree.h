#ifndef TREE_H
#define TREE_H

#include <stdio.h>
#include <stdlib.h>

// Definition of the AVL tree node structure
typedef struct AVLNode {
    int key;
    long capacity;
    double consumption;
    struct AVLNode* left;
    struct AVLNode* right;
    int height;
} AVLNode;

// AVL tree function prototypes
AVLNode* createNode(int key, long capacity, double consumption);
int getHeight(AVLNode* node);
int getBalance(AVLNode* node);
AVLNode* rotateRight(AVLNode* y);
AVLNode* rotateLeft(AVLNode* x);
AVLNode* insertNode(AVLNode* root, int key, long capacity, double consumption);
AVLNode* searchNode(AVLNode* root, int key);
void freeTree(AVLNode* root);

// Gnuplot script generation prototype (declared here for convenience)
void generateGnuplotScript(const char* scriptPath, const char* top10Path, const char* bottom10Path, const char* outputPath);

#endif
