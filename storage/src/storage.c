#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ROWS 100
#define MAX_NAME_LENGTH 50


typedef struct {
    int id;
    char name[MAX_NAME_LENGTH];
    int age;
} Row;

typedef struct {
    Row rows[MAX_ROWS];
    int row_count;
} Table;


Table* createTable() {
    Table* table = (Table*)malloc(sizeof(Table));
    if (table) {
        table->row_count = 0;
    }
    return table;
}

void insertRow(Table* table, int id, const char* name, int age) {
    if (table->row_count < MAX_ROWS) {
        Row* row = &table->rows[table->row_count++];
        row->id = id;
        strncpy(row->name, name, MAX_NAME_LENGTH - 1);
        row->name[MAX_NAME_LENGTH - 1] = '\0'; 
        row->age = age;
    } else {
        printf("Error: Table is full.\n");
    }
}

void printTable(const Table* table) {
    for (int i = 0; i < table->row_count; ++i) {
        const Row* row = &table->rows[i];
        printf("ID: %d, Name: %s, Age: %d\n", row->id, row->name, row->age);
    }
}


void freeTable(Table* table) {
    free(table);
}

Row* getRowById(const Table* table, int id) {
    for (int i = 0; i < table->row_count; ++i) {
        if (table->rows[i].id == id) {
            return &table->rows[i];
        }
    }
    return NULL;
}
