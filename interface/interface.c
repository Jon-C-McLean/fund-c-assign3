/**
 * @file interface.c
 * @author Jon McLean (13515869)
 */
#include "interface.h"

void GUI_PrintWelcome() {
    SCREEN_ClearScreen();
    SCREEN_Print(" _______         _______  .______   \n", kColor_Red);
    SCREEN_Print("|   ____|       |       \\ |   _  \\  \n", kColor_Red);
    SCREEN_Print("|  |__    ______|  .--.  ||  |_)  | \n", kColor_Red);
    SCREEN_Print("|   __|  |______|  |  |  ||   _  <  \n", kColor_Red);
    SCREEN_Print("|  |____        |  '--'  ||  |_)  | \n", kColor_Red);
    SCREEN_Print("|_______|       |_______/ |______/  \n", kColor_Red);
    SCREEN_Print("\n", kColor_Red);
    SCREEN_Print("Welcome to E-DB, the customisable employee database system!\n", kColor_Red);
    SCREEN_Print("Written by Jon McLean (other members failed to contribute)\n", kColor_Red);
    SCREEN_Print("===============================================================\n", kColor_Red);
    SCREEN_Print("\n\n\n", kColor_Red);
}

void GUI_PrintMainMenu() {
    printf("What would you like to do?\n");
    printf("===========================\n");
    printf("1. Load Database\n");
    printf("2. Create Database\n");
    printf("3. Data Operations\n");
    printf("4. Schema Management\n");
    printf("5. Save Database and Exit \n");
    printf("6. Exit\n");
}

int GUI_GetOptionSelection(int min, int max, char* prompt) {
    int value = 0;
    while(1) {
        SCREEN_PrintInput(prompt);
        printf("\n");
        INPUT_GetInteger(&value);
        if(value >= min && value <= max) {
            return value;
        } else {
            SCREEN_PrintError("Invalid selection, please try again.\n");
        }
    }

    return -1;
}
/* Startup Operations */
database_t* GUI_LoadDatabase(void) {
    status_t result;
    database_t *db = NULL;
    int selection;
    char fileName[256];
    char key[AES_KEY_SIZE];
    
    SCREEN_ClearScreen();

    SCREEN_PrintInput("Enter the name of the file you want to load the DB from: \n> ");
    INPUT_GetString(fileName, 256);

    printf("Please select whether this database is encrypted\n");
    printf("1. Encrypted\n");
    printf("2. Plain\n");

    selection = GUI_GetOptionSelection(1, 2, "Please select an option (1-2): ");
    if(selection == -1) {
        SCREEN_PrintError("An internal error has occured, please try agian\n");
        return NULL;
    }

    memset(key, 0, sizeof(key));
    if(selection == 1) {
        SCREEN_PrintInput("Enter the key to use for decryption: \n> ");
        INPUT_GetString(key, AES_KEY_SIZE);
    }

    result = DB_LoadFromDisk(&db, fileName, selection == 1 ? key : NULL, sizeof(key));
    if(result != kStatus_Success) {
        SCREEN_PrintError("Error loading database\n");
        return NULL;
    }

    return db;
}

status_t GUI_SaveDatabase(database_t *db) {
    int compress, encrypt, selection;
    char fileName[256];
    char key[AES_KEY_SIZE];
    status_t result;

    SCREEN_ClearScreen();
    if(db == NULL) {
        SCREEN_PrintError("No database loaded, please load or create a database\n");
        return kStatus_Fail;
    }

    SCREEN_PrintInput("Enter the name of the file you want to save the DB to: \n> ");
    INPUT_GetString(fileName, 256);

    printf("Please select whether you want compression and encryption\n");
    printf("1. Encryption and Compression \n");
    printf("2. Compression\n");
    printf("3. None (Plain Text)\n");

    selection = GUI_GetOptionSelection(1, 3, "Please select an option (1-3): ");
    if(selection == -1) {
        SCREEN_PrintError("An internal error has occured, please try agian\n");
        return kStatus_Fail;
    }

    compress = selection != 3;
    encrypt = selection == 1;

    memset(key, 0, sizeof(key));
    if(encrypt) {
        SCREEN_PrintInput("Enter the key to use for encryption: \n> ");
        INPUT_GetString(key, AES_KEY_SIZE);
    }

    result = DB_SaveDatabase(db, fileName, compress, encrypt ? key : NULL, sizeof(key));
    if(result != kStatus_Success) {
        SCREEN_PrintError("Error saving database\n");
    }

    return result;
}

