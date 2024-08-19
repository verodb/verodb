#include "query.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

static void trimWhitespace(char* str) {
    char *end;
    while (isspace((unsigned char)*str)) str++;
    if (*str == 0) return;
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    *(end+1) = 0;
}


Query* parseCreateDatabaseQuery(const char* queryStr) {
    if (queryStr == NULL) return NULL;

    Query* query = malloc(sizeof(Query));
    if (!query) return NULL;

    memset(query, 0, sizeof(Query));

    if (strncmp(queryStr, "CREATE DATABASE", 15) == 0) {
        query->type = CREATE_DATABASE;

        if (sscanf(queryStr, "CREATE DATABASE %31s", query->databaseName) == 1) {
            trimWhitespace(query->databaseName);
        } else {
            free(query);
            return NULL; 
        }
    } else {
        free(query);
        return NULL; 
    }

    return query;
}

void executeCreateDatabaseQuery(Database* db, Query* query) {
    if (db == NULL || query == NULL) {
        printf("Invalid parameters.\n");
        return;
    }

    if (query->type == CREATE_DATABASE) {
        Database* newDb = createDatabase(query->databaseName);
        if (newDb) {
            printf("Database %s created successfully.\n", query->databaseName);
        } else {
            printf("Error creating database %s.\n", query->databaseName);
        }
    } else {
        printf("Unsupported query type.\n");
    }
}

Query* parseCreateTableQuery(const char* queryStr) {
    if (queryStr == NULL) return NULL;

    Query* query = malloc(sizeof(Query));
    if (!query) return NULL;

    memset(query, 0, sizeof(Query));

    if (strncmp(queryStr, "CREATE TABLE", 12) == 0) {
        query->type = CREATE_TABLE;
        if (sscanf(queryStr, "CREATE TABLE %63s", query->tableName) == 1) {
            trimWhitespace(query->tableName);
        } else {
            free(query);
            return NULL; 
        }
    } else {
        free(query);
        return NULL; 
    }

    return query;
}

void executeCreateTableQuery(Database* db, Query* query) {
    if (db == NULL || query == NULL) {
        printf("Invalid parameters.\n");
        return;
    }

    if (query->type == CREATE_TABLE) {
        if (db->table_count >= MAX_TABLES) {
            printf("Error: Database is full.\n");
            return;
        }

        Table* table = createTable(db, query->tableName);
        if (table) {
            printf("Table %s created successfully.\n", query->tableName);
        } else {
            printf("Error creating table %s.\n", query->tableName);
        }
    } else {
        printf("Unsupported query type.\n");
    }
}