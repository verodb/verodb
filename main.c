#include <stdio.h>
#include "storage.h"

int main() {
    Table* table = createTable();
    if (table == NULL) {
        printf("Failed to create table.\n");
        return 1;
    }

    table->schema.columns[0].type = INTEGER;
    strncpy(table->schema.columns[0].name, "ID", MAX_NAME_LENGTH - 1);
    table->schema.columns[0].name[MAX_NAME_LENGTH - 1] = '\0';

    table->schema.columns[1].type = STRING;
    strncpy(table->schema.columns[1].name, "Name", MAX_NAME_LENGTH - 1);
    table->schema.columns[1].name[MAX_NAME_LENGTH - 1] = '\0';

    table->schema.columns[2].type = INTEGER;
    strncpy(table->schema.columns[2].name, "Age", MAX_NAME_LENGTH - 1);
    table->schema.columns[2].name[MAX_NAME_LENGTH - 1] = '\0';

    table->schema.column_count = 3;

    int id1 = 1;
    char name1[] = "Aditya";
    int age1 = 22;
    void* data1[] = { &id1, name1, &age1 };
    insertRow(table, data1);

    int id2 = 2;
    char name2[] = "Harsh";
    int age2 = 21;
    void* data2[] = { &id2, name2, &age2 };
    insertRow(table, data2);

    printf("All rows in the table:\n");
    printTable(table);

    int index_to_retrieve = 0;
    const Row* row = getRowById(table, index_to_retrieve);
    if (row) {
        printf("\nRetrieved row with index %d:\n", index_to_retrieve);
        for (int i = 0; i < row->column_count; ++i) {
            if (table->schema.columns[i].type == INTEGER) {
                printf("%s: %d\n", table->schema.columns[i].name, *(int*)row->data[i]);
            } else if (table->schema.columns[i].type == STRING) {
                printf("%s: %s\n", table->schema.columns[i].name, (char*)row->data[i]);
            }
        }
    } else {
        printf("\nNo row found with index %d.\n", index_to_retrieve);
    }

    printf("\nUpdating row with index 0...\n");
    int new_id = 100;
    char new_name[] = "Updated Aditya";
    int new_age = 30;
    void* new_data[] = { &new_id, new_name, &new_age };
    updateRowById(table, 1, new_data);
    printTable(table);

    printf("\nDeleting row with index 1...\n");
    deleteRowById(table, 1);
    printTable(table);

    freeTable(table);

    return 0;
}
