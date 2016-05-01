/*=========================================================*//**
 * @file field.h
 * @brief Definition of Battleship field
 *//*=========================================================*/

#ifndef _FIELD_H_
#define _FIELD_H_

// Standard library
#include <stdio.h>      // FILE

// This project
#include "ship.h"

/*============================================================*
 * Field definitions
 *============================================================*/

// Field macros
#define FIELD_SIZE 10

/*-----------------------------------------*//**
 * @brief The hit status
 *//*-----------------------------------------*/
typedef enum {
    FREE,
    UNTRIED,
    MISS,
    HIT,
    SUNK,
} Status;

/*-----------------------------------------*//**
 * @brief The field elements
 *//*-----------------------------------------*/
typedef struct {
    Status status;
    Ship ship;
} Entry;

/*-----------------------------------------*//**
 * @brief Type of each Field
 *//*-----------------------------------------*/
typedef struct {
    Entry entry[FIELD_SIZE][FIELD_SIZE];
    unsigned char ship_health[N_SHIPS];
} Field;

/*-----------------------------------------*//**
 * @brief Direction on the field
 *//*-----------------------------------------*/
typedef enum {
    LEFT,
    RIGHT,
    UP,
    DOWN,
} View;

/*============================================================*
 * Function definitions
 *============================================================*/

/*-----------------------------------------*//**
 * @brief Transform a View to a unit vector
 * @param view      The view to transform
 * @param out_di    Output for x coordinate
 * @param out_dj    Output for y coordinate
 * @return 0 on success, else error code
 *//*-----------------------------------------*/
int view_GetVector(View view, int *out_di, int *out_dj);

/*-----------------------------------------*//**
 * @brief Clear a field
 * It is necessary to clear the field before
 * setting it up. This initializes the field.
 * @param field     The field to initialize
 * @return 0 on success, else error code
 *//*-----------------------------------------*/
int field_Clear(Field *field);

/*-----------------------------------------*//**
 * @brief Load a random field
 * @param field     A clear field
 * @return 0 on success, else error code
 *//*-----------------------------------------*/
int field_LoadRandom(Field *field);

/*-----------------------------------------*//**
 * @brief Place a ship
 * @param field     Field to place on
 * @param view      Direction to place in
 * Use only RIGHT or DOWN for the direction.
 * @param x         x-coordinate to place
 * @param y         y-coordinate to place
 * The ship emanates in the view direction
 * from the (x, y) coordinates.
 * @param ship      The ship ID
 * @return 0 on success, else error code
 *//*-----------------------------------------*/
int field_PlaceShip(Field *field, View dir, int x, int y, Ship ship);

/*-----------------------------------------*//**
 * @brief Ready the field for play
 * @param field     The field
 * @return 0 on success, else error code
 *//*-----------------------------------------*/
int field_ReadyPlay(Field *field);

/*-----------------------------------------*//**
 * @brief Check if the coordinates in bounds
 * @param field     Field to bounds check
 * @param x         x-coordinate to check
 * @param y         y-coordinate to check
 * @return 1 is true, 0 is false
 *//*-----------------------------------------*/
int field_IsInBounds(const Field *field, int x, int y);

/*-----------------------------------------*//**
 * @brief Get the amount of equal entries
 * in the View direction from the given point
 * @param field     The field to check
 * @param view      The direction to check in
 * @param x         The origin x
 * @param y         The origin y
 * @param status    The status to check
 * This goes along the view vector from (x, y)
 * until it is out of bounds or reaches a
 * entry which has a different status than
 * the parameter provided.
 * @return -1 on failure, else distance
 *//*-----------------------------------------*/
int field_GetExtent(const Field *field, View dir, int x, int y, Status status);

/*-----------------------------------------*//**
 * @brief Make an attack on the field
 * @param field     Field to attack
 * @param x         x-coordinate to attack
 * @param y         y-coordinate to attack
 * @return 0 on success, else error code
 *//*-----------------------------------------*/
int field_Attack(Field *field, int x, int y);

/*-----------------------------------------*//**
 * @brief Check if the field has been won
 * @param field     Field to check
 * @return 1 is true, 0 is false
 *//*-----------------------------------------*/
int field_Win(const Field *field);

/*-----------------------------------------*//**
 * @brief Print the field on the terminal
 * @param field     Field to check
 *//*-----------------------------------------*/
void field_Print(const Field *field, FILE *file);

/*============================================================*/
#endif
