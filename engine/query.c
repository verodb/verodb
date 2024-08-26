#include "query.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

static void trimWhitespace(char* str) {
    if (str == NULL) return;
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
    if (query == NULL) return NULL;

    memset(query, 0, sizeof(Query));

    if (strncmp(queryStr, "CREATE DATABASE", 15) == 0) {
        query->type = CREATE_DATABASE;

        if (sscanf(queryStr, "CREATE DATABASE %63s", query->databaseName) == 1) {
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

Query* parseCreateTableQuery(const char* queryStr) {
    if (queryStr == NULL) return NULL;

    Query* query = malloc(sizeof(Query));
    if (query == NULL) return NULL;

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

Query* parseInsertQuery(const char* queryStr) {
    if (queryStr == NULL) return NULL;

    Query* query = malloc(sizeof(Query));
    if (query == NULL) return NULL;

    memset(query, 0, sizeof(Query));

    if (strncmp(queryStr, "INSERT INTO", 11) == 0) {
        query->type = INSERT_INTO;

        char tableName[MAX_NAME_LENGTH];
        if (sscanf(queryStr, "INSERT INTO %63s VALUES ", tableName) == 1) {
            trimWhitespace(tableName);
            strncpy(query->tableName, tableName, MAX_NAME_LENGTH - 1);
            query->tableName[MAX_NAME_LENGTH - 1] = '\0';
        } else {
            free(query);
            return NULL;
        }

        const char* valuesPart = strstr(queryStr, "VALUES ");
        if (valuesPart) {
            valuesPart += 7;  // Skip "VALUES "
            char* dataStr = strdup(valuesPart);
            char* token = strtok(dataStr, ",");
            int i = 0;

            while (token != NULL && i < MAX_NAME_LENGTH) {
                trimWhitespace(token);

                query->rowData[i] = malloc(strlen(token) + 1);
                if (query->rowData[i] == NULL) {
                    // Clean up previously allocated memory
                    for (int j = 0; j < i; j++) {
                        free(query->rowData[j]);
                    }
                    free(dataStr);
                    free(query);
                    return NULL;
                }
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

Query* parseSelectQuery(const char* queryStr) {
    if (queryStr == NULL) return NULL;

    Query* query = malloc(sizeof(Query));
    if (query == NULL) return NULL;

    memset(query, 0, sizeof(Query));

    if (strncmp(queryStr, "SELECT", 6) == 0) {
        query->type = SELECT_FROM;
        
        // For simplicity, we'll implement "SELECT * FROM tablename"
        char tableName[MAX_NAME_LENGTH];
        if (sscanf(queryStr, "SELECT * FROM %63s", tableName) == 1) {
            strncpy(query->tableName, tableName, MAX_NAME_LENGTH - 1);
            query->tableName[MAX_NAME_LENGTH - 1] = '\0';
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

void executeCreateDatabaseQuery(Database* db, Query* query) {
    if (db == NULL || query == NULL) {
        printf("Invalid parameters.\n");
        return;
    }

    if (query->type == CREATE_DATABASE) {
        strncpy(db->name, query->databaseName, MAX_NAME_LENGTH - 1);
        db->name[MAX_NAME_LENGTH - 1] = '\0';
        printf("Database %s created successfully.\n", db->name);
    } else {
        printf("Unsupported query type.\n");
    }
}

void executeCreateTableQuery(Database* db, Query* query) {
    if (db == NULL || query == NULL) {
        printf("Invalid parameters.\n");
        return;
    }

    if (query->type == CREATE_TABLE) {
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

void executeInsertQuery(Database* db, Query* query) {
    if (db == NULL || query == NULL) {
        printf("Invalid parameters.\n");
        return;
    }

    if (query->type == INSERT_INTO) {
        Table* table = NULL;
        for (uint32_t i = 0; i < db->table_count; i++) {
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

        for (uint32_t i = 0; i < table->schema.column_count; i++) {
            ColumnType type = table->schema.columns[i].type;

            if (i < query->dataCount) {
                char* value = query->rowData[i];
                
                if (type == INTEGER) {
                    int* intValue = malloc(sizeof(int));
                    if (intValue == NULL) {
                        printf("Memory allocation failed.\n");
                        return;
                    }
                    *intValue = atoi(value);
                    rowData[i] = intValue;
                } else if (type == FLOAT) {
                    float* floatValue = malloc(sizeof(float));
                    if (floatValue == NULL) {
                        printf("Memory allocation failed.\n");
                        return;
                    }
                    *floatValue = atof(value);
                    rowData[i] = floatValue;
                } else if (type == STRING) {
                    rowData[i] = strdup(value);
                    if (rowData[i] == NULL) {
                        printf("Memory allocation failed.\n");
                        return;
                    }
                } else if (type == ARRAY) {
                    // Example: "[1,2,3]" -> array of integers
                    int* array = malloc(table->schema.columns[i].array_size * sizeof(int));
                    if (array == NULL) {
                        printf("Memory allocation failed.\n");
                        return;
                    }
                    // Parsing logic to convert `value` to `array`
                    rowData[i] = array;
                } else if (type == DICTIONARY) {
                    rowData[i] = strdup(value);
                    if (rowData[i] == NULL) {
                        printf("Memory allocation failed.\n");
                        return;
                    }
                } else if (type == DATE) {
                    rowData[i] = strdup(value);
                    if (rowData[i] == NULL) {
                        printf("Memory allocation failed.\n");
                        return;
                    }
                }
            }
        }

        if (insertRow(table, (const void**)rowData)) {
            printf("Row inserted successfully into table %s.\n", query->tableName);
        } else {
            printf("Error inserting row into table %s.\n", query->tableName);
        }
        for (uint32_t i = 0; i < table->schema.column_count; i++) {
            free(rowData[i]);
        }
    } else {
        printf("Unsupported query type.\n");
    }
}

void executeSelectQuery(const Database* db, const Query* query) {
    if (db == NULL || query == NULL) {
        printf("Invalid parameters.\n");
        return;
    }

    if (query->type != SELECT_FROM) {
        printf("Invalid query type for select operation.\n");
        return;
    }

    const Table* table = getTableByName(db, query->tableName);
    if (table == NULL) {
        printf("Table %s not found.\n", query->tableName);
        return;
    }

    // Print table header
    for (uint32_t i = 0; i < table->schema.column_count; i++) {
        printf("%-15s", table->schema.columns[i].name);
    }
    printf("\n");

    // Print table data
    for (uint32_t i = 0; i < table->row_count; i++) {
        const Row* row = &table->rows[i];
        for (uint32_t j = 0; j < row->column_count; j++) {
            ColumnType type = table->schema.columns[j].type;
            if (type == INTEGER) {
                printf("%-15d", *(int*)row->data[j]);
            } else if (type == FLOAT) {
                printf("%-15f", *(float*)row->data[j]);
            } else if (type == STRING || type == DATE) {
                printf("%-15s", (char*)row->data[j]);
            } else if (type == ARRAY) {
                printf("%-15s", "[ARRAY]");  // Placeholder for array printing
            } else if (type == DICTIONARY) {
                printf("%-15s", "{DICT}");  // Placeholder for dictionary printing
            }
        }
        printf("\n");
    }
}