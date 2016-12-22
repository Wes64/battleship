/**********************************************************//**
 * @file ship.h
 * @brief Definition of Battleship ships
 * @author Wes64
 **************************************************************/

#ifndef _SHIP_H_
#define _SHIP_H_

/**********************************************************//**
 * @enum SHIP
 * @brief All Battleship ship types.
 **************************************************************/
typedef enum {
    EMPTY      = -1,    ///< No ship is present here.
    CARRIER    =  0,    ///< The Carrier ship.
    BATTLESHIP =  1,    ///< The Battleship ship.
    SUBMARINE  =  2,    ///< The Submarine ship.
    CRUISER    =  3,    ///< The cruiser ship.
    DESTROYER  =  4,    ///< The destroyer ship.
} SHIP;

/// The total number of ships on the board.
#define N_SHIPS 5

/**********************************************************//**
 * @brief Get the name of a ship.
 * @param ship: The ship identity.
 * @return The name of the ship, or NULL on failure.
 **************************************************************/
extern const char *ship_GetName(SHIP ship);

/**********************************************************//**
 * @brief Get the length of a ship.
 * @param ship: The ship identity.
 * @return The length of the ship or -1 on failure.
 **************************************************************/
extern int ship_GetLength(SHIP ship);

/*============================================================*/
#endif