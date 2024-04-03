#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXLEN 19
/* COP 3502C Assignment 4
This program is written by: Devon Villalona */

typedef struct customer {
    char name[MAXLEN + 1];
    int points;
} customer;

typedef struct treenode {
    customer* cPtr;
    int size; 
    struct treenode* left;
    struct treenode* right;
} treenode;

//Function Declarations
treenode* createTreeNode(customer* c);
treenode* addTreeNode(treenode* root, customer* c, int addPoints, int* isNew);
treenode* locateNode(treenode* root, char* name);
int getNodeDepth(treenode* root, char* name, int* depth);
void modifyPoints(treenode* root, char* name, int points);
treenode* removeTreeNode(treenode* root, char* name);
treenode* findMaxValueNode(treenode* node);
int smallerCount(treenode* root, char* name);
void collectInOrder(treenode* root, customer** arr, int* index);
customer* initializeCustomer(char* name, int points);
int sortCustomers(const void* a, const void* b);
void performQuickSort(customer** arr, int low, int high);
int quickSortPartition(customer** arr, int low, int high);
void exchange(customer** a, customer** b);
void destroyTree(treenode* root);

// Create a tree node with given customer data
treenode* createTreeNode(customer* c) {
    treenode* newNode = (treenode*)malloc(sizeof(treenode));
    if (newNode == NULL) {
        fprintf(stderr, "Memory allocation failed for new TreeNode\n");
        exit(EXIT_FAILURE);
    }
    newNode->cPtr = c; // Assign customer data
    newNode->left = newNode->right = NULL; // Initialize child pointers
    newNode->size = 1; // Set initial size of subtree
    return newNode;
}

// Add a node to the tree or update existing node's points
treenode* addTreeNode(treenode* root, customer* c, int addPoints, int* isNew) {
    // If the spot is found, create a new node
    if (root == NULL) {
        *isNew = 1;
        return createTreeNode(c);
    }
    // Compare the customer name and navigate the tree accordingly
    int cmp = strcmp(c->name, root->cPtr->name);
    if (cmp < 0) {
        root->left = addTreeNode(root->left, c, addPoints, isNew);
    } else if (cmp > 0) {
        root->right = addTreeNode(root->right, c, addPoints, isNew);
    } else {
        *isNew = 0; // Customer already exists, so update points
        root->cPtr->points += addPoints;
    }
    // Update the size of current node's subtree
    root->size = 1 + (root->left ? root->left->size : 0) + (root->right ? root->right->size : 0);
    return root;
}

// Initialize a customer with the provided name and points
customer* initializeCustomer(char* name, int points) {
    customer* c = (customer*)malloc(sizeof(customer));
    if (c == NULL) {
        fprintf(stderr, "Memory allocation failed for new Customer\n");
        exit(EXIT_FAILURE);
    }
    strncpy(c->name, name, MAXLEN); // Copy name safely
    c->name[MAXLEN] = '\0'; // Ensure null termination
    c->points = points; // Set points
    return c;
}

// Search for a node by customer name in the tree
treenode* locateNode(treenode* root, char* name) {
    if (root == NULL) {
        return NULL; // Base case: node not found
    }
    // Compare the customer name to navigate the tree
    int cmp = strcmp(name, root->cPtr->name);
    if (cmp == 0) {
        return root; // Node found
    } else if (cmp < 0) {
        return locateNode(root->left, name); // Search left subtree
    } else {
        return locateNode(root->right, name); // Search right subtree
    }
}

// Find a node by name and return its depth in the tree
int getNodeDepth(treenode* root, char* name, int* depth) {
    if (root == NULL) {
        return -1; // Node not found
    }
    int cmp = strcmp(name, root->cPtr->name);
    if (cmp == 0) {
        return root->cPtr->points; // Node found, return points
    } else {
        (*depth)++;
        if (cmp < 0) {
            return getNodeDepth(root->left, name, depth); // Search left subtree
        } else {
            return getNodeDepth(root->right, name, depth); // Search right subtree
        }
    }
}

// Update the points for a given customer in the tree
void modifyPoints(treenode* root, char* name, int points) {
    treenode* node = locateNode(root, name);
    if (node != NULL) {
        node->cPtr->points = points; // Update points if node is found
    }
}


// Remove a node from the tree by name
treenode* removeTreeNode(treenode* root, char* name) {
    if (root == NULL) {
        return NULL; // Node not found
    }
    int cmp = strcmp(name, root->cPtr->name);
    // Navigate the tree to find the node
    if (cmp < 0) {
        root->left = removeTreeNode(root->left, name);
    } else if (cmp > 0) {
        root->right = removeTreeNode(root->right, name);
    } else {
        // Handle node with one or no children
        if (root->left == NULL || root->right == NULL) {
            treenode* temp = root->left ? root->left : root->right;
            free(root->cPtr); // Free the customer data
            free(root); // Free the node
            return temp;
        } else {
            // Handle node with two children
            treenode* temp = findMaxValueNode(root->left);
            customer* tempCust = temp->cPtr; // Swap the customer data
            temp->cPtr = root->cPtr;
            root->cPtr = tempCust;
            root->left = removeTreeNode(root->left, temp->cPtr->name); // Delete the inorder predecessor
        }
    }
    // Update the size of the current node's subtree
    if (root != NULL) {
        root->size = 1 + (root->left ? root->left->size : 0) + (root->right ? root->right->size : 0);
    }
    return root;
}

