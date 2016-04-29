/*=========================================================*//**
 * @file field.c
 * @brief Implementation of Battleship field
 *//*=========================================================*/

// Standard library
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

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
 * Field creation
 *============================================================*/
int field_Create(Field *field) {
    // Create an empty field
    
    Field memory = (Field)malloc(sizeof(Entry)*FIELD_SIZE*FIELD_SIZE + sizeof(Entry *)*FIELD_SIZE);
    if (field == NULL) {
        // malloc failed
        return -1;
    }
    
    // Memory is one contiguous block
    // [10 Entry *][100 Entry]
    // We will use the [10 Entry *] to point at blocks of 10 Entry
    // in the [100 Entry] block. Thus we can free everything at once.
    
    // Form the second-level array
    int i = 0;
    while (i < FIELD_SIZE) {
        memory[i] = (Entry *)&memory[FIELD_SIZE+i];
        i++;
    }
    
    // Set all to empty
    field_Clear(memory);
    
    // Set up the field
    *field = memory;

    // Success
    return 0;
}

/*============================================================*
 * Random field loading
 *============================================================*/
int field_LoadRandom(Field field) {
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
 * Clear a field
 *============================================================*/
int field_Clear(Field field) {
    // Wipe all squares in the field
    
    if (!field) {
        return -1;
    }
    
    int i, j;
    for (i=0; i < FIELD_SIZE; i++) {
        for (j=0; j < FIELD_SIZE; j++) {
            // Initialize Entry
            field[i][j].status = FREE;
            field[i][j].ship = EMPTY;
        }
    }
    return 0;
}

/*============================================================*
 * Field destruction
 *============================================================*/
void field_Destroy(Field field) {
    // Just free all at once
    free((void *)field);
}

/*============================================================*
 * Get ready to play on this field
 *============================================================*/
int field_ReadyPlay(Field field) {
    // Ready the field for play
    
    // Make all statuses UNTRIED
    int x, y;
    for (x=0; x < FIELD_SIZE; x++) {
        for (y=0; y < FIELD_SIZE; y++) {
            // Use special FREE token here to denote if square occupied
            field[x][y].status = UNTRIED;
        }
    }
    return 0;
}

/*============================================================*
 * Field bounds check
 *============================================================*/
int field_IsInBounds(const Field field, int x, int y) {
    // Check if the coordinates are in bounds
    // Don't need the field pointer, just included for uniformity
    
    // Bounds check
    return ((0 <= x) && (x < FIELD_SIZE)) && ((0 <= y) && (y < FIELD_SIZE));
}

/*============================================================*
 * Field ships check
 *============================================================*/
int field_GetShipsRemaining(const Field field, char *out_info) {
    // Assume out_info is a char[N_SHIPS]
    
    // Null pointer error
    if (out_info == NULL) {
        return -1;
    }
    
    // Blank the output array
    int i;
    for (i=0; i<N_SHIPS; out_info[i++]=0);
    
    // Determine which ships remain on the map
    int x, y;
    for (x=0; x < FIELD_SIZE; x++) {
        for (y=0; y < FIELD_SIZE; y++) {
            // Check if the value is a ship
            Ship ship_id = field[x][y].ship;
            Status status = field[x][y].status;
            if (ship_id != EMPTY && status == HIT) {
                // Ships exists and was hit
                out_info[(int)ship_id]++;
            }
        }
    }
    
    // Success
    return 0;
}

/*============================================================*
 * Field analysis
 *============================================================*/
int field_GetExtent(const Field field, View dir, int x, int y, Status status) {
    // Get the distance from x, y to an obstruction on the field
    
    if (!field_IsInBounds(field, x, y)) {
        // Error, not in bounds whatsoever
        return -1;
    }
    
    // Accumulator
    int distance = 0;
    
    // Steps
    int di, dj;
    if (!view_GetVector(dir, &di, &dj)) {
        fprintf(stderr, "view_GetVector failed\n");
        return -1;
    }
    
    // Get horizontal distance to an obstruction
    int i=x, j=y;
    while (field_IsInBounds(field, i, j) && status == field[i][j].status) {
        i+=di, j+=dj, distance++;
    }
    
    // Done (gurantee >= 0)
    assert(distance >= 0);
    return distance;
}

/*============================================================*
 * Place a ship on the field
 *============================================================*/
int field_PlaceShip(const Field field, View view, int x, int y, Ship ship_id) {
    // Put a ship on the field
    
    int length = ship_GetLength(ship_id);
    if (field_GetExtent(field, view, x, y, FREE) < length) {
        // Doesn't fit
        return -1;
    }
    
    // Steps
    int di, dj;
    if (!view_GetVector(view, &di, &dj)) {
        fprintf(stderr, "view_GetVector failed\n");
        return -1;
    }
    
    // Write ship into array
    int i=x, j=y, distance=0;
    while (distance < length) {
        // Ensure valid coordinates are here
        assert(field_IsInBounds(field, i, j));
        assert(field[i][j].ship == EMPTY);
        
        // Write the ship element
        field[i][j].ship = ship_id;
        field[i][j].status = UNTRIED;
        distance++;
        
        // Advance
        i += di;
        j += dj;
    }
    return 0;
}

/*============================================================*
 * Attack a square on the field
 *============================================================*/
int field_Attack(Field field, int x, int y) {
    // Attack the UNTRIED square
    if (!field_IsInBounds(field, x, y)) {
        // Out of bounds
        return -1;
    } else if (field[x][y].status != UNTRIED) {
        // Already tried this square
        return -1;
    }
    
    // Apply attack
    Ship ship = field[x][y].ship;
    field[x][y].status = (ship == EMPTY)? MISS: HIT;
    return 0;
}

/*============================================================*
 * Field win
 *============================================================*/
int field_Win(const Field field) {
    // Check win condition - return 0 on False, 1 on True
    
    // Get which ships are remaining
    char remaining[N_SHIPS];
    if (field_GetShipsRemaining(field, remaining)) {
        fprintf(stderr, "field_GetShipsRemaining failed\n");
        return -1;
    }
    
    // Get the maximum length remaining
    int ship_id = 0;
    while (ship_id < N_SHIPS) {
        if (remaining[ship_id]) {
            return 0;
        }
    }
    
    // No ships remain
    return 1;
}

/*============================================================*/