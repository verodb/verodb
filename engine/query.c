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

    char* tableName = malloc(MAX_NAME_LENGTH);
    char* whereClause = malloc(strlen(queryStr));
    if (tableName == NULL || whereClause == NULL) {
        free(query);
        free(tableName);
        free(whereClause);
        return NULL;
    }

    int result = sscanf(queryStr, "SELECT * FROM %63s WHERE %[^\n]", tableName, whereClause);
    if (result >= 1) {
        query->type = SELECT_FROM;
        strncpy(query->tableName, tableName, MAX_NAME_LENGTH - 1);
        query->tableName[MAX_NAME_LENGTH - 1] = '\0';
        trimWhitespace(query->tableName);

        if (result == 2) {
            strncpy(query->whereClause, whereClause, sizeof(query->whereClause) - 1);
            query->whereClause[sizeof(query->whereClause) - 1] = '\0';
            trimWhitespace(query->whereClause);
        }
    } else {
        free(query);
        free(tableName);
        free(whereClause);
        return NULL;
    }

    free(tableName);
    free(whereClause);
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

        strncpy(query->whereClause, whereClause, sizeof(query->whereClause) - 1);
        query->whereClause[sizeof(query->whereClause) - 1] = '\0';
        trimWhitespace(query->whereClause);
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

Query* parseDeleteQuery(const char* queryStr) {
    if (queryStr == NULL) return NULL;

    Query* query = malloc(sizeof(Query));
    if (query == NULL) return NULL;

    memset(query, 0, sizeof(Query));

    char* tableName = malloc(MAX_NAME_LENGTH);
    char* whereClause = malloc(strlen(queryStr));
    if (tableName == NULL || whereClause == NULL) {
        free(query);
        free(tableName);
        free(whereClause);
        return NULL;
    }

    if (sscanf(queryStr, "DELETE FROM %63s WHERE %[^\n]", tableName, whereClause) == 2) {
        query->type = DELETE_FROM;
        strncpy(query->tableName, tableName, MAX_NAME_LENGTH - 1);
        query->tableName[MAX_NAME_LENGTH - 1] = '\0';
        trimWhitespace(query->tableName);

        strncpy(query->whereClause, whereClause, sizeof(query->whereClause) - 1);
        query->whereClause[sizeof(query->whereClause) - 1] = '\0';
        trimWhitespace(query->whereClause);
    } else {
        free(query);
        free(tableName);
        free(whereClause);
        return NULL;
    }

    free(tableName);
    free(whereClause);
    return query;
}

Query* parseDropTableQuery(const char* queryStr) {
    if (queryStr == NULL) return NULL;

    Query* query = malloc(sizeof(Query));
    if (query == NULL) return NULL;

    memset(query, 0, sizeof(Query));

    if (sscanf(queryStr, "DROP TABLE %63s", query->tableName) == 1) {
        query->type = DROP_TABLE;
        trimWhitespace(query->tableName);
    } else {
        free(query);
        return NULL;
    }

    return query;
}

