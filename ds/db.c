/**
 * @file db.c
 * @author Jon McLean (13515869)
 */

#include "db.h"
#include <stdio.h>

const unsigned char MAGIC[] = {0x6A, 0x6F, 0x6E, 0x20, 0x64, 0x62, 0x0A};
const unsigned char ENC_CHECK_MAGIC[] = {'D', 'B', 'E', 'N', 'C'};

const unsigned char COMP_MAGIC[] = {'R', 'L', 'E'};
const unsigned char NORM_MAGIC[] = {'N', 'O', 'R'};

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
    DEBUG_PRINT("Destroying database: %s\n", db->schema->dbName);

    int i = 0;
    for(i = 0; i < db->schema->numTables; i++) {
        if(db->tables[i].data != NULL) free(db->tables[i].data);
    }

    SCHEMA_DestroyDatabaseSchema(db->schema);

    free(db);

    return kStatus_Success;
}

status_t DB_CreateTable(database_t *db, char *name, table_col_def_t *columns, int numColumns) {
    status_t result;
    int rowSize = 0;
    int i = 0;

    if(db == NULL || name == NULL || columns == NULL) return kStatus_InvalidArgument;

    DEBUG_PRINT("Creating table schema: %s\n", name);
    result = SCHEMA_DefineTableStructure(db->schema, name, columns, numColumns);

    if(result != kStatus_Success) return result;/* Break out if schema failed */

    DEBUG_PRINT("Table schema created\n");
    DEBUG_PRINT("Allocating memory for new table\n");
    if(db->tables == NULL) {
        DEBUG_PRINT("No existing tables, allocating memory\n");
        db->tables = (table_t *)malloc(sizeof(table_t));
        
        if(db->tables == NULL) { 
            /* Attempt to roll back schema creation */
            DEBUG_PRINT("Failed to allocate memory\n");
            (void)SCHEMA_DestroyTableStructure(db->schema, db->schema->numTables - 1);
            return kStatus_AllocError; 
        }
    } else {
        DEBUG_PRINT("Reallocating memory for to allow for new table\n");
        table_t *tableRealloc = (table_t *)realloc(db->tables, sizeof(table_t) * (db->schema->numTables));
        if(db->tables == NULL) { 
            /* Attempt to roll back schema creation */
            DEBUG_PRINT("Failed to reallocate memory\n");
            (void)SCHEMA_DestroyTableStructure(db->schema, db->schema->numTables - 1);
            return kStatus_AllocError;
        }
        
        db->tables = tableRealloc;
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

    DEBUG_PRINT("New Table Row Size: %d\n", rowSize);

    db->tables[db->schema->numTables-1].data = NULL;
    db->tables[db->schema->numTables-1].rows = 0;
    db->tables[db->schema->numTables-1].rowSize = rowSize;
    db->tables[db->schema->numTables-1].tableId = db->schema->numTables - 1;

    return kStatus_Success;
}

status_t DB_InsertRow(database_t *db, int tableId, void *values) {
    if(db == NULL || values == NULL) return kStatus_InvalidArgument;
    if(tableId > db->schema->numTables) return kStatus_Schema_UnknownTableId;

    DEBUG_PRINT("Inserting row into table: %d\n", tableId);

    if(db->tables[tableId].data == NULL) {
        DEBUG_PRINT("Allocating data region for table (no existing data region)\n");
        db->tables[tableId].data = (char *)malloc(db->tables[tableId].rowSize);
        if(db->tables[tableId].data == NULL) return kStatus_AllocError;
    } else {
        DEBUG_PRINT("Reallocating data region for new record\n");
        db->tables[tableId].data = (char *)realloc(db->tables[tableId].data, 
            (db->tables[tableId].rows+1) * db->tables[tableId].rowSize);
        if(db->tables[tableId].data == NULL) return kStatus_AllocError;
    }

    if(db->tables[tableId].data == NULL) return kStatus_AllocError;
    int offset = db->tables[tableId].rows * db->tables[tableId].rowSize;
    memcpy(db->tables[tableId].data + offset, (char *)values, db->tables[tableId].rowSize);

    db->tables[tableId].rows++;
    return kStatus_Success;
}

status_t DB_FindRowWithKey(database_t *db, int tableId, int key, int *index) {
    if(db == NULL || index == NULL) return kStatus_InvalidArgument;

    DEBUG_PRINT("Finding row with key: %d in table: %d\n", key, tableId);

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
            DEBUG_PRINT("Found row with key at index %d\n", i);
            *index = i;
            return kStatus_Success;
        }
    }

    DEBUG_PRINT("Failed to find row with key: %d in table: %d\n", key, tableId);

    *index = -1;
    return kStatus_Fail;
}

