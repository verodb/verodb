#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "storage.h"
#include "query.h"

int main() {
    // Create a database
    const char* createDbQueryStr = "CREATE DATABASE TestDB";
    Query* query = parseCreateDatabaseQuery(createDbQueryStr);
    if (!query) {
        printf("Error parsing create database query.\n");
        return 1;
    }

    Database* db = createDatabase(query->databaseName);
    if (!db) {
        printf("Error creating database %s.\n", query->databaseName);
        free(query);
        return 1;
    }

    executeCreateDatabaseQuery(db, query);
    free(query);

    // Create a table within the database
    const char* createTableQueryStr = "CREATE TABLE Users";
    query = parseCreateTableQuery(createTableQueryStr);
    if (query) {
        executeCreateTableQuery(db, query);
        free(query);
    } else {    
        printf("Failed to parse create table query.\n");
        freeDatabase(db);
        return 1;
    }

    Table* table = &db->tables[0];
    table->schema.column_count = 4;
    strncpy(table->schema.columns[0].name, "ID", MAX_NAME_LENGTH);
    table->schema.columns[0].type = INTEGER;

    strncpy(table->schema.columns[1].name, "Name", MAX_NAME_LENGTH);
    table->schema.columns[1].type = STRING;

    strncpy(table->schema.columns[2].name, "Age", MAX_NAME_LENGTH);
    table->schema.columns[2].type = INTEGER;

    strncpy(table->schema.columns[3].name, "JoinDate", MAX_NAME_LENGTH);
    table->schema.columns[3].type = DATE;

    const char* insertQueryStr1 = "INSERT INTO Users VALUES 1, Harsh, 21, 2024/08/02";
    query = parseInsertQuery(insertQueryStr1);
    if (query) {
        executeInsertQuery(db, query);
        free(query);
    } else {
        printf("Failed to parse insert query.\n");
    }

    const char* insertQueryStr2 = "INSERT INTO Users VALUES 2, Bhat, 25, 2014/02/02";
    query = parseInsertQuery(insertQueryStr2);
    if (query) {
        executeInsertQuery(db, query);
        free(query);
    } else {
        printf("Failed to parse insert query.\n");
    }

    // Test SELECT query
    const char* selectQueryStr = "SELECT * FROM Users";
    query = parseSelectQuery(selectQueryStr);
    if (query) {
        executeSelectQuery(db, query);
        free(query);
    } else {
        printf("Failed to parse select query.\n");
    }

    freeDatabase(db);

    return 0;
}