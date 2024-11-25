#include "arbre.h"

// Create a tree node
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
    node->height = 1; // Initial height
    return node;
}

// Get the height of a node
int getHeight(AVLNode* node) {
    return node ? node->height : 0;
}

// Calculate the balance factor
int getBalance(AVLNode* node) {
    return node ? getHeight(node->left) - getHeight(node->right) : 0;
}

// Right rotation
AVLNode* rotateRight(AVLNode* y) {
    AVLNode* x = y->left;
    AVLNode* T = x->right;

    x->right = y;
    y->left = T;

    y->height = 1 + (getHeight(y->left) > getHeight(y->right) ? getHeight(y->left) : getHeight(y->right));
    x->height = 1 + (getHeight(x->left) > getHeight(x->right) ? getHeight(x->left) : getHeight(x->right));

    return x;
}

// Left rotation
AVLNode* rotateLeft(AVLNode* x) {
    AVLNode* y = x->right;
    AVLNode* T = y->left;

    y->left = x;
    x->right = T;

    x->height = 1 + (getHeight(x->left) > getHeight(x->right) ? getHeight(x->left) : getHeight(x->right));
    y->height = 1 + (getHeight(y->left) > getHeight(y->right) ? getHeight(y->left) : getHeight(y->right));

    return y;
}

// Insert into the AVL tree
AVLNode* insertNode(AVLNode* root, int key, long capacity, double consumption) {
    // If the tree is empty, create a new node
    if (!root) {
        return createNode(key, capacity, consumption);
    }

    // Recursive insertion
    if (key < root->key) {
        root->left = insertNode(root->left, key, capacity, consumption);
    } else if (key > root->key) {
        root->right = insertNode(root->right, key, capacity, consumption);
    } else {
        // Update if the node already exists
        root->capacity += capacity;
        root->consumption += consumption;
        return root;
    }

    // Update the height
    root->height = 1 + (getHeight(root->left) > getHeight(root->right) ? getHeight(root->left) : getHeight(root->right));

    // Check balance and perform rotations if necessary
    int balance = getBalance(root);

   // Left-left rotation
    if (balance > 1 && key < root->left->key) {
        return rotateRight(root);
    }

    // Right-right rotation
    if (balance < -1 && key > root->right->key) {
        return rotateLeft(root);
    }

    // Left-right rotation
    if (balance > 1 && key > root->left->key) {
        root->left = rotateLeft(root->left);
        return rotateRight(root);
    }

    // Right-left rotation
    if (balance < -1 && key < root->right->key) {
        root->right = rotateRight(root->right);
        return rotateLeft(root);
    }

    return root;
}

// Search in the AVL tree
AVLNode* searchNode(AVLNode* root, int key) {
    if (!root || root->key == key) {
        return root;
    }

    if (key < root->key) {
        return searchNode(root->left, key);
    } else {
        return searchNode(root->right, key);
    }
}

// In-order traversal of the tree (for CSV)
void inorderTraversalToCSV(AVLNode* root, FILE* outputFile) {
    if (!root) {
        return;
    }

    inorderTraversalToCSV(root->left, outputFile);
    fprintf(outputFile, "%d:%ld:%.2f\n", root->key, root->capacity, root->consumption);
    inorderTraversalToCSV(root->right, outputFile);
}

// Free the memory of the AVL tree
void freeTree(AVLNode* root) {
    if (root) {
        freeTree(root->left);
        freeTree(root->right);
        free(root);
    }
}
