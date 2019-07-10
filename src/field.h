/**********************************************************//**
 * @file field.h
 * @brief Defines a generic battleship field that can be
 * interacted with by an agent (user or AI).
 * @author Rena Shinomiya
 * @date April 28, 2016
 **************************************************************/

#ifndef _FIELD_H_
#define _FIELD_H_

#include <stdbool.h>
#include <stdio.h>

/**************************************************************/
/// @brief The total number of ships possible on the board
/// (from the standard Hasbro game). We have one carrier, one
/// battleship, one submarine, one cruiser, and one destroyer.
#define N_SHIPS 5

/// @brief The size of the field square in tiles (from standard
/// Hasbro game).
#define FIELD_SIZE 10

/// @brief The maximum number of turns possible (the same as the
/// number of different tiles on the field).
#define TURN_MAX (FIELD_SIZE*FIELD_SIZE)

/// @brief Denotes an invalid sink turn.
#define TURN_INVALID -1

/**********************************************************//**
 * @enum SHIP
 * @brief Enumerates all possuble battleships as well as the
 * empty square.
 **************************************************************/
typedef enum {
    EMPTY      = -1,
    CARRIER    =  0,
    BATTLESHIP =  1,
    SUBMARINE  =  2,
    CRUISER    =  3,
    DESTROYER  =  4,
} SHIP;

/**********************************************************//**
 * @enum STATUS
 * @brief Describes what we know about any ENTRY.
 **************************************************************/
typedef enum {
    /// An error occurred (not a valid status).
    ERROR,
    /// @brief The entry is empty (used for board placement to
    /// tell if a ship has been placed here yet).
    FREE,
    /// The entry has not been examined by an agent yet.
    UNTRIED,
    /// There is certainly no ship present at the entry.
    MISS,
    /// We hit a ship at the entry.
    HIT,
    /// We sank the ship at the entry.
    SUNK,
} STATUS;

/**********************************************************//**
 * @struct ENTRY
 * @brief Stores data for one tile on the game field.
 **************************************************************/
typedef struct {
    STATUS status;
    SHIP ship;
} ENTRY;

/**********************************************************//**
 * @struct FIELD
 * @brief Stores all game board information.
 **************************************************************/
typedef struct {
    ENTRY entry[FIELD_SIZE][FIELD_SIZE];
    /// The health of each ship (health 0 means the ship sank).
    int health[N_SHIPS];
    /// Turns taken on the field by whichever agent is playing.
    int turns;
    /// @brief Administrative logging data for when a ship sank.
    /// It will be TURN_INVALID if the ship didn't sink yet.
    int sinkTurn[N_SHIPS];
} FIELD;

/**********************************************************//**
 * @enum VIEW
 * @brief Enumerates all possible viewing directions from a
 * given tile on the field. We can look left, right, up, and
 * down (although special cases happen on edge tiles).
 **************************************************************/
typedef enum {
    LEFT,
    RIGHT,
    UP,
    DOWN,
} VIEW;

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

/**************************************************************/
extern void field_Clear(FIELD *field);
extern int field_GetShipLength(SHIP ship);
extern void field_CreateRandom(FIELD *field);
extern int field_GetExtent(const FIELD *field, VIEW dir, int x, int y, STATUS status);
extern STATUS field_Attack(FIELD *field, int x, int y);
extern bool field_IsWon(const FIELD *field);

/**************************************************************/
#endif // _FIELD_H_