status_t DB_DeleteRow(database_t *db, int tableId, int rowId) {
    if(db == NULL) return kStatus_InvalidArgument;
    if(tableId < 0 || rowId < 0) return kStatus_InvalidArgument;

    if(tableId > db->schema->numTables) return kStatus_Schema_UnknownTableId;
    if(rowId > db->tables[tableId].rows) return kStatus_Fail;

    DEBUG_PRINT("Deleting row %d from table %d\n", rowId, tableId);

    char *newData = (char *)malloc(db->tables[tableId].rowSize * (db->tables[tableId].rows - 1));
    if(newData == NULL) return kStatus_AllocError;

    int i = 0;
    int dataIndex = 0;
    DEBUG_PRINT("Copying data to new memory region\n");
    for(i = 0; i < db->tables[tableId].rows; i++) {
        if(i == rowId) continue;

        (void)memcpy(newData + (dataIndex * db->tables[tableId].rowSize), 
            db->tables[tableId].data + (i * db->tables[tableId].rowSize), 
            db->tables[tableId].rowSize);
        dataIndex++;
    }

    DEBUG_PRINT("Freeing old data region\n");
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

    char *binarized = (char *)malloc(calcSize);
    if(binarized == NULL) return kStatus_AllocError;

    *data = binarized;
    *size = calcSize;
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

        memcpy(binarized + offset, &db->tables[i].rowSize, sizeof(db->tables[i].rowSize));
        offset += sizeof(db->tables[i].rowSize);

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

    DEBUG_PRINT("Opening file for writing: %s\n", filename);
    FILE *file = fopen(filename, "wb");
    if(file == NULL) return kStatus_Fail;

    DEBUG_PRINT("Writing magic number to file\n");
    (void)fwrite(MAGIC, sizeof(MAGIC), 1, file);

    char *binaryData;
    int binarySize;
    status_t result = DB_BuildBinaryData(db, &binaryData, &binarySize);
    int originalSize = binarySize;

    if(result != kStatus_Success) {
        (void)fclose(file);
        if(binaryData != NULL) free(binaryData);
        return result;
    }

    /* Compress and realloc */
    if(compress) {
        DEBUG_PRINT("Compressing data using RLE");
        char *compressedData;
        int compressedSize;
        RLE_Compress(binaryData, binarySize, &compressedData, &compressedSize);

        free(binaryData);
        binaryData = compressedData;
        binarySize = compressedSize;

        DEBUG_PRINT("Data compressed to %d bytes,"
             "writing compression magic to file\n", binarySize);

        (void)fwrite(COMP_MAGIC, sizeof(COMP_MAGIC), 1, file);
    } else {
        DEBUG_PRINT("Writing normal data to file\n");
        (void)fwrite(NORM_MAGIC, sizeof(NORM_MAGIC), 1, file);
    }

    /* Reallocate and append encryption magic at end */
    binarySize += sizeof(ENC_CHECK_MAGIC);
    binaryData = (char *)realloc(binaryData, binarySize);
    (void)memcpy(binaryData + binarySize - sizeof(ENC_CHECK_MAGIC), ENC_CHECK_MAGIC, sizeof(ENC_CHECK_MAGIC));

    /* Pad data to be multiple of 16 bytes */
    int paddingBytes = 0;
    if(binarySize % 16 != 0) {
        paddingBytes = 16 - (binarySize % 16);
        DEBUG_PRINT("Padding data with %d bytes\n", paddingBytes);
        binarySize += paddingBytes;
        binaryData = (char *)realloc(binaryData, binarySize);
    }

    aes_context_t context;
    unsigned char iv[16] = {0};
    if(key != NULL) {
        DEBUG_PRINT("Encrypting data using AES\n");
        DEBUG_PRINT("Generating random IV\n");
        RAND_GenerateInitVector(iv, 16);

        DEBUG_PRINT("Initializing AES context with user key\n");
        AES_InitContext(&context, (unsigned char *)key, iv);

        DEBUG_PRINT("Encrypting data\n");
        AES_Encrypt(&context, (unsigned char *)binaryData, binarySize);
    }

    DEBUG_PRINT("Writing data to file\n");
    /* Do compression and encryption here */
    int actualSize = binarySize - paddingBytes;
    fwrite(&paddingBytes, sizeof(paddingBytes), 1, file); /* Save padding bytes */
    fwrite(&actualSize, sizeof(binarySize), 1, file); /* Save actual size */
    fwrite(&originalSize, sizeof(originalSize), 1, file); /* Save original data size */
    fwrite(iv, sizeof(iv), 1, file); /* Save IV */
    (void)fwrite(binaryData, binarySize, 1, file);
    (void)fclose(file);

    free(binaryData);

    DEBUG_PRINT("Finished exporting database\n");

    return kStatus_Success;
}

