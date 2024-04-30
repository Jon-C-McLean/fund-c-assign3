/**
 * @file db.c
 * @author Jon McLean (13515869)
 */

#include "db.h"

status_t DB_CreateDefaultDatabase(database_t **db) {
    database_schema_t *schema;
    status_t result;

    if(db == NULL) return kStatus_InvalidArgument;

    *db = (database_t *)malloc(sizeof(database_t));

    result = SCHEMA_CreateDefaultDatabaseSchema(&schema);
    if(result != kStatus_Success) {
        if(*db != NULL) free(*db);
        return result;
    }

    (*db)->schema = schema;

    /* Create initial table for corresponding schema (i.e. index 0) */
    (*db)->tables = (table_t *)malloc(sizeof(table_t)); /* XXX: Debug will need to be removed */
    (*db)->tables[0].data = NULL;
    (*db)->tables[0].rows = 0;
    (*db)->tables[0].tableId = (*db)->schema->numTables-1; /* Table ID is 0 */

    return kStatus_Success;
}

status_t DB_DestroyDatabase(database_t *db) {
    if(db == NULL) return kStatus_InvalidArgument;

    SCHEMA_DestroyDatabaseSchema(db->schema);
    free(db);

    return kStatus_Success;
}

status_t DB_CreateTable(database_t *db, char *name, table_col_def_t *columns, int numColumns) {
    table_t *tables;
    status_t result;

    if(db == NULL || name == NULL || columns == NULL) return kStatus_InvalidArgument;

    result = SCHEMA_DefineTableStructure(db->schema, name, columns, numColumns);

    if(result != kStatus_Success) return result;/* Break out if schema failed */

    if(db->tables == NULL) {
        tables = (table_t *)malloc(sizeof(table_t));
        
        if(tables == NULL) { 
            /* Attempt to roll back schema creation */
            (void)SCHEMA_DestroyTableStructure(db->schema, db->schema->numTables - 1);
            return kStatus_AllocError; 
        }
    } else {
        tables = (table_t *)realloc(db->tables, sizeof(table_t) * (db->schema->numTables));
        if(tables == NULL) { 
            /* Attempt to roll back schema creation */
            (void)SCHEMA_DestroyTableStructure(db->schema, db->schema->numTables - 1);
            return kStatus_AllocError ;
        }
    }

    tables[db->schema->numTables].data = NULL;
    tables[db->schema->numTables].rows = 0;
    tables[db->schema->numTables].tableId = db->schema->numTables;

    db->tables = tables;
    db->schema->numTables++;

    return kStatus_Success;
}