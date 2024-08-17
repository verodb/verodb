//
// Created by Abhishek Belgaonkar on 17/08/24.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "storage.h"
#include "indexing.h"

// Define the IndexNode structure
typedef struct IndexNode {
    void* key;
    int row_id;
    struct IndexNode* left;
    struct IndexNode* right;
} IndexNode;

// Define the Index structure
struct Index {
    IndexNode* root;
    int column_index;
    ColumnType column_type;
};

// Function to create a new index
Index* createIndex(Table* table, int column_index) {
    if (column_index < 0 || column_index >= table->schema.column_count) {
        printf("Error: Invalid column index for indexing.\n");
        return NULL;
    }

    Index* index = (Index*)malloc(sizeof(Index));
    if (!index) {
        printf("Error: Failed to allocate memory for index.\n");
        return NULL;
    }

    index->root = NULL;
    index->column_index = column_index;
    index->column_type = table->schema.columns[column_index].type;

    // Build the index for existing rows
    for (int i = 0; i < table->row_count; i++) {
        insertIndexEntry(index, table->rows[i].data[column_index], i);
    }

    return index;
}

// Function to insert a new entry into the index
void insertIndexEntry(Index* index, void* key, int row_id) {
    IndexNode* new_node = (IndexNode*)malloc(sizeof(IndexNode));
    new_node->key = key;
    new_node->row_id = row_id;
    new_node->left = NULL;
    new_node->right = NULL;

    if (index->root == NULL) {
        index->root = new_node;
        return;
    }

    IndexNode* current = index->root;
    while (1) {
        int cmp;
        if (index->column_type == INTEGER) {
            cmp = *(int*)key - *(int*)current->key;
        } else if (index->column_type == STRING) {
            cmp = strcmp((char*)key, (char*)current->key);
        } else {
            cmp = 0;
        }

        if (cmp < 0) {
            if (current->left == NULL) {
                current->left = new_node;
                break;
            }
            current = current->left;
        } else {
            if (current->right == NULL) {
                current->right = new_node;
                break;
            }
            current = current->right;
        }
    }
}

// Function to search the index
int searchIndex(Index* index, void* key) {
    IndexNode* current = index->root;
    while (current != NULL) {
        int cmp;
        if (index->column_type == INTEGER) {
            cmp = *(int*)key - *(int*)current->key;
        } else if (index->column_type == STRING) {
            cmp = strcmp((char*)key, (char*)current->key);
        } else {
            cmp = 0;
        }

        if (cmp == 0) {
            return current->row_id;
        } else if (cmp < 0) {
            current = current->left;
        } else {
            current = current->right;
        }
    }
    return -1; // Not found
}

// Function to delete an entry from the index
void deleteIndexEntry(Index* index, void* key) {
    // Implementation of deletion from binary search tree
    printf("Warning: deleteIndexEntry not implemented.\n");
}

// Function to free the index
void freeIndex(Index* index) {
    // Recursive function to free nodes
    void freeNode(IndexNode* node) {
        if (node != NULL) {
            freeNode(node->left);
            freeNode(node->right);
            free(node);
        }
    }

    if (index != NULL) {
        freeNode(index->root);
        free(index);
    }
}
