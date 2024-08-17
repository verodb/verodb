#ifndef STORAGE_H
#define STORAGE_H

// Define the structure for a row in the table
typedef struct {
    int id;
    char name[50];
    int age;
} Row;

// Define the structure for the table
typedef struct {
    Row rows[100];
    int row_count;
} Table;

// Function to create a new table
Table* createTable();

// Function to insert a row into the table
void insertRow(Table* table, int id, const char* name, int age);

// Function to print all rows in the table
void printTable(const Table* table);

// Function to free the table
void freeTable(Table* table);

#endif // STORAGE_H
