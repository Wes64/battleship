/**********************************************************//**
 * @file ai.h
 * @brief Definition of Battleship AI
 * @author Wes64
 **************************************************************/

#ifndef _AI_H_
#define _AI_H_

// Standard library
#include <stdio.h>      // FILE
#include <stdbool.h>    // bool

// This project
#include "field.h"      // FIELD, FIELD_SIZE
#include "ship.h"       // N_SHIPS

/**********************************************************//**
 * @struct AI
 * @brief Struct for AI data.
 **************************************************************/
typedef struct {
    FIELD field;            ///< The field the AI is playing on
    char turns;             ///< The turn count
    char sink_turn[N_SHIPS];///< When each ship sank
} AI;

/// @brief The maximum number of turns
#define TURN_MAX (FIELD_SIZE*FIELD_SIZE)

/**********************************************************//**
 * @brief Initializes the AI parameters.
 * @param ai: The AI instance to initialize.
 **************************************************************/
extern void ai_Create(AI *ai);

/**********************************************************//**
 * @brief Get the turn count.
 * @param ai: The AI that plays the game.
 * @return The number of turns taken.
 **************************************************************/
static inline int ai_GetTurnCount(const AI *ai) {
    return ai->turns;
}

/**********************************************************//**
 * @brief Get the turn the given ship sank.
 * @param ai: The AI that plays the game.
 * @param ship: The identity of a ship.
 * @return The number of turns taken to sink the ship.
 **************************************************************/
static inline int ai_GetSinkTurn(const AI *ai, SHIP ship) {
    return ai->sink_turn[ship];
}

/**********************************************************//**
 * @brief Play an entire game.
 * @param ai: The AI that plays the game.
 * @param output: File to output game data to or NULL.
 * @return Whether the gameplay succeeded.
 **************************************************************/
extern bool ai_Play(AI *ai, FILE *output);

/**********************************************************//**
 * @brief Play one turn of a game.
 * @param ai: The AI that plays the game.
 * @return Whether the gameplay succeeded.
 **************************************************************/
extern bool ai_PlayTurn(AI *ai);

/*============================================================*/
#endif
