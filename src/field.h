/**********************************************************//**
 * @file field.h
 * @brief Definition of Battleship field
 * @author Wes64
 **************************************************************/

#ifndef _FIELD_H_
#define _FIELD_H_

// Standard library
#include <stdio.h>      // FILE
#include <stdbool.h>    // bool

// This project
#include "ship.h"       // SHIP

/// The dimension of the field
#define FIELD_SIZE 10

/**********************************************************//**
 * @enum STATUS
 * @brief The status of each ENTRY.
 **************************************************************/
typedef enum {
    FREE,       ///< The entry is empty.
    UNTRIED,    ///< The entry has not been examined yet.
    MISS,       ///< There is no ship present at the entry.
    HIT,        ///< We hit a ship at the entry.
    SUNK,       ///< We sank a ship at the entry.
} STATUS;

/**********************************************************//**
 * @struct ENTRY
 * @brief Stores the hit status and ship at each tile of the
 * game board.
 **************************************************************/
typedef struct {
    STATUS status;  ///< The status of the tile.
    SHIP ship;      ///< The ship present at the location.
} ENTRY;

/**********************************************************//**
 * @struct FIELD
 * @brief Stores all game board information.
 **************************************************************/
typedef struct {
    ENTRY entry[FIELD_SIZE][FIELD_SIZE];    ///< Tile data
    unsigned char ship_health[N_SHIPS];     ///< Health of the ships
} FIELD;

/**********************************************************//**
 * @enum VIEW
 * @brief Direction of items on the field.
 **************************************************************/
typedef enum {
    LEFT,   ///< We are looking to the left.
    RIGHT,  ///< We are looking to the right.
    UP,     ///< We are looking up.
    DOWN,   ///< We are looking down.
} VIEW;

/**********************************************************//**
 * @brief Clear all items on the field. It is necessary to
 * clear a field before setting it up. This initializes the
 * field.
 * @param field: The field to initialize.
 **************************************************************/
extern void field_Clear(FIELD *field);

/**********************************************************//**
 * @brief Places all the ships randomly on the field.
 * @param field: The field to set up.
 **************************************************************/
extern void field_LoadRandom(FIELD *field);

/**********************************************************//**
 * @brief Get the status at the location on the field.
 * @param field: The field to get information from.
 * @param x: The x-coordinate to get.
 * @param y: Te y-coordinate to get.
 * @return The status at the location.
 **************************************************************/
static inline STATUS field_GetStatus(const FIELD *field, int x, int y) {
    return field->entry[x][y].status;
}

/**********************************************************//**
 * @brief Check if the coordinates are in bounds.
 * @param x: The x-coordinate to check.
 * @param y: Te y-coordinate to check.
 * @return Whether the location is in bounds.
 **************************************************************/
static inline bool field_IsInBounds(int x, int y) {
    return ((0 <= x) && (x < FIELD_SIZE)) && ((0 <= y) && (y < FIELD_SIZE));
}

/**********************************************************//**
 * @brief Get the amount of equal entries in the VIEW
 * direction from the origin point. This goes along the unit
 * vector of the direction from (x, y) until it is out of
 * bounds or reaches an entry with a different status. If the
 * origin is out of bounds, -1 is returned.
 * @param field: The field to get information from.
 * @param dir: The view direction to look in.
 * @param x: The x-coordinate of the origin.
 * @param y: The y-coordinate of the origin.
 * @param status: The status of squares to check.
 * @return The distance travelled, or -1 on failure.
 **************************************************************/
extern int field_GetExtent(const FIELD *field, VIEW dir, int x, int y, STATUS status);

/**********************************************************//**
 * @brief Make an attack on the field.
 * @param field: The field to attack.
 * @param x: The x-coordinate to attack.
 * @param y: The y-coordinate to attack.
 * @return The result of the attack or -1 on failure.
 **************************************************************/
extern STATUS field_Attack(FIELD *field, int x, int y);

/**********************************************************//**
 * @brief Check if the field has been won.
 * @param field: The field to check.
 * @return Whether the field is complete or not.
 **************************************************************/
extern bool field_Win(const FIELD *field);

/**********************************************************//**
 * @brief Print the field statuses to the file.
 * @param field: The field to check.
 * @param file: The output file.
 **************************************************************/
void field_PrintStatus(const FIELD *field, FILE *file);

/**********************************************************//**
 * @brief Print the field ships to the file.
 * @param field: The field to check.
 * @param file: The output file.
 **************************************************************/
void field_PrintShips(const FIELD *field, FILE *file);

/*============================================================*/
#endif
