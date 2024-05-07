/**
 * @file db.h
 * @author Jon McLean (13515869)
 */

#ifndef DB_H_
#define DB_H_

#include "schema.h"

typedef struct {
    char *data;
    
    int rows;
    int tableId;
    int rowSize;
} table_t;

typedef struct {
    database_schema_t *schema;
    table_t *tables;
} database_t;

/**
 * @brief Creates a new database with the default schema and table.
 * @author Jon McLean (13515869)
 * 
 * @param[inout] db Resulting database
 * @return status_t Status of the operation
 */
status_t DB_CreateDefaultDatabase(database_t **db);

/**
 * @brief Destroys a database and frees all memory associated with it.
 * @author Jon McLean (13515869)
 * 
 * @warning This operation will free all memory associated with the database
 * and the associated schema. This operation will not save the database to disk.
 * 
 * @param[inout] db The database to destroy.
 * @return status_t Status of the operation
 */
status_t DB_DestroyDatabase(database_t *db);

/**
 * @brief Creates a new table in the database with the specified columns.
 * @author Jon McLean (13515869)
 * 
 * @note This function will handle schema creation and updating.
 * 
 * @param[inout] db The database to create the table in.
 * @param[in] name The name of the table to create.
 * @param[in] columns The columns to create in the table.
 * @param[in] numColumns The number of columns in the table.
 * @return status_t Status of the operation
 */
status_t DB_CreateTable(database_t *db, char *name, table_col_def_t *columns, int numColumns);

/**
 * @brief Drops a table from the database.
 * @author Jon McLean (13515869)
 * 
 * @note This function will free all memory associated with the table and remove
 * it from the schema.
 * 
 * @param[inout] db The database to drop the table from.
 * @param[in] name The name of the table to drop.
 * @return status_t Status of the operation
 */
status_t DB_DropTable(database_t *db, char *name);

/**
 * @brief Inserts a row into the specified table.
 * @author Jon McLean (13515869)
 * 
 * @param[inout] db The database to insert the row into.
 * @param[in] tableId The ID of the table to insert the row into.
 * @param[in] values The values to insert into the row.
 * @return status_t Status of the operation
 */
status_t DB_InsertRow(database_t *db, int tableId, void *values);

/**
 * @brief Deletes a row from the specified table.
 * @author Jon McLean (13515869)
 * 
 * @param[inout] db The database to delete the row from.
 * @param[in] tableName The name of the table to delete the row from.
 * @param[in] rowId The ID of the row to delete.
 * @return status_t Status of the operation
 */
status_t DB_DeleteRow(database_t *db, char *tableName, int rowId);

/**
 * @brief Updates a row in the specified table.
 * @author Jon McLean (13515869)
 * 
 * @param[inout] db The database to update the row in.
 * @param[in] tableName The name of the table to update the row in.
 * @param[in] rowId The ID of the row to update.
 * @param[in] values The new values for the row.
 * @return status_t Status of the operation
 */
status_t DB_UpdateRow(database_t *db, char *tableName, int rowId, void **values);

/**
 * @brief Selects a row from the specified table.
 * @author Jon McLean (13515869)
 * 
 * @param[in] db The database to select the row from.
 * @param[in] tableName The name of the table to select the row from.
 * @param[in] rowId The ID of the row to select.
 * @param[out] values The values of the row.
 * @return status_t Status of the operation
 */
status_t DB_SelectRow(database_t *db, char *tableName, int rowId, void **values);

/* TODO: Search/select functions */

/**
 * @brief Saves the database to disk.
 * @author Jon McLean (13515869)
 * 
 * @param[in] db The database to save.
 * @param[in] filename The name of the file to save the database to.
 * @param[in] compress Whether or not to compress the database.
 * @param[in] key The key to encrypt the database with.
 * @param[in] keySize The size of the key (only supports 128 bit).
 * @return status_t Status of the operation
 */
status_t DB_SaveDatabase(database_t *db, char *filename, int compress, char *key, int keySize);

/**
 * @brief Loads a database from disk.
 * @author Jon McLean (13515869)
 * 
 * @param[inout] db The database to load the data into.
 * @param[in] filename The name of the file to load the database from.
 * @param[in] key The key to decrypt the database with.
 * @param[in] keySize The size of the key (only supports 128 bit).
 * @return status_t Status of the operation
 */
status_t DB_LoadFromDisk(database_t *db, char *filename, char *key, int keySize);

status_t DB_FindRowWithKey(database_t *db, int tableId, int key, int *index);

#endif
