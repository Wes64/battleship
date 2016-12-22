/**********************************************************//**
 * @file audrey.c
 * @brief Implementation of Battleship AI
 * @author Wes64
 **************************************************************/

// Standard library
#include <stdio.h>      // fprintf, stderr
#include <stdbool.h>    // bool
#include <limits.h>     // INT_MAX

// This project
#include "debug.h"      // eprintf, assert
#include "audrey.h"     // AUDREY
#include "ship.h"       // SHIP, N_SHIPS
#include "field.h"      // FIELD

/*============================================================*
 * Initialization
 *============================================================*/
void audrey_Create(AUDREY *audrey) {
    
    // Set up the field
    field_Clear(&audrey->field);
    
    // Initialize turn count
    audrey->turns = 0;
    for (int i = 0; i < N_SHIPS; i++) {
        audrey->sink_turn[i] = 0;
    }
}

/*============================================================*
 * Turn chooser
 *============================================================*/
bool audrey_PlayTurn(AUDREY *audrey) {
    
    // Get the maximum length remaining
    int length_min = INT_MAX;
    int temp;
    int length;
    int health;
    for (SHIP ship_id = 0; ship_id < N_SHIPS; ship_id++) {
        if ((health = audrey->field.ship_health[ship_id]) > 0) {
            length = ship_GetLength(ship_id);
            temp = (length == health)? length: ((length - health)/2);
            length_min = (temp < length_min)? temp: length_min;
        }
    }
    
    // Get probability maximum
    int prob_max = -1;
    int prob_x = -1;
    int prob_y = -1;
    for (int x = 0; x < FIELD_SIZE; x++) {
        for (int y = 0; y < FIELD_SIZE; y++) {
            // Already tried
            if (audrey->field.entry[x][y].status != UNTRIED) {
                continue;
            }

            // Calculate the probability here (all guaranteed >= 1 if UNTRIED)
            int view_left  = field_GetExtent(&audrey->field, LEFT,  x, y, UNTRIED);
            int view_right = field_GetExtent(&audrey->field, RIGHT, x, y, UNTRIED);
            int view_up    = field_GetExtent(&audrey->field, UP,    x, y, UNTRIED);
            int view_down  = field_GetExtent(&audrey->field, DOWN,  x, y, UNTRIED);
            assert(view_left  >= 1);
            assert(view_right >= 1);
            assert(view_up    >= 1);
            assert(view_down  >= 1);

            // Calculate the audrey->field hits nearby
            int near_horizontal = 0, near_vertical = 0, temp;
            if ((temp = field_GetExtent(&audrey->field, LEFT, x-1, y, HIT)) > 0) {
                near_horizontal += temp;
            }
            if ((temp = field_GetExtent(&audrey->field, RIGHT, x+1, y, HIT)) > 0) {
                near_horizontal += temp;
            }
            if ((temp = field_GetExtent(&audrey->field, UP, x, y-1, HIT)) > 0) {
                near_vertical += temp;
            }
            if ((temp = field_GetExtent(&audrey->field, DOWN, x, y+1, HIT)) > 0) {
                near_vertical += temp;
            }
            assert(near_horizontal >= 0);
            assert(near_vertical >= 0);
            
            // Blocked
            int blocked_horizontal = (view_right+view_left-1) < (length_min-near_horizontal);
            int blocked_vertical = (view_up+view_down-1) < (length_min-near_vertical);
            
            // View is blocked when total distance is less than the length of the
            // smallest ship
            int prob;
            if (blocked_horizontal && blocked_vertical) {
                prob = 0;
            } else {
                prob = (view_left*view_right) + (view_up*view_down);
                // Nearness weight
                prob += (near_horizontal+near_vertical)*FIELD_SIZE*FIELD_SIZE;
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
    if (!field_Attack(&audrey->field, prob_x, prob_y)) {
        eprintf("Attack at (%d, %d) failed.\n", prob_x, prob_y);
        return false;
    }
    assert(audrey->field.entry[prob_x][prob_y].status != UNTRIED);
    
    // Advance turn
    if (audrey->turns > TURN_MAX) {
        eprintf("Turn overflow\n");
        return false;
    }
    audrey->turns++;

    // Get the turn sunk for newly sunk ships
    if (audrey->field.entry[prob_x][prob_y].status == SUNK) {
        SHIP ship = audrey->field.entry[prob_x][prob_y].ship;
        assert(ship >= 0);
        audrey->sink_turn[ship] = audrey->turns;
    }
    
    // Done
    return true;
}

/*============================================================*
 * Game driver
 *============================================================*/
bool audrey_Play(AUDREY *audrey, FILE *output) {
    
    // Print the ship configuration
    if (output) {
        fprintf(output, "===== SHIP configuration =====\n");
        field_PrintShips(&audrey->field, output);
        fprintf(output, "\n");
        fflush(output);
    }
    
    // Execute one game
    while (!field_Win(&audrey->field)) {
        // Get the current option
        if (!audrey_PlayTurn(audrey)) {
            eprintf("Failed to play turn.\n");
            return false;
        }
        
        // Output file
        if (output) {
            fprintf(output, "===== Turn %d status =====\n", audrey->turns);
            field_PrintStatus(&audrey->field, output);
            fprintf(output, "\n");
            fflush(output);
        }
    }
    
    // Print the final stats
    if (output) {
        fprintf(output, "===== Final stats =====\n");
        fprintf(output, "Turns: %d\n", audrey->turns);
        for (SHIP ship_id = 0; ship_id < N_SHIPS; ship_id++) {
            fprintf(output, "%s sink turn: %d\n", ship_GetName(ship_id), audrey->sink_turn[ship_id]);
        }
        fflush(output);
    }
    
    // Success
    return true;
}

/*============================================================*/