status_t DB_LoadFromDisk(database_t **db, char *filename, char *key, int keySize) {
    if(db == NULL || filename == NULL) return kStatus_InvalidArgument;

    FILE *file = fopen(filename, "rb");
    if(file == NULL) return kStatus_Fail;

    unsigned char magic[7];
    (void)fread(magic, sizeof(magic), 1, file);

    if(memcmp(magic, MAGIC, sizeof(MAGIC)) != 0) {
        DEBUG_PRINT("Missing/bad magic number\n");
        /* Likely corrupted or not a valid DB */
        (void)fclose(file);
        return kStatus_IO_BadMagicNumber;
    }

    char compStatus[sizeof(COMP_MAGIC)];
    (void)fread(compStatus, sizeof(compStatus), 1, file);

    int isCompressed = 0;
    if(memcmp(compStatus, COMP_MAGIC, sizeof(COMP_MAGIC)) == 0) {
        DEBUG_PRINT("Data is compressed\n");
        isCompressed = 1;
    }

    int padding, binarySize, originalSize;
    (void)fread(&padding, sizeof(padding), 1, file);
    (void)fread(&binarySize, sizeof(binarySize), 1, file);
    (void)fread(&originalSize, sizeof(originalSize), 1, file);

    unsigned char iv[16];
    (void)fread(iv, sizeof(iv), 1, file);

    char *binaryData = (char *)malloc(binarySize);
    (void)fread(binaryData, binarySize, 1, file);

    if(memcmp(binaryData + binarySize - sizeof(ENC_CHECK_MAGIC), ENC_CHECK_MAGIC, sizeof(ENC_CHECK_MAGIC)) != 0) {
        DEBUG_PRINT("Data is encrypted\n");
        if(key == NULL) {
            DEBUG_PRINT("User encryption key is not provided\n");
            (void)fclose(file);
            free(binaryData);
            return kStatus_IO_MissingKey;
        }

        if(padding != 0) { /* TODO: Fixed unused result*/
            (void)realloc(binaryData, binarySize + padding);

            /* Read in padding bytes */
            (void)fread(binaryData + binarySize, padding, 1, file);
            binarySize += padding;
        }

        aes_context_t context;
        DEBUG_PRINT("Initializing AES context with user key\n");
        AES_InitContext(&context, (unsigned char *)key, iv);

        DEBUG_PRINT("Decrypting data\n");
        AES_Decrypt(&context, (unsigned char *)binaryData, binarySize);
        
        if(memcmp(binaryData + (binarySize - sizeof(ENC_CHECK_MAGIC) - padding), ENC_CHECK_MAGIC, sizeof(ENC_CHECK_MAGIC)) != 0) {
            DEBUG_PRINT("Failed to decrypt data, user key is likely invalid\n");
            (void)fclose(file);
            free(binaryData);
            return kStatus_IO_BadKey;
        }
    }

    if(isCompressed) {
        DEBUG_PRINT("Data is compressed, decompressing\n");
        char *decompressedData;
        int decompressedSize;
        RLE_Decompress(binaryData, binarySize, &decompressedData, &decompressedSize);

        free(binaryData);
        binaryData = decompressedData;
        binarySize = decompressedSize;
        DEBUG_PRINT("Data decompressed to %d bytes\n", binarySize);
    }

    int offset = 0;
    int numTables;
    char dbName[MAX_DB_NAME_SIZE];
    int i, j;

    *db = (database_t *)malloc(sizeof(database_t));

    memcpy(&numTables, binaryData + offset, sizeof(numTables));
    offset += sizeof(numTables);
    memcpy(dbName, binaryData + offset, sizeof(dbName));
    offset += sizeof(dbName);

    SCHEMA_CreateDatabaseSchema(&((*db)->schema), dbName);

    (*db)->tables = (table_t *)malloc(sizeof(table_t) * numTables);
    (*db)->schema->tables = (table_schema_def_t *)malloc(sizeof(table_schema_def_t) * numTables);
    (*db)->schema->numTables = numTables;
    strncpy((*db)->schema->dbName, dbName, sizeof(dbName));

    for(i = 0; i < numTables; i++) {
        table_schema_def_t *table = &((*db)->schema->tables[i]);
        table_t *tableData = &((*db)->tables[i]);
        if(table == NULL) {
            DEBUG_PRINT("Failed to allocate memory for table schema\n");
            (void)fclose(file);
            free(binaryData);
            (void)SCHEMA_DestroyDatabaseSchema((*db)->schema);
            free(*db);
            return kStatus_AllocError;
        }

        memcpy(table->tableName, binaryData + offset, sizeof(table->tableName));
        offset += sizeof(table->tableName);

        memcpy(&table->tableId, binaryData + offset, sizeof(table->tableId));
        offset += sizeof(table->tableId);

        memcpy(&table->numColumns, binaryData + offset, sizeof(table->numColumns));
        offset += sizeof(table->numColumns);

        memcpy(&tableData->rows, binaryData + offset, sizeof(tableData->rows));
        offset += sizeof(tableData->rows);

        memcpy(&tableData->rowSize, binaryData + offset, sizeof(tableData->rowSize));
        offset += sizeof(tableData->rowSize);

        table->columns = (table_col_def_t *)malloc(sizeof(table_col_def_t) * table->numColumns);
        if(table->columns == NULL) {
            DEBUG_PRINT("Failed to allocate memory for table columns\n");
            (void)fclose(file);
            free(binaryData);
            (void)SCHEMA_DestroyDatabaseSchema((*db)->schema);
            free(*db);
            return kStatus_AllocError;
        }

        for(j = 0; j < table->numColumns; j++) {
            memcpy(&(table->columns[j]), binaryData + offset, sizeof(table_col_def_t));
            offset += sizeof(table_col_def_t);
        }

        tableData->tableId = table->tableId;
        tableData->data = (char *)malloc(tableData->rows * tableData->rowSize);
        if(tableData->data == NULL) {
            DEBUG_PRINT("Failed to allocate memory for table data\n");
            (void)fclose(file);
            free(binaryData);
            (void)SCHEMA_DestroyDatabaseSchema((*db)->schema);
            free(*db);
            return kStatus_AllocError;
        }

        for(j = 0; j < tableData->rows; j++) {
            memcpy(tableData->data + (j * tableData->rowSize), binaryData + offset, tableData->rowSize);
            offset += tableData->rowSize;
        }
    }

    DEBUG_PRINT("Finished loading database\n");
    (void)free(binaryData);
    (void)fclose(file);

    return kStatus_Success;
}

