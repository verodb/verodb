#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "storage.h"
#include "query.h"

// Flags for operations
bool createdDatabase = false;
bool createdTable = false;
bool insertedData = false;
bool selectedData = false;
bool updatedData = false;
bool deletedData = false;
bool alteredTable = false;
bool droppedTable = false;
bool droppedDatabase = false;
bool createdIndex = false;
bool droppedIndex = false;
bool createdView = false;
bool queriedView = false;

void printTableContents(const Table* table) {
    printf("Table: %s\n", table->name);
    printf("Columns: ");
    for (uint32_t i = 0; i < table->schema.column_count; i++) {
        printf("%s (%d) | ", table->schema.columns[i].name, table->schema.columns[i].type);
    }
    printf("\n");

    for (uint32_t i = 0; i < table->row_count; i++) {
        printf("Row %d: ", i);
        for (uint32_t j = 0; j < table->schema.column_count; j++) {
            switch (table->schema.columns[j].type) {
                case INTEGER:
                    printf("%d | ", *(int*)table->rows[i].data[j]);
                    break;
                case FLOAT:
                    printf("%.2f | ", *(float*)table->rows[i].data[j]);
                    break;
                case STRING:
                case DATE:
                    printf("%s | ", (char*)table->rows[i].data[j]);
                    break;
            }
        }
        printf("\n");
    }
    printf("\n");
}

void printChecklist() {
    printf("Checklist:\n");
    printf(" Create Database%s\n", createdDatabase ? " [X]" : " [ ]");
    printf(" Create Table%s\n", createdTable ? " [X]" : " [ ]");
    printf(" Insert Data%s\n", insertedData ? " [X]" : " [ ]");
    printf(" Select Data%s\n", selectedData ? " [X]" : " [ ]");
    printf(" Update Data%s\n", updatedData ? " [X]" : " [ ]");
    printf(" Delete Data%s\n", deletedData ? " [X]" : " [ ]");
    printf(" Create Table with Different Data Types%s\n", createdTable ? " [X]" : " [ ]");
    printf(" Insert Multiple Rows%s\n", insertedData ? " [X]" : " [ ]");
    printf(" Join Tables%s\n", " [ ]");  // Placeholder for future implementation
    printf(" Create Index%s\n", createdIndex ? " [X]" : " [ ]");
    printf(" Drop Table%s\n", droppedTable ? " [X]" : " [ ]");
    printf(" Drop Database%s\n", droppedDatabase ? " [X]" : " [ ]");
    printf(" Alter Table%s\n", alteredTable ? " [X]" : " [ ]");
    printf(" Count Rows%s\n", " [ ]");  // Placeholder for future implementation
    printf(" Select with Conditions%s\n", selectedData ? " [X]" : " [ ]");
    printf(" Select with Sorting%s\n", " [ ]");  // Placeholder for future implementation
    printf(" Select with Aggregation%s\n", " [ ]");  // Placeholder for future implementation
    printf(" Select Distinct Values%s\n", " [ ]");  // Placeholder for future implementation
    printf(" Drop Index%s\n", droppedIndex ? " [X]" : " [ ]");
    printf(" Create View%s\n", createdView ? " [X]" : " [ ]");
    printf(" Query View%s\n", queriedView ? " [X]" : " [ ]");
}

