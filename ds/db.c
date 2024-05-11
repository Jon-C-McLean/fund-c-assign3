/**
 * @file db.c
 * @author Jon McLean (13515869)
 */

#include "db.h"
#include <stdio.h>

const unsigned char MAGIC[] = {0x6A, 0x6F, 0x6E, 0x20, 0x64, 0x62, 0x0A};

status_t DB_CreateDefaultDatabase(database_t **db) {
    database_schema_t *schema;
    status_t result;
    int i, rowSize = 0;
    table_schema_def_t *table;

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
    

    table = &((*db)->schema->tables[0]);
    for(i = 0; i < table->numColumns; i++) {
        switch(table->columns[i].type) {
            case INT:
                rowSize += sizeof(int);
                break;
            case STRING:
                rowSize += table->columns[i].size;
                break;
            case KEY:
                exit(-3); /* XXX: Not implemented */
            default:
                return kStatus_Schema_UnknownColumn; /* XXX: Is this the right error? */
        }
    }
    (*db)->tables[0].rowSize = rowSize;

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
    int rowSize = 0;
    int i = 0;

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
    
    table_schema_def_t *table = &(db->schema->tables[db->schema->numTables-1]);
    for(i = 0; i < table->numColumns; i++) {
        switch(table->columns[i].type) {
            case INT:
                rowSize += sizeof(int);
                break;
            case STRING:
                rowSize += table->columns[i].size;
                break;
            case KEY:
                exit(-3); /* XXX: Not implemented */
            default:
                return kStatus_Schema_UnknownColumn; /* XXX: Is this the right error? */
        }
    }

    tables[db->schema->numTables].data = NULL;
    tables[db->schema->numTables].rows = 0;
    tables[db->schema->numTables].rowSize = rowSize;
    tables[db->schema->numTables].tableId = db->schema->numTables;

    db->tables = tables;

    return kStatus_Success;
}

status_t DB_InsertRow(database_t *db, int tableId, void *values) {
    if(db == NULL || values == NULL) return kStatus_InvalidArgument;
    if(tableId > db->schema->numTables) return kStatus_Schema_UnknownTableId;

    if(db->tables[tableId].data == NULL) {
        db->tables[tableId].data = (char *)malloc(db->tables[tableId].rowSize);
    } else {
        db->tables[tableId].data = (char *)realloc(db->tables[tableId].data, 
            db->tables[tableId].rows * db->tables[tableId].rowSize);
    }

    if(db->tables[tableId].data == NULL) return kStatus_AllocError;
    int offset = db->tables[tableId].rows * db->tables[tableId].rowSize;
    memcpy(db->tables[tableId].data + offset, (char *)values, db->tables[tableId].rowSize);

    db->tables[tableId].rows++;
    return kStatus_Success;
}

status_t DB_FindRowWithKey(database_t *db, int tableId, int key, int *index) {
    if(db == NULL || index == NULL) return kStatus_InvalidArgument;

    status_t result;
    table_schema_def_t *table = NULL;
    int i = 0;
    if((result = SCHEMA_GetTableForId(db->schema, tableId, &table)) != kStatus_Success) {
        return result;
    }

    int keyOffset = 0; /* The primary key's data offset in memory */

    for(i = 0; i < table->numColumns; i++) {
        if(table->columns[i].isPrimaryKey) {
            break;
        }

        keyOffset += table->columns[i].size;
    }

    for(i = 0; i < db->tables[tableId].rows; i++) {
        if(*(int *)(db->tables[tableId].data + (i * db->tables[tableId].rowSize) + keyOffset) == key) {
            *index = i;
            return kStatus_Success;
        }
    }

    *index = -1;
    return kStatus_Fail;
}

status_t DB_DeleteRow(database_t *db, int tableId, int rowId) {
    if(db == NULL) return kStatus_InvalidArgument;
    if(tableId < 0 || rowId < 0) return kStatus_InvalidArgument;

    if(tableId > db->schema->numTables) return kStatus_Schema_UnknownTableId;
    if(rowId > db->tables[tableId].rows) return kStatus_Fail;

    char *newData = (char *)malloc(db->tables[tableId].rowSize * (db->tables[tableId].rows - 1));
    if(newData == NULL) return kStatus_AllocError;

    int i = 0;
    int dataIndex = 0;
    for(i = 0; i < db->tables[tableId].rows; i++) {
        if(i == rowId) continue;

        (void)memcpy(newData + (dataIndex * db->tables[tableId].rowSize), 
            db->tables[tableId].data + (i * db->tables[tableId].rowSize), 
            db->tables[tableId].rowSize);
        dataIndex++;
    }

    free(db->tables[tableId].data);
    db->tables[tableId].data = newData;
    db->tables[tableId].rows--;

    return kStatus_Success;
}

