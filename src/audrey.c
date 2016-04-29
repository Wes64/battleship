/*=========================================================*//**
 * @file audrey.c
 * @brief Implementation of Battleship AI
 *//*=========================================================*/

// Standard library
#include <limits.h> // INT_MAX

// This project
#include "audrey.h"
#include "ship.h"
#include "field.h"

/*============================================================*
 * Initialization
 *============================================================*/
int audrey_Create(Audrey *audrey, Field field) {
    // Initialize this instance of Audrey
    
    // Null pointer exception
    if (!field || !audrey) {
        return -1;
    }
    
    // Setup
    audrey->field = field;
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
    
    // Get which ships are remaining
    char remaining[N_SHIPS];
    field_GetShipsRemaining(audrey->field, remaining);
    
    // Get the maximum length remaining
    int ship_id=0, length_min=INT_MAX;
    while (ship_id < N_SHIPS) {
        if (remaining[ship_id]) {
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
            int view_left = field_GetExtent(audrey->field, LEFT, x, y, UNTRIED);
            int view_right = field_GetExtent(audrey->field, RIGHT, x, y, UNTRIED);
            int view_up = field_GetExtent(audrey->field, UP, x, y, UNTRIED);
            int view_down = field_GetExtent(audrey->field, DOWN, x, y, UNTRIED);
            
            // Calculate the audrey->field hits nearby
            int nearby=0;
            nearby += field_GetExtent(audrey->field, LEFT, x, y, HIT);
            nearby += field_GetExtent(audrey->field, RIGHT, x, y, HIT);
            nearby += field_GetExtent(audrey->field, UP, x, y, HIT);
            nearby += field_GetExtent(audrey->field, DOWN, x, y, HIT);
            
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
    
    // Output for ship info
    char initial_info[N_SHIPS], final_info[N_SHIPS];
    
    // Main loop
    while (!field_Win(audrey->field)) {
        // Get the current option
        int x, y;
        audrey_ChooseTurn(audrey, &x, &y);
        
        // Apply the hit
        field_GetShipsRemaining(audrey->field, initial_info);
        field_Attack(audrey->field, x, y);
        field_GetShipsRemaining(audrey->field, final_info);
        
        // Get the turn sunk for newly sunk ships
        int i = 0;
        while (i < N_SHIPS) {
            if (initial_info[i] == 1 && final_info[i] == 0) {
                audrey->sink_turn[i] = audrey->turns;
             
            }
            i++;
        }
        
        // Advance turn
        audrey->turns++;
    }
    
    // Success
    return 0;
}

/*============================================================*
 * Destructor
 *============================================================*/
void audrey_Destroy(Audrey *audrey) {
    // Destroy the field only
    field_Destroy(audrey->field);
}

/*============================================================*/