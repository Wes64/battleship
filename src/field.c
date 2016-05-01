/*=========================================================*//**
 * @file field.c
 * @brief Implementation of Battleship field
 *//*=========================================================*/

// Standard library
#include <stdio.h>      // fprintf, stderr
#include <assert.h>     // assert
#include <stdlib.h>     // rand

// This project
#include "field.h"
#include "ship.h"

/*============================================================*
 * View vector
 *============================================================*/
int view_GetVector(View dir, int *out_di, int *out_dj) {
    // Use out_di and out_dj as output parameters
    
    // Check for null pointers
    if (out_di && out_dj) {
        *out_di = (dir == LEFT)? -1: (dir == RIGHT)? 1: 0;
        *out_dj = (dir == UP)? -1: (dir == DOWN)? 1: 0;
        assert(out_di || out_dj);
        return 0;
    }
    
    // Failure
    return -1;
}

/*============================================================*
 * Clear a field
 *============================================================*/
int field_Clear(Field *field) {
    // Wipe all squares in the field
    if (!field) {
        return -1;
    }
    
    // Set all entries to null
    int i, j;
    for (i=0; i < FIELD_SIZE; i++) {
        for (j=0; j < FIELD_SIZE; j++) {
            // Initialize Entry
            field->entry[i][j].status = FREE;
            field->entry[i][j].ship = EMPTY;
        }
    }
    
    // Set all ship health to empty
    for (i=0; i < N_SHIPS; field->ship_health[i++] = -1);
    
    // Success
    return 0;
}

/*============================================================*
 * Random field loading
 *============================================================*/
int field_LoadRandom(Field *field) {
    // Use field as output parameter
    
    // Clear the field
    int x, y;
    field_Clear(field);
    
    // Generate each ship
    Ship ship_id;
    for (ship_id=0; (int)ship_id < N_SHIPS; ship_id++) {
        
        // Get the ship length
        int length = ship_GetLength(ship_id);
        
        // Generate positions until we find one that works
        // We can ban any position where the ship will extend
        // off of the board with this anchor (assume the ship extends
        // horizontally or vertically away from (0, 0))
        int anchor = (FIELD_SIZE - length + 1);
        
        // Assume: no deadlock caused by ships here
        View view;
        do {
            // Generate random position
            x = rand() % anchor;
            y = rand() % anchor;

            // Randomly generate view
            view = (rand() % 2)? RIGHT: DOWN;
        } while (field_PlaceShip(field, view, x, y, ship_id));
    }
    
    // All ships generated properly
    return field_ReadyPlay(field);
}

/*============================================================*
 * Get ready to play on this field
 *============================================================*/
int field_ReadyPlay(Field *field) {
    // Ready the field for play
    
    // Make all statuses UNTRIED
    int x, y;
    for (x=0; x < FIELD_SIZE; x++) {
        for (y=0; y < FIELD_SIZE; y++) {
            // Use special FREE token here to denote if square occupied
            field->entry[x][y].status = UNTRIED;
        }
    }
    return 0;
}

/*============================================================*
 * Field bounds check
 *============================================================*/
int field_IsInBounds(const Field *field, int x, int y) {
    // Check if the coordinates are in bounds
    // Don't need the field pointer, just included for uniformity
    
    // Bounds check
    return ((0 <= x) && (x < FIELD_SIZE)) && ((0 <= y) && (y < FIELD_SIZE));
}

/*============================================================*
 * Field analysis
 *============================================================*/
int field_GetExtent(const Field *field, View dir, int x, int y, Status status) {
    // Get the distance from x, y to an obstruction on the field
    
    if (!field_IsInBounds(field, x, y)) {
        // Error, not in bounds whatsoever
        return -1;
    }
    
    // Accumulator
    int distance = 0;
    
    // Steps
    int di, dj;
    if (view_GetVector(dir, &di, &dj)) {
        fprintf(stderr, "view_GetVector failed\n");
        return -1;
    }
    
    // Get horizontal distance to an obstruction
    int i=x, j=y;
    while (field_IsInBounds(field, i, j) && status == field->entry[i][j].status) {
        i+=di, j+=dj, distance++;
    }
    
    // Done (gurantee >= 0)
    assert(distance >= 0);
    return distance;
}

/*============================================================*
 * Place a ship on the field
 *============================================================*/
int field_PlaceShip(Field *field, View view, int x, int y, Ship ship_id) {
    // Put a ship on the field
    
    int length = ship_GetLength(ship_id);
    if (field_GetExtent(field, view, x, y, FREE) < length) {
        // Doesn't fit
        return -1;
    }
    
    // Steps
    int di, dj;
    if (view_GetVector(view, &di, &dj)) {
        fprintf(stderr, "view_GetVector failed\n");
        return -1;
    }
    
    // Write ship into array
    int i=x, j=y, distance=0;
    while (distance < length) {
        // Ensure valid coordinates are here
        assert(field_IsInBounds(field, i, j));
        assert(field->entry[i][j].ship == EMPTY);
        
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
    
    // Success
    return 0;
}

/*============================================================*
 * Attack a square on the field
 *============================================================*/
int field_Attack(Field *field, int x, int y) {
    // Attack the UNTRIED square
    if (!field_IsInBounds(field, x, y)) {
        // Out of bounds
        return -1;
    } else if (field->entry[x][y].status != UNTRIED) {
        // Already tried this square
        return -1;
    }
    
    // Apply attack
    Ship ship = field->entry[x][y].ship;
    int is_hit = ship != EMPTY;
    
    field->entry[x][y].status = (is_hit)? HIT: MISS;
    
    // Change HP
    if (is_hit) {
        field->ship_health[ship]--;
        assert(field->ship_health[ship] >= 0);
    }
    
    return 0;
}

/*============================================================*
 * Field win
 *============================================================*/
int field_Win(const Field *field) {
    // Check win condition - return 0 on False, 1 on True
    
    // Get the maximum length remaining
    int ship_id = 0;
    while (ship_id < N_SHIPS) {
        if (field->ship_health[ship_id]) {
            return 0;
        }
    }
    
    // No ships remain
    return 1;
}

/*============================================================*/
