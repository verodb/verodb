#ifndef QUERY_H
#define QUERY_H

#include "storage.h"

typedef enum {
    CREATE_DATABASE,
    CREATE_TABLE
} QueryType;

typedef struct {
    QueryType type;
    char databaseName[MAX_NAME_LENGTH];
    char tableName[MAX_NAME_LENGTH];
} Query;

Query* parseCreateDatabaseQuery(const char* queryStr);
Query* parseCreateTableQuery(const char* queryStr);
void executeCreateDatabaseQuery(Database* db, Query* query);
void executeCreateTableQuery(Database* db, Query* query);

#endif
