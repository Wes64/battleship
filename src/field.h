/*=========================================================*//**
 * @file field.h
 * @brief Definition of Battleship field
 *//*=========================================================*/

#ifndef _FIELD_H_
#define _FIELD_H_

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
typedef Entry** Field;

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
 *//*-----------------------------------------*/
int view_GetVector(View view, int *out_di, int *out_dj);

/*-----------------------------------------*//**
 * @brief Make a field
 *//*-----------------------------------------*/
int field_Create(Field *field);

/*-----------------------------------------*//**
 * @brief Clear a field
 *//*-----------------------------------------*/
int field_Clear(Field field);

/*-----------------------------------------*//**
 * @brief Load a random field
 *//*-----------------------------------------*/
int field_LoadRandom(Field field);

/*-----------------------------------------*//**
 * @brief Place a ship
 *//*-----------------------------------------*/
int field_PlaceShip(const Field field, View dir, int x, int y, Ship ship);

/*-----------------------------------------*//**
 * @brief Ready the field for play
 *//*-----------------------------------------*/
int field_ReadyPlay(Field field);

/*-----------------------------------------*//**
 * @brief Destroy a Field
 *//*-----------------------------------------*/
void field_Destroy(Field field);

/*-----------------------------------------*//**
 * @brief Check if the coordinates in bounds
 *//*-----------------------------------------*/
int field_IsInBounds(const Field field, int x, int y);

/*-----------------------------------------*//**
 * @brief Get the ships remaining
 *//*-----------------------------------------*/
int field_GetShipsRemaining(const Field field, char *out_info);

/*-----------------------------------------*//**
 * @brief Get the amount of equal Entry
 * in the View direction from the given point
 *//*-----------------------------------------*/
int field_GetExtent(const Field field, View dir, int x, int y, Status status);

/*-----------------------------------------*//**
 * @brief Make an attack on the field
 *//*-----------------------------------------*/
int field_Attack(Field field, int x, int y);

/*-----------------------------------------*//**
 * @brief Check if the field has been won
 *//*-----------------------------------------*/
int field_Win(const Field field);

/*============================================================*/
#endif