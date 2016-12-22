/**********************************************************//**
 * @file audrey.h
 * @brief Definition of Battleship AI
 * @author Wes64
 **************************************************************/

#ifndef _AUDREY_H_
#define _AUDREY_H_

// Standard library
#include <stdio.h>      // FILE
#include <stdbool.h>    // bool

// This project
#include "field.h"      // FIELD, FIELD_SIZE
#include "ship.h"       // N_SHIPS

/**********************************************************//**
 * @struct AUDREY
 * @brief Struct for AI data.
 **************************************************************/
typedef struct {
    FIELD field;            ///< The field the AI is playing on
    char turns;             ///< The turn count
    char sink_turn[N_SHIPS];///< When each ship sank
} AUDREY;

/// @brief The maximum number of turns
#define TURN_MAX (FIELD_SIZE*FIELD_SIZE)

/**********************************************************//**
 * @brief Initializes the AI parameters.
 * @param audrey: The AI instance to initialize.
 **************************************************************/
extern void audrey_Create(AUDREY *audrey);

/**********************************************************//**
 * @brief Get the turn count.
 * @param audrey: The AI that plays the game.
 * @return The number of turns taken.
 **************************************************************/
static inline int audrey_GetTurnCount(const AUDREY *audrey) {
    return audrey->turns;
}

/**********************************************************//**
 * @brief Get the turn the given ship sank.
 * @param audrey: The AI that plays the game.
 * @param ship: The identity of a ship.
 * @return The number of turns taken to sink the ship.
 **************************************************************/
static inline int audrey_GetSinkTurn(const AUDREY *audrey, SHIP ship) {
    return audrey->sink_turn[ship];
}

/**********************************************************//**
 * @brief Play an entire game.
 * @param audrey: The AI that plays the game.
 * @param output: File to output game data to or NULL.
 * @return Whether the gameplay succeeded.
 **************************************************************/
extern bool audrey_Play(AUDREY *audrey, FILE *output);

/**********************************************************//**
 * @brief Play one turn of a game.
 * @param audrey: The AI that plays the game.
 * @return Whether the gameplay succeeded.
 **************************************************************/
extern bool audrey_PlayTurn(AUDREY *audrey);

/*============================================================*/
#endif
