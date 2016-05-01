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
    Field field;
    char turns;
    char sink_turn[N_SHIPS];
} Audrey;

// Macros
#define HIT_WEIGHT (FIELD_SIZE*FIELD_SIZE)

/*============================================================*
 * Audrey functions
 *============================================================*/

/*-----------------------------------------*//**
 * @brief Audrey initialize
 * @param audrey        Instance to initialize
 * @return 0 on success, else error code
 *//*-----------------------------------------*/
int audrey_Create(Audrey *audrey);

/*-----------------------------------------*//**
 * @brief Play an entire game
 * @param audrey        Instance to play
 * @return 0 on success, else error code
 *//*-----------------------------------------*/
int audrey_Play(Audrey *audrey);

/*-----------------------------------------*//**
 * @brief Audrey turn driver
 *//*-----------------------------------------*/
int audrey_PlayTurn(Audrey *audrey);

/*============================================================*/
#endif