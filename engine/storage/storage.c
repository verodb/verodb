#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "storage.h"

// CREATE DATABASE
Database* createDatabase(const char* name) {
    Database* db = (Database*)malloc(sizeof(Database));
    if (db) {
        strncpy(db->name, name, MAX_NAME_LENGTH - 1);
        db->name[MAX_NAME_LENGTH - 1] = '\0';
        db->table_count = 0;
    }
    return db;
}

// CREATE TABLE
Table* createTable(Database* db, const char* table_name) {
    if (db->table_count >= MAX_TABLES) {
        printf("Error: Database is full.\n");
        return NULL;
    }

    Table* table = &db->tables[db->table_count++];
    strncpy(table->name, table_name, MAX_NAME_LENGTH - 1);
    table->name[MAX_NAME_LENGTH - 1] = '\0';
    table->row_count = 0;
    table->schema.column_count = 0;

    return table;
}


// INSERT ROW
bool insertRow(Table* table, const void* data[]) {
    if (table->row_count >= MAX_ROWS) {
        printf("Error: Table is full.\n");
        return false;
    }

    Row* row = &table->rows[table->row_count++];
    row->column_count = table->schema.column_count;

    for (int i = 0; i < row->column_count; ++i) {
        ColumnType type = table->schema.columns[i].type;
        if (type == INTEGER) {
            row->data[i] = malloc(sizeof(int));
            *(int*)row->data[i] = *(int*)data[i];
        } else if (type == FLOAT) {
            row->data[i] = malloc(sizeof(float));
            *(float*)row->data[i] = *(float*)data[i];
        } else if (type == STRING) {
            row->data[i] = malloc(MAX_NAME_LENGTH);
            strncpy((char*)row->data[i], (char*)data[i], MAX_NAME_LENGTH - 1);
            ((char*)row->data[i])[MAX_NAME_LENGTH - 1] = '\0';
        } else if (type == ARRAY) {
            row->data[i] = malloc(table->schema.columns[i].array_size * sizeof(int));
            memcpy(row->data[i], data[i], table->schema.columns[i].array_size * sizeof(int));
        } else if (type == DICTIONARY) {
            row->data[i] = malloc(MAX_NAME_LENGTH);
            strncpy((char*)row->data[i], (char*)data[i], MAX_NAME_LENGTH - 1);
            ((char*)row->data[i])[MAX_NAME_LENGTH - 1] = '\0';
        } else if (type == DATE) {
            row->data[i] = malloc(MAX_DATE_LENGTH);
            strncpy((char*)row->data[i], (char*)data[i], MAX_DATE_LENGTH - 1);
            ((char*)row->data[i])[MAX_DATE_LENGTH - 1] = '\0';
        }
    }

    return true;
}



// PRINT TABLE
void printTable(const Table* table) {
    for (int i = 0; i < table->row_count; ++i) {
        const Row* row = &table->rows[i];
        for (int j = 0; j < row->column_count; ++j) {
            ColumnType type = table->schema.columns[j].type;
            if (type == INTEGER) {
                printf("%d ", *(int*)row->data[j]);
            } else if (type == FLOAT) {
                printf("%lf ", *(float*)row->data[j]);
            } else if (type == STRING) {
                printf("%s ", (char*)row->data[j]);
            } else if (type == ARRAY) {
                int* array = (int*)row->data[j];
                printf("[");
                for (int k = 0; k < table->schema.columns[j].array_size; ++k) {
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


// UPDATE ROW
void updateRowById(Table* table, int index, const void* data[]) {
    if (index < 0 || index >= table->row_count) {
        printf("Error: Row index %d out of bounds.\n", index);
        return;
    }

    Row* row = &table->rows[index];
    for (int i = 0; i < row->column_count; ++i) {
        ColumnType type = table->schema.columns[i].type;
        if (type == INTEGER) {
            *(int*)row->data[i] = *(int*)data[i];
        } else if (type == FLOAT) {
            *(float*)row->data[i] = *(float*)data[i];
        } else if (type == STRING) {
            strncpy((char*)row->data[i], (char*)data[i], MAX_NAME_LENGTH - 1);
            ((char*)row->data[i])[MAX_NAME_LENGTH - 1] = '\0';
        } else if (type == ARRAY) {
            memcpy(row->data[i], data[i], table->schema.columns[i].array_size * sizeof(int));
        } else if (type == DICTIONARY) {
            strncpy((char*)row->data[i], (char*)data[i], MAX_NAME_LENGTH - 1);
            ((char*)row->data[i])[MAX_NAME_LENGTH - 1] = '\0';
        } else if (type == DATE) {
            strncpy((char*)row->data[i], (char*)data[i], MAX_DATE_LENGTH - 1);
            ((char*)row->data[i])[MAX_DATE_LENGTH - 1] = '\0';
        }
    }
}


// DELETE ROW
void deleteRowById(Table* table, int index) {
    if (index < 0 || index >= table->row_count) {
        printf("Error: Row index %d out of bounds.\n", index);
        return;
    }

    for (int i = index; i < table->row_count - 1; ++i) {
        table->rows[i] = table->rows[i + 1];
    }
    --table->row_count;
}

// FREE TABLE
void freeTable(Table* table) {
    if (table) {
        for (int i = 0; i < table->row_count; ++i) {
            Row* row = &table->rows[i];
            for (int j = 0; j < row->column_count; ++j) {
                free(row->data[j]);
            }
        }
    }
}

//FREE DB
void freeDatabase(Database* db) {
    if (db) {
        for (int i = 0; i < db->table_count; ++i) {
            freeTable(&db->tables[i]);
        }
        free(db);
    }
}

const Row* getRowById(const Table* table, int index) {
    if (index < 0 || index >= table->row_count) {
        return NULL;
    }
    return &table->rows[index];
}
