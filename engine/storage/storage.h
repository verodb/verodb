#ifndef STORAGE_H
#define STORAGE_H

#include <stdbool.h>

#define MAX_NAME_LENGTH 64
#define MAX_ROWS 100
#define MAX_TABLES 100
#define MAX_DATE_LENGTH 12
// Enum for column types
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
    int array_size;  // Used only if type is ARRAY
} Column;

// Structure for schema definition of a table
typedef struct {
    Column columns[MAX_NAME_LENGTH];
    int column_count;
} Schema;

// Structure for a row of data
typedef struct {
    void* data[MAX_NAME_LENGTH];
    int column_count;
} Row;

// Structure for a table, including its schema and rows
typedef struct {
    char name[MAX_NAME_LENGTH];
    Schema schema;
    Row rows[MAX_ROWS];
    int row_count;
} Table;

// Structure for a database, including its tables
typedef struct {
    char name[MAX_NAME_LENGTH];
    Table tables[MAX_TABLES];
    int table_count;
} Database;

// Function prototypes
Database* createDatabase(const char* name);
Table* createTable(Database* db, const char* table_name);
bool insertRow(Table* table, const void* data[]);
void printTable(const Table* table);
void updateRowById(Table* table, int index, const void* data[]);
void deleteRowById(Table* table, int index);
void freeTable(Table* table);
void freeDatabase(Database* db);
const Row* getRowById(const Table* table, int index);

#endif // STORAGE_H