database_t* GUI_CreateDefaultDatabase(void) {
    database_t *db;
    status_t status;
    SCREEN_ClearScreen();
    printf("Database Creator\n");
    printf("================\n");

    printf("Setting up default database...\n");
    status = DB_CreateDefaultDatabase(&db);
    if(status != kStatus_Success) {
        printf("Error creating default database\n");
        return NULL;
    }


    SCREEN_PrintInput("Database name: \n>");
    INPUT_GetString(db->schema->dbName, MAX_DB_NAME_SIZE);

    printf("Database created successfully\n");
    return db;
}

/* General Operations */
void GUI_DataOperationsLoop(database_t *db) {
    if(db == NULL) {
        SCREEN_PrintError("No database loaded, please load or create a database\n");
        return;
    }

    SCREEN_ClearScreen();
    printf("Data Operations\n");
    printf("===============\n");
    GUI_PrintDataOperationsMenu(db);

    while(1) {
        int selection = GUI_GetOptionSelection(1, 7, "Please select an option (1-7): ");
        status_t status = kStatus_Success;

        switch(selection) {
            case 1: 
                (void)GUI_ListTables(db);

                SCREEN_ClearScreen();
                break;
            case 2:
                (void)GUI_DisplayTable(db, NULL, 1);

                SCREEN_ClearScreen();
                break;
            case 3:
                if((status = GUI_CreateRecordForTable(db)) != kStatus_Success) {
                } else {
                    SCREEN_ClearScreen();
                }

                break;
            case 4:
                if((status = GUI_UpdateRecordForTable(db)) != kStatus_Success && status != kStatus_Fail) {
                } else if (status == kStatus_Success) {
                    SCREEN_ClearScreen();
                }

                break;
            case 5:
                if((status = GUI_RemoveRowForTable(db)) != kStatus_Success) {
                    SCREEN_PrintError("An error occured deleting \
                        the record, please try again\n");
                } else {
                    SCREEN_ClearScreen();
                }

                break;
            case 6:
                if((status = GUI_FindRowsForTable(db)) != kStatus_Success) {
                    SCREEN_PrintError("An error occured finding "
                        "the record, please try again\n");
                } else {
                    SCREEN_ClearScreen();
                }
                break;
            case -1:
                SCREEN_PrintError("An internal error has occured, "
                    "please try agian\n");
                return;
            case 7: 
                /* Return to main menu*/
                SCREEN_ClearScreen();
                return;
        }

        printf("Data Operations\n");
        printf("===============\n");
        GUI_PrintDataOperationsMenu(db);
    }
}

void GUI_PrintDataOperationsMenu(database_t *db) {
    printf("1. List Tables\n");
    printf("2. Display Table\n");
    printf("3. Create Record\n");
    printf("4. Update Record\n");
    printf("5. Delete Record\n");
    printf("6. Find Record(s)\n");
    printf("7. Return to Main Menu\n");
}

void GUI_ListTables(database_t *db) {
    int i = 0;
    char lineBuffer[MAX_TABLE_NAME_SIZE+1] = {'\0'};
    SCREEN_ClearScreen();
    printf("TableID | %-*s |\n", MAX_TABLE_NAME_SIZE, "Tables");

    memset(lineBuffer, '-', MAX_TABLE_NAME_SIZE);
    printf("--------|-%s-|\n", lineBuffer);

    for(i = 0; i < db->schema->numTables; i++) {
        printf("%7d | %-*s |\n", i, MAX_TABLE_NAME_SIZE, db->schema->tables[i].tableName);

        memset(lineBuffer, '-', MAX_TABLE_NAME_SIZE);
        printf("--------|-%s-|\n", lineBuffer);
    }

    INPUT_WaitForAnyKey("\n\nPress any key to return to menu");
    
}