int main() {
    // Create a database
    Database* db = createDatabase("TestDB");
    if (!db) {
        printf("Failed to create database.\n");
        return 1;
    }
    printf("Database 'TestDB' created successfully.\n\n");
    createdDatabase = true;

    // Create a table
    Query* query = parseCreateTableQuery("CREATE TABLE Users");
    if (query) {
        executeCreateTableQuery(db, query);
        free(query);
        createdTable = true;
    } else {
        printf("Failed to parse create table query.\n");
        freeDatabase(db);
        return 1;
    }

    // Set up table schema
    Table* table = getTableByName(db, "Users");
    if (!table) {
        printf("Failed to get Users table.\n");
        freeDatabase(db);
        return 1;
    }

    table->schema.column_count = 4;
    strncpy(table->schema.columns[0].name, "ID", MAX_NAME_LENGTH);
    table->schema.columns[0].type = INTEGER;
    strncpy(table->schema.columns[1].name, "Name", MAX_NAME_LENGTH);
    table->schema.columns[1].type = STRING;
    strncpy(table->schema.columns[2].name, "Age", MAX_NAME_LENGTH);
    table->schema.columns[2].type = INTEGER;
    strncpy(table->schema.columns[3].name, "JoinDate", MAX_NAME_LENGTH);
    table->schema.columns[3].type = DATE;
    printf("Table 'Users' created with schema: ID (INTEGER), Name (STRING), Age (INTEGER), JoinDate (DATE)\n\n");

    // Insert data
    const char* insertQueries[] = {
        "INSERT INTO Users VALUES 1, John, 30, 2023/01/01",
        "INSERT INTO Users VALUES 2, Alice, 25, 2023/02/15",
        "INSERT INTO Users VALUES 3, Bob, 35, 2023/03/20"
    };

    for (int i = 0; i < 3; i++) {
        query = parseInsertQuery(insertQueries[i]);
        if (query) {
            executeInsertQuery(db, query);
            free(query);
        } else {
            printf("Failed to parse insert query: %s\n", insertQueries[i]);
        }
    }

    printf("Inserted 3 rows into 'Users' table.\n");
    insertedData = true;
    printTableContents(table);

    // Select all data
    printf("Selecting all data from 'Users' table:\n");
    query = parseSelectQuery("SELECT * FROM Users");
    if (query) {
        executeSelectQuery(db, query);
        free(query);
        selectedData = true;
    } else {
        printf("Failed to parse select query.\n");
    }
    printf("\n");

    // Select data with WHERE clause
    printf("Selecting data with WHERE clause:\n");
    query = parseSelectQuery("SELECT * FROM Users WHERE Age > 28");
    if (query) {
        executeSelectQuery(db, query);
        free(query);
    } else {
        printf("Failed to parse select query with WHERE clause.\n");
    }
    printf("\n");

    // Update data
    printf("Updating data:\n");
    query = parseUpdateQuery("UPDATE Users SET Age = 31 WHERE ID = 1");
    if (query) {
        executeUpdateQuery(db, query);
        free(query);
        updatedData = true;
    } else {
        printf("Failed to parse update query.\n");
    }
    printTableContents(table);

    // Delete data
    printf("Deleting data:\n");
    query = parseDeleteQuery("DELETE FROM Users WHERE ID = 2");
    if (query) {
        executeDeleteQuery(db, query);
        free(query);
        deletedData = true;
    } else {
        printf("Failed to parse delete query.\n");
    }
    printTableContents(table);

    // Alter table (add column)
    printf("Altering table (adding column):\n");
    query = parseAlterTableQuery("ALTER TABLE Users ADD Email STRING");
    if (query) {
        executeAlterTableQuery(db, query);
        free(query);
        alteredTable = true;
    } else {
        printf("Failed to parse alter table query (add column).\n");
    }
    printTableContents(table);

    // Alter table (drop column)
    printf("Altering table (dropping column):\n");
    query = parseAlterTableQuery("ALTER TABLE Users DROP JoinDate");
    if (query) {
        executeAlterTableQuery(db, query);
        free(query);
        alteredTable = true;
    } else {
        printf("Failed to parse alter table query (drop column).\n");
    }
    printTableContents(table);

    // Drop table
    printf("Dropping table:\n");
    query = parseDropTableQuery("DROP TABLE Users");
    if (query) {
        executeDropTableQuery(db, query);
        free(query);
        droppedTable = true;
    } else {
        printf("Failed to parse drop table query.\n");
    }

    // Try to select from dropped table (should fail)
    printf("Trying to select from dropped table:\n");
    query = parseSelectQuery("SELECT * FROM Users");
    if (query) {
        executeSelectQuery(db, query);
        free(query);
    } else {
        printf("Failed to parse select query.\n");
    }
    printf("\n");

    // Drop database
    printf("Dropping database:\n");
    query = parseDropDatabaseQuery("DROP DATABASE TestDB");
    if (query) {
        executeDropDatabaseQuery(&db, query);
        free(query);
        droppedDatabase = true;
    } else {
        printf("Failed to parse drop database query.\n");
    }

    // Check if database is NULL after dropping
    if (db == NULL) {
        printf("Database successfully dropped.\n");
    } else {
        printf("Error: Database not properly dropped.\n");
        freeDatabase(db);
    }

    // Print checklist
    printChecklist();

    return 0;
}
