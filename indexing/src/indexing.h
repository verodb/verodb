//
// Created by Abhishek Belgaonkar on 17/08/24.
//
#ifndef INDEXING_H
#define INDEXING_H

#include "storage.h"

// Define the IndexNode structure
typedef struct IndexNode {
    void* key;
    int row_id;
    struct IndexNode* left;
    struct IndexNode* right;
} IndexNode;

// Define the Index structure
typedef struct Index {
    IndexNode* root;
    int column_index;
    ColumnType column_type;
} Index;

// Function to create a new index
Index* createIndex(Table* table, int column_index);

// Function to insert a new entry into the index
void insertIndexEntry(Index* index, void* key, int row_id);

// Function to search the index
int searchIndex(Index* index, void* key);

// Function to delete an entry from the index
void deleteIndexEntry(Index* index, void* key);

// Function to free the index
void freeIndex(Index* index);

#endif // INDEXING_H