status_t GUI_DisplayTable(database_t *db, char *inputName, int showPrompt) {
    int i = 0, j = 0, length;
    int totalWidth = 0, columnOffset, columnWidth, tableId;
    table_schema_def_t *table = NULL;
    char tableName[MAX_TABLE_NAME_SIZE];
    char *lineBuffer;
    status_t result;
    if(db == NULL ) return kStatus_InvalidArgument;
    SCREEN_ClearScreen();

    /* Get Table Name */
    tableId = 0;
    if(inputName == NULL) {
        while(1) {
            SCREEN_PrintInput("Enter the name of the table you wish to display records for: \n> ");
            length = INPUT_GetString(tableName, MAX_TABLE_NAME_SIZE);

            if (length > 0) break;
        }

        result = SCHEMA_GetTableIdForName(db->schema, tableName, &tableId);
        if(result != kStatus_Success) return result;
        if(tableId == -1) return kStatus_Schema_UnknownTableId;   
    } else {
        result = SCHEMA_GetTableIdForName(db->schema, inputName, &tableId);
        if(result != kStatus_Success) return result;
        if(tableId == -1) return kStatus_Schema_UnknownTableId;
    }

    result = SCHEMA_GetTableForId(db->schema, tableId, &table);
    if(result != kStatus_Success) return result;

    /* Display Table */
    printf("\n\n");
    for(i = 0; i < table->numColumns; i++) {
        columnWidth = max(strlen(table->columns[i].columnName), table->columns[i].size);
        totalWidth += columnWidth + 3;
        printf("%-*s | ", columnWidth, table->columns[i].columnName);
    }
    printf("\n");
    lineBuffer = (char *)malloc(totalWidth+1);
    memset(lineBuffer, '-', totalWidth);
    lineBuffer[totalWidth] = '\0';

    for(i = 0; i < db->tables[tableId].rows; i++) {
        printf("%s\n", lineBuffer);
        columnOffset = 0;
        for(j = 0; j < table->numColumns; j++) {
            columnWidth = max(strlen(table->columns[j].columnName), table->columns[j].size);
            switch(table->columns[j].type) {
                case INT:
                    printf("%-*d | ", columnWidth, *((int *)(db->tables[tableId].data + (i * db->tables[tableId].rowSize) + columnOffset)));
                    columnOffset += sizeof(int);
                    break;
                case STRING:
                    printf("%-*s | ", columnWidth, db->tables[tableId].data + (i * db->tables[tableId].rowSize) + columnOffset);
                    columnOffset += table->columns[j].size;
                    break;
                default:
                    return kStatus_Schema_UnknownError;
            }
        }
        printf("\n");
    }
    printf("%s\n", lineBuffer);

    if(showPrompt) {
        INPUT_WaitForAnyKey("\n\nPress any key to return to menu");
    }

    free(lineBuffer);

    return kStatus_Success;
}

