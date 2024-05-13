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
    SCREEN_Print("Written by Jon McLean, Samuel Morgan, Andre Mury, and David Lua\n", kColor_Red);
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
    printf("5. View Data/Tables\n");
    printf("6. Perform Query\n");
    printf("7. Save Database and Exit \n");
    printf("8. Exit\n");
}

int GUI_GetOptionSelection(int min, int max, char* prompt) {
    int value = 0;
    while(1) {
        printf("%s\n", prompt);
        INPUT_GetInteger(&value);
        if(value >= min && value <= max) {
            return value;
        } else {
            printf("Invalid selection, please try again.\n");
        }
    }

    return -1;
}
/* Startup Operations */
database_t* GUI_LoadDatabase(void) {
    SCREEN_ClearScreen();
    
    database_t *db = NULL;

    printf("Enter the name of the file you want to load the DB from: \n> ");
    char fileName[256];
    INPUT_GetString(fileName, 256);

    printf("Please select whether this database is encrypted\n");
    printf("1. Encrypted\n");
    printf("2. Plain\n");

    int selection = GUI_GetOptionSelection(1, 2, "Please select an option (1-2): ");
    if(selection == -1) {
        SCREEN_PrintError("An internal error has occured, please try agian\n");
        return NULL;
    }

    char key[AES_KEY_SIZE];
    memset(key, 0, sizeof(key));
    if(selection == 1) {
        printf("Enter the key to use for decryption (%d bytes): \n> ", AES_KEY_SIZE);
        INPUT_GetString(key, AES_KEY_SIZE);
    }

    status_t result = DB_LoadFromDisk(&db, fileName, selection == 1 ? key : NULL, sizeof(key));
    if(result != kStatus_Success) {
        SCREEN_PrintError("Error loading database\n");
        return NULL;
    }

    return db;
}

status_t GUI_SaveDatabase(database_t *db) {
    SCREEN_ClearScreen();
    if(db == NULL) {
        SCREEN_PrintError("No database loaded, please load or create a database\n");
        return kStatus_Fail;
    }

    printf("Enter the name of the file you want to save the DB to: \n> ");
    char fileName[256];
    INPUT_GetString(fileName, 256);

    printf("Please select whether you want compression and encryption\n");
    printf("1. Encryption \n");
    printf("2. Encryption and Compression\n");
    printf("3. None\n");

    int selection = GUI_GetOptionSelection(1, 3, "Please select an option (1-3): ");
    if(selection == -1) {
        SCREEN_PrintError("An internal error has occured, please try agian\n");
        return kStatus_Fail;
    }

    int compress = selection == 2 ? 1 : 0;
    int encrypt = selection != 3 ? 1 : 0;

    char key[AES_KEY_SIZE];
    memset(key, 0, sizeof(key));
    if(encrypt) {
        printf("Enter the key to use for encryption (%d bytes): \n> ", AES_KEY_SIZE);
        INPUT_GetString(key, AES_KEY_SIZE);
    }

    status_t result = DB_SaveDatabase(db, fileName, compress, encrypt ? key : NULL, sizeof(key));
    if(result != kStatus_Success) {
        SCREEN_PrintError("Error saving database\n");
    }

    return result;
}

database_t* GUI_CreateDefaultDatabase(void) {
    database_t *db;
    SCREEN_ClearScreen();
    printf("Database Creator\n");
    printf("================\n");

    printf("Setting up default database...\n");
    status_t status = DB_CreateDefaultDatabase(&db);
    if(status != kStatus_Success) {
        printf("Error creating default database\n");
        return NULL;
    }


    printf("Database name: >");
    INPUT_GetString(db->schema->dbName, MAX_DB_NAME_SIZE);

    printf("Database created successfully\n");
    return db;
}

