#ifndef INTERFACE_H_
#define INTERFACE_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../utils/misc.h"

#include "../ds/db.h"
#include "../utils/input.h"
#include "../utils/screen.h"

/**
 * @brief Prints the welcome screen ASCII art to the console.
 * @author Jon McLean (13515869)
 */
void GUI_PrintWelcome();

/**
 * @brief Prints the main options menu to the console. This will be the first
 * screen the user sees when the application starts. It includes options to
 * load the DB from file, create a new DB, exit the app, or perform data and
 * schema operations.
 * @author Jon McLean (13515869)
 */
void GUI_PrintMainMenu();

/**
 * @brief Gets the user's selection from the main menu. This function will
 * prompt the user to enter a number corresponding to the desired option.
 * The function will continue to prompt the user until a valid option is
 * entered.
 * @author Jon McLean (13515869)
 * 
 * @param[in] min The lowest valid option
 * @param[in] max The largest valid options
 * @param[in] prompt Prompt that should be printed to the console each loop
 * @return int The selected option (between min and max)
 */
int GUI_GetOptionSelection(int min, int max, char* prompt);

/* Startup Operations */
/**
 * @brief Prompts the user to enter a file path to load a database from.
 * This function will then load the database and return a pointer to the
 * allocated region
 * @author Jon McLean (13515869)
 * 
 * @return database_t* The loaded database
 */
database_t* GUI_LoadDatabase(void);

/**
 * @brief Prompts the user to create a default database and name it. This
 * function will then return a pointer to the allocated region.
 * @author Jon McLean (13515869)
 * 
 * @note At this point it is memory only and will not be saved to disk
 * until commanded to do so by the user.
 * 
 * @return database_t* The created database. 
 */
database_t* GUI_CreateDefaultDatabase(void);

/* General Operations */
/**
 * @brief Main loop for the data operations menu. This function will display
 * the data operations menu and prompt the user to select an option. The user
 * can then perform operations on the database such as listing tables, viewing
 * a table, creating a record, or updating a record.
 * @author Jon McLean (13515869)
 * 
 * @param[inout] db The database to perform operations on.
 */
void GUI_DataOperationsLoop(database_t *db);

/**
 * @brief Prints the data operations menu to the console
 * @author Jon McLean (13515869)
 * 
 * @param[inout] db The database to perform operations on. 
 */
void GUI_PrintDataOperationsMenu(database_t *db);

/**
 * @brief Lists all tables in the database to the console.
 * @author Jon McLean (13515869)
 * 
 * @param[in] db The database to list tables from. 
 */
void GUI_ListTables(database_t *db);

/**
 * @brief Displays the contents of a user specified table to 
 * the console. The function will prompt the user to enter the name
 * of the table they wish to view.
 * @author Jon McLean (13515869)
 * 
 * @param[in] db The database to display the table from.
 * @param[in] tableName The name of the table to display (Optional).
 * @param[in] showPrompt Whether or not to show the prompt to the user.
 * 
 * @return status_t Status of the operation
 */
status_t GUI_DisplayTable(database_t *db, char *tableName, int showPrompt);

/**
 * @brief Prompts the user to create a record for a specified table.
 * The function will then prompt the user to enter values for each
 * column in the table.
 * @author Jon McLean (13515869)
 * 
 * @param[inout] db The database to create the record in.
 * @return status_t Status of the operation
 */
status_t GUI_CreateRecordForTable(database_t *db);

/**
 * @brief Prompts the user to update a record in a specified table.
 * The function will then prompt the user to enter the primary key
 * of the record/row they wish to update. The user will then be prompted
 * to enter new values for each column in the table.
 * @author Jon McLean (13515869)
 * 
 * @param[inout] db The database to update the record in.
 * @param[in] tableName The name of the table to update the record in.
 * @return status_t Status of the operation
 */
status_t GUI_UpdateRecordForTable(database_t *db);

status_t GUI_RemoveRowForTable(database_t *db);

/* Schema Operations */

/**
 * @brief Main loop for the schema operations menu. This function will display
 * the schema operations menu and prompt the user to select an option. The user
 * can then perform operations on the database schema such as creating a table,
 * deleting a table, adding a column, or deleting a column.
 * @author Jon McLean (13515869)
 * 
 * @param[inout] db The database to perform operations on.
 */
void GUI_SchemaOperationsLoop(database_t *db);

/**
 * @brief Prints the schema operations menu to the console
 * @author Jon McLean (13515869)
 */
void GUI_PrintSchemaOperationsMenu();

/**
 * @brief Prompts the user to create a table in the database. The function
 * will then prompt the user to enter the name of the table and the number
 * of columns. The user will then be prompted to enter the name and type
 * of each column.
 * @author Jon McLean (13515869)
 * 
 * @param[inout] db The database to create the table in.
 * @return status_t Status of the operation
 */
status_t GUI_CreateTable(database_t *db);

/**
 * @brief Prompts the user to delete a table in the database. The function
 * will then prompt the user to enter the name of the table they wish to
 * delete.
 * @author Jon McLean (13515869)
 * 
 * @warning The operations performed by this function are irreversible.
 * 
 * @param[inout] db The database to delete the table from.
 * @return status_t Status of the operation
 */
status_t GUI_DeleteTable(database_t *db);

/**
 * @brief Prompts the user to add a column to a table in the database. The
 * function will then prompt the user to enter the name of the table they
 * wish to add a column to. The user will then be prompted to enter the
 * name and type of the column.
 * @author Jon McLean (13515869)
 * 
 * @param[inout] db The database to add the column to.
 * @return status_t Status of the operation
 */
status_t GUI_AddColumn(database_t *db);

/**
 * @brief Prompts the user to delete a column from a table in the database.
 * The function will then prompt the user to enter the name of the table they
 * wish to delete a column from. The user will then be prompted to enter the
 * name of the column they wish to delete.
 * @author Jon McLean (13515869)
 * 
 * @warning The operations performed by this function are irreversible.
 * 
 * @param[inout] db The database to delete the column from.
 * @return status_t Status of the operation
 */
status_t GUI_DeleteColumn(database_t *db);

/**
 * @brief Displays the schema of a table to the console. The function will
 * prompt the user to enter the name of the table they wish to view.
 * @author Jon McLean (13515869)
 * 
 * @param[in] db The database to display the table schema from.
 * @return status_t Status of the operation
 */
status_t GUI_DisplayTableSchema(database_t *db);

/* Main (App Loop) */
/**
 * @brief Main GUI loop for the application.
 * @author Jon McLean (13515869)
 */
void GUI_Main();

#endif
