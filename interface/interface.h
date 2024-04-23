#ifndef INTERFACE_H_
#define INTERFACE_H_

#include <stdio.h>
#include <string.h>

#include "ds/db.h"

void GUI_PrintWelcome();
void GUI_PrintMainMenu();

int GUI_GetOptionSelection(int min, int max);

/* General Operations */
void GUI_PrintDataOperationsMenu(database_t *db);
void GUI_ListTables(database_t *db);
void GUI_DisplayTable(database_t *db, char *tableName);
int GUI_CreateRecordForTable(database_t *db, char *tableName);
int GUI_UpdateRecordForTable(database_t *db, char *tableName);

/* Schema Operations */
void GUI_PrintSchemaOperationsMenu(database_t *db);
void GUI_CreateTable(database_t *db);
void GUI_DeleteTable(database_t *db);
void GUI_AddColumn(database_t *db);
void GUI_DeleteColumn(database_t *db);
void GUI_DisplayTableSchema(database_t *db);

/* Main (App Loop) */
void GUI_Main();

#endif