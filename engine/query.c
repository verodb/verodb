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

    if (sscanf(queryStr, "CREATE DATABASE %63s", query->databaseName) == 1) {
        query->type = CREATE_DATABASE;
        trimWhitespace(query->databaseName);
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

    if (sscanf(queryStr, "CREATE TABLE %63s", query->tableName) == 1) {
        query->type = CREATE_TABLE;
        trimWhitespace(query->tableName);
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

    char* tableName = malloc(MAX_NAME_LENGTH);
    char* values = malloc(strlen(queryStr));
    if (tableName == NULL || values == NULL) {
        free(query);
        free(tableName);
        free(values);
        return NULL;
    }

    if (sscanf(queryStr, "INSERT INTO %63s VALUES %[^\n]", tableName, values) == 2) {
        query->type = INSERT_INTO;
        strncpy(query->tableName, tableName, MAX_NAME_LENGTH - 1);
        query->tableName[MAX_NAME_LENGTH - 1] = '\0';
        trimWhitespace(query->tableName);

        char* token = strtok(values, ",");
        while (token != NULL && query->columnCount < MAX_COLUMNS) {
            trimWhitespace(token);
            strncpy(query->columnValues[query->columnCount], token, MAX_NAME_LENGTH - 1);
            query->columnValues[query->columnCount][MAX_NAME_LENGTH - 1] = '\0';
            query->columnCount++;
            token = strtok(NULL, ",");
        }
    } else {
        free(query);
        free(tableName);
        free(values);
        return NULL;
    }

    free(tableName);
    free(values);
    return query;
}

Query* parseSelectQuery(const char* queryStr) {
    if (queryStr == NULL) return NULL;

    Query* query = malloc(sizeof(Query));
    if (query == NULL) return NULL;

    memset(query, 0, sizeof(Query));

    if (sscanf(queryStr, "SELECT * FROM %63s", query->tableName) == 1) {
        query->type = SELECT_FROM;
        trimWhitespace(query->tableName);
    } else {
        free(query);
        return NULL;
    }

    return query;
}

Query* parseUpdateQuery(const char* queryStr) {
    if (queryStr == NULL) return NULL;

    Query* query = malloc(sizeof(Query));
    if (query == NULL) return NULL;

    memset(query, 0, sizeof(Query));

    char* tableName = malloc(MAX_NAME_LENGTH);
    char* setClause = malloc(strlen(queryStr));
    char* whereClause = malloc(strlen(queryStr));
    if (tableName == NULL || setClause == NULL || whereClause == NULL) {
        free(query);
        free(tableName);
        free(setClause);
        free(whereClause);
        return NULL;
    }

    if (sscanf(queryStr, "UPDATE %63s SET %[^WHERE] WHERE %[^\n]", tableName, setClause, whereClause) == 3) {
        query->type = UPDATE;
        strncpy(query->tableName, tableName, MAX_NAME_LENGTH - 1);
        query->tableName[MAX_NAME_LENGTH - 1] = '\0';
        trimWhitespace(query->tableName);

        char* token = strtok(setClause, ",");
        while (token != NULL && query->columnCount < MAX_COLUMNS) {
            trimWhitespace(token);
            char* equalSign = strchr(token, '=');
            if (equalSign) {
                *equalSign = '\0';
                strncpy(query->columnNames[query->columnCount], token, MAX_NAME_LENGTH - 1);
                query->columnNames[query->columnCount][MAX_NAME_LENGTH - 1] = '\0';
                trimWhitespace(query->columnNames[query->columnCount]);

                strncpy(query->columnValues[query->columnCount], equalSign + 1, MAX_NAME_LENGTH - 1);
                query->columnValues[query->columnCount][MAX_NAME_LENGTH - 1] = '\0';
                trimWhitespace(query->columnValues[query->columnCount]);

                query->columnCount++;
            }
            token = strtok(NULL, ",");
        }

        sscanf(whereClause, "%63s = %63s", query->whereColumn, query->whereValue);
        trimWhitespace(query->whereColumn);
        trimWhitespace(query->whereValue);
    } else {
        free(query);
        free(tableName);
        free(setClause);
        free(whereClause);
        return NULL;
    }

    free(tableName);
    free(setClause);
    free(whereClause);
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
        Table* table = getTableByName(db, query->tableName);
        if (table == NULL) {
            printf("Table %s not found.\n", query->tableName);
            return;
        }

        if (query->columnCount != table->schema.column_count) {
            printf("Error: Data count does not match table column count.\n");
            return;
        }

        void* rowData[MAX_COLUMNS] = { NULL };

        for (uint32_t i = 0; i < table->schema.column_count; i++) {
            ColumnType type = table->schema.columns[i].type;
            char* value = query->columnValues[i];

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
            } else if (type == STRING || type == DATE) {
                rowData[i] = strdup(value);
                if (rowData[i] == NULL) {
                    printf("Memory allocation failed.\n");
                    return;
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
            }
        }
        printf("\n");
    }
}

void executeUpdateQuery(Database* db, Query* query) {
    if (db == NULL || query == NULL) {
        printf("Invalid parameters.\n");
        return;
    }

    if (query->type != UPDATE) {
        printf("Invalid query type for update operation.\n");
        return;
    }

    Table* table = getTableByName(db, query->tableName);
    if (table == NULL) {
        printf("Table %s not found.\n", query->tableName);
        return;
    }

    int whereColumnIndex = -1;
    for (uint32_t i = 0; i < table->schema.column_count; i++) {
        if (strcmp(table->schema.columns[i].name, query->whereColumn) == 0) {
            whereColumnIndex = i;
            break;
        }
    }

    if (whereColumnIndex == -1) {
        printf("Where column %s not found in table %s.\n", query->whereColumn, query->tableName);
        return;
    }

    for (uint32_t i = 0; i < table->row_count; i++) {
        Row* row = &table->rows[i];
        if (compareValue(row->data[whereColumnIndex], query->whereValue, table->schema.columns[whereColumnIndex].type)) {
            for (uint32_t j = 0; j < query->columnCount; j++) {
                int updateColumnIndex = -1;
                for (uint32_t k = 0; k < table->schema.column_count; k++) {
                    if (strcmp(table->schema.columns[k].name, query->columnNames[j]) == 0) {
                        updateColumnIndex = k;
                        break;
                    }
                }

                if (updateColumnIndex != -1) {
                    updateValue(&row->data[updateColumnIndex], query->columnValues[j], table->schema.columns[updateColumnIndex].type);
                }
            }
            printf("Row updated successfully in table %s.\n", query->tableName);
            return;
        }
    }

    printf("No matching row found for update in table %s.\n", query->tableName);
}