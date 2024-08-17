#ifndef STORAGE_H
#define STORAGE_H

#include <stdbool.h>

#define MAX_ROWS 100
#define MAX_NAME_LENGTH 50
#define MAX_COLUMNS 10

typedef enum {
    INTEGER,
    STRING
} ColumnType;

typedef struct {
    char name[MAX_NAME_LENGTH];
    ColumnType type;
} Column;

typedef struct {
    Column columns[MAX_COLUMNS];
    int column_count;
} Schema;

typedef struct {
    void* data[MAX_COLUMNS];
    int column_count;
} Row;

typedef struct {
    Schema schema;
    Row rows[MAX_ROWS];
    int row_count;
} Table;

Table* createTable();
bool insertRow(Table* table, const void* data[]);
void printTable(const Table* table);
void updateRowById(Table* table, int index, const void* data[]);
void deleteRowById(Table* table, int index);
void freeTable(Table* table);
const Row* getRowById(const Table* table, int index);

#endif 
#ifndef STORAGE_H
#define STORAGE_H

#include <stdbool.h>

#define MAX_ROWS 100
#define MAX_NAME_LENGTH 50
#define MAX_COLUMNS 10

typedef enum {
    INTEGER,
    STRING
} ColumnType;

typedef struct {
    char name[MAX_NAME_LENGTH];
    ColumnType type;
} Column;

typedef struct {
    Column columns[MAX_COLUMNS];
    int column_count;
} Schema;

typedef struct {
    void* data[MAX_COLUMNS];
    int column_count;
} Row;

typedef struct {
    Schema schema;
    Row rows[MAX_ROWS];
    int row_count;
} Table;


Table* createTable();
bool insertRow(Table* table, const void* data[]);
void printTable(const Table* table);
void updateRowById(Table* table, int index, const void* data[]);
void deleteRowById(Table* table, int index);
void freeTable(Table* table);
const Row* getRowById(const Table* table, int index);

#endif
