/**
 * @file schema.c
 * @author Jon McLean (13515869)
 */

#include "schema.h"

status_t SCHEMA_CreateDatabaseSchema(database_schema_t **schema, char *dbName) {
    if(schema == NULL) {
        return kStatus_InvalidArgument;
    }

    *schema = (database_schema_t *)malloc(sizeof(database_schema_t));
    if(*schema == NULL) {
        return kStatus_AllocError;
    }

    memset((*schema)->dbName, 0, MAX_DB_NAME_SIZE);
    if(dbName != NULL) {
        strncpy((*schema)->dbName, dbName, MAX_DB_NAME_SIZE);
    }
    (*schema)->numTables = 0;
    (*schema)->tables = NULL;

    return kStatus_Success;

}

status_t SCHEMA_CreateDefaultDatabaseSchema(database_schema_t **schema) {
    char tableName[MAX_TABLE_NAME_SIZE] = "Employee";
    table_col_def_t *employeeColumns;
    
    status_t result = SCHEMA_CreateDatabaseSchema(schema, NULL);
    if(result != kStatus_Success) return result;
    
    employeeColumns = malloc(sizeof(table_col_def_t) * 3);
    memset(employeeColumns, 0, sizeof(table_col_def_t) * 3);

    strncpy(employeeColumns[0].columnName, "EmployeeID", MAX_COLUMN_NAME_SIZE);
    employeeColumns[0].type = INT;
    employeeColumns[0].size = sizeof(int);
    employeeColumns[0].isPrimaryKey = 1;

    strncpy(employeeColumns[1].columnName, "Name", MAX_COLUMN_NAME_SIZE);
    employeeColumns[1].type = STRING;
    employeeColumns[1].size = 100;
    employeeColumns[1].isPrimaryKey = 0;

    strncpy(employeeColumns[2].columnName, "Date of Birth", MAX_COLUMN_NAME_SIZE);
    employeeColumns[2].type = STRING;
    employeeColumns[2].size = 12;
    employeeColumns[2].isPrimaryKey = 0;

    return SCHEMA_DefineTableStructure(*schema, tableName, employeeColumns, 3);
}

status_t SCHEMA_DestroyDatabaseSchema(database_schema_t *schema) {
    if(schema == NULL) {
        return kStatus_InvalidArgument;
    }

    DEBUG_PRINT(("Destroying schema\n"));
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

    DEBUG_PRINT(("Defining table structure\n"));
    DEBUG_PRINT(("Reallocating table schema array to %d\n", schema->numTables + 1));
    schema->tables = (table_schema_def_t *)realloc(schema->tables, sizeof(table_schema_def_t) * (schema->numTables + 1));
    if(schema->tables == NULL) {
        DEBUG_PRINT(("Failed to reallocate table schema array\n"));
        return kStatus_AllocError;
    }

    strncpy(schema->tables[schema->numTables].tableName, name, MAX_TABLE_NAME_SIZE);
    schema->tables[schema->numTables].numColumns = numColumns;
    schema->tables[schema->numTables].columns = columns;
    schema->tables[schema->numTables].tableId = schema->numTables;

    schema->numTables++;
    DEBUG_PRINT(("New table count: %d\n", schema->numTables));

    return kStatus_Success;
}

status_t SCHEMA_DestroyTableStructure(database_schema_t *schema, int index) {
    int i = 0;
    if(schema == NULL) {
        return kStatus_InvalidArgument;
    }

    if(index < 0 || index >= schema->numTables) {
        return kStatus_InvalidArgument;
    }

    if(schema->tables[index].columns != NULL) {
        free(schema->tables[index].columns);

        if(schema->numTables == 1) {
            DEBUG_PRINT(("Freeing table schema array\n"));
            free(schema->tables);
            schema->tables = NULL;
            schema->numTables = 0;
        } else {
            DEBUG_PRINT(("Shifting table schema array\n"));
            for(i = index; i < schema->numTables - 1; i++) {
                schema->tables[i] = schema->tables[i + 1];
                schema->tables[i].tableId = i;
            }
            schema->numTables--;
            DEBUG_PRINT(("Reallocating table schema array to %d\n", schema->numTables));
            schema->tables = (table_schema_def_t *)realloc(schema->tables, sizeof(table_schema_def_t) * schema->numTables);
        }
    }

    return kStatus_Success;
}

