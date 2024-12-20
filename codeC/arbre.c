#include "arbre.h"

// Create a new AVL tree node
AVLNode* createNode(int key, long capacity, double consumption) {
    AVLNode* node = (AVLNode*)malloc(sizeof(AVLNode));
    if (!node) {
        fprintf(stderr, "Memory allocation error.\n");
        exit(EXIT_FAILURE);
    }
    node->key = key;
    node->capacity = capacity;
    node->consumption = consumption;
    node->left = node->right = NULL;
    node->height = 1; 
    return node;
}

// Get the height of a node
int getHeight(AVLNode* node) {
    return node ? node->height : 0;
}

// Get the balance factor of a node
int getBalance(AVLNode* node) {
    return node ? getHeight(node->left) - getHeight(node->right) : 0;
}

// Right rotation
AVLNode* rotateRight(AVLNode* y) {
    AVLNode* x = y->left;
    AVLNode* T = x->right;

    x->right = y;
    y->left = T;

    y->height = 1 + ( (getHeight(y->left) > getHeight(y->right)) ? getHeight(y->left) : getHeight(y->right) );
    x->height = 1 + ( (getHeight(x->left) > getHeight(x->right)) ? getHeight(x->left) : getHeight(x->right) );

    return x;
}

// Left rotation
AVLNode* rotateLeft(AVLNode* x) {
    AVLNode* y = x->right;
    AVLNode* T = y->left;

    y->left = x;
    x->right = T;

    x->height = 1 + ( (getHeight(x->left) > getHeight(x->right)) ? getHeight(x->left) : getHeight(x->right) );
    y->height = 1 + ( (getHeight(y->left) > getHeight(y->right)) ? getHeight(y->left) : getHeight(y->right) );

    return y;
}

// Insert a node into the AVL tree
AVLNode* insertNode(AVLNode* root, int key, long capacity, double consumption) {
    if (!root) {
        return createNode(key, capacity, consumption);
    }

    if (key < root->key) {
        root->left = insertNode(root->left, key, capacity, consumption);
    } else if (key > root->key) {
        root->right = insertNode(root->right, key, capacity, consumption);
    } else {
        // If the key already exists, just update the capacity and consumption
        root->capacity += capacity;
        root->consumption += consumption;
        return root;
    }

    // Update height
    root->height = 1 + ((getHeight(root->left) > getHeight(root->right)) ? getHeight(root->left) : getHeight(root->right));

    // Check balance and rotate if needed
    int balance = getBalance(root);

    // Left-left case
    if (balance > 1 && key < root->left->key) {
        return rotateRight(root);
    }

    // Right-right case
    if (balance < -1 && key > root->right->key) {
        return rotateLeft(root);
    }

    // Left-right case
    if (balance > 1 && key > root->left->key) {
        root->left = rotateLeft(root->left);
        return rotateRight(root);
    }

    // Right-left case
    if (balance < -1 && key < root->right->key) {
        root->right = rotateRight(root->right);
        return rotateLeft(root);
    }

    return root;
}

// Search a node by key in the AVL tree
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

// Free the memory of the AVL tree
void freeTree(AVLNode* root) {
    if (root) {
        freeTree(root->left);
        freeTree(root->right);
        free(root);
    }
}
