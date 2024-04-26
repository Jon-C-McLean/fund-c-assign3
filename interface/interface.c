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
    printf("What would you like to do?");
    printf("1. Data Operations\n");
    printf("2. Schema Management\n");
    printf("3. View Data/Tables\n");
    printf("4. Perform Query\n");
    printf("5. Exit\n");
}

int GUI_GetOptionSelection(int min, int max) { return 0; }

/* General Operations */
void GUI_PrintDataOperationsMenu(database_t *db) {}
void GUI_ListTables(database_t *db) {}
void GUI_DisplayTable(database_t *db, char *tableName) {}
int GUI_CreateRecordForTable(database_t *db, char *tableName) { return 0; }
int GUI_UpdateRecordForTable(database_t *db, char *tableName) { return 0; }

/* Schema Operations */
void GUI_PrintSchemaOperationsMenu(database_t *db) {}
void GUI_CreateTable(database_t *db) {}
void GUI_DeleteTable(database_t *db) {}
void GUI_AddColumn(database_t *db) {}
void GUI_DeleteColumn(database_t *db) {}
void GUI_DisplayTableSchema(database_t *db) {}

/* Main (App Loop) */
void GUI_Main() {
    GUI_PrintWelcome();
    GUI_PrintMainMenu();
    while(1) {

    }
}