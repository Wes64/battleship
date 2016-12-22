/**********************************************************//**
 * @file field.c
 * @brief Implementation of Battleship field
 * @author Wes64
 **************************************************************/

// Standard library
#include <stdio.h>      // fprintf, stderr, FILE
#include <stdbool.h>    // bool
#include <stdlib.h>     // rand

// This project
#include "debug.h"      // eprintf, assert
#include "field.h"      // FIELD
#include "ship.h"       // SHIP

/*============================================================*
 * Clear a field
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
        field->ship_health[i] = -1;
    }
}

/*============================================================*
 * View vector
 *============================================================*/
static bool view_GetVector(VIEW dir, int *out_di, int *out_dj) {

    switch (dir) {
    case LEFT:
        *out_di = -1;
        *out_dj = 0;
        break;
        
    case RIGHT:
        *out_di = 1;
        *out_dj = 0;
        break;
        
    case UP:
        *out_di = 0;
        *out_dj = -1;
        break;
    
    case DOWN:
        *out_di = 0;
        *out_dj = 1;
        break;
        
    default:
        return false;
    }
    return true;
}

/*============================================================*
 * Fiend analysis
 *============================================================*/
int field_GetExtent(const FIELD *field, VIEW dir, int x, int y, STATUS status) {
    
    // Get the distance from x, y to an obstruction on the field
    if (!field_IsInBounds(x, y)) {
        // Error, not in bounds whatsoever
        return -1;
    }
    
    // Get the directions
    int distance = 0;
    int di=0, dj=0;
    if (!view_GetVector(dir, &di, &dj)) {
        eprintf("Invalid view direction.\n");
        return -1;
    }
    
    // Get horizontal distance to an obstruction
    int i=x, j=y;
    while (field_IsInBounds(i, j) && (status == field->entry[i][j].status)) {
        i += di;
        j += dj;
        distance++;
    }
    assert(distance >= 0);
    return distance;
}

/*============================================================*
 * Place a ship on the field
 *============================================================*/
static bool field_PlaceShip(FIELD *field, VIEW view, int x, int y, SHIP ship_id) {
    
    // Put a ship on the field
    int length = ship_GetLength(ship_id);
    if (field_GetExtent(field, view, x, y, FREE) < length) {
        // Can't place the ship here but not an error
        return false;
    }
    
    // Steps
    int di=0, dj=0;
    if (!view_GetVector(view, &di, &dj)) {
        eprintf("Failed to get the view vector.\n");
        return false;
    }
    
    // Write ship into array
    int i=x, j=y, distance=0;
    while (distance < length) {
        // Ensure valid coordinates are here
        assert(field_IsInBounds(i, j));
        if (field->entry[i][j].ship != EMPTY) {
            eprintf("Ship already placed at the location.\n");
            return false;
        }
        
        // Write the ship element
        field->entry[i][j].ship = ship_id;
        field->entry[i][j].status = UNTRIED;
        distance++;
        
        // Advance
        i += di;
        j += dj;
    }
    
    // Write initial ship HP
    field->ship_health[ship_id] = length;
    return true;
}

/*============================================================*
 * Random field loading
 *============================================================*/
void field_LoadRandom(FIELD *field) {

    // Generate each ship
    field_Clear(field);
    for (SHIP ship_id = 0; ship_id < N_SHIPS; ship_id++) {
        
        // Get the ship length
        int length = ship_GetLength(ship_id);
        assert(length > 0);
        
        // Generate positions until we find one that works
        // We can ban any position where the ship will extend
        // off of the board with this anchor (assume the ship extends
        // horizontally or vertically away from (0, 0))
        int anchor = (FIELD_SIZE - length + 1);
        assert(anchor >= 0);
        assert(anchor < FIELD_SIZE);
        
        // Assume: no deadlock caused by ships here
        VIEW view;
        int x, y;
        do {
            // Generate random position
            x = rand() % anchor;
            y = rand() % anchor;
            assert(field_IsInBounds(x, y));

            // Randomly generate view
            view = (rand() % 2)? RIGHT: DOWN;
        } while (!field_PlaceShip(field, view, x, y, ship_id));
    }
    
    // Make all statuses UNTRIED
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
    
    // Attack the UNTRIED square
    if (!field_IsInBounds(x, y)) {
        eprintf("Attack out of bounds.\n");
        return -1;
    } else if (field->entry[x][y].status != UNTRIED) {
        eprintf("Already attacked that location.\n");
        return -1;
    }
    
    // Apply attack
    SHIP ship = field->entry[x][y].ship;
    int is_hit = ship != EMPTY;
    field->entry[x][y].status = (is_hit)? HIT: MISS;
    
    // Change HP and sink ship
    if (is_hit) {
        field->ship_health[ship]--;
        assert(field->ship_health[ship] >= 0);
    }
    if (field->ship_health[ship] == 0) {
        for (int x = 0; x < FIELD_SIZE; x++) {
            for (int y = 0; y < FIELD_SIZE; y++) {
                if (field->entry[x][y].ship == ship) {
                    field->entry[x][y].status = SUNK;
                }
            }
        }
    }
    return field->entry[x][y].status;
}

/*============================================================*
 * Has the field been completed?
 *============================================================*/
bool field_Win(const FIELD *field) {
    for (SHIP ship_id = 0; ship_id < N_SHIPS; ship_id++) {
        if (field->ship_health[ship_id] > 0) {
            return false;
        }
    }
    return true;
}

/*============================================================*
 * Printing field status
 *============================================================*/
void field_PrintStatus(const FIELD *field, FILE *file) {
    for (int x = 0; x < FIELD_SIZE; x++) {
        for (int y = 0; y < FIELD_SIZE; y++) {
            switch (field->entry[x][y].status) {
            case UNTRIED:
                fprintf(file, ".");
                break;
                
            case MISS:
                fprintf(file, "X");
                break;
            
            case HIT:
                fprintf(file, "O");
                break;
            
            case SUNK:
                fprintf(file, "S");
                break;
                
             
             default:
                fprintf(file, "?");
                break;
            }
        }
        fprintf(file, "\n");
    }
}

/*============================================================*
 * Printing field ships
 *============================================================*/
void field_PrintShips(const FIELD *field, FILE *file) {
    for (int x = 0; x < FIELD_SIZE; x++) {
        for (int y = 0; y < FIELD_SIZE; y++) {
            switch (field->entry[x][y].ship) {
            case EMPTY:
                fprintf(file, ".");
                break;
             
             default:
                fprintf(file, "%d", field->entry[x][y].ship);
                break;
            }
        }
        fprintf(file, "\n");
    }
}


/*============================================================*/
