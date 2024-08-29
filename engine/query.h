#ifndef QUERY_H
#define QUERY_H

#include "storage.h"

typedef enum {
    CREATE_DATABASE,
    CREATE_TABLE,
    INSERT_INTO,
    SELECT_FROM,
    UPDATE,
    DELETE_FROM,
    DROP_TABLE,
    DROP_DATABASE,
    ALTER_TABLE
} QueryType;

typedef struct {
    QueryType type;
    char databaseName[MAX_NAME_LENGTH];
    char tableName[MAX_NAME_LENGTH];
    char columnNames[MAX_COLUMNS][MAX_NAME_LENGTH];
    char columnValues[MAX_COLUMNS][MAX_NAME_LENGTH];
    uint32_t columnCount;
    char whereClause[MAX_NAME_LENGTH * 3];
    char alterAction[MAX_NAME_LENGTH]; // ADD or DROP
    char alterColumnName[MAX_NAME_LENGTH];
    ColumnType alterColumnType;
} Query;

Query* parseCreateDatabaseQuery(const char* queryStr);
Query* parseCreateTableQuery(const char* queryStr);
Query* parseInsertQuery(const char* queryStr);
Query* parseSelectQuery(const char* queryStr);
Query* parseUpdateQuery(const char* queryStr);
Query* parseDeleteQuery(const char* queryStr);
Query* parseDropTableQuery(const char* queryStr);
Query* parseDropDatabaseQuery(const char* queryStr);
Query* parseAlterTableQuery(const char* queryStr);

void executeCreateDatabaseQuery(Database* db, Query* query);
void executeCreateTableQuery(Database* db, Query* query);
void executeInsertQuery(Database* db, Query* query);
void executeSelectQuery(const Database* db, const Query* query);
void executeUpdateQuery(Database* db, Query* query);
void executeDeleteQuery(Database* db, Query* query);
void executeDropTableQuery(Database* db, Query* query);
void executeDropDatabaseQuery(Database** db, Query* query);
void executeAlterTableQuery(Database* db, Query* query);

#endif