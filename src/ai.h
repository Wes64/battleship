/**********************************************************//**
 * @file ai.h
 * @brief Definition of Battleship AI
 * @author Rena Shinomiya
 **************************************************************/

#ifndef _AI_H_
#define _AI_H_

// Standard library
#include <stdbool.h>    // bool
#include <stdio.h>      // FILE

// This project
#include "field.h"      // FIELD

/**********************************************************//**
 * @brief Play one turn of a game.
 * @param field: The field to take a turn on.
 * @return Whether the gameplay succeeded.
 **************************************************************/
extern bool ai_PlayTurn(FIELD *field);

/*============================================================*/
#endif
