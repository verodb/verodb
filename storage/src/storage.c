#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "storage.h"

Table* createTable() {
    Table* table = (Table*)malloc(sizeof(Table));
    if (table) {
        table->row_count = 0;
        table->schema.column_count = 0;
    }
    return table;
}

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
        } else if (type == STRING) {
            row->data[i] = malloc(MAX_NAME_LENGTH);
            strncpy((char*)row->data[i], (char*)data[i], MAX_NAME_LENGTH - 1);
            ((char*)row->data[i])[MAX_NAME_LENGTH - 1] = '\0';
        }
    }

    return true;
}

// Print the contents of the table
void printTable(const Table* table) {
    for (int i = 0; i < table->row_count; ++i) {
        const Row* row = &table->rows[i];
        for (int j = 0; j < row->column_count; ++j) {
            ColumnType type = table->schema.columns[j].type;
            if (type == INTEGER) {
                printf("%d ", *(int*)row->data[j]);
            } else if (type == STRING) {
                printf("%s ", (char*)row->data[j]);
            }
        }
        printf("\n");
    }
}

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
        } else if (type == STRING) {
            strncpy((char*)row->data[i], (char*)data[i], MAX_NAME_LENGTH - 1);
            ((char*)row->data[i])[MAX_NAME_LENGTH - 1] = '\0';
        }
    }
}

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


void freeTable(Table* table) {
    if (table) {
        for (int i = 0; i < table->row_count; ++i) {
            Row* row = &table->rows[i];
            for (int j = 0; j < row->column_count; ++j) {
                free(row->data[j]);
            }
        }
        free(table);
    }
}

const Row* getRowById(const Table* table, int index) {
    if (index < 0 || index >= table->row_count) {
        return NULL;
    }
    return &table->rows[index];
}
