/**
 * @file schema.h
 * @author Jon McLean (13515869)
 */

#ifndef SCHEMA_H_
#define SCHEMA_H_

#include <stdlib.h>
#include <string.h>

#include "../utils/errors.h"

#define MAX_DB_NAME_SIZE 100U
#define MAX_TABLE_NAME_SIZE 100U
#define MAX_COLUMN_NAME_SIZE 100U
#define MAX_TABLES 25U

/* Schema used to define a user-defined table's structure and metadata */

typedef enum {
    INT,
    DECIMAL,
    STRING,
    KEY,
} column_type_t;

typedef struct __attribute__((__packed__)) {
    char columnName[MAX_COLUMN_NAME_SIZE];
    column_type_t type;
    int size;
    int isPrimaryKey; /* Key which will be used for searching and indexing */
} table_col_def_t;

typedef struct __attribute__((__packed__)) {
    char tableName[MAX_TABLE_NAME_SIZE];
    int numColumns;
    table_col_def_t *columns;
    int tableId; /* Used to link data to schema, this should be autogenerated */
} table_schema_def_t;

typedef struct __attribute__((__packed__)) {
    int numTables;
    table_schema_def_t *tables;

    char dbName[MAX_DB_NAME_SIZE];
} database_schema_t;

/**
 * @brief Initializes a database schema with the default options
 * 
 * @warning This operation will allocate memory. It must be destroyed 
 * correctly using SCHEMA_DestroyDatabaseSchema
 * 
 * @param[inout] schema Reference to the database schema to initialize
 * @return status_t Status of the operation
 */
status_t SCHEMA_CreateDefaultDatabaseSchema(database_schema_t **schema);

/**
 * @brief Frees the memory allocated for the database schema.
 * 
 * @param[inout] schema The database schema to free
 * @return status_t Status of the operation
 */
status_t SCHEMA_DestroyDatabaseSchema(database_schema_t *schema);

/**
 * @brief Get the table ID for a given table name
 * 
 * @param[in] schema Database schema
 * @param[in] name Name for which to get the table ID
 * @param[out] tableId Resulting table ID (may be NULL)
 * @return status_t Status of the operation
 */
status_t SCHEMA_GetTableIdForName(database_schema_t *schema, char *name, int *tableId);

/**
 * @brief Get the table schema for a given table ID
 * 
 * @param[in] schema Database schema
 * @param[in] tableId table ID for the desired table
 * @param[out] table Resulting table schema (may be NULL)
 * @return status_t Status of the operation
 */
status_t SCHEMA_GetTableForId(database_schema_t *schema, int tableId, table_schema_def_t **table);

/**
 * @brief Creates and allocates a new table in the database schema
 * 
 * @warning This operation will allocate memory. It must be destroyed
 * correctly using SCHEMA_DestroyTableStructure
 * 
 * @param[inout] schema Database schema to add the table to
 * @param name Name of the new table
 * @param columns Columns for the new table
 * @param numColumns Number of columns in the table
 * @return status_t Status of the operation
 */
status_t SCHEMA_DefineTableStructure(database_schema_t *schema, char *name, table_col_def_t *columns, int numColumns);

/**
 * @brief Deletes a table from the database schema
 * 
 * @param[inout] schema Database schema to delete the table from
 * @param[inout] index Index of the table to delete
 * @return status_t Status of the operation
 */
status_t SCHEMA_DestroyTableStructure(database_schema_t *schema, int index);

/**
 * @brief Get the column for a given column name in a table
 * 
 * @param[in] table Table schema to search
 * @param[in] name Name of the column to search for
 * @param[inout] column The resulting column (may be NULL)
 * @return status_t Status of the operation
 */
status_t SCHEMA_GetColumnForName(table_schema_def_t *table, char *name, table_col_def_t **column);

/**
 * @brief Adds a column to a table schema
 * 
 * @param[inout] schema Database schema containing the table
 * @param[in] tableId ID of the table to add the column to
 * @param[in] name Column name
 * @param[in] type Data type of the column
 * @param[in] maxSize Maximum size of the column (any data will be padded/truncated to this size)
 * @param[in] isPrimaryKey Whether the column is the primary key of the table
 * @return status_t Status of the operation
 */
status_t SCHEMA_AddColumn(database_schema_t *schema, int tableId, char *name, column_type_t type, int maxSize, int isPrimaryKey);

/**
 * @brief Deletes a column from a table schema
 * 
 * @param[inout] schema Schema containing the table
 * @param tableId ID of the table to delete the column from
 * @param name Name of the targeted column
 * @return status_t Status of the operation
 */
status_t SCHEMA_DeleteColumn(database_schema_t *schema, int tableId, char *name);

#endif