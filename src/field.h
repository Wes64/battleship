/**********************************************************//**
 * @file field.h
 * @brief Definition of Battleship field
 * @author Rena Shinomiya
 **************************************************************/

#ifndef _FIELD_H_
#define _FIELD_H_

// Standard library
#include <stdbool.h>    // bool
#include <stdio.h>      // FILE

/**********************************************************//**
 * @enum SHIP
 * @brief Enumerates all possuble battleships as well as the
 * empty square.
 **************************************************************/
typedef enum {
    EMPTY      = -1,    ///< No ship is present here.
    CARRIER    =  0,    ///< The Carrier ship.
    BATTLESHIP =  1,    ///< The Battleship ship.
    SUBMARINE  =  2,    ///< The Submarine ship.
    CRUISER    =  3,    ///< The cruiser ship.
    DESTROYER  =  4,    ///< The destroyer ship.
} SHIP;

/// The total number of ships possible on the board.
#define N_SHIPS 5

/// The size of the field (from standard Hasbro game).
#define FIELD_SIZE 10

/// The maximum number of turns possible.
#define TURN_MAX (FIELD_SIZE*FIELD_SIZE)

/// Denotes an invalid sink turn.
#define TURN_INVALID -1

/**********************************************************//**
 * @enum STATUS
 * @brief The status of each ENTRY.
 **************************************************************/
typedef enum {
	ERROR,		///< An error occurred (not a valid status).
    FREE,       ///< The entry is empty (used for board placement).
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
    ENTRY entry[FIELD_SIZE][FIELD_SIZE];    ///< Tile data.
    int health[N_SHIPS];     				///< Health of the ships.
    int turns;								///< Turns taken on the field.
    int sinkTurn[N_SHIPS];				    ///< When the ship sank.
} FIELD;

/**********************************************************//**
 * @enum VIEW
 * @brief Enumerates all possible viewing directions from a 
 * given tile on the field.
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
 * @brief Get the length of the given ship.
 * @param ship: The ship to check.
 * @return The length of the ship.
 **************************************************************/
extern int field_GetShipLength(SHIP ship);

/**********************************************************//**
 * @brief Check if the coordinates are in bounds.
 * @param x: The x-coordinate to check.
 * @param y: The y-coordinate to check.
 * @return Whether the location is in bounds.
 **************************************************************/
static inline bool field_IsInBounds(int x, int y) {
    return ((0 <= x) && (x < FIELD_SIZE)) && ((0 <= y) && (y < FIELD_SIZE));
}

/**********************************************************//**
 * @brief Get the public tile status at the given coordinates.
 * @param field: The field in question.
 * @param x: The x-coordinate to check.
 * @param y: The y-coordinate to check.
 * @return The tile status.
 **************************************************************/
static inline STATUS field_GetStatus(const FIELD *field, int x, int y) {
	return field->entry[x][y].status;
}

/**********************************************************//**
 * @brief Get the current health of a ship.
 * @param field: The field to check.
 * @param ship: The ship to check.
 * @return The current health of the ship.
 **************************************************************/
static inline int field_GetShipHealth(const FIELD *field, SHIP ship) {
	return field->health[ship];
}

/**********************************************************//**
 * @brief Places all the ships randomly on the field.
 * @param field: The field to set up.
 **************************************************************/
extern void field_CreateRandom(FIELD *field);

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
extern bool field_IsWon(const FIELD *field);

/**********************************************************//**
 * @brief Get the turn count on the given field.
 * @param field: The field in question.
 * @return The number of turns taken on the given field.
 **************************************************************/
static inline int field_GetTurnCount(const FIELD *field) {
    return field->turns;
}

/**********************************************************//**
 * @brief Get the turn the given ship sank.
 * @param field: The field data.
 * @param ship: The identity of a ship.
 * @return The number of turns taken to sink the ship, or
 * NOT_SUNK if the ship hasn't sunk yet.
 **************************************************************/
static inline int field_GetSinkTurn(const FIELD *field, SHIP ship) {
    return field->sinkTurn[ship];
}

/*============================================================*/
#endif
