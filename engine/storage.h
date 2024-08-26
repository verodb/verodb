#ifndef STORAGE_H
#define STORAGE_H

#include <stdbool.h>
#include <stdint.h>

#define MAX_NAME_LENGTH 64
#define MAX_ROWS 100
#define MAX_TABLES 100
#define MAX_DATE_LENGTH 12

typedef enum {
    INTEGER,
    STRING,
    FLOAT,
    ARRAY,
    DICTIONARY,
    DATE
} ColumnType;

typedef struct {
    char name[MAX_NAME_LENGTH];
    ColumnType type;
    uint32_t array_size;
} Column;

typedef struct {
    Column columns[MAX_NAME_LENGTH];
    uint32_t column_count;
} Schema;

typedef struct {
    void* data[MAX_NAME_LENGTH];
    uint32_t column_count;
} Row;

typedef struct {
    char name[MAX_NAME_LENGTH];
    Schema schema;
    Row rows[MAX_ROWS];
    uint32_t row_count;
} Table;

typedef struct {
    char name[MAX_NAME_LENGTH];
    Table tables[MAX_TABLES];
    uint32_t table_count;
} Database;

Database* createDatabase(const char* name);
Table* createTable(Database* db, const char* table_name);
bool insertRow(Table* table, const void* data[]);
void printTable(const Table* table);
void updateRowById(Table* table, int index, const void* data[]);
void deleteRowById(Table* table, int index);
void freeTable(Table* table);
void freeDatabase(Database* db);
const Row* getRowById(const Table* table, int index);
const Table* getTableByName(const Database* db, const char* tableName);

#endif // STORAGE_H