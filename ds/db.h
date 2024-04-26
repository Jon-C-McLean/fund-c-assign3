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
} table_t;

typedef struct {
    database_schema_t *schema;
    table_t *tables;
} database_t;

status_t DB_CreateDefaultDatabase(database_t **db);
status_t DB_DestroyDatabase(database_t *db);

status_t DB_CreateTable(database_t *db, char *name, table_col_def_t *columns, int numColumns);
status_t DB_DropTable(database_t *db, char *name);

status_t DB_InsertRow(database_t *db, char *tableName, void **values);
status_t DB_DeleteRow(database_t *db, char *tableName, int rowId);

status_t DB_UpdateRow(database_t *db, char *tableName, int rowId, void **values);

status_t DB_SaveDatabase(database_t *db, char *filename, int compress, char *key, int keySize);
status_t DB_LoadFromDisk(database_t *db, char *filename, char *key, int keySize);

#endif
