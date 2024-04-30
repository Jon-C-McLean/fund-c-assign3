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

        switch(selection) {
            case 1: 
                GUI_ListTables(db);

                SCREEN_ClearScreen();
                printf("Data Operations\n");
                printf("===============\n");
                GUI_PrintDataOperationsMenu(db);
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

void GUI_DisplayTable(database_t *db, char *tableName) {}
status_t GUI_CreateRecordForTable(database_t *db, char *tableName) { return 0; }
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

    while(1) {}
}
void GUI_PrintSchemaOperationsMenu(database_t *db) {}
status_t GUI_CreateTable(database_t *db) { return 0; }
status_t GUI_DeleteTable(database_t *db) { return 0; }
status_t GUI_AddColumn(database_t *db) { return 0; }
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