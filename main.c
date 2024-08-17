#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "storage.h"

void printTableSchema(const Table* table) {
    printf("Table Schema:\n");
    for (int i = 0; i < table->schema.column_count; ++i) {
        printf("Column %d: %s, Type: %d\n", i, table->schema.columns[i].name, table->schema.columns[i].type);
    }
    printf("\n");
}

int main() {
    // Create a table
    Table* table = createTable();
    if (!table) {
        printf("Failed to create table.\n");
        return 1;
    }

    // Define schema
    table->schema.column_count = 5;

    // Define columns
    strncpy(table->schema.columns[0].name, "ID", MAX_NAME_LENGTH);
    table->schema.columns[0].type = INTEGER;
    strncpy(table->schema.columns[1].name, "Name", MAX_NAME_LENGTH);
    table->schema.columns[1].type = STRING;
    strncpy(table->schema.columns[2].name, "Scores", MAX_NAME_LENGTH);
    table->schema.columns[2].type = ARRAY;
    table->schema.columns[2].array_size = 3;
    strncpy(table->schema.columns[3].name, "Description", MAX_NAME_LENGTH);
    table->schema.columns[3].type = DICTIONARY;
    strncpy(table->schema.columns[4].name, "Date", MAX_NAME_LENGTH);
    table->schema.columns[4].type = DATE;

    // Sample data for insertion
    int id = 1;
    char name[MAX_NAME_LENGTH] = "Alice";
    int scores[3] = {90, 85, 92};
    char description[MAX_NAME_LENGTH] = "subject:Math,grade:A";
    char date[MAX_DATE_LENGTH] = "2024-08-17";
    const void* data[5] = {&id, name, scores, description, date};

    // Insert a row
    if (!insertRow(table, data)) {
        printf("Failed to insert row.\n");
    }

    // Print table schema and contents
    printTableSchema(table);
    printTable(table);

    // Update row
    int newId = 2;
    char newName[MAX_NAME_LENGTH] = "Bob";
    int newScores[3] = {88, 79, 91};
    char newDescription[MAX_NAME_LENGTH] = "subject:Science,grade:B";
    char newDate[MAX_DATE_LENGTH] = "2024-08-18";
    const void* newData[5] = {&newId, newName, newScores, newDescription, newDate};
    updateRowById(table, 0, newData);

    printf("\nUpdated Table:\n");
    printTable(table);

    deleteRowById(table, 0);

    printf("\nTable After Deletion:\n");
    printTable(table);

    // Free the table
    freeTable(table);

    return 0;
}