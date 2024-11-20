#include "arbre.h"

// Créer un nœud de l'arbre
AVLNode* createNode(int key, long capacity, double consumption) {
    AVLNode* node = (AVLNode*)malloc(sizeof(AVLNode));
    if (!node) {
        fprintf(stderr, "Erreur d'allocation mémoire.\n");
        exit(EXIT_FAILURE);
    }
    node->key = key;
    node->capacity = capacity;
    node->consumption = consumption;
    node->left = node->right = NULL;
    node->height = 1; // Hauteur initiale
    return node;
}

// Obtenir la hauteur d'un nœud
int getHeight(AVLNode* node) {
    return node ? node->height : 0;
}

// Calculer le facteur d'équilibre
int getBalance(AVLNode* node) {
    return node ? getHeight(node->left) - getHeight(node->right) : 0;
}

// Rotation droite
AVLNode* rotateRight(AVLNode* y) {
    AVLNode* x = y->left;
    AVLNode* T = x->right;

    x->right = y;
    y->left = T;

    y->height = 1 + (getHeight(y->left) > getHeight(y->right) ? getHeight(y->left) : getHeight(y->right));
    x->height = 1 + (getHeight(x->left) > getHeight(x->right) ? getHeight(x->left) : getHeight(x->right));

    return x;
}

// Rotation gauche
AVLNode* rotateLeft(AVLNode* x) {
    AVLNode* y = x->right;
    AVLNode* T = y->left;

    y->left = x;
    x->right = T;

    x->height = 1 + (getHeight(x->left) > getHeight(x->right) ? getHeight(x->left) : getHeight(x->right));
    y->height = 1 + (getHeight(y->left) > getHeight(y->right) ? getHeight(y->left) : getHeight(y->right));

    return y;
}

// Insertion dans l'arbre AVL
AVLNode* insertNode(AVLNode* root, int key, long capacity, double consumption) {
    if (!root) {
        return createNode(key, capacity, consumption);
    }

    if (capacity < root->capacity) {
        root->left = insertNode(root->left, key, capacity, consumption);
    } else if (capacity > root->capacity) {
        root->right = insertNode(root->right, key, capacity, consumption);
    } else {
        root->consumption += consumption;
        root->key = key; // Mettre à jour la clé associée
    }

    root->height = 1 + (getHeight(root->left) > getHeight(root->right) ? getHeight(root->left) : getHeight(root->right));

    int balance = getBalance(root);

    if (balance > 1 && capacity < root->left->capacity) {
        return rotateRight(root);
    }

    if (balance < -1 && capacity > root->right->capacity) {
        return rotateLeft(root);
    }

    if (balance > 1 && capacity > root->left->capacity) {
        root->left = rotateLeft(root->left);
        return rotateRight(root);
    }

    if (balance < -1 && capacity < root->right->capacity) {
        root->right = rotateRight(root->right);
        return rotateLeft(root);
    }

    return root;
}

// Parcours In-Order pour écrire dans un fichier CSV
void inorderTraversalToCSV(AVLNode* root, FILE* outputFile) {
    if (!root) {
        return;
    }

    // Parcourir le sous-arbre gauche
    inorderTraversalToCSV(root->left, outputFile);

    // Écrire les données du nœud courant dans le fichier
    fprintf(outputFile, "%d:%ld:%.2f\n", root->key, root->capacity, root->consumption);

    // Parcourir le sous-arbre droit
    inorderTraversalToCSV(root->right, outputFile);
}

// Libérer la mémoire
void freeTree(AVLNode* root) {
    if (root) {
        freeTree(root->left);
        freeTree(root->right);
        free(root);
    }
}
