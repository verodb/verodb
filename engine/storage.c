#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "storage.h"

Database* createDatabase(const char* name) {
    Database* db = (Database*)malloc(sizeof(Database));
    if (db) {
        snprintf(db->name, sizeof(db->name), "%s", name);
        db->table_count = 0;
    }
    return db;
}

Table* createTable(Database* db, const char* table_name) {
    if (db == NULL || table_name == NULL || db->table_count >= MAX_TABLES) {
        return NULL;
    }

    Table* table = &db->tables[db->table_count++];
    snprintf(table->name, sizeof(table->name), "%s", table_name);
    table->row_count = 0;
    table->schema.column_count = 0;

    return table;
}

bool insertRow(Table* table, const void* data[]) {
    if (table == NULL || table->row_count >= MAX_ROWS) {
        return false;
    }

    Row* row = &table->rows[table->row_count++];
    row->column_count = table->schema.column_count;

    for (uint32_t i = 0; i < row->column_count; ++i) {
        ColumnType type = table->schema.columns[i].type;
        if (type == INTEGER) {
            row->data[i] = malloc(sizeof(int));
            if (row->data[i] == NULL) return false;
            *(int*)row->data[i] = *(int*)data[i];
        } else if (type == FLOAT) {
            row->data[i] = malloc(sizeof(float));
            if (row->data[i] == NULL) return false;
            *(float*)row->data[i] = *(float*)data[i];
        } else if (type == STRING) {
            row->data[i] = malloc(MAX_NAME_LENGTH);
            if (row->data[i] == NULL) return false;
            snprintf((char*)row->data[i], MAX_NAME_LENGTH, "%s", (char*)data[i]);
        } else if (type == ARRAY) {
            row->data[i] = malloc(table->schema.columns[i].array_size * sizeof(int));
            if (row->data[i] == NULL) return false;
            memcpy(row->data[i], data[i], table->schema.columns[i].array_size * sizeof(int));
        } else if (type == DICTIONARY) {
            row->data[i] = malloc(MAX_NAME_LENGTH);
            if (row->data[i] == NULL) return false;
            snprintf((char*)row->data[i], MAX_NAME_LENGTH, "%s", (char*)data[i]);
        } else if (type == DATE) {
            row->data[i] = malloc(MAX_DATE_LENGTH);
            if (row->data[i] == NULL) return false;
            snprintf((char*)row->data[i], MAX_DATE_LENGTH, "%s", (char*)data[i]);
        }
    }

    return true;
}

void printTable(const Table* table) {
    if (table == NULL) return;

    for (uint32_t i = 0; i < table->row_count; ++i) {
        const Row* row = &table->rows[i];
        for (uint32_t j = 0; j < row->column_count; ++j) {
            ColumnType type = table->schema.columns[j].type;
            if (type == INTEGER) {
                printf("%d ", *(int*)row->data[j]);
            } else if (type == FLOAT) {
                printf("%f ", *(float*)row->data[j]);
            } else if (type == STRING) {
                printf("%s ", (char*)row->data[j]);
            } else if (type == ARRAY) {
                int* array = (int*)row->data[j];
                printf("[");
                for (uint32_t k = 0; k < table->schema.columns[j].array_size; ++k) {
                    printf("%d", array[k]);
                    if (k < table->schema.columns[j].array_size - 1) {
                        printf(", ");
                    }
                }
                printf("] ");
            } else if (type == DICTIONARY) {
                printf("%s ", (char*)row->data[j]);
            } else if (type == DATE) {
                printf("%s ", (char*)row->data[j]);
            }
        }
        printf("\n");
    }
}

void updateRowById(Table* table, int index, const void* data[]) {
    if (table == NULL || index < 0 || (uint32_t)index >= table->row_count) {
        return;
    }

    Row* row = &table->rows[index];
    for (uint32_t i = 0; i < row->column_count; ++i) {
        ColumnType type = table->schema.columns[i].type;
        if (type == INTEGER) {
            *(int*)row->data[i] = *(int*)data[i];
        } else if (type == FLOAT) {
            *(float*)row->data[i] = *(float*)data[i];
        } else if (type == STRING) {
            snprintf((char*)row->data[i], MAX_NAME_LENGTH, "%s", (char*)data[i]);
        } else if (type == ARRAY) {
            memcpy(row->data[i], data[i], table->schema.columns[i].array_size * sizeof(int));
        } else if (type == DICTIONARY) {
            snprintf((char*)row->data[i], MAX_NAME_LENGTH, "%s", (char*)data[i]);
        } else if (type == DATE) {
            snprintf((char*)row->data[i], MAX_DATE_LENGTH, "%s", (char*)data[i]);
        }
    }
}

void deleteRowById(Table* table, int index) {
    if (table == NULL || index < 0 || (uint32_t)index >= table->row_count) {
        return;
    }

    for (int i = index; i < (int)table->row_count - 1; ++i) {
        table->rows[i] = table->rows[i + 1];
    }
    --table->row_count;
}

void freeTable(Table* table) {
    if (table == NULL) return;
    
    for (uint32_t i = 0; i < table->row_count; ++i) {
        Row* row = &table->rows[i];
        for (uint32_t j = 0; j < row->column_count; ++j) {
            free(row->data[j]);
        }
    }
}

void freeDatabase(Database* db) {
    if (db == NULL) return;
    
    for (uint32_t i = 0; i < db->table_count; ++i) {
        freeTable(&db->tables[i]);
    }
    free(db);
}

const Row* getRowById(const Table* table, int index) {
    if (table == NULL || index < 0 || (uint32_t)index >= table->row_count) {
        return NULL;
    }
    return &table->rows[index];
}

const Table* getTableByName(const Database* db, const char* tableName) {
    if (db == NULL || tableName == NULL) return NULL;

    for (uint32_t i = 0; i < db->table_count; i++) {
        if (strcmp(db->tables[i].name, tableName) == 0) {
            return &db->tables[i];
        }
    }
    return NULL;
}