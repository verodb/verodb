#ifndef QUERY_H
#define QUERY_H

#include "storage.h"


typedef enum {
    CREATE_DATABASE
} QueryType;


typedef struct {
    QueryType type;
    char databaseName[256];
} Query;

Query* parseCreateDatabaseQuery(const char* queryStr);
void executeCreateDatabaseQuery(Database* db, Query* query);

#endif