status_t GUI_CreateRecordForTable(database_t *db) {
    int offset = 0, i = 0, length, tableId, index; 
    char tableName[MAX_TABLE_NAME_SIZE];
    table_schema_def_t *table = NULL;
    char *data;
    status_t result;
    if(db == NULL ) return kStatus_InvalidArgument;

    SCREEN_ClearScreen();

    /* Get Table Name */
    while(1) {
        SCREEN_PrintInput("Enter the name of the table you wish to create a record for: \n> ");
        length = INPUT_GetString(tableName, MAX_TABLE_NAME_SIZE);

        if (length > 0) break;
    }

    tableId = -1;
    result = SCHEMA_GetTableIdForName(db->schema, tableName, &tableId);
    if(result != kStatus_Success || tableId == -1) {
        SCREEN_PrintError("Unable to find the specified table\n");
        return kStatus_Fail;
    }
    
    result = SCHEMA_GetTableForId(db->schema, tableId, &table);
    if(result != kStatus_Success)  {
        SCREEN_PrintError("Unable to find the specified table\n");
        return result;
    };

    /* Create Record */
    data = (char *)malloc(db->tables[tableId].rowSize);
    memset(data, 0, db->tables[tableId].rowSize);
    for(i = 0; i < table->numColumns; i++) {
        printf("Enter value for %s: ", table->columns[i].columnName);
        switch(table->columns[i].type) {
            case INT:
                INPUT_GetInteger((int *)(data+offset));
                offset += sizeof(int);

                if(table->columns[i].isPrimaryKey) {
                    index = 0;
                    result = DB_FindRowWithKey(db, tableId, *((int *)(data+offset-sizeof(int))), &index);
                    if(result == kStatus_Success) {
                        SCREEN_PrintError("Primary key already exists\n");
                        return kStatus_Fail;
                    }
                }
                break;
            case STRING:
                INPUT_GetString(data+offset, table->columns[i].size);
                offset += table->columns[i].size;
                break;
            default:
                SCREEN_PrintError("Unknown column type. "
                    "Your database is likely corrupted\n");
                return kStatus_Schema_UnknownError;
        }
    }

    result = DB_InsertRow(db, tableId, (void *)data);
    free(data);
    return result;
}
status_t GUI_UpdateRecordForTable(database_t *db) {
    int offset = 0, i = 0, tableId = -1, index = 0, primaryKey = 0, length = 0;
    int keyIndex;
    char tableName[MAX_TABLE_NAME_SIZE];
    status_t result;
    char *values, *data;
    table_schema_def_t *table = NULL;

    if(db == NULL ) return kStatus_InvalidArgument;

    SCREEN_ClearScreen();

    while(1) {
        SCREEN_PrintInput("Enter the name of the table you wish to create a record for: \n> ");
        length = INPUT_GetString(tableName, MAX_TABLE_NAME_SIZE);

        if (length > 0) break;
    }

    result = SCHEMA_GetTableIdForName(db->schema, tableName, &tableId);
    if(result != kStatus_Success || tableId == -1) {
        SCREEN_PrintError("Unable to find the specified table\n");
        return kStatus_Fail;
    }

    (void)GUI_DisplayTable(db, tableName, 0);
    while(1) {
        SCREEN_PrintInput("\nEnter the primary key of the record you wish to update (-1 to cancel): \n>");
        primaryKey = 0;
        INPUT_GetInteger(&primaryKey);

        if(primaryKey == -1) {
            return kStatus_Success;
        }

        result = DB_FindRowWithKey(db, tableId, primaryKey, &index);
        if(result == kStatus_Fail || index == -1) {
            SCREEN_PrintError("Unknown row ID\n");
        }else if (result != kStatus_Success) {
            SCREEN_PrintError("An error occured finding the row\n");
            return result;
        }else {
            break;
        }
    }

    values = (char *)malloc(db->tables[tableId].rowSize);
    memset(values, 0, db->tables[tableId].rowSize);

    data = (char *)malloc(db->tables[tableId].rowSize);
    memcpy(data, db->tables[tableId].data + (index * db->tables[tableId].rowSize), db->tables[tableId].rowSize);

    result = SCHEMA_GetTableForId(db->schema, tableId, &table);

    for(i = 0; i < table->numColumns; i++) {
        printf("Enter new value for %s: ", table->columns[i].columnName);
        switch(table->columns[i].type) {
            case INT:
                INPUT_GetInteger((int *)(values+offset));
                offset += sizeof(int);

                if(table->columns[i].isPrimaryKey) {
                    keyIndex = 0;
                    result = DB_FindRowWithKey(db, tableId, *((int *)(values+offset-sizeof(int))), &keyIndex);
                    if(result == kStatus_Success && keyIndex != index) {
                        SCREEN_PrintError("Primary key already exists\n");
                        return kStatus_Fail;
                    }
                }

                break;
            case STRING:
                INPUT_GetString(values+offset, table->columns[i].size);
                offset += table->columns[i].size;
                break;
            default:
                SCREEN_PrintError("Unknown column type. "
                    "Your database is likely corrupted\n");
                return kStatus_Schema_UnknownError;
        }
    }

    memcpy(db->tables[tableId].data + (index * db->tables[tableId].rowSize), values, db->tables[tableId].rowSize);

    free(values);
    free(data);

    return kStatus_Success;
}

status_t GUI_RemoveRowForTable(database_t *db) {
    int primaryKey, index, length, tableId = -1;
    char tableName[MAX_TABLE_NAME_SIZE];
    status_t result;
    if(db == NULL ) return kStatus_InvalidArgument;
    SCREEN_ClearScreen();

    while(1) {
        SCREEN_PrintInput("Enter the name of the table you wish to delete a record from: \n> ");
        length = INPUT_GetString(tableName, MAX_TABLE_NAME_SIZE);

        if (length > 0) break;
    }
    result = SCHEMA_GetTableIdForName(db->schema, tableName, &tableId);
    if(result != kStatus_Success) return result;
    if(tableId == -1) return kStatus_Schema_UnknownTableId;

    (void)GUI_DisplayTable(db, tableName, 0);

    while(1) {
        SCREEN_PrintInput("\nEnter the primary key of the record you wish to delete (-1 to cancel): \n>");
        primaryKey = 0;
        INPUT_GetInteger(&primaryKey);

        if(primaryKey == -1) {
            return kStatus_Success;
        }

        result = DB_FindRowWithKey(db, tableId, primaryKey, &index);


        if(result == kStatus_Fail || index == -1) {
            SCREEN_PrintError("Unknown row ID\n");
        }else if (result != kStatus_Success) {
            return result;
        }else {
            break;
        }
    }

    return DB_DeleteRow(db, tableId, index);
}