status_t DB_BuildBinaryData(database_t *db, char **data, int *size) {
    if(db == NULL || data == NULL || size == NULL) return kStatus_InvalidArgument;

    /* 
     * 1. Calculate binary size
     * 2. Allocate memory
     * 3. Write binarized schema
     * 4. Write binarized data
     * 5. Return
    */
   int calcSize = 0, i = 0, j = 0;

    /* Calculate binary size */
    calcSize += sizeof(db->schema->numTables); /* Number of tables */
    calcSize += sizeof(db->schema->dbName);
    
    for(i = 0; i < db->schema->numTables; i++) {
        calcSize += sizeof(db->schema->tables[i].tableId);
        calcSize += MAX_TABLE_NAME_SIZE;
        calcSize += sizeof(db->schema->tables[i].numColumns);

        for(j = 0; j < db->schema->tables[i].numColumns; j++) {
            calcSize += MAX_COLUMN_NAME_SIZE;
            calcSize += sizeof(column_type_t);
            calcSize += sizeof(db->schema->tables[i].columns[j].isPrimaryKey);
            calcSize += sizeof(db->schema->tables[i].columns[j].size);
        }

        calcSize += db->tables[i].rows * db->tables[i].rowSize;
        calcSize += sizeof(db->tables[i].tableId);
        calcSize += sizeof(db->tables[i].rowSize);
        calcSize += sizeof(db->tables[i].rows);
    }

    printf("Calculated size: %d", calcSize);

    char *binarized = (char *)malloc(calcSize);
    if(binarized == NULL) return kStatus_AllocError;

    *data = binarized;
    int offset = 0;
    
    memcpy(binarized + offset, &db->schema->numTables, sizeof(db->schema->numTables));
    offset += sizeof(db->schema->numTables);

    memcpy(binarized + offset, db->schema->dbName, sizeof(db->schema->dbName));
    offset += sizeof(db->schema->dbName);

    for(i = 0; i < db->schema->numTables; i++) {
        table_schema_def_t *table = &(db->schema->tables[i]);

        memcpy(binarized + offset, table->tableName, sizeof(table->tableName));
        offset += sizeof(table->tableName);

        memcpy(binarized + offset, &table->tableId, sizeof(table->tableId));
        offset += sizeof(table->tableId);

        memcpy(binarized + offset, &table->numColumns, sizeof(table->numColumns));
        offset += sizeof(table->numColumns);

        memcpy(binarized + offset, &db->tables[i].rows, sizeof(db->tables[i].rows));
        offset += sizeof(db->tables[i].rows);

        for(j = 0; j < table->numColumns; j++) {
            memcpy(binarized + offset, &(table->columns[j]), sizeof(table_col_def_t));
            offset += sizeof(table_col_def_t);
        }

        for(j = 0; j < db->tables[i].rows; j++) {
            memcpy(binarized + offset, db->tables[i].data + (j * db->tables[i].rowSize), db->tables[i].rowSize);
            offset += db->tables[i].rowSize;
        }
    }

    return kStatus_Success;
}

status_t DB_SaveDatabase(database_t *db, char *filename, int compress, char *key, int keySize) {
    if(db == NULL || filename == NULL) return kStatus_InvalidArgument;

    FILE *file = fopen(filename, "wb");
    if(file == NULL) return kStatus_Fail;

    (void)fwrite(MAGIC, sizeof(MAGIC), 1, file);

    char *binaryData;
    int binarySize;
    status_t result = DB_BuildBinaryData(db, &binaryData, &binarySize);
    if(result != kStatus_Success) {
        (void)fclose(file);
        if(binaryData != NULL) free(binaryData);
        return result;
    }

    /* Do compression and encryption here */

    (void)fwrite(binaryData, binarySize, 1, file);
    (void)fclose(file);


    return kStatus_Success;
}