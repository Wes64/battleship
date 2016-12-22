/**********************************************************//**
 * @file ship.c
 * @brief Implementation of Battleship ship methods
 * @author Wes64
 **************************************************************/

// Standard library
#include <stddef.h> // NULL

// This project
#include "ship.h"   // SHIP

/*============================================================*
 * Ship length
 *============================================================*/
int ship_GetLength(SHIP ship) {
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
}

/*============================================================*
 * Ship name
 *============================================================*/
const char *ship_GetName(SHIP ship) {
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
}
 
/*============================================================*/