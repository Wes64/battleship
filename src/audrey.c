/*=========================================================*//**
 * @file audrey.c
 * @brief Implementation of Battleship AI
 *//*=========================================================*/

// Standard library
#include <limits.h> // INT_MAX
#include <assert.h> // assert

// This project
#include "audrey.h"
#include "ship.h"
#include "field.h"

/*============================================================*
 * Initialization
 *============================================================*/
int audrey_Create(Audrey *audrey) {
    // Initialize this instance of Audrey
    
    // Setup
    field_Clear(&audrey->field);
    audrey->turns = 1;
    
    int i = 0;
    while (audrey->sink_turn[i] = 0, i++ < N_SHIPS);
    
    // Success
    return 0;
}

/*============================================================*
 * Turn chooser
 *============================================================*/
int audrey_ChooseTurn(const Audrey *audrey, int *out_x, int *out_y) {
    // Choose which move to make
    
    // Get the maximum length remaining
    int ship_id=0, length_min=INT_MAX;
    while (ship_id < N_SHIPS) {
        if (audrey->field.ship_health[ship_id]) {
            int temp = ship_GetLength(ship_id);
            length_min = (temp < length_min)? temp: length_min;
        }
    }
    
    // Get probability maximum
    int prob_max = -1;
    int prob_x = -1;
    int prob_y = -1;
    
    int x, y;
    for (x=0; x<FIELD_SIZE; x++) {
        for (y=0; y<FIELD_SIZE; y++) {
            // Calculate the probability here
            int view_left = field_GetExtent(&audrey->field, LEFT, x, y, UNTRIED);
            int view_right = field_GetExtent(&audrey->field, RIGHT, x, y, UNTRIED);
            int view_up = field_GetExtent(&audrey->field, UP, x, y, UNTRIED);
            int view_down = field_GetExtent(&audrey->field, DOWN, x, y, UNTRIED);
            
            // Calculate the audrey->field hits nearby
            int nearby=0;
            nearby += field_GetExtent(&audrey->field, LEFT, x, y, HIT);
            nearby += field_GetExtent(&audrey->field, RIGHT, x, y, HIT);
            nearby += field_GetExtent(&audrey->field, UP, x, y, HIT);
            nearby += field_GetExtent(&audrey->field, DOWN, x, y, HIT);
            
            // View is blocked when total distance is less than the length of the
            // smallest ship
            int prob;
            // TODO verify this works without doing min-nearby
            if ((view_right+view_left < length_min) && (view_up+view_down < length_min)) {
                prob = 0;
            } else {
                prob = (view_left*view_right) + (view_up*view_down);
                // Nearness weight
                prob += (nearby - 4)*HIT_WEIGHT;
            }
            
            // Check probability maximum
            if (prob > prob_max) {
                prob_max = prob;
                prob_x = x;
                prob_y = y;
            }
        }
    }
    
    // Receive probability
    *out_x = prob_x;
    *out_y = prob_y;
    return 0;
}

/*============================================================*
 * Game driver
 *============================================================*/
int audrey_Play(Audrey *audrey) {
    
    // Execute one game
    while (!field_Win(&audrey->field)) {
        // Get the current option
        int x, y;
        audrey_ChooseTurn(audrey, &x, &y);
        
        // Apply the hit
        field_Attack(&audrey->field, x, y);
        int is_hit = audrey->field.entry[x][y].status == HIT;
        
        // Get the turn sunk for newly sunk ships
        if (is_hit) {
            Ship ship = audrey->field.entry[x][y].ship;
            assert(ship >= 0);
            audrey->sink_turn[ship] = audrey->turns;
        }
        
        // Advance turn
        audrey->turns++;
    }
    
    // Success
    return 0;
}

/*============================================================*/