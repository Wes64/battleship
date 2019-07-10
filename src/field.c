/**********************************************************//**
 * @file field.c
 * @brief Implementation of an agent-agnostic battleship field.
 * @author Rena Shinomiya
 * @date April 28, 2016
 **************************************************************/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "debug.h"
#include "field.h"

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

/**********************************************************//**
 * @brief Get the length of the given ship.
 * @param ship: The ship to check.
 * @return The length of the ship.
 **************************************************************/
int field_GetShipLength(SHIP ship) {
    static const int length[N_SHIPS] = {
        [CARRIER]    = 5,
        [BATTLESHIP] = 4,
        [SUBMARINE]  = 3,
        [CRUISER]    = 3,
        [DESTROYER]  = 2,
    };
    return length[ship];
}

/**********************************************************//**
 * @brief Clear all items on the field. It is necessary to
 * clear a field before setting it up. This initializes the
 * field.
 * @param field: The field to initialize.
 **************************************************************/
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
    
    // Reset last attack
    field->lastAttackX = -1;
    field->lastAttackY = -1;
}

/**********************************************************//**
 * @brief Get the amount of equal statuses in the VIEW
 * direction from the origin point. This goes along the unit
 * vector of the direction from (x, y) until it is out of
 * bounds or reaches an entry with a different status.
 * @param field: The field to get information from.
 * @param dir: The view direction to look in.
 * @param x: The x-coordinate of the origin.
 * @param y: The y-coordinate of the origin.
 * @param status: The status of squares to check.
 * @return The distance travelled.
 **************************************************************/
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
 * @return True if the ship could be placed.
 **************************************************************/
static bool field_PlaceShip(FIELD *field, VIEW view, int x, int y, SHIP ship) {
    // Put a ship on the field
    int length = field_GetShipLength(ship);
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

/**********************************************************//**
 * @brief Places all the ships randomly on the field.
 * @param field: The field to set up.
 **************************************************************/
void field_CreateRandom(FIELD *field) {
    for (SHIP ship = 0; ship < N_SHIPS; ship++) {
        int length = field_GetShipLength(ship);

        // Generate positions until we find one that works
        // We ban any position where the ship will extend
        // off of the board. Hence, the top left corner is referred
        // to as an "anchor" point. If the ship points right, the
        // leftmost part of the ship can't be more than thr anchor.
        // If the ship points down, the topmost part of the ship
        // can't be more than the anchor.
        int anchor = (FIELD_SIZE - length + 1);

        // Randomly generate anchor points until we find a place that
        // works. An anchor point doesn't work if it causes ships to
        // overlap or extend off the board.
        VIEW view;
        int x;
        int y;
        do {
            // Pick if the ship is horizontal or vertical.
            view = (rand() % 2)? RIGHT: DOWN;
            // Generate random position for the view.
            switch (view) {
            case RIGHT:
                x = rand()%anchor;
                y = rand()%FIELD_SIZE;
                break;
            
            case DOWN:
                x = rand()%FIELD_SIZE;
                y = rand()%anchor;
                break;
            
            default:
                eprintf("This can't happen.\n");
                break;
            }
        } while (!field_PlaceShip(field, view, x, y, ship));
    }

    // Make all statuses UNTRIED, finalizing the field
    for (int x = 0; x < FIELD_SIZE; x++) {
        for (int y = 0; y < FIELD_SIZE; y++) {
            field->entry[x][y].status = UNTRIED;
        }
    }
}

/**********************************************************//**
 * @brief Make an attack on the field.
 * @param field: The field to attack.
 * @param x: The x-coordinate to attack.
 * @param y: The y-coordinate to attack.
 * @return The result of the attack or -1 on failure.
 **************************************************************/
STATUS field_Attack(FIELD *field, int x, int y) {
    // Error check the input
    if (!field_IsInBounds(x, y)) {
        eprintf("Attack out of bounds.\n");
        return ERROR;
    } else if (field->entry[x][y].status != UNTRIED) {
        eprintf("Already attacked that location.\n");
        return ERROR;
    }

    // Register the turn and attack
    field->turns++;
    field->lastAttackX = x;
    field->lastAttackY = y;
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

/**********************************************************//**
 * @brief Check if the field has been won.
 * @param field: The field to check.
 * @return Whether the field is complete or not.
 **************************************************************/
bool field_IsWon(const FIELD *field) {
    for (SHIP ship = 0; ship < N_SHIPS; ship++) {
        if (field->health[ship] > 0) {
            return false;
        }
    }
    return true;
}

/**********************************************************//**
 * @brief Prints the field using ASCII characters.
 * @param field: Field to output.
 * @param file:  Open file to output the field into.
 **************************************************************/
void field_Print(const FIELD *field, FILE *file) {
    for (int y=0; y<FIELD_SIZE; y++) {
        for (int x=0; x<FIELD_SIZE; x++) {
            bool isLastAttack = x==field->lastAttackX && y==field->lastAttackY;
            fprintf(file, isLastAttack? "[": " ");
            switch (field->entry[x][y].status) {
            case HIT:
            case SUNK:
                fprintf(file, "O");
                break;
            
            case MISS:
                fprintf(file, "X");
                break;
            
            default:
                fprintf(file, "?");
                break;
            }
            fprintf(file, isLastAttack? "]": " ");
        }
        fprintf(file, "\n");
    }
}

/**************************************************************/
