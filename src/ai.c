/**********************************************************//**
 * @file ai.c
 * @brief Implementation of Battleship AI
 * @author Rena Shinomiya
 **************************************************************/

// Standard library
#include <stdbool.h>    // bool
#include <limits.h>     // INT_MAX
#include <math.h>		// log2

// This project
#include "debug.h"      // eprintf, assert
#include "field.h"      // FIELD
#include "ai.h"         // AI

/**********************************************************//**
 * @brief Get the length of the longest ship remaining.
 * @param field: The field to check.
 * @param full: Output parameter for the minimum length
 * that is not next to any hits.
 * @param partial: Output parameter for the minimum length
 * that is next to some hits.
 * @return The longest ship remaining on the field.
 **************************************************************/
static inline void ai_GetMinimumLength(const FIELD *field, int *full, int *partial) {
	// Get the minimum length remaining
    int lengthMin = INT_MAX;
    int partialMin = INT_MAX;
    for (SHIP ship = 0; ship < N_SHIPS; ship++) {
		// Check if the ship is actually afloat.
		int health = field_GetShipHealth(field, ship);
        if (health > 0) {
            // Get the minimum fragment size if the ship is hit.
            int length = field_GetShipLength(ship);
            if (health < length) {
				// Smallest fragment is when everything is in
				// the middle of the ship.
				int fragmentMin = (int)log2(length - health);
				if (fragmentMin == 0) {
					fragmentMin = 1;
				}
				
				// Update minimum length
				if (fragmentMin < partialMin) {
					partialMin = fragmentMin;
				}
			} else if (length < lengthMin) {
				// An unhit ship remains
				lengthMin = length;
				if (lengthMin < partialMin) {
					partialMin = lengthMin;
				}
			}
        }
    }
    
    // Output parameters
    assert(partialMin <= lengthMin);
    if (full) {
		*full = lengthMin;
	}
	if (partial) {
		*partial = partialMin;
	}
}

/*============================================================*
 * Turn chooser
 *============================================================*/
bool ai_PlayTurn(FIELD *field) {
    int fullMin;
    int partialMin;
    ai_GetMinimumLength(field, &fullMin, &partialMin);
    
    // Get probability maximum
    int probabilityMax = -1;
    int tileX = -1;
    int tileY = -1;
    for (int x = 0; x < FIELD_SIZE; x++) {
        for (int y = 0; y < FIELD_SIZE; y++) {
			// Already tried
			if (field_GetStatus(field, x, y) != UNTRIED) {
				continue;
			}
			
			// Calculate view extents from the current tile
			// These are all guaranteed >= 1 if the tile is UNTRIED.
			int viewLeft  = field_GetExtent(field, LEFT,  x, y, UNTRIED);
			int viewRight = field_GetExtent(field, RIGHT, x, y, UNTRIED);
			int viewUp    = field_GetExtent(field, UP,    x, y, UNTRIED);
			int viewDown  = field_GetExtent(field, DOWN,  x, y, UNTRIED);
			assert(viewLeft >= 1);
			assert(viewRight >= 1);
			assert(viewUp >= 1);
			assert(viewDown >= 1);
			
			// Find any nearby hits, beginning at our neighbors
			// and extending outwards.
			int nearLeft  = field_GetExtent(field, LEFT,  x-1, y,   HIT);
			int nearRight = field_GetExtent(field, RIGHT, x+1, y,   HIT);
			int nearUp    = field_GetExtent(field, UP,    x,   y-1, HIT);
			int nearDown  = field_GetExtent(field, DOWN,  x,   y+1, HIT);
			int nearHorizontal = nearLeft + nearRight;
			int nearVertical = nearUp + nearDown;
			assert(nearLeft >= 0);
			assert(nearRight >= 0);
			assert(nearUp >= 0);
			assert(nearDown >= 0);
			
			// If we are actually next to a hit, we need to use the partial
			// minimum. Otherwise we aren't next to a hit and it is safe to
			// use the full ship minimum.
			bool blockedHorizontal;
			if (nearLeft > 0 || nearRight > 0) {
				blockedHorizontal = (viewRight+viewLeft) <= (partialMin-nearHorizontal);
			} else {
				blockedHorizontal = (viewRight+viewLeft) <= (fullMin-nearHorizontal);
			}
			
			// Vertical check corresponds exactly
			bool blockedVertical;
			if (nearUp > 0 || nearDown > 0) {
				blockedVertical = (viewUp+viewDown) <= (partialMin-nearVertical);
			} else {
				blockedVertical = (viewUp+viewDown) <= (fullMin-nearVertical);
			}
			
			// Determine the probability at the tile.
			int probability = 0;
			if (!blockedHorizontal || !blockedVertical) {
				// Weight probability towards the center of the view.
				probability = viewLeft*viewRight + viewUp*viewDown;
				// Weight a lot if near to other hits.
				probability += (nearHorizontal+nearVertical)*(FIELD_SIZE*FIELD_SIZE);
			}
			assert(probability >= 0);
            
            // Check probability maximum
            if (probability > probabilityMax) {
                probabilityMax = probability;
                tileX = x;
                tileY = y;
            }
        }
    }
    
    // Sanity check before making the attack
    assert(tileX != -1);
    assert(tileY != -1);
    assert(field_GetStatus(field, tileX, tileY) == UNTRIED);

    // Make attack
    STATUS result = field_Attack(field, tileX, tileY);
    if (result == ERROR) {
        eprintf("Attack at (%d, %d) failed.\n", tileX, tileY);
        return false;
    }
    
    // Sanity check after attacking
    assert(field_GetStatus(field, tileX, tileY) != UNTRIED);
    assert(field_GetStatus(field, tileX, tileY) == result);
    return true;
}

/*============================================================*/
