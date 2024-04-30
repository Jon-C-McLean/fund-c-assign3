/**
 * @file schema.c
 * @author Jon McLean (13515869)
 */

#include "schema.h"

status_t SCHEMA_CreateDefaultDatabaseSchema(database_schema_t **schema) {
    table_schema_def_t employeeTable;
    char tableName[MAX_TABLE_NAME_SIZE] = "Employee";

    if(schema == NULL) {
        return kStatus_InvalidArgument;
    }

    *schema = (database_schema_t *)malloc(sizeof(database_schema_t));
    if(*schema == NULL) {
        return kStatus_AllocError;
    }

    (*schema)->numTables = 0;
    memset((*schema)->dbName, 0, MAX_DB_NAME_SIZE);
    
    table_col_def_t *employeeColumns = malloc(sizeof(table_col_def_t) * 3);
    memset(employeeColumns, 0, sizeof(table_col_def_t) * 3);

    strncpy(employeeColumns[0].columnName, "EmployeeID", MAX_COLUMN_NAME_SIZE);
    employeeColumns[0].type = INT;
    employeeColumns[0].size = 4;
    employeeColumns[0].isPrimaryKey = 1;

    strncpy(employeeColumns[1].columnName, "Name", MAX_COLUMN_NAME_SIZE);
    employeeColumns[1].type = STRING;
    employeeColumns[1].size = 100;
    employeeColumns[1].isPrimaryKey = 0;

    strncpy(employeeColumns[2].columnName, "Date of Birth", MAX_COLUMN_NAME_SIZE);
    employeeColumns[2].type = STRING;
    employeeColumns[2].size = 10;
    employeeColumns[2].isPrimaryKey = 0;

    employeeTable.numColumns = (sizeof(*employeeColumns) * 3) / sizeof(table_col_def_t); /* i.e. 3 */
    employeeTable.columns = employeeColumns;

    return SCHEMA_DefineTableStructure(*schema, tableName, employeeColumns, 3);
}

status_t SCHEMA_DestroyDatabaseSchema(database_schema_t *schema) {
    /* int i = 0; */
    if(schema == NULL) {
        return kStatus_InvalidArgument;
    }

    free(schema);
    return kStatus_Success;
}

status_t SCHEMA_DefineTableStructure(database_schema_t *schema, char *name, table_col_def_t *columns, int numColumns) {
    if(schema == NULL || name == NULL || columns == NULL) {
        return kStatus_InvalidArgument;
    }

    if(schema->numTables == MAX_TABLES) {
        return kStatus_Schema_UnknownError;
    }

    schema->tables = (table_schema_def_t *)realloc(schema->tables, sizeof(table_schema_def_t) * (schema->numTables + 1));
    if(schema->tables == NULL) {
        return kStatus_AllocError;
    }

    strncpy(schema->tables[schema->numTables].tableName, name, MAX_TABLE_NAME_SIZE);
    schema->tables[schema->numTables].numColumns = numColumns;
    schema->tables[schema->numTables].columns = columns;
    schema->tables[schema->numTables].tableId = schema->numTables;

    schema->numTables++;

    return kStatus_Success;
}

status_t SCHEMA_DestroyTableStructure(database_schema_t *schema, int index) {
    /*int i = 0;*/
    if(schema == NULL) {
        return kStatus_InvalidArgument;
    }

    if(index < 0 || index >= schema->numTables) {
        return kStatus_InvalidArgument;
    }

    /*if(schema->tables[index] != NULL) {
        free(schema->tables[index].columns);

        // Check if any remaining tables exist
        if(schema->numTables == 1) {
            free(schema->tables);
            schema->tables = NULL;
            schema->numTables = 0;
        } else {
            for(i = index; i < schema->numTables - 1; i++) {
                schema->tables[i] = schema->tables[i + 1];
            }
            schema->numTables--;
            schema->tables = (table_schema_def_t *)realloc(schema->tables, sizeof(table_schema_def_t) * schema->numTables);
        }
    }*/

    return kStatus_Success;
}

