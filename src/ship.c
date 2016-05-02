/*=========================================================*//**
 * @file ship.c
 * @brief Implementation of Battleship ship methods
 *//*=========================================================*/

// Standard library
#include <stdlib.h>     // NULL

// This project
#include "ship.h"

/*============================================================*
 * Ship length
 *============================================================*/
int ship_GetLength(Ship ship) {
    // Get the ship length
    
    switch (ship) {
        case CARRIER:
            return 5;
        
        case BATTLESHIP:
            return 4;
        
        case SUBMARINE:
        case CRUISER:
            return 3;
        
        case DESTROYER:
            return 2;
        
        // Never happens
        default:
            return -1;
    }
    
    // Never get here
    return -1;
}

/*============================================================*
 * Ship name
 *============================================================*/
const char *ship_GetName(Ship ship) {
    // Get the ship length
    
    switch (ship) {
        case CARRIER:
            return "Carrier";
        
        case BATTLESHIP:
            return "Battleship";
        
        case SUBMARINE:
            return "Submarine";
        
        case CRUISER:
            return "Cruiser";
        
        case DESTROYER:
            return "Destroyer";
        
        // Never happens
        default:
            return NULL;
    }
    
    // Never get here
    return NULL;
}
 
/*============================================================*/