status_t DB_DropTable(database_t *db, int tableId) {
    if(db == NULL) return kStatus_InvalidArgument;
    if(tableId > db->schema->numTables) return kStatus_Schema_UnknownTableId;
    DEBUG_PRINT("Dropping table: %d\n", tableId);

    SCHEMA_DestroyTableStructure(db->schema, tableId);

    if(db->tables[tableId].data != NULL) free(db->tables[tableId].data);
    
    if(db->schema->numTables == 0) {
        DEBUG_PRINT("No tables left in schema, freeing memory\n");
        free(db->tables);
        db->tables = NULL;
    } else {
        DEBUG_PRINT("Reallocating memory for tables\n");
        table_t *newTables = (table_t *)malloc(sizeof(table_t) * (db->schema->numTables));
        if(newTables == NULL) {
            DEBUG_PRINT("Failed to allocate new memory for remaining tables\n");
            return kStatus_AllocError; 
        }

        DEBUG_PRINT("Copying tables to new memory region\n");
        int i = 0;
        int dataIndex = 0;
        for(i = 0; i < db->schema->numTables+1; i++) {
            if(i == tableId) continue;

            newTables[dataIndex] = db->tables[i];
            newTables[dataIndex].tableId = dataIndex;
            dataIndex++;
        }

        DEBUG_PRINT("Freeing old memory region\n");
        free(db->tables);
        db->tables = newTables;
    }

    return kStatus_Success;
}

