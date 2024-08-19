#include <stdio.h>
#include "storage.h"

int main() {
    // Create the first database
    Database* db1 = createDatabase("MyDatabase1");
    if (!db1) {
        printf("Error creating first database.\n");
        return 1;
    }

    // Create a table within the first database
    Table* table1 = createTable(db1, "Users");
    if (!table1) {
        printf("Error creating table in first database.\n");
        freeDatabase(db1);
        return 1;
    }

    // Define schema for the "Users" table in the first database
    table1->schema.column_count = 3;
    strcpy(table1->schema.columns[0].name, "ID");
    table1->schema.columns[0].type = INTEGER;
    
    strcpy(table1->schema.columns[1].name, "Name");
    table1->schema.columns[1].type = STRING;
    
    strcpy(table1->schema.columns[2].name, "Age");
    table1->schema.columns[2].type = INTEGER;

    // Insert rows into the "Users" table in the first database
    int id1 = 1;
    const char* name1 = "Alice";
    int age1 = 30;
    const void* row1[] = { &id1, name1, &age1 };
    insertRow(table1, row1);

    int id2 = 2;
    const char* name2 = "Bob";
    int age2 = 25;
    const void* row2[] = { &id2, name2, &age2 };
    insertRow(table1, row2);

    // Print the first table
    printf("Table: Users in MyDatabase1\n");
    printTable(table1);

    // Create the second database
    Database* db2 = createDatabase("MyDatabase2");
    if (!db2) {
        printf("Error creating second database.\n");
        freeDatabase(db1);
        return 1;
    }

    // Create a table within the second database
    Table* table2 = createTable(db2, "Employees");
    if (!table2) {
        printf("Error creating table in second database.\n");
        freeDatabase(db1);
        freeDatabase(db2);
        return 1;
    }

    // Define schema for the "Employees" table in the second database
    table2->schema.column_count = 3;
    strcpy(table2->schema.columns[0].name, "EmpID");
    table2->schema.columns[0].type = INTEGER;
    
    strcpy(table2->schema.columns[1].name, "EmpName");
    table2->schema.columns[1].type = STRING;
    
    strcpy(table2->schema.columns[2].name, "Salary");
    table2->schema.columns[2].type = INTEGER;

    // Insert rows into the "Employees" table in the second database
    int empId1 = 100;
    const char* empName1 = "John";
    int salary1 = 50000;
    const void* empRow1[] = { &empId1, empName1, &salary1 };
    insertRow(table2, empRow1);

    int empId2 = 101;
    const char* empName2 = "Doe";
    int salary2 = 60000;
    const void* empRow2[] = { &empId2, empName2, &salary2 };
    insertRow(table2, empRow2);

    // Print the second table
    printf("Table: Employees in MyDatabase2\n");
    printTable(table2);

    // Update a row in the "Employees" table in the second database
    const char* newEmpName = "Jane";
    const void* updatedEmpRow[] = { &empId2, newEmpName, &salary2 };
    updateRowById(table2, 1, updatedEmpRow);

    // Print the "Employees" table after update
    printf("Employees Table after update:\n");
    printTable(table2);

    // Delete a row from the "Users" table in the first database
    deleteRowById(table1, 0);

    // Print the "Users" table after deletion
    printf("Users Table after deletion:\n");
    printTable(table1);

    // Free both databases
    freeDatabase(db1);
    freeDatabase(db2);

    return 0;
}