status_t GUI_FindRowsForTable(database_t *db) {
    int index, columnOffset, columnId = -1;
    char tableName[MAX_TABLE_NAME_SIZE];
    char columnName[MAX_COLUMN_NAME_SIZE];
    int length, tableId;
    int i = 0, j = 0;
    int totalWidth = 0, columnWidth = 0;
    void *value = NULL;
    int *foundIndexes = NULL;
    int numIndexes = 0;
    char *lineBuffer;
    status_t result;
    table_col_def_t *column = NULL;
    table_schema_def_t *table;

    if(db == NULL) return kStatus_InvalidArgument;
    SCREEN_ClearScreen();

    while(1) {
        SCREEN_PrintInput("Enter the name of the table you wish to search within: \n> ");
        length = INPUT_GetString(tableName, MAX_TABLE_NAME_SIZE);

        if (length > 0) break;
    }

    tableId = -1;
    result = SCHEMA_GetTableIdForName(db->schema, tableName, &tableId);
    if(result != kStatus_Success || tableId == -1) {
        DEBUG_PRINT(("Failed to find table with status %d\n", result));
        SCREEN_PrintError("Failed to find table\n");
        return kStatus_Fail;
    }

    result = SCHEMA_GetTableForId(db->schema, tableId, &table);
    if(result != kStatus_Success) return result;

    GUI_DisplayTableSchema(db, tableId);

    while(1) {
        SCREEN_PrintInput("Enter the column name you wish to search by: \n> ");
        INPUT_GetString(columnName, MAX_COLUMN_NAME_SIZE);
        result = SCHEMA_GetIDForColumn(table, columnName, &columnId);
        if(result != kStatus_Success || columnId == -1) {
            SCREEN_PrintError("This column does not exist\n");
        } else {
            column = &(table->columns[columnId]);
            break;
        }
    }

    SCREEN_PrintInput("Enter the value you wish to search for: \n> ");
    value = malloc(column->type == INT ? sizeof(int) : column->size);

    if(value == NULL) {
        DEBUG_PRINT(("Failed to allocate memory for value\n"));
        return kStatus_AllocError;
    }

    switch(column->type) {
        case INT:
            INPUT_GetInteger((int*)value);
            break;
        case STRING:
            INPUT_GetString((char *)value, column->size);
            break;
        default:
            return kStatus_Fail;
    }

    DB_FindRowsWithColumnValue(db, tableId, columnId, value, &foundIndexes, &numIndexes);

    if(numIndexes == 0) {
        if(foundIndexes != NULL) free(foundIndexes);
        free(value);

        SCREEN_PrintError("No records found\n");
        INPUT_WaitForAnyKey("Press any key to return to menu");
        return kStatus_Success;
    }

    printf("\n\n");
    for(i = 0; i < table->numColumns; i++) {
        columnWidth = max(strlen(table->columns[i].columnName), table->columns[i].size);
        totalWidth += columnWidth + 3;
        printf("%-*s | ", columnWidth, table->columns[i].columnName);
    }
    printf("\n");
    lineBuffer = (char *)malloc(totalWidth+1);
    memset(lineBuffer, '-', totalWidth);
    lineBuffer[totalWidth] = '\0';

    DEBUG_PRINT(("Found %d indexes\n", numIndexes));

    for(i = 0; i < numIndexes; i++) {
        index = foundIndexes[i];
        printf("%s\n", lineBuffer);
        columnOffset = 0;
        for(j = 0; j < table->numColumns; j++) {
            columnWidth = max(strlen(table->columns[j].columnName), table->columns[j].size);
            switch(table->columns[j].type) {
                case INT:
                    printf("%-*d | ", columnWidth, *((int *)(db->tables[tableId].data + (index * db->tables[tableId].rowSize) + columnOffset)));
                    columnOffset += sizeof(int);
                    break;
                case STRING:
                    printf("%-*s | ", columnWidth, db->tables[tableId].data + (index * db->tables[tableId].rowSize) + columnOffset);
                    columnOffset += table->columns[j].size;
                    break;
                default:
                    return kStatus_Schema_UnknownError;
            }
        }
        printf("\n");
    }

    free(lineBuffer);
    free(value);

    INPUT_WaitForAnyKey("\n\nPress any key to return to menu");

    return kStatus_Success;
}

