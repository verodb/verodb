#ifndef QUERY_H
#define QUERY_H

#include "storage.h"

typedef enum {
    CREATE_DATABASE,
    CREATE_TABLE,
    INSERT_INTO
} QueryType;

typedef struct {
    QueryType type;
    char databaseName[MAX_NAME_LENGTH];
    char tableName[MAX_NAME_LENGTH];
    void* rowData[MAX_NAME_LENGTH];  // Adjust based on the row data
    int dataCount;
} Query;

Query* parseCreateDatabaseQuery(const char* queryStr);
Query* parseCreateTableQuery(const char* queryStr);
Query* parseInsertQuery(const char* queryStr);  // Added for INSERT INTO
void executeCreateDatabaseQuery(Database* db, Query* query);
void executeCreateTableQuery(Database* db, Query* query);
void executeInsertQuery(Database* db, Query* query);  // Added for INSERT INTO

#endif