/* General Operations */
void GUI_DataOperationsLoop(database_t *db) {
    if(db == NULL) {
        printf("No database loaded, please load or create a database\n");
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
                printf("Data Operations\n");
                printf("===============\n");
                GUI_PrintDataOperationsMenu(db);
                break;
            case 2:
                (void)GUI_DisplayTable(db, NULL, 1);

                SCREEN_ClearScreen();
                printf("Data Operations\n");
                printf("===============\n");
                GUI_PrintDataOperationsMenu(db);
                break;
            case 3:
                if((status = GUI_CreateRecordForTable(db)) != kStatus_Success) {
                } else {
                    SCREEN_ClearScreen();
                }
                printf("Data Operations\n");
                printf("===============\n");
                GUI_PrintDataOperationsMenu(db);
                break;
            case 4:
                if((status = GUI_UpdateRecordForTable(db)) != kStatus_Success && status != kStatus_Fail) {
                } else if (status == kStatus_Success) {
                    SCREEN_ClearScreen();
                }

                printf("Data Operations\n");
                printf("===============\n");
                GUI_PrintDataOperationsMenu(db);
                break;
            case 5:
                if((status = GUI_RemoveRowForTable(db)) != kStatus_Success) {
                    SCREEN_PrintError("An error occured deleting \
                        the record, please try again\n");
                } else {
                    SCREEN_ClearScreen();
                    printf("Data Operations\n");
                    printf("===============\n");
                    GUI_PrintDataOperationsMenu(db);
                }
                break;
            case -1:
                SCREEN_PrintError("An internal error has occured, \
                    please try agian\n");
                return;
            case 7: 
                /* Return to main menu*/
                SCREEN_ClearScreen();
                return;
        }
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
    SCREEN_ClearScreen();
    printf("TableID | %-*s |\n", MAX_TABLE_NAME_SIZE, "Tables");

    char lineBuffer[MAX_TABLE_NAME_SIZE+1] = {'\0'};
    memset(lineBuffer, '-', MAX_TABLE_NAME_SIZE);
    printf("--------|-%s-|\n", lineBuffer);

    for(i = 0; i < db->schema->numTables; i++) {
        printf("%7d | %-*s |\n", i, MAX_TABLE_NAME_SIZE, db->schema->tables[i].tableName);

        char lineBuffer[MAX_TABLE_NAME_SIZE+1] = {'\0'};
        memset(lineBuffer, '-', MAX_TABLE_NAME_SIZE);
        printf("--------|-%s-|\n", lineBuffer);
    }

    INPUT_WaitForAnyKey("\n\nPress any key to return to menu");
    
}