/* Schema Operations */
void GUI_SchemaOperationsLoop(database_t *db) {
    if(db == NULL) {
        SCREEN_PrintError("No database loaded, please load or create a database\n");
        return;
    }

    SCREEN_ClearScreen();
    printf("Schema Operations\n");
    printf("================\n");
    GUI_PrintSchemaOperationsMenu();

    while(1) {
        int selection = GUI_GetOptionSelection(1, 5, "Please select an option (1-5): ");
        status_t status = kStatus_Success;

        switch(selection) {
            case 1: 
                status = GUI_CreateTable(db);
                if(status != kStatus_Success) {
                    SCREEN_PrintError("An error occured creating the table\n");
                } else {
                    SCREEN_ClearScreen();
                }

                break;
            case 2:
                status = GUI_DeleteTable(db);
                if(status != kStatus_Success) {
                    if(status == kStatus_Schema_UnknownTableId) {
                        SCREEN_PrintError("The specified table does not exist or could not be found\n");
                    } else {
                        SCREEN_PrintError("An error occured deleting the table\n");
                    }
                } else {
                    SCREEN_ClearScreen();
                }

                break;
            case 3:
                status = GUI_AddColumn(db);
                if(status != kStatus_Success) {
                    if(status == kStatus_Schema_UnknownTableId) {
                        SCREEN_PrintError("The specified table does not exist or could not be found\n");
                    } else {
                        SCREEN_PrintError("An error occured adding the column\n");
                    }
                } else {
                    SCREEN_ClearScreen();
                }

                break;
            case 4:
                status = GUI_DisplayTableSchema(db, -1);
                if(status != kStatus_Success) {
                    if(status == kStatus_Schema_UnknownTableId) {
                        SCREEN_PrintError("The specified table does not exist or could not be found\n");
                    } else {
                        SCREEN_PrintError("An error occured displaying the schema\n");
                    }
                } else {
                    SCREEN_ClearScreen();
                }

                return;
            case -1:
                SCREEN_PrintError("An internal error has occured, "
                    "please try agian\n");
                return;
            case 5: 
                /* Return to main menu*/
                SCREEN_ClearScreen();
                return;
        }

        printf("Schema Operations\n");
        printf("================\n");
        GUI_PrintSchemaOperationsMenu(db);
    }
}

void GUI_PrintSchemaOperationsMenu() {
    printf("1. Create Table\n");
    printf("2. Delete Table\n");
    printf("3. Add Column\n");
    printf("4. Display Table Schema\n");
    printf("5. Return to Main Menu\n");
}

status_t GUI_CreateTable(database_t *db) {
    char tableName[MAX_TABLE_NAME_SIZE];
    int numColumns = 0;
    int i = 0, length = 0;
    int hasDonePrimaryKey = 0;
    int columnType, columnSize;
    char columnName[MAX_COLUMN_NAME_SIZE];
    table_col_def_t *columns = NULL;

    if(db == NULL) return kStatus_InvalidArgument;

    SCREEN_ClearScreen();
    while(1) {
        SCREEN_PrintInput("Enter the name of the table you wish to create: \n> ");
        length = INPUT_GetString(tableName, MAX_TABLE_NAME_SIZE);

        if (length > 0) break;
    }

    while(1) {
        SCREEN_PrintInput("Enter the number of columns you wish to create: \n> ");
        INPUT_GetInteger(&numColumns);
        if(numColumns > 0) break;
        SCREEN_PrintError("Invalid number of columns, this value must be positive.\n");
    }

    columns = (table_col_def_t *)malloc(sizeof(table_col_def_t) * numColumns);
    if(columns == NULL) return kStatus_AllocError;

    for(i = 0; i < numColumns; i++) {
        while(1) {
            printf("Enter the name of column %d: \n> ", i);
            length = INPUT_GetString(columnName, MAX_COLUMN_NAME_SIZE);

            if (length > 0) break;
        }

        columnType = 0;
        while(1) {
            printf("Enter the type of column %d (1: INT, 2: STRING): \n> ", i);
            INPUT_GetInteger(&columnType);
            if(columnType == 1 || columnType == 2) break;
            SCREEN_PrintError("Invalid selection, please try again.\n");
        }

        columnSize = 0;
        if(columnType == 2) {
            while(1) {
                printf("Enter the size of column %d: \n> ", i);
                INPUT_GetInteger(&columnSize);

                if(columnSize <= 0 || columnSize > MAX_COLUMN_DATA_SIZE) {
                    printf("Invalid column size, must be between 1 and %d\n", MAX_COLUMN_DATA_SIZE);
                }else {
                    break;
                }
            }
        }else {
            columnSize = sizeof(int);
        }

        if(columnType == 1 && !hasDonePrimaryKey) {
            hasDonePrimaryKey = 1;
            columns[i].isPrimaryKey = 1;
        } else {
            columns[i].isPrimaryKey = 0;
        }
        columns[i].size = columnSize;
        columns[i].type = columnType-1;
        strncpy(columns[i].columnName, columnName, MAX_COLUMN_NAME_SIZE);
    }

    return DB_CreateTable(db, tableName, columns, numColumns);
}

