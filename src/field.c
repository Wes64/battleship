/**********************************************************//**
 * @file field.c
 * @brief Implementation of Battleship field
 * @author Rena Shinomiya
 **************************************************************/

// Standard library
#include <stdbool.h>    // bool
#include <stdlib.h>     // rand
#include <stdio.h>      // FILE

// This project
#include "debug.h"      // eprintf, assert
#include "field.h"      // FIELD

/**********************************************************//**
 * @brief Enumerates the lengths of each valid ship.
 **************************************************************/
static const int SHIP_LENGTH[N_SHIPS] = {
	[CARRIER]    = 5,
	[BATTLESHIP] = 4,
	[SUBMARINE]  = 3,
	[CRUISER]    = 3,
	[DESTROYER]  = 2,
};

/**********************************************************//**
 * @brief Converts a VIEW direction to a 2D vector.
 * @param view: The viewing direction.
 * @param di: Output parameter for the x coordinate.
 * @param dj: Output parameter for the y coordinate.
 **************************************************************/
static inline void view_GetVector(VIEW view, int *di, int *dj) {
    switch (view) {
    case LEFT:
        *di = -1;
        *dj = 0;
        break;
    case RIGHT:
        *di = 1;
        *dj = 0;
        break;
    case UP:
        *di = 0;
        *dj = -1;
        break;
    case DOWN:
        *di = 0;
        *dj = 1;
        break;
    default:
        break;
    }
}

/*============================================================*
 * Ship length
 *============================================================*/
int field_GetShipLength(SHIP ship) {
	return SHIP_LENGTH[ship];
}

/*============================================================*
 * Create a new empty field.
 *============================================================*/
void field_Clear(FIELD *field) {
    // Set all entries to null
    for (int i = 0; i < FIELD_SIZE; i++) {
        for (int j = 0; j < FIELD_SIZE; j++) {
            field->entry[i][j].status = FREE;
            field->entry[i][j].ship = EMPTY;
        }
    }
    // Set all ship health to empty
    for (int i = 0; i < N_SHIPS; i++) {
        field->health[i] = -1;
    }
    // Reset the turn counts
    field->turns = 0;
    for (int i = 0; i < N_SHIPS; i++) {
        field->sinkTurn[i] = TURN_INVALID;
    }
}

/*============================================================*
 * Field analysis
 *============================================================*/
int field_GetExtent(const FIELD *field, VIEW dir, int x, int y, STATUS status) {
    // Get the distance from x, y to an obstruction on the field
    if (!field_IsInBounds(x, y)) {
        // Error, not in bounds whatsoever
        return 0;
    }
    
    // Get the directions
    int distance = 0;
    int di = 0;
    int dj = 0;
    view_GetVector(dir, &di, &dj);
    
    // Get horizontal distance to an obstruction (tile without the same status)
    int i = x;
    int j = y;
    while (field_IsInBounds(i, j) && (status == field->entry[i][j].status)) {
        i += di;
        j += dj;
        distance++;
    }
    return distance;
}

/**********************************************************//**
 * @brief Places a ship on the field.
 * @param field: The field to modify.
 * @param view: The viewing direction.
 * @param x: The x-coordinate of the ship's upper left corner.
 * @param y: The y-coordinate of the ship's upper left corner.
 * @param ship: The ship to place.
 **************************************************************/
static bool field_PlaceShip(FIELD *field, VIEW view, int x, int y, SHIP ship) {
    // Put a ship on the field
    int length = SHIP_LENGTH[ship];
    if (field_GetExtent(field, view, x, y, FREE) < length) {
        // Can't place the ship here but not an error
        return false;
    }
    
    // Get the viewing vector
    int di = 0;
    int dj = 0;
    view_GetVector(view, &di, &dj);
    
    // Write ship into array
    int i = x;
    int j = y;
    int distance = 0;
    while (distance < length) {
        // Ensure valid coordinates are here
        if (field->entry[i][j].ship != EMPTY) {
            eprintf("Ship erroneously placed at the location.\n");
            return false;
        }
        
        // Write the ship element
        field->entry[i][j].ship = ship;
        field->entry[i][j].status = UNTRIED;
        distance++;
        i += di;
        j += dj;
    }
    
    // Write initial ship HP
    field->health[ship] = length;
    return true;
}

/*============================================================*
 * Random field loading
 *============================================================*/
void field_CreateRandom(FIELD *field) {
    for (SHIP ship = 0; ship < N_SHIPS; ship++) {
        // Get the ship length
        int length = SHIP_LENGTH[ship];
        
        // Generate positions until we find one that works
        // We can ban any position where the ship will extend
        // off of the board with this anchor (assume the ship extends
        // horizontally or vertically away from (0, 0))
        int anchor = (FIELD_SIZE - length + 1);
        
        // Assume: no deadlock caused by ships here
        VIEW view;
        int x;
        int y;
        do {
            // Generate random position and view
            x = rand() % anchor;
            y = rand() % anchor;
            view = (rand() % 2)? RIGHT: DOWN;
        } while (!field_PlaceShip(field, view, x, y, ship));
    }
    
    // Make all statuses UNTRIED, finalizing the field
    for (int x = 0; x < FIELD_SIZE; x++) {
        for (int y = 0; y < FIELD_SIZE; y++) {
            field->entry[x][y].status = UNTRIED;
        }
    }
}

/*============================================================*
 * Attack a square on the field
 *============================================================*/
STATUS field_Attack(FIELD *field, int x, int y) {
    // Error check the input
    if (!field_IsInBounds(x, y)) {
        eprintf("Attack out of bounds.\n");
        return ERROR;
    } else if (field->entry[x][y].status != UNTRIED) {
        eprintf("Already attacked that location.\n");
        return ERROR;
    }
    
    // Register the turn
    field->turns++;
    
    // Apply attack
    SHIP ship = field->entry[x][y].ship;
    if (ship != EMPTY) {
		// The attack struck a ship
		field->entry[x][y].status = HIT;
        field->health[ship]--;
        
        // Check if the ship sank. If it did, mark the
        // entire ship with SUNK status.
		if (field->health[ship] <= 0) {
			for (int x = 0; x < FIELD_SIZE; x++) {
				for (int y = 0; y < FIELD_SIZE; y++) {
					if (field->entry[x][y].ship == ship) {
						field->entry[x][y].status = SUNK;
					}
				}
			}
			// Register the sink turn.
			field->sinkTurn[ship] = field->turns;
			return SUNK;
		} else {
			// The ship didn't sink
			return HIT;
		}
	} else {
		// The attack missed any ship
		field->entry[x][y].status = MISS;
		return MISS;
	}
}

/*============================================================*
 * Has the field been completed?
 *============================================================*/
bool field_IsWon(const FIELD *field) {
    for (SHIP ship = 0; ship < N_SHIPS; ship++) {
        if (field->health[ship] > 0) {
            return false;
        }
    }
    return true;
}

/*============================================================*/