status_t GUI_DisplayTable(database_t *db, char *tableName, int showPrompt) {
    if(db == NULL ) return kStatus_InvalidArgument;
    SCREEN_ClearScreen();

    /* Get Table Name */
    int tableId = 0;
    status_t result;
    if(tableName == NULL) {
        char tableName[MAX_TABLE_NAME_SIZE];
        while(1) {
            printf("Enter the name of the table you wish to display records for: \n> ");
            int length = INPUT_GetString(tableName, MAX_TABLE_NAME_SIZE);

            if (length > 0) break;
        }

        result = SCHEMA_GetTableIdForName(db->schema, tableName, &tableId);
        if(result != kStatus_Success) return result;
        if(tableId == -1) return kStatus_Schema_UnknownTableId;   
    } else {
        result = SCHEMA_GetTableIdForName(db->schema, tableName, &tableId);
        if(result != kStatus_Success) return result;
        if(tableId == -1) return kStatus_Schema_UnknownTableId;
    }

    table_schema_def_t *table = NULL;
    result = SCHEMA_GetTableForId(db->schema, tableId, &table);
    if(result != kStatus_Success) return result;

    /* Display Table */
    printf("\n\n");
    int i = 0, j = 0;
    int totalWidth = 0;
    for(i = 0; i < table->numColumns; i++) {
        int columnWidth = max(strlen(table->columns[i].columnName), table->columns[i].size);
        totalWidth += columnWidth + 3;
        printf("%-*s | ", columnWidth, table->columns[i].columnName);
    }
    printf("\n");
    char lineBuffer[totalWidth+1];
    memset(lineBuffer, '-', totalWidth);
    lineBuffer[totalWidth] = '\0';

    for(i = 0; i < db->tables[tableId].rows; i++) {
        printf("%s\n", lineBuffer);
        int columnOffset = 0;
        for(j = 0; j < table->numColumns; j++) {
            int columnWidth = max(strlen(table->columns[j].columnName), table->columns[j].size);
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

    return kStatus_Success;
}

status_t GUI_CreateRecordForTable(database_t *db) {
    if(db == NULL ) return kStatus_InvalidArgument;

    SCREEN_ClearScreen();

    /* Get Table Name */
    char tableName[MAX_TABLE_NAME_SIZE];
    while(1) {
        printf("Enter the name of the table you wish to create a record for: \n> ");
        int length = INPUT_GetString(tableName, MAX_TABLE_NAME_SIZE);

        if (length > 0) break;
    }

    int tableId = -1;
    status_t result = SCHEMA_GetTableIdForName(db->schema, tableName, &tableId);
    if(result != kStatus_Success || tableId == -1) {
        SCREEN_PrintError("Unable to find the specified table\n");
        return kStatus_Fail;
    }

    table_schema_def_t *table = NULL;
    result = SCHEMA_GetTableForId(db->schema, tableId, &table);
    if(result != kStatus_Success)  {
        SCREEN_PrintError("Unable to find the specified table\n");
        return result;
    };

    /* Create Record */
    char data[db->tables[tableId].rowSize];
    memset(data, 0, db->tables[tableId].rowSize);
    int offset = 0;
    int i = 0;
    for(i = 0; i < table->numColumns; i++) {
        printf("Enter value for %s: ", table->columns[i].columnName);
        switch(table->columns[i].type) {
            case INT:
                INPUT_GetInteger((int *)(data+offset));
                offset += sizeof(int);

                if(table->columns[i].isPrimaryKey) {
                    int index = 0;
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

    return DB_InsertRow(db, tableId, (void *)data);
}
status_t GUI_UpdateRecordForTable(database_t *db) {
    if(db == NULL ) return kStatus_InvalidArgument;

    SCREEN_ClearScreen();

    char tableName[MAX_TABLE_NAME_SIZE];
    while(1) {
        printf("Enter the name of the table you wish to create a record for: \n> ");
        int length = INPUT_GetString(tableName, MAX_TABLE_NAME_SIZE);

        if (length > 0) break;
    }

    int tableId = -1;
    status_t result = SCHEMA_GetTableIdForName(db->schema, tableName, &tableId);
    if(result != kStatus_Success || tableId == -1) {
        SCREEN_PrintError("Unable to find the specified table\n");
        return kStatus_Fail;
    }

    (void)GUI_DisplayTable(db, tableName, 0);
    
    int index = 0;
    while(1) {
        printf("\nEnter the primary key of the record you wish to update (-1 to cancel): \n>");
        int primaryKey = 0;
        INPUT_GetInteger(&primaryKey);

        if(primaryKey == -1) {
            return kStatus_Success;
        }

        result = DB_FindRowWithKey(db, tableId, primaryKey, &index);
        if(result == kStatus_Fail || index == -1) {
            printf("Unknown row ID\n");
        }else if (result != kStatus_Success) {
            SCREEN_PrintError("An error occured finding the row\n");
            return result;
        }else {
            break;
        }
    }

    char values[db->tables[tableId].rowSize];
    memset(values, 0, db->tables[tableId].rowSize);

    table_schema_def_t *table = NULL;
    result = SCHEMA_GetTableForId(db->schema, tableId, &table);

    int offset = 0;
    int i = 0;
    for(i = 0; i < table->numColumns; i++) {
        printf("Enter new value for %s: ", table->columns[i].columnName);
        switch(table->columns[i].type) {
            case INT:
                INPUT_GetInteger((int *)(values+offset));
                offset += sizeof(int);
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

    return kStatus_Success;
}

status_t GUI_RemoveRowForTable(database_t *db) {
    if(db == NULL ) return kStatus_InvalidArgument;
    SCREEN_ClearScreen();

    char tableName[MAX_TABLE_NAME_SIZE];
    while(1) {
        printf("Enter the name of the table you wish to delete a record from: \n> ");
        int length = INPUT_GetString(tableName, MAX_TABLE_NAME_SIZE);

        if (length > 0) break;
    }

    int tableId = -1;
    status_t result = SCHEMA_GetTableIdForName(db->schema, tableName, &tableId);
    if(result != kStatus_Success) return result;
    if(tableId == -1) return kStatus_Schema_UnknownTableId;

    (void)GUI_DisplayTable(db, tableName, 0);

    int index = 0;
    while(1) {
        printf("\nEnter the primary key of the record you wish to delete (-1 to cancel): \n>");
        int primaryKey = 0;
        INPUT_GetInteger(&primaryKey);

        if(primaryKey == -1) {
            return kStatus_Success;
        }

        result = DB_FindRowWithKey(db, tableId, primaryKey, &index);


        if(result == kStatus_Fail || index == -1) {
            printf("Unknown row ID\n");
        }else if (result != kStatus_Success) {
            return result;
        }else {
            break;
        }
    }

    return DB_DeleteRow(db, tableId, index);
}

/* Schema Operations */
void GUI_SchemaOperationsLoop(database_t *db) {
    if(db == NULL) {
        printf("No database loaded, please load or create a database\n");
        return;
    }

    SCREEN_ClearScreen();
    printf("Schema Operations\n");
    printf("================\n");
    GUI_PrintSchemaOperationsMenu();

    while(1) {
        int selection = GUI_GetOptionSelection(1, 7, "Please select an option (1-7): ");
        status_t status = kStatus_Success;

        switch(selection) {
            case 1: 
                /*status = GUI_CreateTable(db);
                if(status != kStatus_Success) {
                    printf("Error creating table %d\n", status);
                } else {
                    SCREEN_ClearScreen();
                    printf("Schema Operations\n");
                    printf("================\n");
                    GUI_PrintSchemaOperationsMenu(db);
                }*/

                break;
            case 2:
                /*status = GUI_DeleteTable(db);
                if(status != kStatus_Success) {
                    printf("Error deleting table %d\n", status);
                } else {
                    SCREEN_ClearScreen();
                    printf("Schema Operations\n");
                    printf("================\n");
                    GUI_PrintSchemaOperationsMenu(db);
                }*/

                break;
            case 3:
                status = GUI_AddColumn(db);
                if(status != kStatus_Success) {
                    printf("Error adding column %d\n", status);
                } else {
                    SCREEN_ClearScreen();
                    printf("Schema Operations\n");
                    printf("================\n");
                    GUI_PrintSchemaOperationsMenu();
                }
                break;
            case -1:
                printf("FATAL: Internal error occured\n");
                return;
            case 6: 
                /* Return to main menu*/
                SCREEN_ClearScreen();
                return;
        }
    }
}

void GUI_PrintSchemaOperationsMenu() {
    printf("1. Create Table\n");
    printf("2. Delete Table\n");
    printf("3. Add Column\n");
    printf("4. Delete Column\n");
    printf("5. Display Table Schema\n");
    printf("6. Return to Main Menu\n");
}

status_t GUI_CreateTable(database_t *db) { return 0; }
status_t GUI_DeleteTable(database_t *db) { return 0; }

status_t GUI_AddColumn(database_t *db) {
    if(db == NULL) return kStatus_InvalidArgument;

    SCREEN_ClearScreen();
    printf("Add Column\n");
    printf("==========\n");

    /* Get Table Name */
    char tableName[MAX_TABLE_NAME_SIZE];
    while(1) {
        printf("Enter the name of the table you wish to add a column to: \n> ");
        int length = INPUT_GetString(tableName, MAX_TABLE_NAME_SIZE);

        if (length > 0) break;
    }

    int tableId = -1;
    status_t result = SCHEMA_GetTableIdForName(db->schema, tableName, &tableId);
    if(result != kStatus_Success) return result;
    if(tableId == -1) return kStatus_Schema_UnknownTableId;

    table_schema_def_t *table = NULL;
    result = SCHEMA_GetTableForId(db->schema, tableId, &table);
    if(result != kStatus_Success) return result;

    /* Get Column Name */
    char columnName[MAX_COLUMN_NAME_SIZE];
    while(1) {
        printf("Enter the name of the column you wish to add: \n> ");
        int length = INPUT_GetString(columnName, MAX_COLUMN_NAME_SIZE);

        if (length > 0) break;
    }

    /* Get Column Type */
    int columnType = 0;
    while(1) {
        printf("Enter the type of the column you wish to add (1: INT, 2: STRING): \n> ");
        INPUT_GetInteger(&columnType);
        if(columnType == 1 || columnType == 2) break;
        printf("Invalid selection, please try again.\n");
    }

    /* Get Column Size */
    int columnSize = 0;
    if(columnType == 2) {
        printf("Enter the size of the column you wish to add: \n> ");
        INPUT_GetInteger(&columnSize);
    }
    int columnCount = table->numColumns;
    table->columns = table->columns; /* Get columns before change */
    SCHEMA_AddColumn(db->schema, tableId, columnName, columnType-1, columnSize, 0); /* TODO: Add primary key support */

    /* Now we need to update the data without causing misalignment */
    int i = 0, j = 0;
    int newRowSize = 0;
    for(i = 0; i < table->numColumns; i++) {
        switch(table->columns[i].type) {
            case INT:
                newRowSize += sizeof(int);
                break;
            case STRING:
                newRowSize += table->columns[i].size;
                break;
            case KEY:
                exit(-3); /* XXX: Not implemented */
            default:
                printf("Fuck me dead\n");
                return kStatus_Schema_UnknownColumn; /* XXX: Is this the right error? */
        }
    }

    char *newData = (char *)malloc(newRowSize * db->tables[tableId].rows);
    if(newData == NULL) {
        return kStatus_AllocError;
    }

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
                    printf("Fuck me sideways\n");
                    return kStatus_Schema_UnknownColumn;
            }
        }
    }

    free(db->tables[tableId].data);
    db->tables[tableId].data = newData;
    db->tables[tableId].rowSize = newRowSize;

    return kStatus_Success;
}

status_t GUI_DeleteColumn(database_t *db) { return 0; }
status_t GUI_DisplayTableSchema(database_t *db) { return 0; }

/* Main (App Loop) */
void GUI_Main() {
    SCREEN_ClearScreen();
    GUI_PrintWelcome();
    GUI_PrintMainMenu();

    database_t *db = NULL;

    while(1) {
        int selection = GUI_GetOptionSelection(1, 8, "Please select an option (1-8): ");

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
                /* XXX: TODO */
                break;
            case 6:
                printf("Perform Query\n");
                break;
            case 7:
                if(GUI_SaveDatabase(db) == kStatus_Success) {
                    SCREEN_ClearScreen();
                    SCREEN_Print("Saved database successfully\n", kColor_Green);
                    printf("Exiting...\n");
                    exit(0);
                }

                GUI_PrintMainMenu();
                break;
            case 8:
                printf("Exiting...\n");
                DB_DestroyDatabase(db);
                exit(0);
                break;
            case -1:
                printf("FATAL: Internal error occured\n");
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