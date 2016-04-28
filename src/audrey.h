/*=========================================================*//**
 * @file audrey.h
 * @brief Definition of Battleship AI
 *//*=========================================================*/

#ifndef _AUDREY_H_
#define _AUDREY_H_

// This project
#include "field.h"
#include "ship.h"

/*============================================================*
 * Audrey definitions
 *============================================================*/

/*-----------------------------------------*//**
 * @brief Audrey instance stats
 *//*-----------------------------------------*/
typedef struct {
    Field *field;
    short turns;
    short sink_turn[N_SHIPS];
} Audrey;

// Macros
#define HIT_WEIGHT (FIELD_SIZE*FIELD_SIZE)

/*============================================================*
 * Audrey functions
 *============================================================*/

/*-----------------------------------------*//**
 * @brief Audrey initialize
 *//*-----------------------------------------*/
int audrey_Create(Audrey *audrey, const Field *field);

/*-----------------------------------------*//**
 * @brief Audrey play game
 *//*-----------------------------------------*/
int audrey_Play(Audrey *audrey);

/*-----------------------------------------*//**
 * @brief Audrey turn driver
 *//*-----------------------------------------*/
int audrey_ChooseTurn(Audrey *audrey, int *out_x, int *out_y);

/*-----------------------------------------*//**
 * @brief Audrey deletion
 *//*-----------------------------------------*/
void audrey_Destroy(Audrey *audrey);

/*============================================================*/
#endif