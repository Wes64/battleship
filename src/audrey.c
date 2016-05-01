/*=========================================================*//**
 * @file audrey.c
 * @brief Implementation of Battleship AI
 *//*=========================================================*/

// Standard library
#include <stdio.h>      // fprintf, stderr
#include <limits.h>     // INT_MAX
#include <assert.h>     // assert

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
int audrey_PlayTurn(Audrey *audrey) {
    // Choose which move to make
    
    // Get the maximum length remaining
    int ship_id = 0, length_min = INT_MAX;
    while (ship_id < N_SHIPS) {
        if (audrey->field.ship_health[ship_id] > 0) {
            int temp = ship_GetLength(ship_id);
            length_min = (temp < length_min)? temp: length_min;
        }
        ship_id++;
    }
    
    // Get probability maximum
    int prob_max = -1;
    int prob_x = -1;
    int prob_y = -1;
    
    int x, y;
    for (x=0; x<FIELD_SIZE; x++) {
        for (y=0; y<FIELD_SIZE; y++) {
            // Already tried
            if (audrey->field.entry[x][y].status != UNTRIED) {
                continue;
            }

            // Calculate the probability here
            int view_left  = field_GetExtent(&audrey->field, LEFT,  x, y, UNTRIED);
            int view_right = field_GetExtent(&audrey->field, RIGHT, x, y, UNTRIED);
            int view_up    = field_GetExtent(&audrey->field, UP,    x, y, UNTRIED);
            int view_down  = field_GetExtent(&audrey->field, DOWN,  x, y, UNTRIED);

            // Calculate the audrey->field hits nearby
            int nearby = 0;
            nearby += field_GetExtent(&audrey->field, LEFT,  x, y, HIT);
            nearby += field_GetExtent(&audrey->field, RIGHT, x, y, HIT);
            nearby += field_GetExtent(&audrey->field, UP,    x, y, HIT);
            nearby += field_GetExtent(&audrey->field, DOWN,  x, y, HIT);
            
            // View is blocked when total distance is less than the length of the
            // smallest ship
            int prob;
            if (((view_right+view_left) < (length_min-nearby)) && ((view_up+view_down) < (length_min-nearby))) {
                prob = 0;

            } else {
                prob = ((1+view_left)*(1+view_right)) + ((1+view_up)*(1+view_down));
                // Nearness weight
                prob += nearby*HIT_WEIGHT;
                assert(prob > 0);
            }
            assert(prob >= 0);
            
            // Check probability maximum
            if (prob > prob_max) {
                prob_max = prob;
                prob_x = x;
                prob_y = y;
            }
        }
    }
    assert(prob_max >= 0);
    assert(audrey->field.entry[prob_x][prob_y].status == UNTRIED);

    // Make attack
    if (field_Attack(&audrey->field, prob_x, prob_y)) {
        fprintf(stderr, "field_Attack at (%d, %d) failed\n", prob_x, prob_y);
        return -1;
    }
    assert(audrey->field.entry[prob_x][prob_y].status != UNTRIED);

    int is_hit = audrey->field.entry[prob_x][prob_y].status == HIT;
    
    // Get the turn sunk for newly sunk ships
    if (is_hit) {
        Ship ship = audrey->field.entry[prob_x][prob_y].ship;
        assert(ship >= 0);
        audrey->sink_turn[ship] = audrey->turns;
    }
    
    // Advance turn
    if (audrey->turns > TURN_MAX) {
        fprintf(stderr, "Turn overflow\n");
        return -1;
    }
    audrey->turns++;
    
    return 0;
}

/*============================================================*
 * Game driver
 *============================================================*/
int audrey_Play(Audrey *audrey) {
    
    // Execute one game
    while (!field_Win(&audrey->field)) {
        // Get the current option
        if (audrey_PlayTurn(audrey)) {
            fprintf(stderr, "audrey_PlayTurn failed\n");
            return -1;
        }
    }
    
    // Offset for advanced turn
    audrey->turns--;
    
    // Success
    return 0;
}

/*============================================================*/