status_t DB_FindRowsWithColumnValue(database_t *db, int tableId, int columnId, void *value, int **indexes, int *numIndexes) {
    if(db == NULL || value == NULL || indexes == NULL || numIndexes == NULL) return kStatus_InvalidArgument;
    if(tableId > db->schema->numTables) return kStatus_Schema_UnknownTableId;

    DEBUG_PRINT("Finding rows in table %d with column %d matching value\n", tableId, columnId);
    table_schema_def_t *table = &(db->schema->tables[tableId]);
    if(columnId > table->numColumns) return kStatus_Schema_UnknownColumn;

    int i = 0;

    /* Allocate worst case result size */
    int *foundIndexes = (int *)malloc(sizeof(int) * db->tables[tableId].rows);
    if(foundIndexes == NULL) {
        DEBUG_PRINT("Failed to allocate memory for indexes\n");
        return kStatus_AllocError;
    }

    int offset = 0;
    for(i = 0; i < columnId; i++) {
        switch(table->columns[i].type) {
            case INT:
                offset += sizeof(int);
                break;
            case STRING:
                offset += table->columns[i].size;
                break;
            case KEY:
                exit(-3); /* XXX: Not implemented */
            default:
                return kStatus_Schema_UnknownColumn; /* XXX: Is this the right error? */
        }
    }

    table_t *tableData = &db->tables[tableId];
    int found = 0;
    for(i = 0; i < tableData->rows; i++) {
        switch(table->columns[columnId].type) {
            case INT:
                if(*(int *)(tableData->data + (i * tableData->rowSize) + offset) == *(int *)value) {
                    foundIndexes[found] = i;
                    found++;
                }
                break;
            case STRING:
                if(strncmp(tableData->data + (i * tableData->rowSize) + offset, (char *)value, table->columns[columnId].size) == 0) {
                    foundIndexes[found] = i;
                    found++;
                }
                break;
            case KEY:
                exit(-3); /* XXX: Not implemented */
            default:
                return kStatus_Schema_UnknownColumn; /* XXX: Is this the right error? */
        }
    }

    *indexes = foundIndexes;
    *numIndexes = found;

    return kStatus_Success;
}