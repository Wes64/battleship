/**********************************************************//**
 * @file ai.h
 * @brief Definition of Battleship AI
 * @author Rena Shinomiya
 * @date April 3, 2017
 **************************************************************/

#ifndef _AI_H_
#define _AI_H_

#include <stdbool.h>
#include <stdio.h>

#include "field.h"

/**********************************************************//**
 * @brief Play one turn of a game.
 * @param field: The field to take a turn on.
 * @return Whether the gameplay succeeded.
 **************************************************************/
extern bool ai_PlayTurn(FIELD *field);

/**************************************************************/
#endif // _AI_H_
