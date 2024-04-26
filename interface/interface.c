#include "interface.h"

/*
 _______         _______  .______   
|   ____|       |       \ |   _  \  
|  |__    ______|  .--.  ||  |_)  | 
|   __|  |______|  |  |  ||   _  <  
|  |____        |  '--'  ||  |_)  | 
|_______|       |_______/ |______/  
                                    

*/

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

/* General Operations */
void GUI_DataOperationsLoop(database_t *db) {
    if(db == NULL) {
        printf("No database loaded, please load or create a database\n");
        return;
    }

    SCREEN_ClearScreen();
    printf("Data Operations\n");
    printf("===============\n");

    while(1) {
        
    }
}

void GUI_PrintDataOperationsMenu(database_t *db) {}
void GUI_ListTables(database_t *db) {}
void GUI_DisplayTable(database_t *db, char *tableName) {}
int GUI_CreateRecordForTable(database_t *db, char *tableName) { return 0; }
int GUI_UpdateRecordForTable(database_t *db, char *tableName) { return 0; }

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
void GUI_CreateTable(database_t *db) {}
void GUI_DeleteTable(database_t *db) {}
void GUI_AddColumn(database_t *db) {}
void GUI_DeleteColumn(database_t *db) {}
void GUI_DisplayTableSchema(database_t *db) {}

/* Main (App Loop) */
void GUI_Main() {
    SCREEN_ClearScreen();
    GUI_PrintWelcome();
    GUI_PrintMainMenu();

    database_t *db = NULL;

    while(1) {
        int selection = GUI_GetOptionSelection(1, 5, "Please select an option (1-5): ");

        switch(selection) {
            case 1: /* Load DB from file */
                break;
            case 2: /* Create default E-DB */
                break;
            case 3:
                GUI_DataOperationsLoop(db);
                break;
            case 4:
                GUI_SchemaOperationsLoop(db);
                break;
            case 5:
                /* XXX: TODO */
                break;
            case 6:
                printf("Perform Query\n");
                break;
            case 7:
                printf("Exiting...\n");
                exit(0);
                break;
            case -1:
                printf("Fatal error, exiting...\n");
                exit(-1);
                break;
        }
    }
}