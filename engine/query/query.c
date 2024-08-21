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

Query* parseInsertQuery(const char* queryStr) {
    if (queryStr == NULL) return NULL;

    Query* query = malloc(sizeof(Query));
    if (!query) return NULL;

    memset(query, 0, sizeof(Query));

    if (strncmp(queryStr, "INSERT INTO", 11) == 0) {
        query->type = INSERT_INTO;

        // Extract table name
        char tableName[MAX_NAME_LENGTH];
        if (sscanf(queryStr, "INSERT INTO %63s VALUES ", tableName) == 1) {
            trimWhitespace(tableName);
            strncpy(query->tableName, tableName, MAX_NAME_LENGTH - 1);
            query->tableName[MAX_NAME_LENGTH - 1] = '\0';
        } else {
            free(query);
            return NULL;
        }

        // Extract row data
        const char* valuesPart = strstr(queryStr, "VALUES ");
        if (valuesPart) {
            valuesPart += 7;  // Skip "VALUES "
            char* dataStr = strdup(valuesPart);
            char* token = strtok(dataStr, ",");
            int i = 0;

            while (token != NULL && i < MAX_NAME_LENGTH) {
                trimWhitespace(token);

                // Check if the value is in DATE format
                bool isDate = (strlen(token) == 10 && token[4] == '-' && token[7] == '-');

                query->rowData[i] = malloc(strlen(token) + 1);
                strcpy(query->rowData[i], token);

                i++;
                token = strtok(NULL, ",");
            }
            query->dataCount = i;
            free(dataStr);
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
void executeInsertQuery(Database* db, Query* query) {
    if (db == NULL || query == NULL) {
        printf("Invalid parameters.\n");
        return;
    }

    if (query->type == INSERT_INTO) {
        Table* table = NULL;
        for (int i = 0; i < db->table_count; i++) {
            if (strcmp(db->tables[i].name, query->tableName) == 0) {
                table = &db->tables[i];
                break;
            }
        }

        if (table == NULL) {
            printf("Table %s not found.\n", query->tableName);
            return;
        }

        if (query->dataCount != table->schema.column_count) {
            printf("Error: Data count does not match table column count.\n");
            return;
        }

        void* rowData[MAX_NAME_LENGTH] = { NULL };

        for (int i = 0; i < table->schema.column_count; i++) {
            ColumnType type = table->schema.columns[i].type;

            if (i < query->dataCount) {
                char* value = query->rowData[i];
                
                if (type == INTEGER) {
                    int* intValue = malloc(sizeof(int));
                    *intValue = atoi(value);
                    rowData[i] = intValue;
                } else if (type == FLOAT) {
                    float* floatValue = malloc(sizeof(float));
                    *floatValue = atof(value);
                    rowData[i] = floatValue;
                } else if (type == STRING) {
                    rowData[i] = strdup(value);
                } else if (type == ARRAY) {
                    // Example: "[1,2,3]" -> array of integers
                    int* array = malloc(table->schema.columns[i].array_size * sizeof(int));
                    // Parsing logic to convert `value` to `array`
                    rowData[i] = array;
                } else if (type == DICTIONARY) {
                    rowData[i] = strdup(value);
                } else if (type == DATE) {
                    // Here we just copy the string for simplicity
                    rowData[i] = strdup(value);
                }
            }
        }

        if (insertRow(table, rowData)) {
            printf("Row inserted successfully into table %s.\n", query->tableName);
        } else {
            printf("Error inserting row into table %s.\n", query->tableName);
        }
        for (int i = 0; i < table->schema.column_count; i++) {
            free(rowData[i]);
        }
    } else {
        printf("Unsupported query type.\n");
    }
}