status_t GUI_DeleteTable(database_t *db) {
    int tableId = -1;
    status_t result;
    char tableName[MAX_TABLE_NAME_SIZE];
    int length;
    if(db == NULL) return kStatus_InvalidArgument;

    SCREEN_ClearScreen();
    /* Get Table Name */
    while(1) {
        SCREEN_PrintInput("Enter the name of the table you wish to delete: \n> ");
        length = INPUT_GetString(tableName, MAX_TABLE_NAME_SIZE);

        if (length > 0) break;
    }

    result = SCHEMA_GetTableIdForName(db->schema, tableName, &tableId);
    if(result != kStatus_Success) {
        SCREEN_PrintError("Unable to find the specified table\n");
        return result;
    }

    return DB_DropTable(db, tableId);
 }

status_t GUI_AddColumn(database_t *db) {
    int i = 0, j = 0;
    int newRowSize = 0;
    int columnType = 0;
    int columnSize = 0;
    char tableName[MAX_TABLE_NAME_SIZE];
    char columnName[MAX_COLUMN_NAME_SIZE];
    int columnCount;
    char *newData;
    int tableId = -1;
    int length;
    status_t result;
    table_schema_def_t *table = NULL;

    if(db == NULL) return kStatus_InvalidArgument;

    SCREEN_ClearScreen();
    printf("Add Column\n");
    printf("==========\n");

    /* Get Table Name */
    while(1) {
        SCREEN_PrintInput("Enter the name of the table you wish to add a column to: \n> ");
        length = INPUT_GetString(tableName, MAX_TABLE_NAME_SIZE);

        if (length > 0) break;
    }

    result = SCHEMA_GetTableIdForName(db->schema, tableName, &tableId);
    if(result != kStatus_Success) return result;
    if(tableId == -1) return kStatus_Schema_UnknownTableId;

    result = SCHEMA_GetTableForId(db->schema, tableId, &table);
    if(result != kStatus_Success) return result;

    /* Get Column Name */
    while(1) {
        SCREEN_PrintInput("Enter the name of the column you wish to add: \n> ");
        length = INPUT_GetString(columnName, MAX_COLUMN_NAME_SIZE);

        if (length > 0) break;
    }

    /* Get Column Type */
    while(1) {
        SCREEN_PrintInput("Enter the type of the column you wish to add (1: INT, 2: STRING): \n> ");
        INPUT_GetInteger(&columnType);
        if(columnType == 1 || columnType == 2) break;
        SCREEN_PrintError("Invalid selection, please try again.\n");
    }

    /* Get Column Size */
    if(columnType == 2) {
        SCREEN_PrintInput("Enter the size of the column you wish to add: \n> ");
        INPUT_GetInteger(&columnSize);
    } else {
        columnSize = sizeof(int);
    }
    
    table->columns = table->columns; /* Get columns before change */
    SCHEMA_AddColumn(db->schema, tableId, columnName, columnType-1, columnSize, 0); /* TODO: Add primary key support */

    /* Now we need to update the data without causing misalignment */
    for(i = 0; i < table->numColumns; i++) {
        switch(table->columns[i].type) {
            case INT:
                newRowSize += sizeof(int);
                break;
            case STRING:
                newRowSize += table->columns[i].size;
                break;
            default:
                return kStatus_Schema_UnknownColumn; /* XXX: Is this the right error? */
        }
    }

    newData = (char *)malloc(newRowSize * db->tables[tableId].rows);
    if(newData == NULL) {
        return kStatus_AllocError;
    }

    columnCount = table->numColumns;
    for(i = 0; i < db->tables[tableId].rows; i++) {
        int offset = 0;
        int newOffset = 0;
        for(j = 0; j < columnCount; j++) {
            switch(table->columns[j].type) {
                case INT:
                    *((int *)(newData + (i * newRowSize) + newOffset)) = 
                        *((int *)(db->tables[tableId].data + (i * db->tables[tableId].rowSize) + offset));
                    offset += sizeof(int);
                    newOffset += sizeof(int);
                    break;
                case STRING:
                    strncpy(newData + (i * newRowSize) + newOffset, db->tables[tableId].data + (i * db->tables[tableId].rowSize) + offset, table->columns[j].size);
                    offset += table->columns[j].size;
                    newOffset += table->columns[j].size;
                    break;
                default:
                    return kStatus_Schema_UnknownColumn;
            }
        }
    }

    free(db->tables[tableId].data);
    db->tables[tableId].data = newData;
    db->tables[tableId].rowSize = newRowSize;

    return kStatus_Success;
}

