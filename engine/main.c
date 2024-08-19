#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "storage.h"
#include "query.h"

int main() {
    const char* createDbQueryStr = "CREATE DATABASE TestDB";

    Query* query = parseCreateDatabaseQuery(createDbQueryStr);
    if (!query) {
        printf("Error parsing query.\n");
        return 1;
    }

    Database* db = createDatabase(query->databaseName);
    if (!db) {
        printf("Error creating database %s.\n", query->databaseName);
        free(query);
        return 1;
    }

    // Print confirmation of database creation
    printf("Database %s created successfully.\n", query->databaseName);

    // Define a new table within the created database
    Table* table = createTable(db, "Users");
    if (!table) {
        printf("Error creating table in database %s.\n", query->databaseName);
        freeDatabase(db);
        free(query);
        return 1;
    }

    // Define schema for the "Users" table
    table->schema.column_count = 3;
    strcpy(table->schema.columns[0].name, "ID");
    table->schema.columns[0].type = INTEGER;
    
    strcpy(table->schema.columns[1].name, "Name");
    table->schema.columns[1].type = STRING;
    
    strcpy(table->schema.columns[2].name, "Age");
    table->schema.columns[2].type = INTEGER;

    // Insert dummy rows into the "Users" table
    int id1 = 1;
    const char* name1 = "Alice";
    int age1 = 30;
    const void* row1[] = { &id1, name1, &age1 };
    insertRow(table, row1);

    int id2 = 2;
    const char* name2 = "Bob";
    int age2 = 25;
    const void* row2[] = { &id2, name2, &age2 };
    insertRow(table, row2);

    printf("Table: Users in %s\n", query->databaseName);
    printTable(table);

    freeDatabase(db);
    free(query);

    return 0;
}