Query* parseDropDatabaseQuery(const char* queryStr) {
    if (queryStr == NULL) return NULL;

    Query* query = malloc(sizeof(Query));
    if (query == NULL) return NULL;

    memset(query, 0, sizeof(Query));

    if (sscanf(queryStr, "DROP DATABASE %63s", query->databaseName) == 1) {
        query->type = DROP_DATABASE;
        trimWhitespace(query->databaseName);
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

        if (insertRow((Table *)table, (const void**)rowData)) {
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
    Table* table = getTableByName(db, query->tableName);
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
        bool rowMatches = true;

        if (strlen(query->whereClause) > 0) {
            char column[MAX_NAME_LENGTH];
            char operator;
            char value[MAX_NAME_LENGTH];
            sscanf(query->whereClause, "%s %c %s", column, &operator, value);

            for (uint32_t j = 0; j < table->schema.column_count; j++) {
                if (strcmp(table->schema.columns[j].name, column) == 0) {
                    ColumnType type = table->schema.columns[j].type;
                    if (!compareValue(row->data[j], value, type, operator)) {
                        rowMatches = false;
                        break;
                    }
                }
            }
        }

        if (rowMatches) {
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

    char whereColumn[MAX_NAME_LENGTH];
    char whereOperator;
    char whereValue[MAX_NAME_LENGTH];
    sscanf(query->whereClause, "%s %c %s", whereColumn, &whereOperator, whereValue);

    int whereColumnIndex = -1;
    for (uint32_t i = 0; i < table->schema.column_count; i++) {
        if (strcmp(table->schema.columns[i].name, whereColumn) == 0) {
            whereColumnIndex = i;
            break;
        }
    }

    if (whereColumnIndex == -1) {
        printf("Where column %s not found in table %s.\n", whereColumn, query->tableName);
        return;
    }

    int updatedRows = 0;
    for (uint32_t i = 0; i < table->row_count; i++) {
        Row* row = (Row *)&table->rows[i];
        if (compareValue(row->data[whereColumnIndex], whereValue, table->schema.columns[whereColumnIndex].type, whereOperator)) {
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
            updatedRows++;
        }
    }

    printf("%d row(s) updated in table %s.\n", updatedRows, query->tableName);
}

void executeDeleteQuery(Database* db, Query* query) {
    if (db == NULL || query == NULL) {
        printf("Invalid parameters.\n");
        return;
    }

    if (query->type != DELETE_FROM) {
        printf("Invalid query type for delete operation.\n");
        return;
    }

    Table* table = getTableByName(db, query->tableName);
    if (table == NULL) {
        printf("Table %s not found.\n", query->tableName);
        return;
    }

    char whereColumn[MAX_NAME_LENGTH];
    char whereOperator;
    char whereValue[MAX_NAME_LENGTH];
    sscanf(query->whereClause, "%s %c %s", whereColumn, &whereOperator, whereValue);

    int whereColumnIndex = -1;
    for (uint32_t i = 0; i < table->schema.column_count; i++) {
        if (strcmp(table->schema.columns[i].name, whereColumn) == 0) {
            whereColumnIndex = i;
            break;
        }
    }

    if (whereColumnIndex == -1) {
        printf("Where column %s not found in table %s.\n", whereColumn, query->tableName);
        return;
    }

    int deletedRows = 0;
    for (uint32_t i = 0; i < table->row_count; i++) {
        Row* row = (Row *)&table->rows[i];
        if (compareValue(row->data[whereColumnIndex], whereValue, table->schema.columns[whereColumnIndex].type, whereOperator)) {
            deleteRowById((Table *)table, i);
            i--;  // Decrement i to account for the shifted rows
            deletedRows++;
        }
    }

    printf("%d row(s) deleted from table %s.\n", deletedRows, query->tableName);
}

void executeDropTableQuery(Database* db, Query* query) {
    if (db == NULL || query == NULL) {
        printf("Invalid parameters.\n");
        return;
    }

    if (query->type != DROP_TABLE) {
        printf("Invalid query type for drop table operation.\n");
        return;
    }

    for (uint32_t i = 0; i < db->table_count; i++) {
        if (strcmp(db->tables[i].name, query->tableName) == 0) {
            freeTable(&db->tables[i]);
            // Shift the remaining tables
            for (uint32_t j = i; j < db->table_count - 1; j++) {
                db->tables[j] = db->tables[j + 1];
            }
            db->table_count--;
            printf("Table %s dropped successfully.\n", query->tableName);
            return;
        }
    }

    printf("Table %s not found.\n", query->tableName);
}

void executeDropDatabaseQuery(Database** db, Query* query) {
    if (db == NULL || *db == NULL || query == NULL) {
        printf("Invalid parameters.\n");
        return;
    }

    if (query->type != DROP_DATABASE) {
        printf("Invalid query type for drop database operation.\n");
        return;
    }

    if (strcmp((*db)->name, query->databaseName) == 0) {
        freeDatabase(*db);
        *db = NULL;
        printf("Database %s dropped successfully.\n", query->databaseName);
    } else {
        printf("Database %s not found.\n", query->databaseName);
    }
}

Query* parseAlterTableQuery(const char* queryStr) {
    if (queryStr == NULL) return NULL;

    Query* query = malloc(sizeof(Query));
    if (query == NULL) return NULL;

    memset(query, 0, sizeof(Query));

    char action[10];
    char columnType[20];

    if (sscanf(queryStr, "ALTER TABLE %63s %9s %63s %19s", 
               query->tableName, query->alterAction, 
               query->alterColumnName, columnType) == 4) {
        query->type = ALTER_TABLE;
        trimWhitespace(query->tableName);
        trimWhitespace(query->alterAction);
        trimWhitespace(query->alterColumnName);
        trimWhitespace(columnType);

        if (strcmp(columnType, "INTEGER") == 0) {
            query->alterColumnType = INTEGER;
        } else if (strcmp(columnType, "FLOAT") == 0) {
            query->alterColumnType = FLOAT;
        } else if (strcmp(columnType, "STRING") == 0) {
            query->alterColumnType = STRING;
        } else if (strcmp(columnType, "DATE") == 0) {
            query->alterColumnType = DATE;
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

void executeAlterTableQuery(Database* db, Query* query) {
    if (db == NULL || query == NULL) {
        printf("Invalid parameters.\n");
        return;
    }

    if (query->type != ALTER_TABLE) {
        printf("Invalid query type for alter table operation.\n");
        return;
    }

    Table* table = getTableByName(db, query->tableName);
    if (table == NULL) {
        printf("Table %s not found.\n", query->tableName);
        return;
    }

    if (strcmp(query->alterAction, "ADD") == 0) {
        if (table->schema.column_count >= MAX_COLUMNS) {
            printf("Cannot add more columns. Maximum limit reached.\n");
            return;
        }

        Column* newColumn = (Column *)&table->schema.columns[table->schema.column_count];
        strncpy(newColumn->name, query->alterColumnName, MAX_NAME_LENGTH - 1);
        newColumn->name[MAX_NAME_LENGTH - 1] = '\0';
        newColumn->type = query->alterColumnType;

        // Initialize new column data for existing rows
        for (uint32_t i = 0; i < table->row_count; i++) {
            void* newData = NULL;
            switch (query->alterColumnType) {
                case INTEGER:
                    newData = malloc(sizeof(int));
                    *(int*)newData = 0;
                    break;
                case FLOAT:
                    newData = malloc(sizeof(float));
                    *(float*)newData = 0.0f;
                    break;
                case STRING:
                case DATE:
                    newData = malloc(MAX_NAME_LENGTH);
                    ((char*)newData)[0] = '\0';
                    break;
            }
            table->rows[i].data[table->schema.column_count] = newData;
        }

        table->schema.column_count++;
        printf("Column %s added to table %s.\n", query->alterColumnName, query->tableName);
    } else if (strcmp(query->alterAction, "DROP") == 0) {
        int columnIndex = -1;
        for (uint32_t i = 0; i < table->schema.column_count; i++) {
            if (strcmp(table->schema.columns[i].name, query->alterColumnName) == 0) {
                columnIndex = i;
                break;
            }
        }

        if (columnIndex == -1) {
            printf("Column %s not found in table %s.\n", query->alterColumnName, query->tableName);
            return;
        }

        // Free memory for the dropped column
        for (uint32_t i = 0; i < table->row_count; i++) {
            free(table->rows[i].data[columnIndex]);
            // Shift remaining data
            for (uint32_t j = columnIndex; j < table->schema.column_count - 1; j++) {
                table->rows[i].data[j] = table->rows[i].data[j + 1];
            }
        }

        // Shift remaining columns in schema
        for (uint32_t i = columnIndex; i < table->schema.column_count - 1; i++) {
            table->schema.columns[i] = table->schema.columns[i + 1];
        }

        table->schema.column_count--;
        printf("Column %s dropped from table %s.\n", query->alterColumnName, query->tableName);
    } else {
        printf("Invalid ALTER TABLE action.\n");
    }
}

