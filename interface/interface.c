/**
 * @file interface.c
 * @author Jon McLean (13515869)
 */
#include "interface.h"

void GUI_PrintWelcome() {
    SCREEN_ClearScreen();
    printf(" _______         _______  .______   \n");
    printf("|   ____|       |       \\ |   _  \\  \n");
    printf("|  |__    ______|  .--.  ||  |_)  | \n");
    printf("|   __|  |______|  |  |  ||   _  <  \n");
    printf("|  |____        |  '--'  ||  |_)  | \n");
    printf("|_______|       |_______/ |______/  \n");
    printf("\n");
    printf("Welcome to E-DB, the customisable employee database system!\n");
    printf("Written by Jon McLean, Samuel Morgan, Andre Mury, and David Lua\n");
    printf("===============================================================\n");
    printf("\n\n\n");
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
    printf("7. Exit\n");
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
    /* TODO: Load from file */
    return NULL;
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
                GUI_ListTables(db);

                SCREEN_ClearScreen();
                printf("Data Operations\n");
                printf("===============\n");
                GUI_PrintDataOperationsMenu(db);
                break;
            case 2:
                GUI_DisplayTable(db);

                SCREEN_ClearScreen();
                printf("Data Operations\n");
                printf("===============\n");
                GUI_PrintDataOperationsMenu(db);
                break;
            case 3:
                if((status = GUI_CreateRecordForTable(db)) != kStatus_Success) {
                    printf("Error creating record %d\n", status);
                } else {
                    SCREEN_ClearScreen();
                    printf("Data Operations\n");
                    printf("===============\n");
                    GUI_PrintDataOperationsMenu(db);
                }
                break;
            case -1:
                printf("FATAL: Internal error occured\n");
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

status_t GUI_DisplayTable(database_t *db) {
    if(db == NULL ) return kStatus_InvalidArgument;
    SCREEN_ClearScreen();

    /* Get Table Name */
    char tableName[MAX_TABLE_NAME_SIZE];
    while(1) {
        printf("Enter the name of the table you wish to display records for: \n> ");
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

    /* Display Table */
    printf("Table: %s\n", table->tableName);
    printf("Columns: %d\n", table->numColumns);
    printf("\n\n");
    int i = 0, j = 0;
    int totalWidth = 0;
    for(i = 0; i < table->numColumns; i++) {
        int columnWidth = max(strlen(table->columns[i].columnName), table->columns[i].size);
        totalWidth += columnWidth;
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
            switch(table->columns[j].type) {
                case INT:
                    printf("%-*d | ", table->columns[j].size, *((int *)(db->tables[tableId].data + (i * db->tables[tableId].rowSize) + columnOffset)));
                    columnOffset += sizeof(int);
                    break;
                case STRING:
                    printf("%-*s | ", table->columns[j].size, db->tables[tableId].data + (i * db->tables[tableId].rowSize) + columnOffset);
                    columnOffset += table->columns[j].size;
                    break;
                default:
                    return kStatus_Schema_UnknownError;
            }
        }
        printf("\n");
    }

    INPUT_WaitForAnyKey("\n\nPress any key to return to menu");

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
    if(result != kStatus_Success) return result;
    if(tableId == -1) return kStatus_Schema_UnknownTableId;

    table_schema_def_t *table = NULL;
    result = SCHEMA_GetTableForId(db->schema, tableId, &table);
    if(result != kStatus_Success) return result;

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
                break;
            case STRING:
                INPUT_GetString(data+offset, table->columns[i].size);
                offset += table->columns[i].size;
                break;
            default:
                return kStatus_Schema_UnknownError;
        }
    }

    return DB_InsertRow(db, tableId, (void *)data);
}
status_t GUI_UpdateRecordForTable(database_t *db, char *tableName) { return 0; }

/* Schema Operations */
void GUI_SchemaOperationsLoop(database_t *db) {
    if(db == NULL) {
        printf("No database loaded, please load or create a database\n");
        return;
    }

    SCREEN_ClearScreen();
    printf("Schema Operations\n");
    printf("================\n");

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
                    GUI_PrintSchemaOperationsMenu(db);
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

void GUI_PrintSchemaOperationsMenu(database_t *db) {
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
        int selection = GUI_GetOptionSelection(1, 7, "Please select an option (1-7): ");

        switch(selection) {
            case 1: /* Load DB from file */
                break;
            case 2: /* Create default E-DB */
                db = GUI_CreateDefaultDatabase();
                if(db == NULL) {
                    printf("FATAL: Database creation failed...\n");
                    exit(-1);
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