status_t GUI_DisplayTableSchema(database_t *db, int tableId) {
    int i = 0, length = 0;
    status_t result;
    table_schema_def_t *table = NULL;
    int showWait = 0;
    char lineBuffer[MAX_COLUMN_NAME_SIZE+1] = {'\0'};

    if(db == NULL) return kStatus_InvalidArgument;
    SCREEN_ClearScreen();

    if(tableId == -1) {
        char tableName[MAX_TABLE_NAME_SIZE];
        showWait = 1;
        while(1) {
            SCREEN_PrintInput("Enter the name of the table you wish to display the schema for: \n> ");
            length = INPUT_GetString(tableName, MAX_TABLE_NAME_SIZE);

            if (length > 0) break;
        }

        result = SCHEMA_GetTableIdForName(db->schema, tableName, &tableId);
        if(result != kStatus_Success) return result;
        if(tableId == -1) return kStatus_Schema_UnknownTableId;
    }

    result = SCHEMA_GetTableForId(db->schema, tableId, &table);
    if(result != kStatus_Success) return result;

    SCREEN_ClearScreen();
    printf("Table Name: %s\n", table->tableName);
    printf("Table ID: %d\n", tableId);
    printf("\n\n");

    printf(" Column Num | %-*s |\n", MAX_TABLE_NAME_SIZE, "Column Name");

    memset(lineBuffer, '-', MAX_COLUMN_NAME_SIZE);
    printf("------------|-%s-|\n", lineBuffer);
    
    for(i = 0; i < table->numColumns; i++) {
        printf(" %10d | %-*s |\n", i, MAX_COLUMN_NAME_SIZE, table->columns[i].columnName);
        printf("------------|-%s-|\n", lineBuffer);
    }

    if(showWait) {
        INPUT_WaitForAnyKey("\n\nPress any key to return to menu");        
    }

    return kStatus_Success;
}

/* Main (App Loop) */
void GUI_Main() {
    database_t *db = NULL;

    SCREEN_ClearScreen();
    GUI_PrintWelcome();
    GUI_PrintMainMenu();

    while(1) {
        int selection = GUI_GetOptionSelection(1, 6, "Please select an option (1-6): ");

        switch(selection) {
            case 1:
                db = GUI_LoadDatabase();
                
                if(db != NULL) {
                    SCREEN_ClearScreen();
                    SCREEN_Print("Loaded database successfully\n", kColor_Green);
                }

                GUI_PrintMainMenu();
                break;
            case 2: /* Create default E-DB */
                if(db != NULL) {
                    SCREEN_PrintError("A database is already loaded, please " 
                        "save and close it before creating a new one\n");
                    GUI_PrintMainMenu();
                    break;
                }
                db = GUI_CreateDefaultDatabase();
                if(db == NULL) {
                    SCREEN_PrintError("Error creating default database\n");
                }else {
                    SCREEN_ClearScreen();
                    SCREEN_Print("Created database successfully\n", kColor_Green);
                }
                GUI_PrintMainMenu(); /* Print menu again as screen was cleared */
                break;
            case 3:
                GUI_DataOperationsLoop(db);

                GUI_PrintMainMenu(); /* Print menu again as screen was cleared */
                break;
            case 4:
                GUI_SchemaOperationsLoop(db);

                GUI_PrintMainMenu(); /* Print menu again as screen was cleared */
                break;
            case 5:
                if(GUI_SaveDatabase(db) == kStatus_Success) {
                    SCREEN_ClearScreen();
                    SCREEN_Print("Saved database successfully\n", kColor_Green);
                    printf("Exiting...\n");
                    exit(0);
                }

                GUI_PrintMainMenu();
                break;
            case 6:
                printf("Exiting...\n");
                DB_DestroyDatabase(db);
                exit(0);
                break;
            case -1:
                SCREEN_PrintError("An internal error has occured, "
                    "please try agian\n");
                DB_DestroyDatabase(db);
                exit(-1);
                break;
            default: 
                /* 
                * This should not be reached but just in case it will
                * run the loop again 
                */
                break;
        }
    }
}