// Find the node with the maximum value (rightmost node) in a subtree
treenode* findMaxValueNode(treenode* node) {
    treenode* current = node;
    // Traverse to the rightmost node
    while (current && current->right != NULL) {
        current = current->right;
    }
    return current; // Rightmost node is the max value node
}

// Count the number of nodes with names smaller than the given name
int smallerCount(treenode* root, char* name) {
    if (root == NULL) {
        return 0; // Base case
    }
    int cmp = strcmp(name, root->cPtr->name);
    if (cmp > 0) {
        int leftSize = root->left ? root->left->size : 0;
        return leftSize + 1 + smallerCount(root->right, name); // Count in left subtree plus current node
    } else {
        return smallerCount(root->left, name); // Count in left subtree only
    }
}

// Collect customer data in in-order traversal
void collectInOrder(treenode* root, customer** arr, int* index) {
    if (root == NULL) {
        return; // Base case
    }
    collectInOrder(root->left, arr, index); // Traverse left subtree
    arr[*index] = root->cPtr; // Add current node's customer to array
    (*index)++;
    collectInOrder(root->right, arr, index); // Traverse right subtree
}

// Swap two customer pointers
void exchange(customer** a, customer** b) {
    customer* temp = *a;
    *a = *b;
    *b = temp;
}

// Partition the array for quick sort
int quickSortPartition(customer** arr, int low, int high) {
    customer* pivot = arr[high];
    int i = low - 1;
    for (int j = low; j <= high - 1; j++) {
        if (sortCustomers(&arr[j], &pivot) < 0) {
            i++;
            exchange(&arr[i], &arr[j]);
        }
    }
    exchange(&arr[i + 1], &arr[high]);
    return i + 1;
}

// Perform quick sort on the array of customers
void performQuickSort(customer** arr, int low, int high) {
    if (low < high) {
        int pi = quickSortPartition(arr, low, high);
        performQuickSort(arr, low, pi - 1);
        performQuickSort(arr, pi + 1, high);
    }
}

// Compare function for sorting customers (used in quick sort)
int sortCustomers(const void* a, const void* b) {
    customer* custA = *(customer**)a;
    customer* custB = *(customer**)b;
    // Sort primarily by points, then by name
    if (custA->points != custB->points) {
        return custB->points - custA->points;
    }
    return strcmp(custA->name, custB->name);
}

// Free the memory allocated for the entire tree
void destroyTree(treenode* root) {
    if (root == NULL) {
        return; // Base case
    }
    destroyTree(root->left); // Free left subtree
    destroyTree(root->right); // Free right subtree
    free(root->cPtr); // Free customer data
    free(root); // Free the node itself
}



int main() {
    int n, i, points, depth, isNew;
    char command[15], name[MAXLEN + 1];
    treenode* root = NULL;

    // Read the number of operations to be performed
    scanf("%d", &n);

    for (i = 0; i < n; i++) {
        // Read the command to be executed
        scanf("%s", command);

        // Handle 'add' command: add or update a customer in the tree
        if (strcmp(command, "add") == 0) {
            scanf("%s %d", name, &points);
            customer* c = initializeCustomer(name, points);
            isNew = 0;
            root = addTreeNode(root, c, points, &isNew);
            // Free memory if customer already exists
            if (!isNew) {
                free(c);
            }
            // Output the new points of the customer
            printf("%s %d\n", name, locateNode(root, name)->cPtr->points);
        }
        // Handle 'sub' command: subtract points from a customer
        else if (strcmp(command, "sub") == 0) {
            scanf("%s %d", name, &points);
            treenode* node = locateNode(root, name);
            if (node != NULL) {
                int newPoints = node->cPtr->points - points;
                newPoints = newPoints < 0 ? 0 : newPoints;
                modifyPoints(root, name, newPoints);
                printf("%s %d\n", name, newPoints);
            } else {
                printf("%s not found\n", name);
            }
        }
        // Handle 'del' command: delete a customer from the tree
        else if (strcmp(command, "del") == 0) {
            scanf("%s", name);
            if (locateNode(root, name)) {
                root = removeTreeNode(root, name);
                printf("%s deleted\n", name);
            } else {
                printf("%s not found\n", name);
            }
        }
        // Handle 'search' command: find a customer and display their depth
        else if (strcmp(command, "search") == 0) {
            scanf("%s", name);
            depth = 0;
            points = getNodeDepth(root, name, &depth);
            if (points >= 0) {
                printf("%s %d %d\n", name, points, depth);
            } else {
                printf("%s not found\n", name);
            }
        }
        // Handle 'count_smaller' command: count customers with fewer points
        else if (strcmp(command, "count_smaller") == 0) {
            scanf("%s", name);
            int count = smallerCount(root, name);
            printf("%d\n", count);
        }
    }

    // Sorting customers in the tree in order and outputting them
    customer** customers = (customer**)malloc(sizeof(customer*) * n);
    int index = 0;
    collectInOrder(root, customers, &index);
    performQuickSort(customers, 0, index - 1);

    for (i = 0; i < index; i++) {
        printf("%s %d\n", customers[i]->name, customers[i]->points);
    }

    // Free memory allocated for the array of pointers
    free(customers);

    // Free memory allocated for the tree and its nodes
    destroyTree(root);

    return 0;
}