status_t SCHEMA_GetTableForId(database_schema_t *schema, int tableId, table_schema_def_t **table) { 
    if(schema == NULL || table == NULL) {
        return kStatus_InvalidArgument;
    }

    if(tableId < 0 || tableId >= schema->numTables) {
        return kStatus_Schema_UnknownTableId;
        DEBUG_PRINT(("Unknown table ID\n"));
    }

    *table = &schema->tables[tableId];

    return kStatus_Success;
}

status_t SCHEMA_GetTableIdForName(database_schema_t *schema, char *name, int *tableId) {
    int i = 0;
    if(schema == NULL || name == NULL || tableId == NULL) {
        return kStatus_InvalidArgument;
    }

    for(i = 0; i < schema->numTables; i++) {
        if(strcmp(schema->tables[i].tableName, name) == 0) {
            *tableId = i;
            return kStatus_Success;
        }
    }

    DEBUG_PRINT(("Table not found for name: %s\n", name));

    *tableId = -1;
    return kStatus_Schema_UnknownTableId;
}

status_t SCHEMA_AddColumn(database_schema_t *schema, int tableId, char *name, column_type_t type, int maxSize, int isPrimaryKey) {
    table_schema_def_t *table;
    table_col_def_t *columns;
    if(schema == NULL || name == NULL) {
        return kStatus_InvalidArgument;
    }

    if(tableId < 0 || tableId >= schema->numTables) {
        DEBUG_PRINT(("Unknown table ID\n"));
        return kStatus_Schema_UnknownTableId;
    }

    table = &schema->tables[tableId];
    columns = (table_col_def_t *)realloc(table->columns, sizeof(table_col_def_t) * (table->numColumns + 1));
    if(columns == NULL) {
        DEBUG_PRINT(("Failed to reallocate columns array to include new column\n"));
        return kStatus_AllocError;
    }

    strncpy(columns[table->numColumns].columnName, name, MAX_COLUMN_NAME_SIZE);
    columns[table->numColumns].type = type;
    columns[table->numColumns].size = maxSize;
    columns[table->numColumns].isPrimaryKey = isPrimaryKey;

    table->columns = columns;
    table->numColumns++;
    DEBUG_PRINT(("New column count: %d\n", table->numColumns));

    return kStatus_Success;
}

status_t SCHEMA_GetColumnForName(table_schema_def_t *table, char *name, table_col_def_t **column) {
    int i = 0;
    if(table == NULL || name == NULL || column == NULL) {
        return kStatus_InvalidArgument;
    }

    for(i = 0; i < table->numColumns; i++) {
        if(strcmp(table->columns[i].columnName, name) == 0) {
            *column = &table->columns[i];
            return kStatus_Success;
        }
    }

    DEBUG_PRINT(("Column not found for name: %s\n", name));

    return kStatus_Schema_UnknownColumn;
}

status_t SCHEMA_GetIDForColumn(table_schema_def_t *table, char *name, int *columnId) {
    int i = 0;
    if(table == NULL || name == NULL || columnId == NULL) {
        return kStatus_InvalidArgument;
    }

    for(i = 0; i < table->numColumns; i++) {
        if(strcmp(table->columns[i].columnName, name) == 0) {
            *columnId = i;
            return kStatus_Success;
        }
    }

    DEBUG_PRINT(("Column not found for name: %s\n", name));

    *columnId = -1;
    return kStatus_Schema_UnknownColumn;
}